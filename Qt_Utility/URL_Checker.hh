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

idaeim CVS ID: URL_Checker.hh,v 1.18 2014/02/22 02:45:44 castalia Exp


This file is a fork from the UA HiRISE Project source code produced by
Bradford Castalia at the HiRISE Operations Center:

HiROC CVS ID: URL_Checker.hh,v 1.1 2012/10/04 02:11:11 castalia Exp

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

#ifndef URL_CHECKER_HH
#define URL_CHECKER_HH

#include	"Network_Status.hh"

#include	<QThread>
#include <QIODevice>

//	Forward references.
class QUrl;
class QEventLoop;
class QTimer;
class QNetworkReply;


namespace idaeim
{
/**	A <i>URL_Checker</i> checks if a URL refers to an accessible source.

	The URL check may be done asynchronously or synchrously with a
	maximum wait time for it to complete, and a request in progress may
	be canceled. A local file URL may also be checked; this is always
	done synchronously. A maximum time may be specified for the check to
	complete. A signal is emitted whenever a check has been completed.

	The check of a network URL is done by attempting to only obtain the
	server response headers; the content of the source specified by the
	URL is not requested.

	The URL_Checker is a fully thread safe QThread subclass, and a
	subclass of Network_Status. When checking an HTTP URL a thread is run
	that uses a QNetworkAccessManager to check the URL for acccessibility
	without obtaining any source data.

	The results of a URL check include a QNetworkReply::NetworkError
	value with special condition extensions, the server HTTP status code
	and status description for HTTP URL checks, and the URL for a server
	redirection.

	<b>N.B.</b>: A QApplication event loop is not required to use objects
	of this class, but a QCoreApplication (or QApplication) must be
	present for the local event loop that is used to function.

	@author		Bradford Castalia, idaeim
	@version	1.18
*/
class URL_Checker
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


/**	Convenience tokens for the {@link check(const QUrl&, bool) check} method
	synchronous argument.
*/
enum
	{
	ASYNCHRONOUS,
	SYNCHRONOUS
	};


/**	{@link URL_redirection(const QUrl&, int*, qint64*) URL redirection}
	loop detected status.
*/
enum
	{
	REDIRECTION_LOOP	= -999
	};

/*==============================================================================
	Defaults
*/
//!	The default synchronous value for a {@link check(const QUrl&, bool) check}.
static bool
	Default_Synchronous;

/*==============================================================================
	Constructor
*/
/**	Constructs a URL_Checker.

	@param	parent	A QObject that owns the constructed object and will
		destroy it when the parent is destroyed.
*/
explicit URL_Checker (QObject* parent = NULL);

//!	Destroys the URL_Checker.
virtual ~URL_Checker ();

/*==============================================================================
	Accessors
*/
/**	Get the size of the checked file content.

	@return	The size, in bytes, of the last file content that was
		checked. If nothing has yet been checked, the content size is not
		yet known (during a network fetch) or can not be known (e.g. for
		a directory), or the last check failed, -1 is returned.
*/
qint64 content_size () const;

/**	Test if a {@link check(const QUrl&, bool) URL check} is currently in
	progress.

	@return	true if a check is in progress; false otherwise.
*/
bool checking () const; 

/**	Get a brief description of a {@link request_status() request status}
	code.

	<b>N.B.</b>: The errorString method of the QIODevice base class of
	QNetworkReply may provide an additional description.

	@param	code	A network request status code.
	@return	A QString containing a brief, one line, description of the
		code.
*/
static QString status_description (int code);

/**	Get the default synchronous value for a {@link
	check(const QUrl&, bool) check}.

	@return	The default synchronous value.
*/
inline static bool default_synchronous ()
	{return Default_Synchronous;}

/**	Set the default synchronous value for a {@link
	check(const QUrl&, bool) check}.

	@param	synchronous	The default synchronous value.
*/
inline static void default_synchronous (bool synchronous)
	{Default_Synchronous = synchronous;}

/*==============================================================================
	Thread run
*/
/**	Check a URL for an accessible source.

	If a URL check is {@link checking() in progress}, the specified URL
	is empty, or the {@link request_status()const request status} is
	{@link #IN_PROGRESS} nothing is done and false is returned
	immediately.

	The check state is reset to unset values when a check is started.

	The URL is checked to see if it is accessible. Only a URL with an
	HTTP or "file" protocol will be accepted for checking. A URL without
	a protocol is taken to be a file URL. The URL must have a pathname.
	HTTP URLs must have a hostname; a hostname in a file URL is ignored.
	Any other URLs are rejected and the {@link request_status() request
	status} value will be set to {@link #INVALID_URL}.

	For an HTTP URL a network request will be made for the source access
	information. The source content is not acquired; instead of
	requesting the source data only the network headers are requested
	from the server connection. If any error occurs while attempting to
	obtain the network headers for the source - including a content not
	found error - then the source is not accessible.

	<b>N.B.</b>: A URL source will appear to be accessible when the
	server returns a {@link redirected_URL() redirected URL} where the
	ultimate source may or may not be accesible; it is the responsibility
	of the application to decide if a redirected URL is to be checked.

	An HTTP URL check may be synchronous or asynchronous. A synchronous
	check will wait - the thread of the caller will be blocked - until
	the check completes. The wait for the network request to complete
	will timeout if the {@link wait_time() wait time} is exceeded, in
	which case the network request will be {@link cancel() canceled} and
	the {@link request_status() request status} will be set to {@link
	#WAIT_TIMEOUT}. Thread blocking can be avoided - this can be
	important, for example, when the check is being done from the thread
	running the main GUI event loop - by specifying an asynchronous check
	with a connection to the {@link checked(bool) checked} signal to
	obtain the results of the check.

	If the URL specifies the file protocol, or does not specify any
	protocol, then the local filesystem is checked for the path portion
	of the URL. The pathname must exist and be a readable file for it to
	be accessible.

	The {@link checked(bool) checked} signal is always emitted when
	the check completes regardless of whether the check is synchronous
	or asynchronous, or if a timeout occurs.

	@param	URL	A QUrl that is to be checked.

		<b>>>> WARNING <<<</b> A filesystem pathname on MS/Windows that
		includes a drive specification - e.g. "D:" - will be interpreted
		as having a URL scheme specification that is the drive letter
		when a QUrl is constructed from such a pathname string. For this
		reason it is advisable that a {@link normalized_URL(const QUrl&)
		normalized URL} be used here.

	@param	synchronous	This argument is ignored unless the URL specifies
		an HTTP protocol. If synchronous is true the method will wait for
		the network request to complete or timeout before returning;
		otherwise the method will return without waiting for the network
		request to complete. If the argument is not specified the
		{@link default_synchronous() default synchronous} value is used.
	@return	true if the check completed successfully and the source was
		found to be accessible; false otherwise. <b>N.B.</b>: For an
		asynchronous HTTP URL check the return value is always false;
		connect to the {@link checked(bool) checked} signal to get the
		results of the check, or test if the {@link request_status()
		request status} is {@link #ACCESSIBLE_URL} when {@link
		checking() checking} becomes false.
*/
bool check (const QUrl& URL, bool synchronous = Default_Synchronous);


protected:

/**	Begin running the network request thread.

	<b>N.B.</b>: Running the thread should be done by calling the start
	method. The thread will finish when the network fetch has finished.
*/
virtual void run ();

/*==============================================================================
	Utilities
*/
public:

/**	Check accessibility of a named source.

	This is a convenience function that {@link check(const QUrl&, bool)
	checks} the accessibility of the source name as a URL.

	If the source name appears to be for a filesystem pathname its
	absolute pathname is the effective source name.

	The effective source name is used to construct a {@link
	normalized_URL(const QString&) normalized URL} that is synchronously
	checked with {@link URL_redirection(const QUrl&, int*, qint64*) URL
	redirection followed}. The resulting request status is returned.

	@param	source_name	The name of the data source to be checked. This
		may be a filesystem pathname or URL. If empty nothing is done
		and the return status value is {@link #INVALID_PATHNAME}.
	@param[out]	final_reference	A pointer to a QString that, if non-NULL,
		will receive the final reference that was successfully checked.
		If the source name is for a URL that was redirected to a source
		that was found, this will be the redirected URL; if the source
		was found without any redirection, this will be the source name;
		if the check failed, this will be an empty string.
	@param[out]	size	A pointer to a qint64 variable that, if non-NULL,
		will receive the {@link content_size()const size of the content}
		that was found. This will be -1 if no content was found or the
		content size is unknown (e.g. a URL referring to a remote
		directory, rather than a regular file, is expected to produce a
		SUCCESS status but -1 for the content size).
	@return	The {@link request_status()const request status code} of the
		check. This will be {@link #SUCCESS} (zero) if the check was successful;
		a non-zero value otherwise.
*/
static int check (const QString& source_name,
	QString* final_reference = NULL, qint64* size = NULL);

/**	Check a URL for server redirection.

	The URL is {@link check(const QUrl&, bool) checked} for being
	accessible. If it is and it is found to have been {@link
	redirected_URL()const redirected to another URL} the new URL
	is checked in the same way. This continues until no URL redirection
	is found, the check fails, or a URL redirection loop is encountered.

	The URL redirection check is done synchronously.

	@param	URL	The URL to be checked for redirection.
	@param[out]	status	A pointer to an int variable that, if non-NULL,
		will receive the final {@link request_status()const check
		status}. If a redirection loop is detected the status is the
		special {@link #REDIRECTION_LOOP} value.
	@param[out]	size	A pointer to a qint64 variable that, if non-NULL,
		will receive the {@link content_size()const size of the content}
		that was found. This will be -1 if no content was found.
	@return	A QStringList containing the URLs, in string form, that
		were checked, in the order they were checked. The first entry
		will be for the specified URL, unless it is empty in which
		case the returned list will be empty. The last entry is for
		the URL that was not redirected, that was not accessible, or
		was found to already be in the list.
*/
static QStringList URL_redirection (const QUrl& URL,
	int* status = NULL, qint64* size = NULL);

/*==============================================================================
	Qt signals
*/
public:

signals:

/**	Signals the result of a {@link check(const QUrl&, bool) URL check}.

	The signal is emitted whenever a URL check completes.

	@param	exists	true if the checked URL is accessible; false otherwise.
	@see request_status()
*/
void checked (bool exists);

/*==============================================================================
	Data
*/
private:

qint64
	Content_Size;

QEventLoop
	*Event_Loop;
QTimer
	*Timer;
};

