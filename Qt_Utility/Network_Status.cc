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

idaeim CVS ID: Network_Status.cc,v 1.14 2014/02/18 02:23:39 castalia Exp


This file is a fork from the UA HiRISE Project source code produced by
Bradford Castalia at the HiRISE Operations Center:

HiROC CVS ID: Network_Status.cc,v 1.2 2012/10/31 11:46:42 castalia Exp

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

#include	"Network_Status.hh"

#include	"Qstrings.hh"

#include	<QCoreApplication>
#include	<QMutex>
#include	<QMutexLocker>
#include	<QNetworkAccessManager>
#include	<QNetworkReply>
#include	<QUrl>
#include	<QFileInfo>
#include	<QDir>
#include	<QObject>




namespace idaeim
{
/*=*****************************************************************************
	Network_Status
*/
/*==============================================================================
	Constants
*/
const char* const
	Network_Status::ID =
		"idaeim::Network_Status (1.14 2014/02/18 02:23:39)";


const int
	//	Local status codes:
	Network_Status::NO_STATUS			= -1,
	Network_Status::INVALID_URL			= -2,
	Network_Status::INVALID_PATHNAME	= -3,
	Network_Status::IN_PROGRESS			= -4,
	Network_Status::WAIT_TIMEOUT		= -5,
	Network_Status::FILE_WRITE_FAILURE	= -6,

