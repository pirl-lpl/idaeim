/*	URL_Checker

Copyright (C) 2013-2014 Bradford Castalia, idaeim studio.

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

idaeim CVS ID: URL_Checker.cc,v 1.21 2014/02/22 02:36:59 castalia Exp


This file is a fork from the UA HiRISE Project source code produced by
Bradford Castalia at the HiRISE Operations Center:

HiROC CVS ID: URL_Checker.cc,v 1.1 2012/10/04 02:11:11 castalia Exp

Copyright (C) 2011-2012  Arizona Board of Regents on behalf of the
Planetary Image Research Laboratory, Lunar and Planetary Laboratory at
the University of Arizona.

This library is free software; you can redistribute it and/or modify it
under the terms of the GNU Lesser General Public License, version 2.1,
as published by the Free Software Foundation.

This library is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

*******************************************************************************/

#include	"URL_Checker.hh"

#include	"Network_Access_Manager.hh"
using idaeim::network_access_manager;

#include	"Qstrings.hh"
#include	"Qlisters.hh"

#include	<QNetworkAccessManager>
#include	<QNetworkReply>
#include	<QEventLoop>
#include	<QTimer>
#include	<QUrl>
#include	<QFileInfo>
#include	<QDir>
#include	<QMutex>
#include	<QMutexLocker>

#include	<sstream>
using std::ostringstream;
#include	<iomanip>
using std::endl;




