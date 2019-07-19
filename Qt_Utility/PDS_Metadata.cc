/*	PDS_Metadata

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

idaeim CVS ID: PDS_Metadata.cc,v 1.15 2014/01/18 03:42:03 castalia Exp


This file is a fork from the UA HiRISE Project source code produced by
Bradford Castalia at the HiRISE Operations Center:

HiROC CVS ID: PDS_Metadata.cc,v 1.1 2012/10/04 02:11:11 castalia Exp

Copyright (C) 2011-2012  Arizona Board of Regents on behalf of the
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

#include	"PDS_Metadata.hh"

#include	"Qistream.hh"
#include	"Qstrings.hh"

#include	"PVL/Parameter.hh"
#include	"PVL/Parser.hh"
#include	"PVL/Lister.hh"
using namespace idaeim::PVL;

#include	<QNetworkAccessManager>
#include	<QNetworkReply>
#include	<QUrl>
#include	<QFile>
#include	<QDir>
#include	<QMutex>
#include	<QMutexLocker>

#include	<string>
using std::string;
#include	<sstream>
using std::ostringstream;
#include	<iomanip>
using std::endl;




/*	!!! December, 2013 on Apple Mac OS X v10.8.5

	std::string references sent to an ostringstream do not compile!

	PDS_Metadata.cc: In instantiation of 'std::basic_ostream<_CharT,
		_Traits>& std::operator<<(std::basic_ostream<_CharT, _Traits>&,
		const std::basic_string<_CharT, _Traits, _Alloc>&) [with _CharT = char,
		_Traits = std::char_traits<char>, _Alloc = std::allocator<char>]':
	error: explicit instantiation of 'std::basic_ostream<_CharT,
		_Traits>& std::operator<<(std::basic_ostream<_CharT, _Traits>&,
		const std::basic_string<_CharT, _Traits, _Alloc>&) [with _CharT = char,
		_Traits = std::char_traits<char>, _Alloc = std::allocator<char>]'
		but no definition available

	Adding .c_str() to the string reference works around the problem.

	A note on Stack Overflow (and elsewhere) indicates that the problem is
	a version mismatch between the compiler and the standard libraries which
	can be avoided by adding these definitions to the .pro file:

	QMAKE_CFLAGS_X86_64 += -mmacosx-version-min=10.7
	QMAKE_CXXFLAGS_X86_64 = $$QMAKE_CFLAGS_X86_64

	This gotcha does not occur on other platforms.
*/