	//	Aliases:
	Network_Status::SUCCESS
		= QNetworkReply::NoError,
	Network_Status::ACCESSIBLE_URL
		= QNetworkReply::NoError,
	Network_Status::URL_NOT_FOUND
		= QNetworkReply::ContentNotFoundError,
	Network_Status::URL_ACCESS_DENIED
		= QNetworkReply::ContentAccessDenied;

/*==============================================================================
	Static Data
*/
#ifndef DEFAULT_NETWORK_WAIT_TIME
#define DEFAULT_NETWORK_WAIT_TIME		10000
#endif
unsigned long
	Network_Status::Default_Wait_Time	= DEFAULT_NETWORK_WAIT_TIME;

QString
	Network_Status::Application_Location;

/*==============================================================================
	Constructor
*/
Network_Status::Network_Status ()
	:
	Requested_URL (),
	Redirected_URL (),
	Request_Status (NO_STATUS),
	HTTP_Status (NO_STATUS),
	HTTP_Status_Description (),
	Wait_Time (Default_Wait_Time),
	Status_Lock (new QMutex (QMutex::Recursive)),
	//	N.B.: Subclass provides Network_Access_Manager and Network_Reply.
	Network_Access_Manager (NULL),
	Network_Reply (NULL)
{
}


Network_Status::~Network_Status ()
{
delete Network_Reply;
delete Network_Access_Manager;
delete Status_Lock;
}

/*==============================================================================
	Accessors
*/
void
Network_Status::requested_URL
	(
	const QUrl&	URL
	)
{
QMutexLocker
	lock (Status_Lock);
Requested_URL = URL;
}


QUrl
Network_Status::requested_URL () const
{
QMutexLocker
	lock (Status_Lock);
return Requested_URL;
}


bool
Network_Status::reset ()
{
QMutexLocker
	lock (Status_Lock);
if (Request_Status != IN_PROGRESS)
	reset_state ();
return Request_Status != IN_PROGRESS;
}


bool
Network_Status::reset
	(
	const QNetworkReply&	network_reply
	)
{
QMutexLocker
	lock (Status_Lock);
if (Request_Status != IN_PROGRESS)
	reset_state (network_reply);
return Request_Status != IN_PROGRESS;
}


void
Network_Status::reset_state ()
{
QMutexLocker
	lock (Status_Lock);
Requested_URL.clear ();
Redirected_URL.clear ();
Request_Status = NO_STATUS;
HTTP_Status = NO_STATUS;
HTTP_Status_Description.clear ();
if (Network_Reply)
	{
	if (! Network_Reply->isFinished ())
		Network_Reply->abort ();
	delete Network_Reply;
	Network_Reply = NULL;
	}
}


void
Network_Status::reset_state
	(
	const QNetworkReply&	network_reply
	)
{
QMutexLocker
	lock (Status_Lock);
//	Request status.
Request_Status = request_status (network_reply);

//	Attributes.
HTTP_Status = HTTP_status (network_reply);
HTTP_Status_Description = HTTP_status_description (network_reply);
Redirected_URL = redirected_URL (network_reply);
}


int
Network_Status::request_status () const
{
QMutexLocker
	lock (Status_Lock);
if (Network_Reply)
	Request_Status = request_status (*Network_Reply);
return Request_Status;
}


int
Network_Status::request_status
	(
	const QNetworkReply&	network_reply
	)
{
int
	status_code (network_reply.error ());
if (status_code == SUCCESS)
	{
	int
		HTTP_status_code (HTTP_status (network_reply));
	if (HTTP_status_code >= 400)
		{
		if (HTTP_status_code < 500)
			{
			//	Client Error codes.
    		switch (HTTP_status_code)
				{
				//  From statusCodeFromHttp (in qhttpthreaddelegate.cpp):
				case 401:		// Authorization required
					status_code =
						QNetworkReply::AuthenticationRequiredError;
					break;
				case 403:		// Access denied
					status_code =
						URL_ACCESS_DENIED;
					break;
				case 404:		// Not Found
					status_code =
						URL_NOT_FOUND;
					break;
				case 405:		// Method Not Allowed
					status_code =
						QNetworkReply::ContentOperationNotPermittedError;
					break;
				case 407:
					status_code =
						QNetworkReply::ProxyAuthenticationRequiredError;
					break;
				case 418:		// I'm a teapot
					status_code =
						QNetworkReply::ProtocolInvalidOperationError;
					break;
				default:
					status_code =
						QNetworkReply::UnknownContentError;
				}
			}
		else
		if (HTTP_status_code < 600)
			//	Server Error codes.
			status_code = QNetworkReply::ProtocolUnknownError;
		else
			//	Invalid HTTP status code.
			status_code = NO_STATUS;
		}
	else
	if (HTTP_status_code < 100)
		//	Invalid HTTP status code.
		status_code = NO_STATUS;
	}
return status_code;
}


void
Network_Status::request_status
	(
	int		status
	)
{
QMutexLocker
	lock (Status_Lock);
Request_Status = status;
}


int
Network_Status::HTTP_status () const
{
QMutexLocker
	lock (Status_Lock);
if (HTTP_Status == NO_STATUS &&
	Network_Reply)
	HTTP_Status = HTTP_status (*Network_Reply);
return HTTP_Status;
}


int
Network_Status::HTTP_status
	(
	const QNetworkReply&	network_reply
	)
{
int
	status = NO_STATUS;
QVariant
	value (network_reply.attribute
		(QNetworkRequest::HttpStatusCodeAttribute));
if (value.isValid ())
	{
	bool
		OK;
	status = value.toInt (&OK);
	if (! OK)
		{
		status = NO_STATUS;
		}
	}
return status;
}


QString
Network_Status::HTTP_status_description () const
{
QMutexLocker
	lock (Status_Lock);
if (HTTP_Status_Description.isEmpty () &&
	Network_Reply)
	HTTP_Status_Description =
		HTTP_status_description (*Network_Reply);
return HTTP_Status_Description;
}


QString
Network_Status::HTTP_status_description
	(
	const QNetworkReply&	network_reply
	)
{
QString
	description;
QVariant
	value (network_reply.attribute
		(QNetworkRequest::HttpReasonPhraseAttribute));
if (value.isValid ())
	description = value.toString ();
return description;
}


void
Network_Status::HTTP_status_description
	(
	const QString&	description
	)
{
QMutexLocker
	lock (Status_Lock);
HTTP_Status_Description = description;
}



QUrl
Network_Status::redirected_URL () const
{
QMutexLocker
	lock (Status_Lock);
if (Redirected_URL.isEmpty () &&
	Network_Reply)
	Redirected_URL = redirected_URL (*Network_Reply);
return Redirected_URL;
}


QUrl
Network_Status::redirected_URL
	(
	const QNetworkReply&	network_reply
	)
{
QUrl
	URL;
QVariant
	value (network_reply.attribute
		(QNetworkRequest::RedirectionTargetAttribute));
if (value.isValid ())
	URL = value.toUrl ();
return URL;
}


qint64
Network_Status::content_size () const
{
if (Network_Reply)
	return content_size (*Network_Reply);
return -1;
}


qint64
Network_Status::content_size
	(
	const QNetworkReply&	network_reply
	)
{
qint64
	size = -1;
if (HTTP_status (network_reply) < 300)	//	Success threshold.
	{
	QVariant
		value (network_reply.header (QNetworkRequest::ContentLengthHeader));
	if (value.isValid ())
		{
		bool
			OK;
		size = value.toLongLong (&OK);
		if (! OK)
			{
			size = -1;
			}
		}
	}
return size;
}


unsigned long
Network_Status::wait_time () const
{
QMutexLocker
	lock (Status_Lock);
return Wait_Time;
}


void
Network_Status::wait_time
	(
	unsigned long	msecs
	)
{
QMutexLocker
	lock (Status_Lock);
if (msecs == 0)
	msecs = ULONG_MAX;
Wait_Time = msecs;
}


QNetworkAccessManager*
Network_Status::network_access_manager ()
{
QMutexLocker
	lock (Status_Lock);
return Network_Access_Manager;
}


void
Network_Status::network_access_manager
	(
	QNetworkAccessManager*	manager
	)
{
QMutexLocker
	lock (Status_Lock);
if (Network_Access_Manager != manager)
	{
	delete Network_Access_Manager;
	Network_Access_Manager = manager;
	}
}


QNetworkReply*
Network_Status::network_reply ()
{
QMutexLocker
	lock (Status_Lock);
return Network_Reply;
}


void
Network_Status::network_reply
	(
	QNetworkReply*	reply
	)
{
QMutexLocker
	lock (Status_Lock);
if (Network_Reply != reply)
	{
	delete Network_Reply;
	Network_Reply = reply;
	}
}


void
Network_Status::cancel ()
{
QMutexLocker
	lock (Status_Lock);
if (Network_Reply)
	{
	Network_Reply->abort ();
	}
}

/*==============================================================================
	Utility
*/
QUrl
Network_Status::normalized_URL
	(
	const QUrl&	URL
	)
{
QUrl
	normalized (URL);
if (is_file_URL (URL))
	{
	normalized.setScheme ("file");	//	In case the URL has no protocol.
	QString
		path (URL.path ());
	if (QFileInfo (path).isRelative ())
		normalized.setPath (application_location () + QDir::separator ()
				+ QDir::toNativeSeparators (path));
	else
		normalized.setPath (QDir::toNativeSeparators (path));
	}
else
	normalized.setPath (QDir::fromNativeSeparators (URL.path ()));
return normalized;
}


QUrl
Network_Status::normalized_URL
	(
	const QString&	source_name
	)
{
QUrl
	normalized (source_name);
if (is_URL (normalized))
	normalized = normalized_URL (normalized);
else
	{
	/*	Dealing with MS/Windows pathnames starting with a drive spec.

		When the source name starts with a drive spec (e.g. "D:") the URL
		that results from this will have a "D" protocol, which is not
		correct. However, since this protocol is not recognized by the
		is_URL function the source name is taken to be a filesystem
		pathname, which is correct.

		It is possible that the source name is for a URL with an
		unrecognized protocol, in which case the source name will be
		inorrectly treated as a filesystem pathname. Since all possible
		protocols can not be known in advance, there seems to be no way
		to avoid this ambiguous situation. The "probably is a filesystem
		pathname" guess is reasonable but not definite.
	*/
	normalized.clear ();
	normalized.setScheme ("file");
	if (QFileInfo (source_name).isRelative ())
		normalized.setPath (application_location () + QDir::separator ()
				+ QDir::toNativeSeparators (source_name));
	else
		normalized.setPath (QDir::toNativeSeparators (source_name));
	}
return normalized;
}


QString
Network_Status::status_description
	(
	int		code
	)
{
QString
	description;
switch (code)
	{
	//	Local (negative) status code values.
	case NO_STATUS:
		description =
			QObject::tr ("No status"); break;
	case INVALID_URL:
		description = 
			QObject::tr ("Invalid URL"); break;
	case INVALID_PATHNAME:
		description = 
			QObject::tr ("Invalid pathname"); break;
	case IN_PROGRESS:
		description = 
			QObject::tr ("Operation in progress"); break;
	case WAIT_TIMEOUT:
		description = 
			QObject::tr ("Wait timeout"); break;
	case FILE_WRITE_FAILURE:
		description =
			QObject::tr ("File write failed"); break;

	case QNetworkReply::NoError:	//	SUCCESS or ACCESSIBLE_URL:
		description = 
			QObject::tr ("Success"); break;
	case QNetworkReply::ConnectionRefusedError:
		description = 
			QObject::tr ("Connection refused"); break;
	case QNetworkReply::RemoteHostClosedError:
		description = 
			QObject::tr ("Server closed the connection prematurely"); break;
	case QNetworkReply::HostNotFoundError:
		description = 
			QObject::tr ("Hostname not found"); break;
	case QNetworkReply::TimeoutError:
		description = 
			QObject::tr ("Server connection timeout"); break;
	case QNetworkReply::OperationCanceledError:
		description = 
			QObject::tr ("Operation canceled"); break;
	case QNetworkReply::SslHandshakeFailedError:
		description = 
			QObject::tr ("SSL/TLS handshake failed"); break;
	#if QT_VERSION >= 0x40700
	case QNetworkReply::TemporaryNetworkFailureError:
		description = 
			QObject::tr ("Broken connection for access point roaming"); break;
	#endif
	case QNetworkReply::ProxyConnectionRefusedError:
		description = 
			QObject::tr ("Proxy server connection refused"); break;
	case QNetworkReply::ProxyConnectionClosedError:
		description = 
			QObject::tr ("Proxy server connection closed prematurely"); break;
	case QNetworkReply::ProxyNotFoundError:
		description = 
			QObject::tr ("Proxy server hostname not found"); break;
	case QNetworkReply::ProxyTimeoutError:
		description = 
			QObject::tr ("Proxy server connection timeout"); break;
	case QNetworkReply::ProxyAuthenticationRequiredError:
		description = 
			QObject::tr ("Proxy server authentication failed"); break;
	case QNetworkReply::ContentAccessDenied:	//	URL_ACCESS_DENIED
		description = 
			QObject::tr ("Content access denied"); break;
	case QNetworkReply::ContentOperationNotPermittedError:
		description = 
			QObject::tr ("Operation not permitted"); break;
	case QNetworkReply::ContentNotFoundError:	//	URL_NOT_FOUND
		description = 
			QObject::tr ("Content not found"); break;
	case QNetworkReply::AuthenticationRequiredError:
		description = 
			QObject::tr ("Server authentication failed"); break;
	case QNetworkReply::ContentReSendError:
		description = 
			QObject::tr ("Request resent failed"); break;
	case QNetworkReply::ProtocolUnknownError:
		description = 
			QObject::tr ("Unknown protocol"); break;
	case QNetworkReply::ProtocolInvalidOperationError:
		description = 
			QObject::tr ("Invalid operation for protocol"); break;
	case QNetworkReply::UnknownNetworkError:
		description = 
			QObject::tr ("Unknown network error"); break;
	case QNetworkReply::UnknownProxyError:
		description = 
			QObject::tr ("Unknown proxy error"); break;
	case QNetworkReply::UnknownContentError:
		description = 
			QObject::tr ("Unknown content error"); break;
	case QNetworkReply::ProtocolFailure:
		description = 
			QObject::tr ("Protocol failure"); break;
	default:
		description = 
			QObject::tr ("Unknown status code (%1)").arg (code); break;
	}
return description;
}


QString
Network_Status::application_location ()
{
if (Application_Location.isEmpty ())
	Application_Location =
		QDir::toNativeSeparators (QCoreApplication::applicationDirPath ());
return Application_Location;
}


}	//	namespace idaeim