namespace idaeim
{
/*==============================================================================
	Constants
*/
const char* const
	URL_Checker::ID =
		"idaeim::URL_Checker (1.21 2014/02/22 02:36:59)";

/*==============================================================================
	Defaults
*/
#ifndef DEFAULT_URL_CHECKER_SYNCHRONOUS
#define DEFAULT_URL_CHECKER_SYNCHRONOUS	true
#endif
bool
	URL_Checker::Default_Synchronous	= DEFAULT_URL_CHECKER_SYNCHRONOUS;

/*==============================================================================
	Constructor
*/
URL_Checker::URL_Checker
	(
	QObject*	parent
	)
	:	QThread (parent),
		Network_Status (),
		Event_Loop (NULL),
		Timer (NULL)
{
setObjectName ("URL_Checker");
}


URL_Checker::~URL_Checker ()
{
cancel ();
wait (Default_Wait_Time);
if (Event_Loop)
	{
	delete Event_Loop;
	delete Timer;
	}
}

/*==============================================================================
	Accessors
*/
qint64
URL_Checker::content_size () const
{
QMutexLocker
	lock (Status_Lock);
return
	Network_Reply ?
		Network_Status::content_size () :
		Content_Size;
}


QString
URL_Checker::status_description
	(
	int		code
	)
{return (code == REDIRECTION_LOOP) ?
	tr ("Redirection loop") : Network_Status::status_description (code);}


bool
URL_Checker::checking () const
{return isRunning () || request_status () == IN_PROGRESS;}

/*==============================================================================
	Run the thread
*/
bool
URL_Checker::check
	(
	const QUrl&	URL,
	bool		synchronous
	)
{
int
	result = IN_PROGRESS;
if (! isRunning () &&
	! URL.isEmpty ())
	{
	Status_Lock->lock ();

	if (Request_Status == IN_PROGRESS)
		{
		Status_Lock->unlock ();
		return false;
		}

	//	Reset the check state.
	reset_state ();
	requested_URL (URL);
	request_status (result);	//	IN_PROGRESS

	Content_Size = -1;

	Status_Lock->unlock ();

	if (is_HTTP_URL (URL))
		{
		//	HTTP server check.
		start ();

		if (synchronous)
			{
			wait ();	//	Wait for the thread to finish;
			result = Request_Status;
			/*
				N.B.: The synchronous flag is being overloaded as a
				send-signal flag.
			*/
			synchronous = false;	//	No signal from here.
			}
		}
	else
	if (is_file_URL (URL))
		{
		//	Local filesystem check.
		synchronous = true;
		QFileInfo
			file (QDir::toNativeSeparators (URL.path ()));
		if (file.exists ())
			{
			if (file.isFile () &&
				file.isReadable ())
				{
				Content_Size = file.size ();
				result = ACCESSIBLE_URL;
				}
			else
				result = URL_ACCESS_DENIED;
			}
		else
			result = URL_NOT_FOUND;
		}
	else
		{
		synchronous = true;
		result = INVALID_URL;
		}

	if (synchronous)
		{
		//	Set the request status.
		request_status (result);

		//	>>> SIGNAL <<<
		emit checked (result == ACCESSIBLE_URL);
		}
	}

return result == ACCESSIBLE_URL;
}


void
URL_Checker::run ()
{
Status_Lock->lock ();
if (! Event_Loop)
	Event_Loop = new QEventLoop;
if (! Timer)
	{
	Timer = new QTimer;
	Timer->setSingleShot (true);
	connect
		(Timer,			SIGNAL (timeout ()),
		 Event_Loop,	SLOT (quit ()));
	}
if (! Network_Access_Manager)
	/*
		The QNetworkAccessManager MUST be constructed on the same thread
		where it is used with its QNetworkReply.
	*/
	Network_Access_Manager = new QNetworkAccessManager;

if (Network_Reply)
	delete Network_Reply;	//	From previous operation.
Network_Reply = Network_Access_Manager->head (QNetworkRequest (Requested_URL));
connect
	(Network_Reply,	SIGNAL (finished ()),
	 Event_Loop,	SLOT (quit ()));
connect
	(Network_Reply,	SIGNAL (readyRead ()),
	 Event_Loop,	SLOT (quit ()));
Status_Lock->unlock ();

//	Wait in a local event loop on network reply for data or finished.
Timer->start (Wait_Time);
Event_Loop->exec ();

//	Event ocurred.
Status_Lock->lock ();	//	Lock the Network_Status.
if (Timer->isActive ())
	{
	Timer->stop ();
	reset_state (*Network_Reply);	
	}
else
	{
	//	Timeout.
	Network_Reply->abort ();
	request_status (WAIT_TIMEOUT);
	}
int
	result = Request_Status;
Status_Lock->unlock ();

//	>>> SIGNAL <<<
emit checked (result == ACCESSIBLE_URL);

}

/*==============================================================================
	Utilities
*/
int
URL_Checker::check
	(
	const QString&	source_name,
	QString*		final_reference,
	qint64*			size
	)
{
int
	status (INVALID_PATHNAME);
if (! source_name.isEmpty ())
	{
	QUrl
		URL (source_name);
	if (! has_recognized_scheme (URL))
		{
		URL = normalized_URL (QFileInfo (source_name).absoluteFilePath ());
		}

	QStringList
		references (URL_Checker::URL_redirection (URL, &status, size));
	if (final_reference)
		{
		if (status == Network_Status::SUCCESS)
			*final_reference = references.last ();
		else
			final_reference->clear ();
		}
	}
return status;
}


QStringList
URL_Checker::URL_redirection
	(
	const QUrl&	URL,
	int*		status,
	qint64*		size
	)
{
QStringList
	URL_list;
int
	final_status (NO_STATUS);
qint64
	content_size (-1);
URL_Checker
	URL_checker;
QUrl
	url (URL);
QString
	representation;
while (! url.isEmpty ())
	{
	representation = url.toString ();
	if (URL_list.contains (representation))
		{
		URL_list.append (representation);
		final_status = REDIRECTION_LOOP;
		break;
		}
	URL_list.append (representation);
	if (! URL_checker.check (url, URL_Checker::SYNCHRONOUS))
		{
		final_status = URL_checker.request_status ();
		break;
		}
	final_status = URL_checker.request_status ();
	content_size = URL_checker.content_size ();
	url = URL_checker.redirected_URL ();
	}
if (status)
	*status = final_status;
if (size)
	*size = content_size;

return URL_list;
}


QIODevice*
QIODevice_for
	(
	const QString&	source_name,
	QString*		failure_message
	)
{
QIODevice*
	source (NULL);
QString
	message;
if (! source_name.isEmpty ())
	{
	QString
		pathname (source_name);
	if (is_URL (source_name))
		{
		QUrl
			URL (source_name);
		if (is_file_URL (URL))
			pathname = URL.path ();
		else
			{
			int
				status;
			QStringList
				URLs (URL_Checker::URL_redirection (URL, &status));
			if (status == URL_Checker::ACCESSIBLE_URL)
				{
				URL.setUrl (URLs.last ());

				//	The global QNetworkAccessManager is used.
				source = network_access_manager ()->get (QNetworkRequest (URL));

				status = static_cast<QNetworkReply*>(source)->error ();
				if (status != URL_Checker::SUCCESS)
					{
					message += QObject::tr ("The URL couldn't be read: ");
					message += URL_Checker::status_description (status);
					delete source;
					source = NULL;
					goto Done;
					}
				}
			else
				{
				message += QObject::tr ("The URL couldn't be accessed: ");
				message += URL_Checker::status_description (status);
				message += '.';
				goto Done;
				}
			}
		}
	if (! source)
		{
		QFileInfo
			file (pathname);
		if (! file.exists ())
			message += QObject::tr ("The file does not exist.");
		else
		if (! file.isFile ())
			message += QObject::tr ("The file is not a regular file.");
		else
			{
			source = new QFile (pathname);
			if (! source->open (QIODevice::ReadOnly))
				{
				message += QObject::tr ("Couldn't open the file: ");
				message += source->errorString ();
				message += '.';
				delete source;
				source = NULL;
				}
			}
		}
	}

Done:
if (failure_message &&
	! source &&
	! message.isEmpty ())
	{
	ostringstream
		report;
	report
		<< QObject::tr ("Couldn't get a QIODevice for \"")
			<< source_name << "\"." << endl
		<< message;
	failure_message->append (report.str ().c_str ());
	}
return source;
}


}	//	namespace idaeim