/*==============================================================================
	Global Utilities
*/
/**	Get a QIODevice for a named data source.

	If the source name is empty nothing is done and NULL is returned.

	If the source name is a plain filesystem pathname or a {@link
	is_file_URL(const QUrl&) represents a file URL} an attempt is made to
	open a QFile for reading on the pathname.

	If the source name is for an HTTP URL it is {@link
	URL_Checker::check(const QUrl&, bool) checked for accessibility} and
	any URL redirection is tracked to the ultimate source URL. Then the
	{@link network_access_manager() global QNetworkAccessManager} is used
	to get a QNetworkReply for the URL.

	<b>N.B.</b>: The QIODevice is constructed on the caller's thread.
	If the QIODevice is a QNetworkReply it likely to need to use the
	event loop of the thread to deliver operation signals.

	@param	source_name	The name of the data source. This may be a
		filesystem pathname or a URL.
	@param	failure_message	A pointer to a QString to which a failure
		message will be appended. May be NULL if no failure message is
		desired.
	@return	A QIODevice pointer. This will be for a QFile in the case
		where the source name is a filesystem pathname (or a URL with a
		file protocol), or a QNetworkReply in the case where the source
		name is a URL. This will be NULL if a QIODevice could not be
		opened on the source name, in which case if a failure message
		pointer was provided the reason for the failure is appended.

		<b>N.B.</b>: Ownership of the QIODevice is passed to the caller.
		It is the responsibility of the caller to delete the the
		QIODevice when it is no longer to be used.
*/
QIODevice* QIODevice_for
	(const QString& source_name, QString* failure_message = NULL);


}	//	namespace idaeim
#endif
