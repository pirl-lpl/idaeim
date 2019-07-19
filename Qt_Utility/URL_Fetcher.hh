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

idaeim CVS ID: URL_Fetcher.hh,v 1.8 2014/01/23 00:25:59 castalia Exp
*******************************************************************************/

#ifndef URL_FETCHER_HH
#define URL_FETCHER_HH

#include	"Network_Status.hh"

#include	<QThread>

//	Forward references.
class QUrl;
class QFile;
class QEventLoop;
class QTimer;
class QIODevice;


namespace idaeim
{
/**	A <i>URL_Fetcher</i> fetches data content from a file to which a URL
	refers.

	The URL that is fetched may be an HTTP reference or a file reference
	including a plain local filesystem pathname.

	The URL fetch may be done asynchronously or synchrously with a
	maximum wait time for it to complete, and a request in progress may
	be canceled. The progress of the fetch is signaled and a signal is
	emitted whenever a fetch has been completed.

	The fetched data may be written to a file on the host's filesystem,
	or placed in the user's data buffer. In the latter case the maximum
	amount of data fetched is limited to the size of the data buffer.

	The URL_Fetcher is a Network_Status subclass that provides status
	information about fetches.

	The URL_Fetcher is thread safe.

	<b>N.B.</b>: A QApplication event loop is not required to use objects
	of this class, but a QCoreApplication (or QApplication) must be
	present for the local event loop that is used to function. However,
	synchronously fetching will cause the event loop of the calling
	thread to be blocked until the fetch completes. So GUI applications
	are advised to use asynchrous fetching to prevent the GUI from
	freezing on long fetches.

	@author		Bradford Castalia, idaeim
	@version	1.8
*/
class URL_Fetcher
:	public QThread,
	public Network_Status
{
//	Qt Object declaration.
Q_OBJECT

public:
/*==============================================================================
	Constants
*/
//!	Class identification name with source code version and date.
static const char* const
	ID;


/**	Convenience tokens for the {@link
	fetch(const QUrl&, const QString&, bool, bool) fetch} method
	allow_overwrite argument.
*/
enum
	{
	DONT_ALLOW_OVERWRITE,
	ALLOW_OVERWRITE
	};

/**	Convenience tokens for the fetch methods synchronous argument.

	@see fetch(const QUrl&, const QString&, bool, bool)
	@see fetch(const QUrl&, char*, qint64, bool)
*/
enum
	{
	ASYNCHRONOUS,
	SYNCHRONOUS
	};

/*==============================================================================
	Defaults
*/
/**	The default destination file allow overwrite value for a {@link
	fetch(const QUrl&, const QString&, bool, bool) fetch}.
*/
static bool
	Default_Allow_Overwrite;

/**	The default synchronous value for a {@link
	fetch(const QUrl&, const QString&, bool, bool) fetch}.
*/
static bool
	Default_Synchronous;

/*==============================================================================
	Constructor
*/
/**	Constructs a URL_Fetcher.

	@param	parent	A QObject that owns the constructed object and will
		destroy it when the parent is destroyed.
*/
explicit URL_Fetcher (QObject* parent = NULL);

//!	Destroys the URL_Fetcher.
virtual ~URL_Fetcher ();

/*==============================================================================
	Accessors
*/
/**	Gets the absolute pathname of the destination for the last file that
	was {@link fetch(const QUrl&, const QString&, bool, bool) fetched}.

	<b>N.B.</b>: The destination pathname is not updated if {@link
	fetching()const fetching} was in progress, or the destination
	pathname was empty, when the last fetch was requested.
	
	@return	A QString that contains the pathname of the last file
		that was fetched, whether the fetch was successsful or not.
*/
QString destination () const;

/**	Get the size of the fetched data content.

	@return	The size, in bytes, of the last file content that was
		fetched. If nothing has yet been fetched, the content size
		is not yet known (during a network fetch), or the last fetch
		failed, -1 is returned.
*/
qint64 content_size () const;

/** Get the amount of data fetched.

	@return	The amount of data, in bytes, obtained from the
		last fetch. If the {@link request_status()const request
		status} is {@link #IN_PROGRESS} the value will be the
		amount fetched so far.
*/
qint64 amount_fetched () const;

/**	Get the default synchronous value for a {@link
	fetch(const QUrl&, const QString&, bool, bool) fetch}.

	@return	The default synchronous value.
	@see default_synchronous(bool)
*/
inline static bool default_synchronous ()
	{return Default_Synchronous;}

/**	Set the default synchronous value for a {@link
	fetch(const QUrl&, const QString&, bool, bool) fetch}.

	@param	synchronous	The default synchronous value.
	@see default_synchronous()
*/
inline static void default_synchronous (bool synchronous)
	{Default_Synchronous = synchronous;}

/**	Get the default destination file allow overwrite value for a {@link
	fetch(const QUrl&, const QString&, bool, bool) fetch}.

	@return	The default destination file allow overwrite value.
	@see default_allow_overwrite(bool)
*/
inline static bool default_allow_overwrite ()
	{return Default_Allow_Overwrite;}

/**	Set the default allow destination file overwrite value for a {@link
	fetch(const QUrl&, const QString&, bool, bool) fetch}.

	@param	allow_overwrite	The default destination file allow overwrite
		value.
	@see default_allow_overwrite()
*/
inline static void default_allow_overwrite (bool allow_overwrite)
	{Default_Allow_Overwrite = allow_overwrite;}

/*==============================================================================
	Thread run
*/
/**	Fetch data referenced by a URL and write it to a file.

	If a URL fetch is {@link fetching()const in progress}, or the
	specified URL path is empty, nothing is done and false is returned
	immediately.

	When a fetch is started the network request state is {@link reset()
	reset} to unset values with the {@link request_status() request
	status} set to {@link #IN_PROGRESS}.

	Only an {@link is_HTTP_URL(const QUrl&) HTTP URL} or a {@link
	is_file_URL(const QUrl&) file URL}, including a plain filesystem
	pathname, will be accepted for fetching. Any other URLs are rejected
	and the {@link request_status() request status} value will be set to
	{@link #INVALID_URL}.

	If the destination file exists and allow overwriting has not been
	enabled nothing is done and false is returned. In this case the fetch
	is not started, the request status is set to {@link
	#INVALID_PATHNAME}, and the reason for the failure will be provided
	in the {@link HTTP_status_description()const HTTP status
	description}. If the file can not be opened for writing the fetch
	will fail and the request status will also be set to {@link
	#FILE_WRITE_FAILURE}; this may happend asynchronously. In this
	case any existing destination file will not be modified.

	If the URL specifies the file protocol, or does not specify any
	protocol, then the file specified by the pathname of the URL is
	fetched (i.e. copied). The pathname must, of course, exist and be a
	readable regular file. If the canonical pathname to the source file
	is the same as the canonical pathname to the destination file the
	fetch returns true immediately without copying any data. However, it
	is possible for the same file to be located at different pathnames
	(e.g. hard links on Unix filesystems) in which case that the files
	are identical will not be detected.

	A copy of the file content referenced by the URL is written to the
	destination file. If a network request fails because the source file
	is not found the fetch fails with a {@link #URL_NOT_FOUND} request
	status and any sting destination file will not be modified.
	<b>N.B.</b>: If the URL is redirected by the server but the network
	request is not automatically redirected to the new URL, the content
	that is fetched will be the server's error message which will be
	written to the destination file.

	A fetch may be synchronous or asynchronous. A synchronous fetch will
	wait - the thread of the caller will be blocked - until the fetch
	completes. The data fetch process will  wait for each chunk of data
	to arrive up to the {@link wait_time() wait time}. A timeout waiting
	for data will result in the network request will be {@link cancel()
	canceled} and the {@link request_status() request status} will be set
	to {@link #WAIT_TIMEOUT}.

	Thread blocking can be avoided - this can be important, for example,
	when the fetch is being done from the thread running the main GUI
	event loop - by specifying an asynchronous fetch with a connection to
	the {@link fetched(bool) fetched} signal to obtain the results of the
	fetch.

	As the data is being fetched, whether asynchronously or
	synchronously, the {@link fetch_progress(qint64, qint64) progress of
	the fetch is signalled} after each data chunk has been written to the
	destination file.

	The {@link fetched(bool) fetched} signal is always emitted when
	the fetch completes regardless of whether the fetch is synchronous or
	asynchronous, or if the fetch was successful or not.

	@param	URL	A QUrl that is to be fetched. If this is an empty URL
		nothing is done, the request status will be {@link
		#INVALID_URL}, and false is returned.

		<b>>>> WARNING <<<</b> A filesystem pathname on MS/Windows that
		includes a drive specification - e.g. "D:" - will be interpreted
		as having a URL scheme specification that is the drive letter
		when a QUrl is constructed from such a pathname string. For this
		reason it is advisable that a {@link normalized_URL(const QUrl&)
		normalized URL} be used here.

	@param	destination	A QString providing the pathname to a file where
		the fetched data will be written. <b>N.B.</b>: An existing file
		will not be overwritten.
	@param	allow_overwrite	If true and the destination file exits it
		will be overwritten. If false and the destination file exits
		it will not be overwritten and the fetch fails with an {@link
		#INVALID_PATHNAME} request status.
	@param	synchronous	If true the method will wait for the fetch to
		complete or timeout before returning; otherwise the method will
		return without waiting for the network request to complete. If
		the argument is not specified the {@link default_synchronous()
		default synchronous} value is used.
	@return	For a synchronous fetch: true if the fetch completed
		successfully; false otherwise. For an asynchronous fetch: true if
		the fetch has been started; false otherwise. Check the {@link
		Network_Status::request_status()const request status} to obtain
		the reason for a false return value.
	@see fetch(const QUrl&, char*, qint64, bool)
	@see fetching()const
*/
bool fetch (const QUrl& URL, const QString& destination,
	bool allow_overwrite = Default_Allow_Overwrite,
	bool synchronous = Default_Synchronous);

/**	Fetch data referenced by a URL into a data buffer.

	This method functions the same as the companion {@link
	fetch(const QUrl&, const QString&, bool, bool) fetch into a file},
	except that the content is placed in the specified data buffer.

	@param	URL	A QUrl that is to be fetched. If this is an empty URL
		nothing is done, the request status will be {@link
		#INVALID_URL}, and false is returned.
	@param	data_buffer	The address of the data buffer that is to receive
		that fetched content. The data buffer capacity must be sufficient
		to hold at least the amount of data to be fetched. If NULL
		nothing is done, the request status will be {@link
		#INVALID_PATHNAME}, and false is returned.
	@param	amount	The maximum amount, in bytes, of data to be fetched.
		If less than or equal to zero no data will be fetched.
	@param	synchronous	If true the method will wait for the fetch to
		complete or timeout before returning; otherwise the method will
		return without waiting for the network request to complete.
	@return	For a synchronous fetch: true if the fetch completed
		successfully; false otherwise. For an asynchronous fetch: true if
		the fetch has been started; false otherwise. Check the {@link
		Network_Status::request_status()const request status} to obtain
		the reason for a false return value.
	@see fetch(const QUrl&, const QString&, bool, bool)
*/
bool fetch (const QUrl& URL, char* data_buffer, qint64 amount,
	bool synchronous = Default_Synchronous);

/**	Test if a {@link fetch(const QUrl&, const QString&, bool, bool) URL
	fetch} is currently in progress.

	@return	true if a fetch is in progress; false otherwise.
*/
bool fetching () const; 


protected:

/**	Begin running the network request thread.

	<b>N.B.</b>: Running the thread should be done by calling the start
	method. The thread will finish when the network fetch has finished.
*/
virtual void run ();

/*==============================================================================
	Helpers
*/
private:

/**	Fetch the URL data.

	This helper provides the final source setup, thread start and
	wait, and finish operations used by the fetch methods.

	@param	URL	The QUrl that is to be fetched.
	@param	synchronous	This argument is ignored unless the URL specifies
		an HTTP protocol. If synchronous is true the method will wait for
		the network request to complete or timeout before returning;
		otherwise the method will return without waiting for the network
		request to complete.
	@return	For a synchronous fetch: true if the fetch completed
		successfully; false otherwise. For an asynchronous fetch: true if
		the fetch has been started; false otherwise.
*/
bool fetch_it (const QUrl& URL, bool synchronous);

/**	Wait for data to become available for reading.

	If no data is available from the data source, and the source is an
	unfinished Network_Reply, the wait Timer is started with the {@link
	wait_time()const wait time} and the Event_Loop is exec-ed, which will
	block the thread. When the Event_Loop quits data has arrived, the
	Network_Reply is finished, or the timer has expired. If the timer has
	expired the request is {@link cancel() canceled}, the Pending_Status
	is set to {@link #WAIT_TIMEOUT}, and -1 is returned. Otherwise the
	amount of available data is obtained.

	@param	source	A pointer to the QIODevice that is the source of
		data. <b>CAUTION</b>: This must not be NULL.
	@return	The amount of data available to read from the source. This
		will be zero when no more data is available for reading. It will
		be -1 if a timeout occured while waiting for data from a network
		reply.
*/
qint64 wait_for_data (QIODevice* source);

/**	Get data from a data source.

	For the first time through - when Pending_Status is NO_STATUS -
	Pending_Status reset. It is set to SUCCESS when the data source is
	the Soure_File. When the data source is the Network_Reply the
	Pending_Status is set from the {@link
	request_status(const QNetworkReply&) Network_Reply request status}.
	If the request status is not SUCCESS the request is {@link cancel()
	canceled} and -1 is returned.

	Data is read in chunks into the data buffer while the requested
	amount has not been read and {@link wait_for_data(QIODevice*) data is
	available to read}.

	For each successful read of a data chunk Content_Fetched is
	incremented by the amount of data read. If the data source is the
	Network_Reply and the Content_Size is unknown (-1) an attempt is made
	to {@link Network_Status::content_size(const QNetworkReply&) set the
	Content_Size from the Network_Reply}. Then the {@link
	fetch_progress(qint64, qint64) fetch progress signal} is emitted.

	@param	source	A pointer to the QIODevice that is the source of
		data. <b>CAUTION</b>: This must not be NULL.
	@param	data	A pointer to the buffer that is to receive the data.
		The data buffer must have a capacity of at least the amount of
		data to be read. <b>CAUTION</b>: This must not be NULL.
	@param	amount	The amount, in bytes, of data to read into the data
		buffer. If this is less than or equal to zero nothing is done and
		zero is returned.
	@return	The actual amount of data read into the buffer. This will be
		zero if the amount to read is less than or equal to zero, or when
		no more data is available for reading. It will be -1 if readin
		data failed and Pending_Status will be set to indicate the reason
		for the failure.
*/
qint64 get_data (QIODevice* source, char* data, qint64 amount);

/**	Write data to the Destination_File.

	If the Destination_File is not open it is opened for writing
	and truncated. If this fails the Pending_Status is set to
	{@link #FILE_WRITE_FAILURE} and the HTTP_Status_Description
	is set to an appropriate message.

	If writing the data fails the Pending_Status is set to
	{@link #FILE_WRITE_FAILURE} and the HTTP_Status_Description
	is set to the Destination_File errorString.

	@param	data	The address of the data.
	@param	amount	The number of data bytes to be written.
	@return	true if the data was written successfully; false otherwise.
*/
bool put_data (char* data, qint64 amount);

/**	Wait until the request is finished.

	If the request is for a local file true is returned immediately.

	For a network request the local event loop is used to wait for
	the network reply to be finished or a wait timeout. If a wait
	timeout occurs the network reply is canceled.

	@return	true if the request is finished; false if the wait
		timed out.
*/
bool wait_until_finished ();

/**	Reset the transitory data resources.

	If the Destination_File is being used it is deleted along
	with the locally allocated Data_Buffer; otherwise the
	Data_Buffer is owned by the user. The Data_Buffer is
	always set to NULL and the Data_Buffer_Size set to zero.

	The Source_File is deleted and set to NULL.
*/
void release_resources ();

/*==============================================================================
	Qt signals
*/
public:

signals:

/**	Signals fetch progress.

	This signal is emitted whenever a chunk of data has been transferred
	from its source to the destination file, after the data has been
	written to the destintation file.

	@param	bytes_received	The cumulative number of bytes that have
		been fetched so far.
	@param	bytes_total	The total number of bytes to be fetched. This
		will be -1 if the total size of the file is not known, but will
		become non-negative when the fetch has finished and the amount
		received equals the total amount.
*/
void fetch_progress (qint64 bytes_received, qint64 bytes_total);

/**	Signals the result of a {@link
	fetch(const QUrl&, const QString&, bool, bool) URL fetch}.

	The signal is emitted whenever a URL fetch completes.

	@param	success	true if the URL was successfully fetched; false
		otherwise.
	@see request_status()
*/
void fetched (bool success);

/*==============================================================================
	Data
*/
private:

int
	Pending_Status;

QString
	Destination_Pathname;

QFile
	*Destination_File,
	*Source_File;

char*
	Data_Buffer;
qint64
	Data_Buffer_Size;

qint64
	Content_Fetched,
	Content_Size;

QEventLoop
	*Event_Loop;
QTimer
	*Timer;

};


}	//	namespace idaeim
#endif
