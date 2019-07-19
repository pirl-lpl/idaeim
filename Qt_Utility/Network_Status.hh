/*	Network_Status

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

idaeim CVS ID: Network_Status.hh,v 1.14 2014/02/18 02:27:24 castalia Exp


This file is a fork from the UA HiRISE Project source code produced by
Bradford Castalia at the HiRISE Operations Center:

HiROC CVS ID: Network_Status.hh,v 1.3 2012/10/31 11:45:00 castalia Exp

Copyright (C) 2012  Arizona Board of Regents on behalf of the
Planetary Image Research Laboratory, Lunar and Planetary Laboratory at
the University of Arizona.

This library is free software; you can redistribute it and/or modify it
under the terms of the GNU Lesser General Public License, version 2.1,
as published by the Free Software Foundation.

This library is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
Lesser General Public License for more details.

*******************************************************************************/

#ifndef NETWORK_STATUS_HH
#define NETWORK_STATUS_HH

#include	<QUrl>
#include	<QString>

//	Forward references.
class QMutex;
class QNetworkAccessManager;
class QNetworkReply;

#include	<limits.h>	//	For ULONG_MAX


namespace idaeim
{
/**	The <i>Network_Status</i> class provides thread safe network status
	support for classes performing network operations.

	The Network_Status class is typically used as a base class for a
	class performing network operations and/or provides helper functions
	used in network operations.

	The network status state data members are:
<dl>
<dt>{@link network_access_manager() Network_Access_Manager}
<dd>The QNetworkAccessManager used by the network operations class.
	It is the responsibility of the network operations class to
	instantiate a QNetworkAccessManager. <b>N.B.</b>: This should be done
	on the thread where the network request operations will be done.
<dt>{@link network_reply() Network_Reply}
<dd>The currently active QNetworkReply handling a network request. This
	will be NULL if no network request is active.
<dt>{@link requested_URL()const Requested_URL}
<dd>The last URL that was requested. It is the responsibility of the
	network operations class to set this value. The value may be empty
	if no network request has been issued.
<dt>{@link redirected_URL()const Redirected_URL}
<dd>The URL to which the last network request was redirected by the
	server for the requested URL. This value is automatically set
	when the {@link reset(const QNetworkReply&) state data is reset
	from a QNetworkReply}. It will be empty if no network request has
	been issued, after the state has been {@link reset() reset}, and
	when no URL redirection occured for the last network request.
<dt>{@link request_status()const Request_Status}
<dd>The status code of the last network request. The value is
	automatically set when the {@link reset(const QNetworkReply&) state
	data is reset from a QNetworkReply}. It may also be set by the
	network operation class. This will be a negative value if no request
	has been issued or the last request has not been, or will not be,
	completed; otherwise it is a positive QNetworkReply::NetworkError
	value. A {@link status_description(int) brief description for a
	status code} may be obtained.
<dt>{@link HTTP_status()const HTTP_Status}
<dd>The internet HTTP status code of the last network request. This value
	is provided by the server for the network request and is
	automatically set when the {@link reset(const QNetworkReply&) state
	data is reset from a QNetworkReply}. A {@link
	HTTP_status_description()const description of the HTTP status} may
	also be available if the server provides one.
<dt>{@link wait_time()const Wait_Time}
<dd>The amount of time to wait for a network request to complete. The
	intitial value is set from the {@link default_wait_time() default
	wait time}.
<dt>{@link status_lock()const Status_Lock}
<dd>A recursive QMutex used to control access to the network status data
	members. The lock is constructed when a Network_Status is
	instantiated. It should not be changed. All data member accessor
	methods uses this lock, except where noted otherwise.
</dl>

All Network_Status data member accessor methods use the status lock to
ensure thread safety. Network operations classes may manage the
data members directly by employing the status lock to protect access
to the values.

	@author		Bradford Castalia, idaeim
	@version	1.14
*/
class Network_Status
{
public:
/*==============================================================================
	Constants
*/
//!	Class identification name with source code version and date.
static const char* const
	ID;


static const int
	/**	Local {@link request_status()const status} code:
		No status has been set.
	*/
	NO_STATUS,

	/** Local {@link request_status()const status} code:
		The specified URL is invalid.
	*/
	INVALID_URL,

	/**	Local {@link request_status()const status} code:
		The client side pathname is invalid.
	*/
	INVALID_PATHNAME,

	/**	Local {@link request_status()const status} code:
		A network request is in progress.
	*/
	IN_PROGRESS,

