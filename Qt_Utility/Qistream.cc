/*	Qistream

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

CVS ID: Qistream.cc,v 1.7 2014/01/15 05:23:14 castalia Exp


This file is a fork from the UA HiRISE Project source code produced by
Bradford Castalia at the HiRISE Operations Center:

HiROC CVS ID: Qstream.hh,v 1.1 2012/10/04 02:11:11 castalia Exp

Copyright (C) 2012  Arizona Board of Regents on behalf of the
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

#include	"Qistream.hh"

#include	<QIODevice>
#include	<QNetworkReply>
#include	<QEventLoop>
#include	<QTimer>

#include	<sstream>
using std::ostringstream;
#include	<iomanip>
using std::endl;
#include	<stdexcept>
using std::invalid_argument;
using std::runtime_error;




namespace idaeim
{
/*******************************************************************************
	Qistream
*/
const char* const
	Qistream::ID =
		"idaeim::Qistream (1.7 2014/01/15 05:23:14)";

Qistream::Qistream
	(
	QIODevice*	qiodevice
	)
	: std::istream (qistreambuf = new Qistreambuf (qiodevice))
{}


Qistream::~Qistream ()
{
rdbuf (NULL);	//	Unbind the Qistreambuf from the istream.
delete qistreambuf;
}

/*******************************************************************************
	Qistreambuf
*/
/*==============================================================================
	Constants
*/
const char* const
	Qistreambuf::ID =
		"idaeim::Qistreambuf (1.7 2014/01/15 05:23:14)";


#ifndef QSTREAM_BUFFER_SIZE
#define QSTREAM_BUFFER_SIZE				1024
#endif
#if QSTREAM_BUFFER_SIZE <= 0
#error	Invalid QSTREAM_BUFFER_SIZE less than or equal to zero.
#endif
//!	The default size of the data buffer.
const std::streamsize
	Qistreambuf::DEFAULT_BUFFER_SIZE	= QSTREAM_BUFFER_SIZE;

#ifndef QSTREAM_MIN_PUTBACK
#define QSTREAM_MIN_PUTBACK				4
#endif
#if QSTREAM_MIN_PUTBACK < 0
#error	Invalid QSTREAM_MIN_PUTBACK less than zero.
#endif
#if QSTREAM_MIN_PUTBACK >= (QSTREAM_BUFFER_SIZE / 2)
#error	The QSTREAM_MIN_PUTBACK is not less than half the QSTREAM_BUFFER_SIZE.
#endif
const std::streamsize
	Qistreambuf::MIN_PUTBACK			= QSTREAM_MIN_PUTBACK;

enum
	{
	WAIT_TIMEOUT	= -1,
	NO_DATA			= 0,
	HAS_DATA		= 1
	};

/*==============================================================================
	Static data
*/
#ifndef QSTREAM_DEFAULT_WAIT_TIME
#define QSTREAM_DEFAULT_WAIT_TIME		10000
#endif
int
	Qistreambuf::Default_Wait_Time		= QSTREAM_DEFAULT_WAIT_TIME;

/*==============================================================================
	Constructors
*/
Qistreambuf::Qistreambuf
	(
	QIODevice*	qiodevice
	)
	:	QObject (),
		std::streambuf (),
		QIO_Device (qiodevice),
		Network_Reply (NULL),
		Buffer (NULL),
		Buffer_Size (DEFAULT_BUFFER_SIZE),
		Event_Loop (NULL),
		Timer (NULL),
		Wait_Time (Default_Wait_Time),
		Timeout (false)
{
setg (0, 0, 0);	//	No buffer.
if (QIO_Device)
	{
	if (! QIO_Device->isReadable ())
		{
		QIO_Device = NULL;
		ostringstream
			message;
		message
			<< ID << endl
			<< "The QIODevice is not readable.";
		throw runtime_error (message.str ());
		}

	//	Allocate the data buffer.
	Buffer = new std::streambuf::char_type[Buffer_Size];

	//	Set buffer pointers to empty buffer.
	setg (Buffer, Buffer, Buffer);

	//	Test for the case where the QIODevice is a QNetworkReply.
	Network_Reply = dynamic_cast<QNetworkReply*>(QIO_Device);
	if (Network_Reply)
		{
		Event_Loop = new QEventLoop;
		connect
			(Network_Reply,	SIGNAL (finished ()),
			 Event_Loop,	SLOT (quit ()));
		connect
			(Network_Reply,	SIGNAL (readyRead ()),
			 Event_Loop,	SLOT (quit ()));

		Timer = new QTimer;
		Timer->setSingleShot (true);
		connect
			(Timer,			SIGNAL (timeout ()),
			 Event_Loop,	SLOT (quit ()));
		}
	}
else
	{
	ostringstream
		message;
	message
		<< ID << endl
		<< "Can't construct a Qistreambuf on a NULL QIODevice.";
	throw invalid_argument (message.str ());
	}
}