namespace idaeim
{
/*==============================================================================
	Constants
*/
const char* const
	PDS_Metadata::ID =
		"idaeim::PDS_Metadata (1.15 2014/01/18 03:42:03)";


#ifndef PDS_METADATA_GROUP_NAME
#define PDS_METADATA_GROUP_NAME			"PDS"
#endif
const char* const
	PDS_Metadata::PDS_METADATA_GROUP	= PDS_METADATA_GROUP_NAME;

const char
	* const PDS_Metadata::PDS_ID_PARAMETER_NAME
		= "PDS_VERSION_ID",
	* const PDS_Metadata::RECORD_TYPE_PARAMETER_NAME
		= "RECORD_TYPE",
	* const PDS_Metadata::BYTE_RECORD_TYPE
			= "UNDEFINED",
	* const PDS_Metadata::FIXED_LENGTH_RECORD_TYPE
			= "FIXED_LENGTH",
	* const PDS_Metadata::LABEL_RECORDS_PARAMETER_NAME
		= "LABEL_RECORDS",
	* const PDS_Metadata::RECORD_BYTES_PARAMETER_NAME
		= "RECORD_BYTES",
	* const PDS_Metadata::FILE_RECORDS_PARAMETER_NAME
		= "FILE_RECORDS",
	* const PDS_Metadata::INTERCHANGE_FORMAT_PARAMETER_NAME
		= "INTERCHANGE_FORMAT",
	* const PDS_Metadata::IMAGE_DATA_BLOCK_NAME
		= "IMAGE",
	* const PDS_Metadata::BYTES_UNITS
		= "BYTES";

const char
	PDS_Metadata::RECORD_POINTER_PARAMETER_MARKER
		= '^';

/*==============================================================================
	Constructor
*/
PDS_Metadata::PDS_Metadata
	(
	QObject*	parent
	)
	:	QThread (parent),
		Network_Status (),
		Metadata (NULL),
		Source_File (NULL)
{
setObjectName ("PDS_Metadata");
}


PDS_Metadata::~PDS_Metadata ()
{
cancel ();
wait (Default_Wait_Time);
delete Metadata;
delete Source_File;
}

/*==============================================================================
	Accessors
*/
Aggregate*
PDS_Metadata::metadata () const
{
QMutexLocker
	lock (Status_Lock);
return Metadata;
}


QString
PDS_Metadata::metadata_parse_failure () const
{
QMutexLocker
	lock (Status_Lock);
return Metadata_Parse_Failure;
}


bool
PDS_Metadata::reset ()
{
QMutexLocker
	lock (Status_Lock);
bool
	was_reset = Network_Status::reset ();
if (was_reset &&
	Metadata)
	{
	delete Metadata;
	Metadata = NULL;
	}
return was_reset;
}


bool
PDS_Metadata::fetching () const
{return isRunning () || request_status () == IN_PROGRESS;}

/*==============================================================================
	Fetch the Metadata
*/
bool
PDS_Metadata::fetch
	(
	const QUrl&	URL,
	bool		synchronous
	)
{
int
	result = IN_PROGRESS;
if (! isRunning () &&
	! URL.path ().isEmpty () &&
	! URL.isEmpty ())
	{
	Status_Lock->lock ();

	if (Request_Status == IN_PROGRESS)
		{
		Status_Lock->unlock ();
		return false;
		}

	//	Reset the fetch state.

	reset_state ();
	requested_URL (URL);
	request_status (result);	//	IN_PROGRESS

	if (Metadata)
		{
		delete Metadata;
		Metadata = NULL;
		}
	Metadata_Parse_Failure.clear ();

	Status_Lock->unlock ();

	if (is_file_URL (URL))
		{
		//	Local file fetch: prepare the Source_File.
		Source_File = new QFile (QDir::toNativeSeparators (URL.path ()));
		if (! Source_File->exists ())
			{
			result = URL_NOT_FOUND;
			HTTP_Status_Description = tr ("The source file does not exist.");
			}
		else
		if (! QFileInfo (*Source_File).isFile ())
			{
			result = URL_ACCESS_DENIED;
			HTTP_Status_Description =
				tr ("The source file is not a regular file.");
			}
		else
		if (! Source_File->open (QIODevice::ReadOnly))
			{
			result = URL_ACCESS_DENIED;
			HTTP_Status_Description = Source_File->errorString ();
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

			result = request_status ();
			}
		else
			result = SUCCESS;
		}
	else
		{
		//	Setup failure.

		delete Source_File;
		Source_File = NULL;

		//	Set the request status.
		request_status (result);

		//	>>> SIGNAL <<<
		emit fetched (NULL);
		}
	}

return result == SUCCESS;
}


void
PDS_Metadata::run ()
{
Status_Lock->lock ();

QIODevice*
	source;
if (Source_File)
	source = Source_File;
else
	{
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
	source = Network_Reply;
	}

Status_Lock->unlock ();	//	Unlock access during data fetch and parse.

Qistream
	qistream (source);
Qistreambuf
	*stream_buffer = dynamic_cast<Qistreambuf*>(qistream.rdbuf ());
stream_buffer->wait_time (Wait_Time);
Parser
	parser (qistream);

//	Assemble the Metadata Aggregate.
Aggregate
	*metadata (NULL);
try {metadata = new Aggregate (parser, Parser::CONTAINER_NAME);}
catch (idaeim::Exception except)
	{
	Metadata_Parse_Failure = except.message ().c_str ();
	}

Status_Lock->lock ();

if (Network_Reply)
	{
	//	Reset the request status.
	reset_state (*Network_Reply);

	if (request_status () == SUCCESS &&
		! metadata)
		request_status (NO_STATUS);

	//	Halt the network request, but leave the reply for user inspection.
	cancel ();
	}
else
	request_status (metadata ? SUCCESS : NO_STATUS);


if (Source_File)
	{
	delete Source_File;
	Source_File = NULL;
	}

if (stream_buffer->timeout ())
	request_status (WAIT_TIMEOUT);

if (metadata)
	{
	if (request_status () == SUCCESS)
		Metadata = metadata;
	else
		{
		//	Fetch failure. Invalid metadata.
		delete metadata;
		metadata = NULL;
		}
	}

Status_Lock->unlock ();

//	>>> SIGNAL <<<
emit fetched (metadata);

}

/*==============================================================================
	Utilities
*/
double
PDS_Metadata::numeric_value
	(
	const Aggregate&				parameters,
	const std::string&				pathname,
	bool							case_sensitive,
	int								skip
	)
{
Parameter
	*parameter (parameters.find
		(pathname, case_sensitive, skip, Parameter::ASSIGNMENT));
if (! parameter)
	{
	ostringstream
		message;
	message 
		<< "Can't find the numeric value assignment parameter "
			<< pathname.c_str ();
	throw idaeim::Invalid_Argument (message.str (), ID);
	}
return PDS_Metadata::numeric_value (*parameter);
}


double
PDS_Metadata::numeric_value
	(
	const std::string&				pathname,
	bool							case_sensitive,
	int								skip
	) const
{
Aggregate
	*parameters (metadata ());
if (! parameters)
	{
	ostringstream
		message;
	message 
		<< "No metadata for numeric value parameter "
			<< pathname.c_str ();
	throw idaeim::Invalid_Argument (message.str (), ID);
	}
return numeric_value (*parameters, pathname, case_sensitive, skip);
}


double
PDS_Metadata::numeric_value
	(
	const Parameter&	parameter
	)
{
if (! parameter.value ().is_Numeric ())
	{
	ostringstream
		message;
	message
		<< "Numeric value expected for parameter "
			<< parameter.pathname ().c_str () << endl
		<< "but " << parameter.value ().type_name ().c_str ()
			<< " value found.";
	throw Invalid_Value (message.str (), -1, PDS_Metadata::ID);
	}
return static_cast<double>(parameter.value ());
}


std::string
PDS_Metadata::string_value
	(
	const Aggregate&				parameters,
	const std::string&				pathname,
	bool							case_sensitive,
	int								skip
	)
{
Parameter
	*parameter (parameters.find
		(pathname, case_sensitive, skip, Parameter::ASSIGNMENT));
if (! parameter)
	{
	ostringstream
		message;
	message
		<< "Can't find the string value assignment parameter "
			<< pathname.c_str ();
	throw idaeim::Invalid_Argument (message.str (), ID);
	}
return PDS_Metadata::string_value (*parameter);
}


std::string
PDS_Metadata::string_value
	(
	const std::string&				pathname,
	bool							case_sensitive,
	int								skip
	) const
{
Aggregate
	*parameters (metadata ());
if (! parameters)
	{
	ostringstream
		message;
	message 
		<< "No metadata for string value parameter "
			<< pathname.c_str ();
	throw idaeim::Invalid_Argument (message.str (), ID);
	}
return string_value (*parameters, pathname, case_sensitive, skip);
}


std::string
PDS_Metadata::string_value
	(
	const Parameter&	parameter
	)
{
if (parameter.is_Aggregate ())
	{
	ostringstream
		message;
	message
		<< "A string value can not be obtained for parameter "
			<< parameter.pathname ().c_str () << endl
		<< "because it an Aggregate (" << parameter.type_name ().c_str ()
			<< ").";
	throw Invalid_Value (message.str (), -1, PDS_Metadata::ID);
	}
if (parameter.value ().is_Array ())
	{
	ostringstream
		message;
	message
		<< "A string value can not be obtained for parameter "
			<< parameter.pathname ().c_str () << endl
		<< "because it has an Array ("
			<< parameter.value ().type_name ().c_str () << ") value.";
	throw Invalid_Value (message.str (), -1, PDS_Metadata::ID);
	}
return static_cast<std::string>(parameter.value ()).c_str ();
}


std::string
PDS_Metadata::absolute_pathname
	(
	const std::string&	pathname
	)
{
char
	delimiter = Parameter::path_delimiter ();
if (pathname.empty () ||
	pathname[0] == delimiter)
	return pathname;
std::string
	absolute_path (1, delimiter);
return absolute_path += pathname;
}


Lister*
PDS_Metadata::metadata_lister ()
{
Lister
	*lister (new Lister);
(*lister)
	.strict					(true)			
	.begin_aggregates		(false)
	.uppercase_aggregates	(true)
	.name_end_aggregates	(true)
	.assign_align			(true)
	.array_indenting		(false)
	.use_statement_delimiter(false)
	.single_line_comments	(true);
return lister;
}


}	//	namespace idaeim