	/** Local {@link request_status()const status} code:
		Timeout of the opertion.
	*/
	WAIT_TIMEOUT,

	/**	Local {@link request_status()const status} code:
		Writing to a local file failed.
	*/
	FILE_WRITE_FAILURE,

	/**	Network {@link request_status()const status} code:
		The operation was successful.

		Equivalent to QNetworkReply::NoError
	*/
	SUCCESS,

	/**	Network {@link request_status()const status} code:
		The requested URL was accessible.

		Equivalent to QNetworkReply::NoError
	*/
	ACCESSIBLE_URL,

	/**	Network {@link request_status()const status} code:
		The requested URL was not found.

		Equivalent to QNetworkReply::ContentNotFoundError
	*/
	URL_NOT_FOUND,

	/**	Network {@link request_status()const status} code:
		Access to the requested URL was denied.

		Equivalent to QNetworkReply::ContentAccessDenied
	*/
	URL_ACCESS_DENIED;

/*==============================================================================
	Defaults
*/
protected:

//!	The default network request {@link wait_time() wait_time}.
static unsigned long
	Default_Wait_Time;


private:
/**	The application executable runtime directory location.

	<b>>>> WARNING <<<</b> The Application_Location will be emtpy until
	the application_location function is called for the first time.
*/
static QString
	Application_Location;

/*==============================================================================
	Constructor
*/
public:

//!	Constructs the Network_Status with status values in their unset state.
Network_Status ();

//!	Destructor.
~Network_Status ();

/*==============================================================================
	Accessors
*/
/** Get the network access manager.

	It is the responsibility of the network operations class to
	instantiate a QNetworkAccessManager. <b>N.B.</b>: This should be done
	on the thread where the network request operations will be done.

	@return	A pointer to a QNetworkAccessManager. This will be NULL if
		no network access manager has been instantiated for this
		Network_Status object.
*/
QNetworkAccessManager* network_access_manager ();

/**	Get the current network request reply.

	<b>>>> WARNING <<<</b> The QNetworkReply is likely to be deleted
	when the request has been completed.

	@return	A pointer to a QNetworkReply. This will be NULL if no
		network request is active.
	@see network_reply(QNetworkReply*)
*/
QNetworkReply* network_reply ();

/**	Get the requested URL.

	@return	The URL used by the last network request. This will be empty
		if the network status values have been {@link reset() reset} or
		the value was forcibly cleared.
	@see requested_URL (const QUrl&)
*/
QUrl requested_URL () const;

/**	Set the default amount of time to wait for a network request to
	complete.

	New objects will use this time; existing objects will not be affected.

	@param	msecs	The time to wait, in milliseconds. A zero or negative
		value will be set to the maximum wait time, i.e. effectively no
		timeout.
	@see wait_time(unsigned long)
*/
inline static void default_wait_time (unsigned long msecs)
	{Default_Wait_Time = (msecs ? msecs : ULONG_MAX);}

/**	Get the default amount of time to wait for a network request to
	complete.

	@return	The time to wait, in milliseconds.
	@see default_wait_time(unsigned long)
*/
inline static unsigned long default_wait_time ()
	{return Default_Wait_Time;}

/**	Set the amount of time to wait for a network request to complete.

	The wait time is used for network data delivery and for synchronous
	network request timeout.

	@param	msecs	The time to wait, in milliseconds. A zero or negative
		value will be set to the maximum wait time, i.e. effectively no
		timeout.
	@see wait_time()const
	@see default_wait_time(unsigned long)
*/
void wait_time (unsigned long msecs);

/**	Get the amount of time to wait for a network request to complete.

	@return	The time to wait, in milliseconds.
	@see	wait_time(unsigned long)
*/
unsigned long wait_time () const;

/**	Get the network request status.

	The request status code will be a positive
	QNetworkReply::NetworkError value if the last network request
	completed, successfully or not.

	If no request has been issued or the last request has not been, or
	will not be, completed the value will be a negative value for one of
	the local codes:
	<dl>
	<dt>{@link #NO_STATUS}
	<dd>If no request has been submitted since the last {@link
		reset() reset} to the default status values.
	<dt>{@link #INVALID_URL}
	<dd>If the URL submitted for the network request was found to be
		invalid.
	<dt>{@link #INVALID_PATHNAME}
	<dd>The client side pathname is invalid.
	<dt>{@link #IN_PROGRESS}
	<dd>If a network request is currently in progress.
	<dt>{@link #WAIT_TIMEOUT}
	<dd>If a network request did not complete in within the {@link
		wait_time(unsigned long) wait time}.
	<dt>{@link #FILE_WRITE_FAILURE}
	<dd>Writing to a local file failed.
	</dl>

	@return	A status code value. This will be {@link #NO_STATUS} if the
		network status values have been {@link reset() reset} or no
		{@link network_reply() network reply} is available.
	@see request_status(int)
	@see status_description(int)
*/
int request_status () const;

/**	Get a request status code for a network reply.

	If the network reply has an error code that is not
	QNetworkReply::NoError, that is returned. Note that the network reply
	machinery may encounter error conditions outside of HTTP processing
	(e.g. connection failure) that will be indicated by other request
	status codes when the error condition is encountered during
	processing of the request.

	When the network reply error code is QNetworkReply::NoError
	the {@link HTTP_status()const HTTP status code} is examined. If it is
	not {@link #NO_STATUS} the code an associated request status code is
	returned.

	HTTP status codes come in five groups:
	<dl>
	<dt>1xx Informational
	<dd>This class of status code indicates a provisional response.
		The request status for this group is {@link #SUCCESS}.
	<dt>2xx Success
	<dd>This class of status codes indicates the action requested by the
		client was received, understood, accepted and processed
		successfully. The request status for this group is {@link
		#SUCCESS}.
	<dt>3xx Redirection
	<dd>The client must take additional action to complete the request.
		The request status for this group is {@link #SUCCESS}.
	<dt>4xx Client Error
	<dd>This class of status code is intended for cases in which the
		client seems to have errored. The request status for this group
		depends on the specific HTTP status code:
		<dl>
		<dt>401: Authorization required
		<dd>The request status is
			QNetworkReply::AuthenticationRequiredError.
		<dt>403: Access denied
		<dd>The request status is {@link #URL_ACCESS_DENIED}.
		<dt>404: Not Found
		<dd>The request status is {@link #URL_NOT_FOUND}.
		<dt>405: Method Not Allowed
		<dd>The request status is
			QNetworkReply::ContentOperationNotPermittedError.
		<dt>407: Proxy Authentication Required
		<dd>The request status is
			QNetworkReply::ProxyAuthenticationRequiredError.
		<dt>418: I'm a teapot (RFC 2324)
		<dd>This code was defined in 1998 as one of the traditional IETF
			April Fools' jokes. The request status is
			QNetworkReply::ProtocolInvalidOperationError.
		<dt>Any other 4xx code.
		<dd>The request status is QNetworkReply::UnknownContentError.
		</dl>
	<dt>5xx Server Error
	<dd>The server failed to fulfill an apparently valid request.
		The request status is QNetworkReply::ProtocolUnknownError.
	</dl>

	An HTTP status code not within the recognized groups results in
	a request status of {@link #NO_STATUS}.

	<b>N.B.</b>: The ultimate HTTP status code for a network request may
	not become known until the network reply is finished. It is only
	after the network reply is finished that its request status becomes
	definitive.

	@param	network_reply	A reference to a QNetworkReply.
	@return	A request status code for the network replay.
	@see request_status()const
	@see HTTP_status(const QNetworkReply&)
*/
static int request_status (const QNetworkReply& network_reply);

/**	Get the HTTP status code for the last network request.

	If the HTTP status code is {@link #NO_STATUS} and a {@link
	network_reply() network reply} is available the status will be
	updated from the last reply. <b>N.B.</b>: Updating the status may
	result in a NO_STATUS value.

	@return	The HTTP status code for the last network request URL. This
		will be {@link #NO_STATUS} if the network status values have been
		{@link reset() reset} or no HTTP status was available for the
		last request.
	@see HTTP_status(const QNetworkReply&)
	@see reset(const QNetworkReply&)
*/
int HTTP_status () const;

/**	Get the HTTP status code for a network reply.

	@param	network_reply	A QNetworkReply from which to obtain the
		HTTP status code.
	@return	The HTTP status code for the network reply. This
		will be {@link #NO_STATUS} if no HTTP status is available.
*/
static int HTTP_status (const QNetworkReply& network_reply);

/**	Get the HTTP status description provided by the server for the last
	network request.

	If the HTTP status description is empty and a {@link network_reply()
	network reply} is available the description will be updated from the
	last reply. <b>N.B.</b>: Updating the description may result in an
	empty description.

	@return	A QString containing the HTTP status description provided by
		the last network request. This will be an empty string if the
		network status values have been {@link reset() reset} or no HTTP
		status description was available for last request.
	@see HTTP_status_description(const QNetworkReply&)
	@see reset(const QNetworkReply&)
*/
QString HTTP_status_description () const;

/**	Get the HTTP status description for a network reply.

	@param	network_reply	A QNetworkReply from which to obtain the
		HTTP status description.
	@return	A QString containing the HTTP status description for the
		network reply. This will be an empty string if no HTTP
		status description is available.
	@see reset(const QNetworkReply&)
*/
static QString HTTP_status_description (const QNetworkReply& network_reply);

/**	Get the URL to which the last network request was redirected by the
	server.

	If the redirected URL is empty and a {@link network_reply() network
	reply} is available the redirected URL will be updated from the last
	reply. <b>N.B.</b>: Updating the redirected URL may result in an
	empty URL.

	<b>N.B.</b>: A redirected URL that is a relative URL is resolved
	using the {@link requested_URL()const requested URL} to a fully
	qualified URL.

	@return	A QUrl containing the URL to which the last network request
		was redirected. This will be empty if no redirection was done by
		the server.
	@see redirected_URL(const QNetworkReply&)
	@see reset(const QNetworkReply&)
*/
QUrl redirected_URL () const;

/**	Get the URL to which a network reply was redirected by the server.

	<b>N.B.</b>: A network request may not follow URL redirection. This
	is done at the discretion of the server, and if not done by the
	server is the responsibility of the application. If the netowrk
	request was not redirected then the redirected URL is empty;
	otherwise the network request can be repeated using the provided URL
	to follow the redirection.

	@param	network_reply	A QNetworkReply from which to obtain the
		redirected URL.
	@return	A QUrl containing the URL to which the last network request
		was redirected. This will be empty if the server does not
		indicate that redirection is needed for the {@link
		requested_URL()const requested URL}.
	@see reset(const QNetworkReply&)
*/
static QUrl redirected_URL (const QNetworkReply& network_reply);

/**	Get the content size for the last network request.

	@return	The content size, in bytes, of the last reply. The value is
		obtained from the the Content-Length header entry. If a {@link
		network_reply() network reply} is not available, its request
		error status is not {@link #SUCCESS}, or it does not contain the
		Content-Length header entry, -1 is returned.
	@see content_size(const QNetworkReply&)
*/
qint64 content_size () const;

/**	Get the content size for a network reply.

	The size of the data content returned from a request is provided by
	the Content-Length header entry of the network reply. Obviously, if
	the reply has not yet been received for a request that has been
	issued the headers will not yet be available and so the data content
	size, if any, will not yet be known.

	<b>N.B.</b>: Note that a request for data content that fails - e.g.
	because the file does not exist - may result in the server reply
	providing content that describes the reason for the failure - e.g. an
	{@link HTTP_status()const HTTP status} 404 Not Found message - which
	provides a Content-Length header entry for the failure description
	message. But in such cases the {@link HTTP_status(const QNetworkReply&)
	HTTP status} will indicate that an error occurred and the
	Content-Length value will not be used.

	@param	network_reply	A QNetworkReply from which to obtain the
		content size.
	@return	The content size, in bytes, for the network reply. The value is
		obtained from the the Content-Length header entry. If a {@link
		network_reply() network reply} is not available, its request
		error status is not {@link #SUCCESS}, or it does not contain the
		Content-Length header entry, -1 is returned.
*/
static qint64 content_size (const QNetworkReply& network_reply);

/**	Reset the network request status values to their unset state.

	If the {@link request_status() request_status} value is {@link
	#IN_PROGRESS} nothing is done.

	The {@link requested_URL() requested URL} and {@link redirected_URL()
	redirected URL} are cleared. The {@link request_status() request
	status} and {@link HTTP_status() HTTP status} values are set to
	{@link #NO_STATUS} and the {@link HTTP_status_description()const HTTP
	status description} is cleared.

	If a {@link network_reply() network request reply} is active it is
	aborted if it is not finished, then deleted and the reply value is
	set to NULL. <b>N.B.</b>: Do  <b>N.B.</b>: Do not reset when the
	current network request reply value is non-NULL from within a slot
	handling the {@link network_access_manager() network access manager}
	or {@link network_reply() network request reply} finished signals.

	@return	true if the network request status values were reset to their
		unset state; false if a network request is in progress and the
		values are unchanged.
*/
bool reset ();

/**	Cancel a network request in progress.

	The current {@link network_reply() network request reply} - if
	non-NULL - is aborted.

	It is safe to cancel when no request is in progress.
*/
void cancel ();

/*------------------------------------------------------------------------------
	Subclass accessors
*/
protected:

/**	Set the requested URL.

	@param	URL	A QUrl reference that will be used to set the
		{@link requested_URL() requested URL}.
*/
void requested_URL (const QUrl& URL);

/**	Set the network access manager.

	It is the responsibility of the network operations subclass to
	instantiate a QNetworkAccessManager. <b>N.B.</b>: This should be done
	on the thread where the network request operations will be done.

	<b>N.B.</b>: The Network_Status class takes ownership of the
	QNetworkAccessManager object: It will be deleted by the
	Network_Status destructor.

	@param	manager	A pointer to a QNetworkAccessManager. If the manager
		is the same is the current value nothing is done. Any existing
		network manager is deleted.
	@see network_access_manager()
*/
void network_access_manager (QNetworkAccessManager* manager);

/**	Set the current network request reply.

	A pointer to a QNetworkReply network request reply is returned from a
	network request operation on the {@link network_access_manager()
	network access manager}. It is the responsibility of the network
	operations class to set the network request reply.

	<b>N.B.</b>: The QNetwork_Reply object will be deleted by the
	Network_Status destructor. If the subclass deletes it it must
	be set to NULL.

	<b>N.B.</b>: Do not delete a QNetworkReply - by calling this method
	when the current network request reply value is non-NULL - from
	within a slot handling the {@link network_access_manager() network
	access manager} or {@link network_reply() network request reply}
	finished signals.

	@param	reply	A pointer to a QNetworkReply. If the reply is the
		same is the current value nothing is done. Any existing network
		request reply is deleted. This may be NULL to indicate that no
		network request is active.
	@see network_reply()
*/
void network_reply (QNetworkReply* reply);

/**	Set the network request status.

	@param	status	A network request status code.
	@see request_status()const
*/
void request_status (int status);

/**	Set the {@link HTTP_status_description()const HTTP status description}.

	@param	description	A QString that provides the description.
*/
void HTTP_status_description (const QString& description);

/**	Reset the network request status values based on a QNetworkReply state.

	If the {@link request_status() request_status} value is {@link
	#IN_PROGRESS} nothing is done.

	The {@link request_status() request status} is set to the {@link
	network_reply() network reply} error code. The {@link HTTP_status()
	HTTP status} is set to the network reply
	QNetworkRequest::HttpStatusCodeAttribute value, or {@link #NO_STATUS}
	if the attribute is not present in the reply. The {@link
	HTTP_status_description()const HTTP status description} is set to the
	network reply QNetworkRequest::HttpReasonPhraseAttribute value, or
	cleared if the attribute is not present in the reply. The {@link
	redirected_URL() redirected URL} is set to the network reply
	QNetworkRequest::RedirectionTargetAttribute value, or cleared if the
	attribute is not present in the reply.

	@param	network_reply	A reference to a QNetworkReply.
	@return	true if the network request status values were reset; false
		if a network request is in progress and the values are unchanged.
*/
bool reset (const QNetworkReply& network_reply);

/**	Unconditionally reset the network request status values.

	@see reset()
*/
void reset_state ();

/**	Unconditionally reset the network request status values from a
	QNetworkReply.

	@see reset(const QNetworkReply&)
*/
void reset_state (const QNetworkReply& network_reply);

/**	Get the lock that controls access to the network status
	data members.

	@return A pointer to a recursive QMutex.
*/
inline QMutex* status_lock () const
	{return Status_Lock;}

/*==============================================================================
	Utilities
*/
public:

/**	Get a normalized, host independent, form of a URL.

	If the URL {@link idaeim::is_file_URL(const QUrl&) has a file
	protocol} the path segment is coverted to use the host's native
	filesystem separators. On Unix systems the separater is the forward
	slash ('/'); on MS/Windows systems the separater is the back slash
	('\'). A URL without a protocol is coerced to have the file protocol.

	<b>N.B.</b>: If the path of a file URL is relative the path is
	prepended with the application's runtime directory location.

	For any other URL protocol the path segment of the URL is converted
	to use the network standard forward slash ('/') separators.

	@param	URL	The QUrl to be normalized.
	@return	The normalized URL.
	@see	normalized_URL(const QString&)
*/
static QUrl normalized_URL (const QUrl& URL);

/**	Get a normalized, host independent, URL for a file source.

	If the source does not represent a {@link idaeim::is_URL(const QString&) 
	recognized URL} it is taken to be a pathname for the local
	filesystem. <b>N.B.</b>: A relative pathname is prepended with the
	application's runtime directory location.

	The pathname is coverted to use the host's native filesystem
	separators. On Unix systems the separater is the forward slash ('/');
	on MS/Windows systems the separater is the back slash ('\'). A URL
	with the "file" protocol and pathname path segment is returned.

	A recognized URL is {@link normalized_URL(const QUrl&) normalized}
	as usual.

	<b>>>> WARNING <<<</b> A filesystem pathname on MS/Windows that
	starts with a drive specification - e.g. "D:" - will be interpreted
	as having a URL scheme specification that is the drive letter when a
	QUrl is constructed from such a pathname string. This method works
	around this gotcha by coercing such a source name into a file URL
	with the source name as the pathname field.

	@param	source_name	A QString with the source name to be converted to
		a normalized URL. The source name may be a filesystem pathname or
		a URL representation.
	@return	The normalized URL.
	@see	normalized_URL(const QUrl&)
*/
static QUrl normalized_URL (const QString& source_name);

/**	Get a brief description of a {@link request_status() request status}
	code.

	<b>N.B.</b>: The errorString method of the QIODevice base class of
	QNetworkReply may provide an additional description.

	@param	code	A network request status code.
	@return	A QString containing a brief, one line, description of the
		code.
*/
static QString status_description (int code);

/**	Get the pathname for the directory where the application executable
	is located.

	@return	A QString containing the application location directory
		pathname.
*/
static QString application_location ();

/*==============================================================================
	Data
*/
protected:

/**	The last URL that was requested.

	It is the responsibility of the network operations class to set
	this value. The value may be empty if no network request has been
	issued.
*/
QUrl
	Requested_URL;

/**	The URL to which the last network request was redirected by the
	server for the requested URL.

	This value is automatically set when the {@link reset(const
	QNetworkReply&) state data is reset from a QNetworkReply}. It
	will be empty if no network request has been issued, after the
	state has been {@link reset() reset}, and when no URL redirection
	occured for the last network request.
*/
mutable QUrl
	Redirected_URL;

/**	The status code of the last network request.

	The value is automatically set when the {@link reset(const
	QNetworkReply&) state data is reset from a QNetworkReply}. It may
	also be set by the network operation class. This will be a
	negative value if no request has been issued or the last request
	has not been, or will not be, completed; otherwise it is a
	positive QNetworkReply::NetworkError value. A {@link
	status_description(int) brief description for a status code} may
	be obtained.
*/
mutable int
	Request_Status;

/**	The internet HTTP status code of the last network request.

	This value is provided by the server for the network request and
	is automatically set when the {@link reset(const QNetworkReply&)
	state data is reset from a QNetworkReply}. A {@link
	HTTP_status_description()const description of the HTTP status}
	may also be available if the server provides one.
*/
mutable int
	HTTP_Status;

/**	A brief description of the HTTP_Status.

	This value is provided by the server for the network request and
	is automatically set when the {@link reset(const QNetworkReply&)
	state data is reset from a QNetworkReply}. The server is not
	required to provide a description for its HTTP_Status.
*/
mutable QString
	HTTP_Status_Description;

/**	The amount of time to wait for a network request to complete.

	The intitial value is set from the {@link default_wait_time()
	default wait time}.
*/
unsigned long
	Wait_Time;

/**	A recursive QMutex used to control access to the network status
	data members.

	The lock is constructed when a Network_Status is instantiated. It
	should not be changed. All data member accessor methods uses this
	lock, except where noted otherwise.
*/
QMutex
	*Status_Lock;

/**	The network access manager used by the network operations class.

	It is the responsibility of the network operations subclass to
	instantiate the QNetworkAccessManager that is provided. <b>N.B.</b>:
	This instantiated object may need to be associated with the QThread
	where the network request operations will be done.

	This will be NULL until instantiated.
*/
QNetworkAccessManager
	*Network_Access_Manager;

/**	The currently effective QNetworkReply handling a network request.

	It is the responsibility of the network operations subclass to
	provide and manage the Network_Reply.

	May be NULL.
*/
QNetworkReply
	*Network_Reply;

};


}	//	namespace idaeim
#endif