Qistreambuf::~Qistreambuf ()
{
if (Buffer)
	delete[] Buffer;
if (Network_Reply)
	{
	delete Event_Loop;
	delete Timer;
	}
}


Qistreambuf::Qistreambuf (const Qistreambuf&) : QObject (), std::streambuf () {}
Qistreambuf& Qistreambuf::operator= (const Qistreambuf&) {return *this;}

/*==============================================================================
	streambuf implementation
*/
std::streamsize
Qistreambuf::xsgetn
	(
	std::streambuf::char_type*	data,
	std::streamsize				amount
	)
{
std::streamsize
	got = 0,
	available;
if (data)
	{
	while (amount)
		{
		if (refill_buffer () != HAS_DATA)
			break;

		available = egptr () - gptr ();
		if (available > amount)
			available = amount;
		traits_type::copy (data, gptr (), available);
		gbump ((int)available);
		data += available;
		got += available;
		amount -= available;
		}
	}
return got;
}


int
Qistreambuf::refill_buffer ()
{
int
	status = NO_DATA;
if (gptr () < egptr ())
	{
	status = HAS_DATA;
	}
else
if (QIO_Device)
	{
	int
		putback = gptr () - eback ();
	if (putback > MIN_PUTBACK)
		putback = MIN_PUTBACK;
	if (putback)
		{
		//	Move last read put-back bytes to the buffer head.
		traits_type::move (Buffer, gptr () - putback, putback);

		//	Reset the buffer pointers.
		setg (Buffer, Buffer + putback, Buffer + putback);
		}

	//	Refill the buffer.
	std::streamsize
		available = QIO_Device->bytesAvailable ();
	if (! available &&
		Network_Reply &&
		! Network_Reply->isFinished () &&
		Wait_Time)
		{
		Timeout = false;
		//	Wait in a local event loop on network reply for data or finished.
		Timer->start (Wait_Time);
		Event_Loop->exec ();
		//	Event occured.
		if (Timer->isActive ())
			{
			Timer->stop ();
			//	Did more data arrive?
			available = QIO_Device->bytesAvailable ();
			}
		else
			{
			status = WAIT_TIMEOUT;
			Timeout = true;
			}
		}
	if (available)
		{
		status = HAS_DATA;
		available = QIO_Device->read (Buffer + putback, Buffer_Size - putback);

		//	Reset the buffer pointers.
		setg
			(Buffer,
			 Buffer + putback,
			 Buffer + putback + available);
		}
	}
return status;
}
 

std::streambuf::pos_type
Qistreambuf::seekoff
	(
	std::streambuf::off_type	offset,	//	signed integer type.
	std::ios_base::seekdir		direction,
	std::ios_base::openmode		mode
	)
{
/*	N.B. a pos_type value is not necessarily an integer type
	but it is required to provide conversion to/from int and off_type
	as well as the addition and substraction of off_type values.
*/
std::streambuf::pos_type
	position = -1;
if (QIO_Device)
	{
	switch (direction)
		{
		case std::ios_base::cur:
			position = QIO_Device->pos () - (egptr () - gptr ()) + offset;
			if (! offset)
				//	Return current position (tellg implementation).
				goto Done;
			break;
		case std::ios_base::beg:
			position = offset;
			break;
		case std::ios_base::end:
			if (! QIO_Device->isSequential ())
				{
				position = QIO_Device->size () + offset;
				}
			break;
		default:
			break;
		}
	if (position != std::streambuf::pos_type (-1))
		//	Move to the new position.
		position = seekpos (position, mode);
	}
Done:
return position;
}


std::streambuf::pos_type
Qistreambuf::seekpos
	(
	std::streambuf::pos_type	position,
	std::ios_base::openmode		/* mode */
	)
{
if (QIO_Device)
	{
	if (position < QIO_Device->pos () &&
		position >= (QIO_Device->pos () - (egptr () - eback ())))
		{
		//	Move the get pointer.
		setg
			(eback (),
			 egptr () - (QIO_Device->pos () - position),
			 egptr ());
		}
	else
	if (QIO_Device->seek (position))
		{
		position = QIO_Device->pos ();
		//	Reset buffer pointers to empty buffer.
		setg (Buffer, Buffer, Buffer);
		}
	else
		//	Seek failed.
		position = std::streambuf::pos_type (-1);
	}
else
	{
	position = std::streambuf::pos_type (-1);
	}
return position;
}


std::streambuf::int_type
Qistreambuf::underflow ()
{
std::streambuf::int_type
	result;
if (gptr () < egptr () ||
	refill_buffer () == HAS_DATA)
	{
	result = traits_type::to_int_type (*gptr ());
	}
else
	{
    result = traits_type::eof ();
	}
return result;
}


}	//	namespace idaeim
