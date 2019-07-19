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

CVS ID: Qistream.hh,v 1.7 2014/01/15 05:23:14 castalia Exp


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

#ifndef QISTREAM_HH
#define QISTREAM_HH

#include	<istream>
#include	<streambuf>

#include	<QObject>
//	Forward references.
class QIODevice;
class QNetworkReply;
class QEventLoop;
class QTimer;


namespace idaeim
{
//	Forward reference.
class Qistreambuf;

/**	A <i>Qistream</i> implements a std::istream on a Qt QIODevice.

	@author		Bradford Castaia, idaeim
	@version	1.7
*/
class Qistream
:	public std::istream
{
public:
/*==============================================================================
	Constants
*/
//!	Class identification name with source code version and date.
static const char* const
	ID;

/*==============================================================================
	Constructors
*/
/**	Construct a Qistream on a QIODevice.

	<b>N.B.</b>: The QIODevice must be readable; i.e. its openMode must
	have at least the ReadOnly flag set.

	The QIODevice will be used to construct a Qistreambuf that is bound
	to the base istream. The Qistreambuf supports the asynchronous input
	of QNetworkReply QIODevice with a timeout.

	@param	qiodevice	A pointer to the QIODevice to be used as the
		source of data.
	@throws	std::invalid_argument if the QIODevice pointer is NULL.
	@throws std::runtime_error if the QIODevice is not readable.
*/
explicit Qistream (QIODevice* qiodevice);

/**	Destroy the Qistream.

	The streambuf for the base istream will be set to NULL before the
	Qistreambuf associated with the QIODevice is deleted.
*/
virtual ~Qistream ();

private:
Qistreambuf
	*qistreambuf;

};	//	Class Qistream


/**	A <i>Qistreambuf</i> implements a std::streambuf on a Qt QIODevice.

	Asynchronous input of a QNetworkReply QIODevice with a timeout is
	supported.

	<b>N.B.</b>: This streambuf only supports data input for use with a
	Qistream.

	@author		Bradford Castaia, idaeim (formerly UA/HiROC)
	@version	1.7
*/
class Qistreambuf
:	public QObject,
	public std::streambuf
{
Q_OBJECT

public:
/*==============================================================================
	Constants
*/
//!	Class identification name with source code version and date.
static const char* const
	ID;


//!	The default size of the data buffer.
static const std::streamsize
	DEFAULT_BUFFER_SIZE;

//!	The size of the put-back area in the data buffer.
static const std::streamsize
	MIN_PUTBACK;

/*==============================================================================
	Constructors
*/
/**	Construct a Qistreambuf on a QIODevice.

	<b>N.B.</b>: The Qistreambuf does not take ownership of the QIODevice.

	@param	qiodevice	A pointer to the QIODevice to which the stream
		buffer is to be bound as the source of data.
	@throws	std::invalid_argument if the QIODevice pointer is NULL.
	@throws std::runtime_error if the QIODevice is not readable.
*/
explicit Qistreambuf (QIODevice* qiodevice);

//!	Destroys the Qistreambuf.
virtual ~Qistreambuf ();

private:
//!	Copying not allowed.
Qistreambuf (const Qistreambuf&);
Qistreambuf& operator= (const Qistreambuf&);

/*==============================================================================
	Accessors
*/
public:

/**	Set the default read wait time.

	The default wait time will be applied when a new Qistreambuf is
	constructed. The wait time of existing Qistreambuf objects are not
	affected.

	@param	msecs	The default read wait time in milliseconds.
	@see	wait_time(int)
*/
inline static void default_wait_time (int msecs)
	{Default_Wait_Time = ((msecs < 0) ? -1 : msecs);}

/**	Get the default read wait time.

	@return	The default read wait time in milliseconds.
	@see	default_wait_time (int)
*/
inline static int default_wait_time ()
	{return Default_Wait_Time;}

/**	Set the read wait time.

	A QIODevice implements asynchronous, non-blocking I/O. Thus when a
	data read operation is requested there may be no data available even
	though the end of the source stream has not yet been reached. To
	accommodate this, blocking I/O is applied when no data is available
	for a read operation. If data does not become available within the
	wait time a timeout occurs and the read operation fails.

	<b>N.B.</b>: The only way to distinguish a timeout from other read
	failures, such as having reached the end of the stream, is by testing
	this Qistreambuf for a {@link timeout() timeout} condition.

	<b>N.B.</b>: Waiting for data to become available will block the
	current thread.

	@param	msecs	The read wait time in milliseconds. <b>N.B.</b>: If
		less than zero a read wait will never timeout.
	@see	wait_time()
	@see	default_wait_time(int)
*/
inline void wait_time (int msecs)
	{Wait_Time = ((msecs < 0) ? -1 : msecs);}

/**	Get the read wait time.

	@return	The read wait time in milliseconds.
	@see	wait_time (int)
*/
inline int wait_time () const
	{return Wait_Time;}

/**	Test if a timeout occured while data was being read.

	<b>N.B.</b>: The timeout condition is reset whenever a new attempt is
	made to refill the internal data buffer; i.e. a timeout condition
	does not prevent retrying to read data.

	@return	true if a timeout occurred while attemping to read data from
		the QIODevice into the internal data buffer; false if no timeout
		has occurred.
*/
inline bool timeout () const
	{return Timeout;}

/**	Get the size of the data buffer.

	@return	The size of the data buffer.
*/
inline std::streamsize buffer_size () const
	{return Buffer_Size;}

/*==============================================================================
	streambuf implementation
*/
protected:

/**	Get data from the stream.

	Data is transferred from the internal source buffer to the user
	specified data buffer until amount bytes have been transferred, no
	more data is available from the stream source, or a {@link timeout()
	timeout} occured while {@link wait_time() waiting} for data to arrive
	from the data source. The internal source buffer is refilled as
	needed.

	@param	data	A pointer to a char_type buffer that will receive the
		data from the stream. <b>WARNING</b>: This data buffer must have
		capacity for at least amount bytes. If NULL nothing is done and
		zero is returned.
	@param	amount	The number of bytes to transfer into the data buffer.
		If zero nothing is done and zero is returned.
	@return	The number of bytes transferred to the data buffer. This will
		be zero if amount is zero or no bytes could be obtained from the
		data source. <b>N.B.</b>: A return value of zero when amount is
		non-zero indicates that the end of the source stream has been
		reached or a {@link timeout() timeout} has occured while {@link
		wait_time() waiting} for data to arrive from the data source.
*/
virtual std::streamsize xsgetn
	(std::streambuf::char_type* data, std::streamsize amount);

/**	Moves the current stream position by a relative offset.

	The position of the stream from which the next input datum will be
	obtained is changed, if possible, to some offset relative to the
	current stream position, the beginning of the stream, or the end of
	the stream.

	<b>N.B.</b>: Changing the stream position may not be possible for a
	sequential access QIODevice (such as a network data source or process
	standard input channel; see QIODevice::isSequential()). However the
	Qistreambuf implementation will always reposition the next stream
	input position if the new stream position is within the range of data
	currently in the internal data buffer, even when the QIODevice is
	sequential.

	@param	offset	The offset to be applied. Positive values will move
		the position down stream (increased position), negative values
		will move the position up stream (decreased position).
	@param	direction	The direction from which the offset is to be
		applied. This can be std::ios_base::cur to offset relative to the
		current stream position, std::ios_base::beg to offset relative to
		the beginning of the stream, or std::ios_base::end to offset
		relative to the end of the stream. <b>N.B.</b>: The end of the
		stream is the position immediately following the final datum
		available from the stream; and a negative offset from the end of
		the stream specifies a position before the end of the stream.
	@param	mode	Which stream position, read or write, is to be
		modified. <b>N.B.</b>: A Qistreambuf only supports input (read
		mode), so this argument is ignored.
	@return	The new stream position. This will be -1 if the operation
		could not be done; e.g. if the QIODevice can not be repositioned
		as requested because it is sequential.
	@see seekpos(std::streambuf::pos_type, std::ios_base::openmode)
*/
virtual std::streambuf::pos_type seekoff
	(std::streambuf::off_type offset, std::ios_base::seekdir direction,
	 std::ios_base::openmode mode);

/**	Sets the current stream position to an absolute location.

	The position of the stream is changed, if possible, so that the next
	input datum will be obtained from the specified position in the
	stream.

	<b>N.B.</b>: Changing the stream position may not be possible for a
	sequential access QIODevice (such as a network data source or process
	standard input channel; see QIODevice::isSequential()). However the
	Qistreambuf implementation will always reposition the next stream
	input position if the new stream position is within the range of data
	currently in the internal data buffer, even when the QIODevice is
	sequential.

	@param	position	The next input datum position to which the stream
		is to be set.
	@param	mode	Which stream position, read or write, is to be
		modified. <b>N.B.</b>: A Qistreambuf only supports input (read
		mode), so this argument is ignored.
	@return	The new stream position. This will be -1 if the operation
		could not be done; e.g. if the QIODevice can not be repositioned
		as requested because it is sequential.
	@see seekoff(std::streambuf::off_type, std::ios_base::seekdir,
		std::ios_base::openmode)
*/
virtual std::streambuf::pos_type seekpos
	(std::streambuf::pos_type position, std::ios_base::openmode mode);

/**	Get the next byte from the stream.

	If the internal buffer is empty it is refilled before the byte at the
	buffer's get pointer is returned.

	@return	The next byte, as an int_type value, from the stream. This
		will be the special eof byte if no more bytes are available from
		the stream.
	@see	timeout()
*/
virtual std::streambuf::int_type underflow ();

/*==============================================================================
	Helpers
*/
private:

/**	Refill the internal data buffer.

	If no QIODevice has been bound to the Qistreambuf nothing is done and
	zero (NO_DATA) is returned.

	If the buffer pointers indicate that the buffer is not empty nothing
	is done and the status value 1 (HAS_DATA) is returned.

	If the buffer pointers indicate that the entire buffer content has
	been read (i.e. transferred out of the buffer or skipped over), the
	last - up to MIN_PUTBACK - bytes read are moved to the beginning of
	the buffer and the buffer pointers are reset to indicate that the
	buffer only contains the moved put-back bytes.

	If data is not available from the QIODevice data source but the data
	source is a QNetworkReply (detected in the constructor) and the
	{@link wait_time() wait time} is non-zero, then a single-shot QTimer
	for the wait time is started and a local QEventLoop - with its quit
	slot connected to the timer's timeout slot and the QNetworkReply's
	readyRead and finished slots (setup in the constructor) - is exec-ed.
	When the event loop quits if the timer is not active then the timer
	expired, the {@link timeout() timeout condition} is set to true, and
	the status value -1 (WAIT_TIMEOUT) is returned; otherwise the timer
	is stopped.

	If data is now available from the QIODevice data source up to the
	available capacity (not including any put-back bytes) of the buffer
	is read into the buffer and the buffer pointers are updated to
	indicated the amount actually read.

	@return	A status indicator that will be positive (1, HAS_DATA) if the
		buffer now has more data availabe; negative (-1, WAIT_TIMEOUT) if
		a timeout occured while waiting for data to arrive from the data
		source; zero (NO_DATA) if no more data is available from the data
		source.
	@see timeout()
*/
int refill_buffer ();

/*==============================================================================
	Data
*/
private:

//!	The data source. May be NULL.
QIODevice
	*QIO_Device;
//!	Non-NULL if, and only if, the data source is a QNetworkReply.
QNetworkReply
	*Network_Reply;

//!	The internal data buffer. NULL if the data source is NULL.
std::streambuf::char_type
	*Buffer;
//!	The size of the data buffer.
std::streamsize
	Buffer_Size;

//!	Local event loop used to pend on data arrival from a QNetworkReply.
QEventLoop
	*Event_Loop;
//!	Timer attatched to the local event loop.
QTimer
	*Timer;
//!	@see wait_time(int)
int
	Wait_Time;
//!	@see default_wait_time(int)
static int
	Default_Wait_Time;
//!	@see timeout()
bool
	Timeout;

};		//	Class Qistreambuf


}		//	namespace idaeim
#endif	//	QISTREAM_HH
