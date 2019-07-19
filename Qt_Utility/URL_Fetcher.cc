/*	URL_Fetcher

Copyright (C) 2014 Bradford Castalia, idaeim studio.

This file is part of the idaeim libraries.

The idaeim libraries are free software; you can redistribute them and/or
modify them under the terms of the GNU Lesser General Public License as
published by the Free Software Foundation, either version 3 of the
License, or (at your option) any later version.

The idaeim libraries are distributed in the hope that they will be
useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser
General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with this program. If not, see <http://www.gnu.org/licenses/>.

idaeim CVS ID: URL_Fetcher.cc,v 1.10 2014/01/31 04:02:13 castalia Exp
*******************************************************************************/

#include	"URL_Fetcher.hh"

#include	"Qstrings.hh"

#include	<QNetworkAccessManager>
#include	<QNetworkReply>
#include	<QEventLoop>
#include	<QTimer>
#include	<QUrl>
#include	<QFileInfo>
#include	<QDir>
#include	<QMutex>
#include	<QMutexLocker>




namespace idaeim
{
/*==============================================================================
	Constants
*/
const char* const
	URL_Fetcher::ID =
		"idaeim::URL_Fetcher (1.10 2014/01/31 04:02:13)";

/*==============================================================================
	Defaults
*/
#ifndef DEFAULT_URL_FETCHER_ALLOW_OVERWRITE
#define DEFAULT_URL_FETCHER_ALLOW_OVERWRITE	false
#endif
bool
	URL_Fetcher::Default_Allow_Overwrite
		= DEFAULT_URL_FETCHER_ALLOW_OVERWRITE;

#ifndef DEFAULT_URL_FETCHER_SYNCHRONOUS
#define DEFAULT_URL_FETCHER_SYNCHRONOUS		true
#endif
bool
	URL_Fetcher::Default_Synchronous
		= DEFAULT_URL_FETCHER_SYNCHRONOUS;

#ifndef URL_FETCHER_DATA_BUFFER_SIZE
#define URL_FETCHER_DATA_BUFFER_SIZE		(1 << 16)
#endif

/*==============================================================================
	Constructor
*/
URL_Fetcher::URL_Fetcher
	(
	QObject*	parent
	)
	:	QThread (parent),
		Network_Status (),
		Pending_Status (NO_STATUS),
		Destination_File (NULL),
		Source_File (NULL),
		Data_Buffer (NULL),
		Data_Buffer_Size (0),
		Content_Fetched (0),
		Content_Size (-1),
		Event_Loop (NULL),
		Timer (NULL)
{
setObjectName ("URL_Fetcher");
}


URL_Fetcher::~URL_Fetcher ()
{
cancel ();
wait (Default_Wait_Time);
release_resources ();
delete Timer;
delete Event_Loop;
}

/*==============================================================================
	Accessors
*/
QString
URL_Fetcher::destination () const
{
QMutexLocker
	lock (Status_Lock);
return Destination_Pathname;
}


qint64
URL_Fetcher::content_size () const
{
QMutexLocker
	lock (Status_Lock);
return
	Network_Reply ?
		Network_Status::content_size () :
		Content_Size;
}


qint64
URL_Fetcher::amount_fetched () const
{
QMutexLocker
	lock (Status_Lock);
return Content_Fetched;
}


bool
URL_Fetcher::fetching () const
{return isRunning () || request_status () == IN_PROGRESS;}

/*==============================================================================
	Run the thread
*/
bool
URL_Fetcher::fetch
	(
	const QUrl&		URL,
	const QString&	destination,
	bool			allow_overwrite,
	bool			synchronous
	)
{
bool
	succeeded (false);

if (! isRunning ())
	{
	Status_Lock->lock ();

	if (Request_Status == IN_PROGRESS)
		{
		Status_Lock->unlock ();
		return false;
		}

	//	Reset the fetch state:

	reset_state ();
	requested_URL (URL);
	request_status (IN_PROGRESS);

	Content_Fetched = 0;
	Content_Size = -1;

	//	Prepare the destination file; open deferred to first write.
	Destination_Pathname = QFileInfo (destination).absoluteFilePath ();

	Status_Lock->unlock ();

	if (destination.isEmpty ())
		request_status (INVALID_PATHNAME);
	else
		{
		Destination_File = new QFile (Destination_Pathname);
		if (Destination_File->exists () &&
			! allow_overwrite)
			{
			//	Don't overwrite existing file.
			request_status (INVALID_PATHNAME);
			HTTP_status_description (tr ("The destination file exists."));
			}
		else
			{
			Pending_Status = NO_STATUS;
			Data_Buffer_Size = URL_FETCHER_DATA_BUFFER_SIZE;

			succeeded = fetch_it (URL, synchronous);
			}
		}
	}

return succeeded;
}


bool
URL_Fetcher::fetch
	(
	const QUrl&	URL,
	char*		data_buffer,
	qint64		amount,
	bool		synchronous
	)
{
bool
	succeeded (false);

if (! isRunning ())
	{
	Status_Lock->lock ();

	if (Request_Status == IN_PROGRESS)
		{
		Status_Lock->unlock ();
		return false;
		}

	//	Reset the fetch state:

	reset_state ();
	requested_URL (URL);
	request_status (IN_PROGRESS);

	Pending_Status = NO_STATUS;
	Content_Fetched = 0;
	Content_Size = -1;

	Status_Lock->unlock ();

	if (! data_buffer)
		{
		request_status (INVALID_PATHNAME);
		return false;
		}
	Data_Buffer = data_buffer;
	Data_Buffer_Size = ((amount >= 0) ? amount : 0);

	succeeded = fetch_it (URL, synchronous);
	}

return succeeded;
}


bool
URL_Fetcher::fetch_it
	(
	const QUrl&		URL,
	bool			synchronous
	)
{
int
	result = IN_PROGRESS;

if (is_file_URL (URL))
	{
	//	Local file fetch: prepare the Source_File.
	Source_File = new QFile (QDir::toNativeSeparators (URL.path ()));
	if (! Source_File->exists ())
		{
		result = URL_NOT_FOUND;
		HTTP_Status_Description =
			tr ("The source file does not exist.");
		}
	else
	if (! QFileInfo (*Source_File).isFile ())
		{
		result = URL_ACCESS_DENIED;
		HTTP_Status_Description =
			tr ("The source file is not a regular file.");
		}
	else
	if (Destination_File &&
		QFileInfo (*Source_File) == QFileInfo (*Destination_File))
		{
		result = SUCCESS;
		HTTP_Status_Description =
			tr ("The source and destination files are the same.");
		}
	else
	if (! Source_File->open (QIODevice::ReadOnly))
		{
		result = URL_ACCESS_DENIED;
		HTTP_Status_Description = Source_File->errorString ();
		}
	else
		{
		Status_Lock->lock ();
		Content_Size = Source_File->size ();
		Status_Lock->unlock ();
		}
	}
else
if (! is_HTTP_URL (URL))
	{
	result = INVALID_URL;
	}

if (result == IN_PROGRESS)
	{
	//	Start the tread that will do the data fetch.
	start ();

	if (synchronous)
		{
		wait ();	//	Wait for the thread to finish;

		result = Pending_Status;
		}
	else
		result = SUCCESS;
	}
else
	{
	//	Setup failure.
	release_resources ();

	//	Set the request status.
	request_status (result);

	//	>>> SIGNAL <<<
	emit fetched (false);
	}

return result == SUCCESS;
}


void
URL_Fetcher::release_resources ()
{
//	Release transitory resources. Leave the network reply for user inspection.
if (Destination_File)
	{
	delete Destination_File;
	Destination_File = NULL;

	//	The Data_Buffer is owned by the caller if no Destination_File.
	delete[] Data_Buffer;
	}
Data_Buffer = NULL;
Data_Buffer_Size = 0;
delete Source_File;
Source_File = NULL;
}


void
URL_Fetcher::run ()
{
Status_Lock->lock ();

if (! Data_Buffer)
	Data_Buffer = new char[Data_Buffer_Size];

QIODevice*
	source;
if (Source_File)
	source = Source_File;
else
	{
	if (! Event_Loop)
		Event_Loop = new QEventLoop;

	if (! Timer)
		{
		Timer = new QTimer;
		Timer->setSingleShot (true);
		connect
			(Timer, 		SIGNAL (timeout ()),
			 Event_Loop,	SLOT (quit ()));
		}

	if (! Network_Access_Manager)
		/*
			The QNetworkAccessManager must be constructed on the same thread
			where it is used with its QNetworkReply.
		*/
		Network_Access_Manager = new QNetworkAccessManager;

	if (Network_Reply)
		delete Network_Reply;	//	From previous operation.
	Network_Reply =
		Network_Access_Manager->get (QNetworkRequest (Requested_URL));
	connect
		(Network_Reply,	SIGNAL (readyRead ()),
		 Event_Loop, 	SLOT (quit ()));
	connect
		(Network_Reply,	SIGNAL (finished ()),
		 Event_Loop,	SLOT (quit ()));

	source = Network_Reply;
	}

Status_Lock->unlock ();

//	Fetch the data.
qint64
	amount;
while ((amount = get_data (source, Data_Buffer, Data_Buffer_Size)) > 0)
	{
	if (! Destination_File ||
		! put_data (Data_Buffer, amount))
		break;
	}
if (Destination_File)
	Destination_File->flush ();

Status_Lock->lock ();

if (amount == 0 &&	//	Successful EOF
	Content_Size < 0)
	{
	Content_Size = Content_Fetched;
	//	>>> SIGNAL <<<
	emit fetch_progress (Content_Fetched, Content_Size);
	}

//	Release transitory resources. Leave the network reply for user inspection.
release_resources ();

if (Network_Reply)
	{
	if (Pending_Status != SUCCESS)
		wait_until_finished ();

	reset_state (*Network_Reply);

	if (Pending_Status != SUCCESS)
		Request_Status = Pending_Status;

	if (! Destination_File &&
		! Network_Reply->isFinished ())
		//	Cancel remaining data.
		cancel ();
	}
else
	Request_Status = Pending_Status;

Status_Lock->unlock ();

//	>>> SIGNAL <<<
emit fetched ((amount == 0));

}


qint64
URL_Fetcher::wait_for_data
	(
	QIODevice*	source
	)
{
qint64
	available (source->bytesAvailable ());
if (! available)
	{
	if (Network_Reply)
		{
		if (! Network_Reply->isFinished ())
			{
			//	Wait for data to arrive over the network.
			Timer->start (wait_time ());

			//	Wait for data, finished, or timeout.
			Event_Loop->exec ();

			//	Event occured.
			if (Timer->isActive ())
				{
				Timer->stop ();
				//	Did more data arrive?
				available = source->bytesAvailable ();
				}
			else
				{
				cancel ();	//	Abort the reply.
				Pending_Status = WAIT_TIMEOUT;
				available = -1;
				}
			}

		if (available == 0)
			/*	Avoid possible race:

				There is the possibility that data may have arrived between
				the initial bytesAvailable call and isFinished became true.
				Thus the double-check here.
			*/
			available = source->bytesAvailable ();
		}
	}
return available;
}


qint64
URL_Fetcher::get_data
	(
	QIODevice*	source,
	char*		data,
	qint64		amount
	)
{
qint64
	amount_read (0),
	available;

while (amount_read < amount)
	{
	available = wait_for_data (source);

	if (Pending_Status == NO_STATUS)
		{
		//	First data get.
		if (Network_Reply)
			{
			Pending_Status = request_status (*Network_Reply);
			if (Pending_Status != SUCCESS)
				{
				//	Error condition; Abort fetch.
				cancel ();	//	Abort the reply.
				amount_read = -1;
				break;
				}
			}
		else
			Pending_Status = SUCCESS;
		}

	if (available > 0)
		{
		if (amount > available)
			amount = available;

		available = source->read (data, amount);

		if (available > 0)
			{
			amount -= available;
			amount_read += available;

			Status_Lock->lock ();
			Content_Fetched += available;
			if (Network_Reply &&
				Content_Size < 0)
				{
				Content_Size = Network_Status::content_size (*Network_Reply);
				}
			Status_Lock->unlock ();

			//	>>> SIGNAL <<<
			emit fetch_progress (Content_Fetched, Content_Size);
			}
		else
			{
			if (available < 0)
				{
				//	Read error.
				Pending_Status = (Network_Reply ?
					Network_Reply->error () : URL_ACCESS_DENIED);
				HTTP_Status_Description = source->errorString ();
				amount_read = -1;
				}
			//	if (available == 0) EOF.
			break;
			}
		}
	else
		//	No data available. Probably EOF.
		break;
	}

return amount_read;
}


bool
URL_Fetcher::put_data
	(
	char*	data,
	qint64	amount
	)
{
if (! Destination_File->isOpen () &&
	! Destination_File->open (QIODevice::WriteOnly | QIODevice::Truncate))
	{
	Pending_Status = FILE_WRITE_FAILURE;
	HTTP_Status_Description
		= tr ("The destination file could not be opened for writing.\n")
		+= Destination_File->errorString ();
	return false;
	}

qint64
	remaining (amount);
while (remaining > 0)
	{
	amount = Destination_File->write (data, remaining);
	if (amount > 0)
		{
		data += amount;
		remaining -= amount;
		}
	else
	if (amount < 0)
		{
		Pending_Status = FILE_WRITE_FAILURE;
		HTTP_Status_Description =
			Destination_File->errorString ();
		break;
		}
	}
return (remaining == 0);
}


bool
URL_Fetcher::wait_until_finished ()
{
bool
	finished (true);
if (Network_Reply)
	{
	while (! Network_Reply->isFinished ())
		{
		Timer->start (wait_time ());

		//	Wait in a local event loop on network reply for data or finished.
		Event_Loop->exec ();

		//	Event occured.
		if (! Timer->isActive ())
			{
			finished = false;
			cancel ();	//	Abort the reply.
			break;
			}
		}
	}
return finished;
}


}	//	namespace idaeim
