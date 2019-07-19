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

idaeim CVS ID: PDS_Metadata.hh,v 1.10 2014/01/23 00:25:59 castalia Exp


This file is a fork from the UA HiRISE Project source code produced by
Bradford Castalia at the HiRISE Operations Center:

HiROC CVS ID: PDS_Metadata.hh,v 1.2 2012/10/23 07:45:24 castalia Exp

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

#ifndef PDS_METADATA_HH
#define PDS_METADATA_HH

#include	"Network_Status.hh"

#include	<QThread>

//	Forward references.
class QUrl;
class QFile;

#include	"PVL/Parameter.hh"
namespace idaeim {
namespace PVL {
class Lister;
}}
using idaeim::PVL::Parameter;
using idaeim::PVL::Aggregate;
using idaeim::PVL::Lister;


namespace idaeim
{
/**	A <i>PDS_Metadata</i> fetches PDS metadata from a URL source.

	The URL that is fetched may be an HTTP reference or a file reference
	including a plain local filesystem pathname.

	The URL fetch may be done asynchronously or synchrously with a
	maximum wait time for it to complete, and a fetch in progress may be
	canceled. A signal is emitted whenever a fetch has been completed.

	PDS_Metadata is a Network_Status subclass that provides status
	information about network fetches.

	The data that is fetched is parsed to produce PDS metadata parameters
	in the form of a {@link idaeim::PVL::Aggregate PVL Aggregate}.

	PDS_Metadata fetching is thread safe.

	<b>N.B.</b>: A QApplication event loop is not required to use objects
	of this class, but a QCoreApplication (or QApplication) must be
	present for the local event loop that is used to function.

	A set of utility functions are provided to find parameters in an
	Aggregate, and get and set parameter values.

	@author		Bradford Castalia, idaeim
	@version	1.10
*/
class PDS_Metadata
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


//!	Name of the PDS metadata parameters group.
static const char* const
	PDS_METADATA_GROUP;


//	Structure definition parameters:

//! PDS ID.
static const char* const
	PDS_ID_PARAMETER_NAME;

//!	Type of metadata records.
static const char* const
	RECORD_TYPE_PARAMETER_NAME;

//!	The RECORD_TYPE_PARAMETER_NAME used to define byte count based records.
static const char* const
	BYTE_RECORD_TYPE;

//!	The RECORD_TYPE_PARAMETER_NAME used to define fixed length records.
static const char* const
	FIXED_LENGTH_RECORD_TYPE;

//!	Number of records in the metadata label.
static const char* const
	LABEL_RECORDS_PARAMETER_NAME;

//!	Number of bytes per record.
static const char* const
	RECORD_BYTES_PARAMETER_NAME;

//!	Number of records in the file.
static const char* const
	FILE_RECORDS_PARAMETER_NAME;

//!	Data format.
static const char* const
	INTERCHANGE_FORMAT_PARAMETER_NAME;

//!	Name of the Image data block parameter group.
static const char* const
	IMAGE_DATA_BLOCK_NAME;

//!	Units name for storage measured in bytes.
static const char* const
	BYTES_UNITS;

/**	Record pointer parameter prefix.

	When this character is the first character of a parameter name it is
	marked as a record pointer parameter for a data block. An associated
	group of parameters having the same name, but without the initial
	marker character, is expected to be present that describes the
	content of the data block.
*/
static const char
	RECORD_POINTER_PARAMETER_MARKER;

/*==============================================================================
	Constructor
*/
/**	Constructs a PDS_Metadata object.

	@param	parent	A QObject that owns the constructed object and will
		destroy it when the parent is destroyed.
*/
explicit PDS_Metadata (QObject* parent = NULL);

//!	Destroys the PDS_Metadata.
virtual ~PDS_Metadata ();

/*==============================================================================
	Accessors
*/
/**	Get the most recently {@link fetch(const QUrl&, bool) fetched}
	PDS metadata.

	@return	A pointer to an Aggregate containing the PDS metadata
		parameters. This will be NULL if no metadata is available.
*/
Aggregate* metadata () const;

/**	Get a description of a metadata parsing failure.
*/
QString metadata_parse_failure () const;

/**	Test if a {@link fetch(const QUrl&, bool) URL fetch} is currently in
	progress.

	@return	true if a fetch is in progress; false otherwise.
*/
bool fetching () const;

/**	Reset this PDS_Metadata.

	The {@link metadata() metadata parameters}, if any, are deleted and the
	metadata is set to NULL. The network status values are also {@link
	Network_Status::reset() reset}.
*/
bool reset ();

/*==============================================================================
	Utilities
*/
/**	Selects the class of parameter to {@link
	find_parameter(const Aggregate&, const std::string&, bool, int, Parameter_Class_Selection)
	find} or {@link
	remove_parameter(const Aggregate&, const std::string&, bool, int, Parameter_Class_Selection)
	remove}.
*/
enum Parameter_Class_Selection
	{
	AGGREGATE_PARAMETER	 = -1,
	ANY_PARAMETER        =  0,
	ASSIGNMENT_PARAMETER =  1
	};

//!	Flag values for case sensitivity.
enum
	{
	CASE_INSENSITIVE,
	CASE_SENSITIVE
	};

/**	Find a named Parameter within a Parameter Aggregate.

	The Parameter to be found may be specified by a simple name, a
	relative pathname or an absolute pathname. A simple name is the name
	of a Parameter itself. A relative pathname is a simple name preceeded
	by the pathname of an Aggregate Parameter that must contain the
	Parameter with the simple name. An {@link
	absolute_pathname(const std::string&) absolute pathname} is a
	pathname that begins with a Parameter path delimiter character ('/'
	by default); i.e. it is a fully qualified pathname beginning at the
	root of the Aggregate hierarchy, compared to a relative pathname that
	may begin at any Aggregate in the hierarchy.

	Since it is not necessary that the pathname to each Parameter be
	unique, the number of matching pathnames to skip before selecting a
	Parameter may need to be specified to get the desired Parameter.
	Also, a specific type of Parameter - an Aggregate of Parameters or an
	Assignment of Values - may be specified. When skipping parameters,
	only the specified type are counted.

	@param	parameters	The Aggregate within which to search.
	@param	pathname	The pathname for the parameter to be found.
	@param	case_sensitive	If true, the name search is case sensitive;
		case insensitive otherwise.
	@param	skip		The number of parameters that match the
		name to skip before selecting a matching parameter.
	@param	parameter_class		A PDS::Parameter_Class_Selection: If
		PDS_Data::AGGREGATE_PARAMETER, only Aggregate parameters will be
		selected; if PDS_Data::ASSIGNMENT_PARAMETER, only Assignment
		parameters will be selected; otherwise any type of parameter
		(PDS_Data::ANY_PARAMETER) is acceptable.
	@return	A pointer to the matching parameter, or NULL if it could not
		be found.
	@see	find_parameter(const Aggregate&, const QString&,
		bool, int, Parameter_Class_Selection)
*/
inline static Parameter*
find_parameter
	(
	const Aggregate&				parameters,
	const std::string&				pathname,
	bool							case_sensitive = false,
	int								skip = 0,
	Parameter_Class_Selection		parameter_class = ANY_PARAMETER
	)
{
return parameters.find (pathname, case_sensitive, skip,
	static_cast<Parameter::Type>(
		((parameter_class == ANY_PARAMETER) ?
			Parameter::ASSIGNMENT |
			Parameter::AGGREGATE :
		((parameter_class == ASSIGNMENT_PARAMETER) ?
		  	Parameter::ASSIGNMENT :
			Parameter::AGGREGATE))));
}

/**	Find a named Parameter within a Parameter Aggregate.

	This is a convenience function that overloads the equivalent function,
	but for use with a char* pathname.

	@see	find_parameter(const Aggregate&, const std::string&,
		bool, int, Parameter_Class_Selection)
*/
inline static Parameter*
find_parameter
	(
	const Aggregate&				parameters,
	const char* const				pathname,
	bool							case_sensitive = false,
	int								skip = 0,
	Parameter_Class_Selection		parameter_class = ANY_PARAMETER
	)
{return PDS_Metadata::find_parameter (parameters, std::string (pathname),
	case_sensitive, skip, parameter_class);}

/**	Find a named Parameter within a Parameter Aggregate.

	This is a convenience function that overloads the equivalent function,
	but for use with a QString pathname.

	@see	find_parameter(const Aggregate&, const std::string&,
		bool, int, Parameter_Class_Selection)
*/
inline static Parameter*
find_parameter
	(
	const Aggregate&				parameters,
	const QString&					pathname,
	bool							case_sensitive = false,
	int								skip = 0,
	Parameter_Class_Selection		parameter_class = ANY_PARAMETER
	)
{return PDS_Metadata::find_parameter (parameters, pathname.toStdString (),
	case_sensitive, skip, parameter_class);}

/**	Find a named Parameter within the metadata parameters.

	This is a convenience method that overloads the equivalent function.

	@see	find_parameter(const Aggregate&, const std::string&,
		bool, int, Parameter_Class_Selection)
*/
inline Parameter*
find_parameter
	(
	const std::string&				pathname,
	bool							case_sensitive = false,
	int								skip = 0,
	Parameter_Class_Selection		parameter_class = ANY_PARAMETER
	) const
{return metadata () ?
	find_parameter (*metadata (), pathname,
		case_sensitive, skip, parameter_class) :
	NULL;}

/**	Find a named Parameter within the metadata parameters.

	This is a convenience method that overloads the equivalent function.

	@see	find_parameter(const Aggregate&, const std::string&,
		bool, int, Parameter_Class_Selection)
*/
inline Parameter*
find_parameter
	(
	const char* const				pathname,
	bool							case_sensitive = false,
	int								skip = 0,
	Parameter_Class_Selection		parameter_class = ANY_PARAMETER
	) const
{return metadata () ?
	find_parameter (*metadata (), std::string (pathname),
		case_sensitive, skip, parameter_class) :
	NULL;}

/**	Find a named Parameter within the metadata parameters.

	This is a convenience method that overloads the equivalent method,
	but for use with a QString pathname.

	@see	find_parameter(const std::string&, bool, int,
		Parameter_Class_Selection)
*/
inline Parameter*
find_parameter
	(
	const QString&					pathname,
	bool							case_sensitive = false,
	int								skip = 0,
	Parameter_Class_Selection		parameter_class = ANY_PARAMETER
	) const
{return find_parameter (pathname.toStdString (),
	case_sensitive, skip, parameter_class);}


/**	Removes a named Parameter within a Parameter Aggregate.

	@param	parameters	The Aggregate within which to search.
	@param	pathname	The pathname for the parameter to be found.
	@param	case_sensitive	If true, the name search is case sensitive;
		case insensitive otherwise.
	@param	skip	The number of parameters that match the name to skip
		before selecting a matching parameter.
	@param	parameter_class		A Parameter_Class_Selection: If
		AGGREGATE_PARAMETER, only Aggregate parameters will be selected;
		if ASSIGNMENT_PARAMETER, only Assignment parameters will be
		selected; otherwise any type of parameter (ANY_PARAMETER) is
		acceptable.
	@return	A pointer to the parameter that was removed, or NULL if it
		could not be found.
	@see	remove_parameter(const Aggregate&, const QString&,
		bool, int, Parameter_Class_Selection)
	@see	find_parameter(const Aggregate&, const std::string&,
		bool, int, Parameter_Class_Selection)
*/
inline static Parameter*
remove_parameter
	(
	const Aggregate&				parameters,
	const std::string&				pathname,
	bool							case_sensitive = false,
	int								skip = 0,
	Parameter_Class_Selection		parameter_class = ANY_PARAMETER
	)
{
Parameter
	*parameter (PDS_Metadata::find_parameter
		(parameters, pathname, case_sensitive, skip, parameter_class));
if (parameter)
	parameter = parameter->parent ()->remove (parameter);
return parameter;
}

/**	Removes a named Parameter within a Parameter Aggregate.

	This is a convenience function that overloads the equivalent function,
	but for use with a char* pathname.

	@see	remove_parameter(const Aggregate&, const std::string&,
		bool, int, Parameter_Class_Selection)
*/
inline static Parameter*
remove_parameter
	(
	const Aggregate&				parameters,
	const char* const				pathname,
	bool							case_sensitive = false,
	int								skip = 0,
	Parameter_Class_Selection		parameter_class = ANY_PARAMETER
	)
{return PDS_Metadata::remove_parameter (parameters, std::string (pathname),
	case_sensitive, skip, parameter_class);}

/**	Removes a named Parameter within a Parameter Aggregate.

	This is a convenience function that overloads the equivalent function,
	but for use with a QString pathname.

	@see	remove_parameter(const Aggregate&, const std::string&,
		bool, int, Parameter_Class_Selection)
*/
inline static Parameter*
remove_parameter
	(
	const Aggregate&				parameters,
	const QString&					pathname,
	bool							case_sensitive = false,
	int								skip = 0,
	Parameter_Class_Selection		parameter_class = ANY_PARAMETER
	)
{return PDS_Metadata::remove_parameter (parameters, pathname.toStdString (),
	case_sensitive, skip, parameter_class);}

/**	Removes a named Parameter within the metadata parameters.

	This is a convenience method that overloads the equivalent function.

	@see	remove_parameter(const Aggregate&, const std::string&,
		bool, int, Parameter_Class_Selection)
*/
inline Parameter*
remove_parameter
	(
	const std::string&				pathname,
	bool							case_sensitive = false,
	int								skip = 0,
	Parameter_Class_Selection		parameter_class = ANY_PARAMETER
	) const
{return metadata () ?
	remove_parameter (*metadata (), pathname,
		case_sensitive, skip, parameter_class) :
	NULL;}

/**	Removes a named Parameter within the metadata parameters.

	This is a convenience method that overloads the equivalent method,
	but for use with a char* pathname.

	@see	remove_parameter(const std::string&, bool, int,
		Parameter_Class_Selection)
*/
inline Parameter*
remove_parameter
	(
	const char* const				pathname,
	bool							case_sensitive = false,
	int								skip = 0,
	Parameter_Class_Selection		parameter_class = ANY_PARAMETER
	) const
{return remove_parameter (std::string (pathname),
	case_sensitive, skip, parameter_class);}

/**	Removes a named Parameter within the metadata parameters.

	This is a convenience method that overloads the equivalent method,
	but for use with a QString pathname.

	@see	remove_parameter(const std::string&, bool, int,
		Parameter_Class_Selection)
*/
inline Parameter*
remove_parameter
	(
	const QString&					pathname,
	bool							case_sensitive = false,
	int								skip = 0,
	Parameter_Class_Selection		parameter_class = ANY_PARAMETER
	) const
{return remove_parameter (pathname.toStdString (),
	case_sensitive, skip, parameter_class);}


/**	Get the numeric value of a named parameter within a Parameter Aggregate.

	@param	parameters	The Aggregate within which to search.
	@param	pathname	The pathname for the parameter to be found.
	@param	case_sensitive	If true, the name search is case sensitive;
		case insensitive otherwise.
	@param	skip	The number of parameters that match the pathname to
		skip before selecting the parameter to modify.
	@return	The value of the parameter as a double.
	@throws	idaeim::Invalid_Argument	If an assignment parameter can
		not be found at the pathname.
	@throws	idaeim::PVL::Invalid_Value	If the Value is not Numeric.
	@see	numeric_value(const Aggregate&, const QString&,
		bool, int, Parameter_Class_Selection)
	@see	find_parameter(const Aggregate&, const std::string&,
		bool, int, Parameter_Class_Selection)
*/
static double
numeric_value
	(
	const Aggregate&				parameters,
	const std::string&				pathname,
	bool							case_sensitive = false,
	int								skip = 0
	);

/**	Get the numeric value of a named parameter within a Parameter Aggregate.

	This is a convenience function that overloads the equivalent function,
	but for use with a char* pathname.

	@see	find_parameter(const Aggregate&, const std::string&,
		bool, int, Parameter_Class_Selection)
*/
inline static double
numeric_value
	(
	const Aggregate&				parameters,
	const char* const				pathname,
	bool							case_sensitive = false,
	int								skip = 0
	)
{return PDS_Metadata::numeric_value (parameters, std::string (pathname),
	case_sensitive, skip);}

/**	Get the numeric value of a named parameter within a Parameter Aggregate.

	This is a convenience function that overloads the equivalent function,
	but for use with a QString pathname.

	@see	find_parameter(const Aggregate&, const std::string&,
		bool, int, Parameter_Class_Selection)
*/
inline static double
numeric_value
	(
	const Aggregate&				parameters,
	const QString&					pathname,
	bool							case_sensitive = false,
	int								skip = 0
	)
{return PDS_Metadata::numeric_value (parameters, pathname.toStdString (),
	case_sensitive, skip);}

/**	Get the numeric value of a named parameter within the metadata parameters.

	This is a convenience method that overloads the equivalent function.

	@see	numeric_value(const Aggregate&, const std::string&,
		bool, int, Parameter_Class_Selection)
*/
double
numeric_value
	(
	const std::string&				pathname,
	bool							case_sensitive = false,
	int								skip = 0
	) const;

/**	Get the numeric value of a named parameter within the metadata parameters.

	This is a convenience method that overloads the equivalent method,
	but for use with a char* pathname.

	@see	numeric_value(const std::string&, bool, int,
		Parameter_Class_Selection)
*/
double
numeric_value
	(
	const char* const				pathname,
	bool							case_sensitive = false,
	int								skip = 0
	) const
{return numeric_value (std::string (pathname), case_sensitive, skip);}

/**	Get the numeric value of a named parameter within the metadata parameters.

	This is a convenience method that overloads the equivalent method,
	but for use with a QString pathname.

	@see	numeric_value(const std::string&, bool, int,
		Parameter_Class_Selection)
*/
double
numeric_value
	(
	const QString&					pathname,
	bool							case_sensitive = false,
	int								skip = 0
	) const
{return numeric_value (pathname.toStdString (), case_sensitive, skip);}

/**	Get the numeric value of a parameter.

	@param	parameter	The parameter from which to obtain a numeric value.
	@return	The value of the parameter as a double.
	@throws	idaeim::PVL::Invalid_Value	If the Value is not Numeric.
*/
static double numeric_value (const Parameter& parameter);


/**	Get the string value of a named parameter within a Parameter Aggregate.

	@param	parameters	The Aggregate within which to search.
	@param	pathname	The pathname for the parameter to be found.
	@param	case_sensitive	If true, the name search is case sensitive;
		case insensitive otherwise.
	@param	skip	The number of parameters that match the pathname to
		skip before selecting the parameter to modify.
	@return	The string value of the parameter.
	@throws	idaeim::Invalid_Argument	If an assignment parameter can
		not be found at the pathname.
	@throws	idaeim::PVL::Invalid_Value	If the Value is not a String.
	@see	string_value(const Aggregate&, const QString&,
		bool, int)
	@see	find_parameter(const Aggregate&, const std::string&,
		bool, int, Parameter_Class_Selection)
*/
static std::string
string_value
	(
	const Aggregate&				parameters,
	const std::string&				pathname,
	bool							case_sensitive = false,
	int								skip = 0
	);

/**	Get the string value of a named parameter within a Parameter Aggregate.

	This is a convenience function that overloads the equivalent function,
	but for use with a char* pathname.

	@see	string_value(const Aggregate&, const std::string&,
		bool, int)
*/
inline static std::string
string_value
	(
	const Aggregate&				parameters,
	const char* const				pathname,
	bool							case_sensitive = false,
	int								skip = 0
	)
{return PDS_Metadata::string_value (parameters, std::string (pathname),
	case_sensitive, skip);}

/**	Get the string value of a named parameter within a Parameter Aggregate.

	This is a convenience function that overloads the equivalent function,
	but for use with a QString pathname.

	@see	string_value(const Aggregate&, const std::string&,
		bool, int)
*/
inline static QString
string_value
	(
	const Aggregate&				parameters,
	const QString&					pathname,
	bool							case_sensitive = false,
	int								skip = 0
	)
{return QString::fromStdString (PDS_Metadata::string_value
	(parameters, pathname.toStdString (), case_sensitive, skip));}

/**	Get the string value of a named parameter within the metadata parameters.

	This is a convenience method that overloads the equivalent function.

	@see	find_parameter(const Aggregate&, const std::string&,
		bool, int, Parameter_Class_Selection)
*/
std::string
string_value
	(
	const std::string&				pathname,
	bool							case_sensitive = false,
	int								skip = 0
	) const;

/**	Get the string value of a named parameter within the metadata parameters.

	This is a convenience method that overloads the equivalent method,
	but for use with a char* pathname.

	@see	string_value(const Aggregate&, const std::string&,
		bool, int)
*/
inline std::string
string_value
	(
	const char* const				pathname,
	bool							case_sensitive = false,
	int								skip = 0
	)
{return string_value (std::string (pathname), case_sensitive, skip);}

/**	Get the string value of a named parameter within the metadata parameters.

	This is a convenience method that overloads the equivalent method,
	but for use with a QString pathname.

	@see	string_value(const Aggregate&, const std::string&,
		bool, int)
*/
inline QString
string_value
	(
	const QString&					pathname,
	bool							case_sensitive = false,
	int								skip = 0
	)
{return QString::fromStdString (string_value
	(pathname.toStdString (), case_sensitive, skip));}

/**	Get the string value of a parameter.

	@param	parameter	The parameter from which to obtain a string value.
	@return	The value of the parameter as a string.
*/
static std::string string_value (const Parameter& parameter);

/**	Get the string value of a parameter.

	This is a convenience function that overloads the string_value function,
	but returns a QString value.

	@param	parameter	The parameter from which to obtain a string value.
	@return	The value of the parameter as a string.
	@see	string_value(const Parameter&)
*/
inline static QString QString_value (const Parameter& parameter)
{return QString::fromStdString (PDS_Metadata::string_value (parameter));}

/**	Set the value of a named parameter within a Parameter Aggregate.

	Only Assignment Parameters with a Value that is not an Array will be
	modified. However, the search for a matching name does not make this
	distinction.

	@param	parameters	The Aggregate within which to search.
	@param	pathname	The pathname for the parameter to be found.
	@param	value	The value to assign to the parameter.
	@param	case_sensitive	If true, the name search is case sensitive;
		case insensitive otherwise.
	@param	skip	The number of parameters that match the pathname to
		skip before selecting the parameter to modify.
	@return	true if the parameter was found; false otherwise.
	@see	find_parameter(const std::string&, bool, int,
				Parameter_Class_Selection)
*/
template <typename T>
bool
parameter_value
	(
	const Aggregate&				parameters,
	const std::string&				pathname,
	T&								value,
	bool							case_sensitive = false,
	int								skip = 0
	)
{
Parameter
	*parameter (parameters.find
		(pathname, case_sensitive, skip, Parameter::ASSIGNMENT));
if (! parameter ||
	  parameter->value ().is_Array ())
	return false;
parameter->value () = value;
return true;
}

/**	Set the value of a named parameter within a Parameter Aggregate.

	This is a convenience function that overloads the equivalent function,
	but for use with a char* pathname.

	@see	parameter_value(const Aggregate&, const std::string&,
		T&, bool, int)
*/
template <typename T>
bool
parameter_value
	(
	const Aggregate&				parameters,
	const char* const				pathname,
	T&								value,
	bool							case_sensitive = false,
	int								skip = 0
	)
{return PDS_Metadata::parameter_value (parameters, std::string (pathname),
	value, case_sensitive, skip);}

/**	Set the value of a named parameter within a Parameter Aggregate.

	This is a convenience function that overloads the equivalent function,
	but for use with a QString pathname.

	@see	parameter_value(const Aggregate&, const std::string&,
		T&, bool, int)
*/
template <typename T>
bool
parameter_value
	(
	const Aggregate&				parameters,
	const QString&					pathname,
	T&								value,
	bool							case_sensitive = false,
	int								skip = 0
	)
{return PDS_Metadata::parameter_value (parameters, pathname.toStdString (),
	value, case_sensitive, skip);}

/**	Set the value of a named parameter within the metadata parameters.

	This is a convenience method that overloads the equivalent function.

	@see	parameter_value(const Aggregate&, const std::string&,
		T&, bool, int)
*/
template <typename T>
bool
parameter_value
	(
	const std::string&				pathname,
	T&								value,
	bool							case_sensitive = false,
	int								skip = 0
	) const
{return metadata () ?
	parameter_value (*metadata (), pathname, value, case_sensitive, skip) :
	false;}

/**	Set the value of a named parameter within the metadata parameters.

	This is a convenience method that overloads the equivalent method,
	but for use with a char* pathname.

	@see	parameter_value(const std::string&, T&, bool, int)
*/
template <typename T>
bool
parameter_value
	(
	const char* const				pathname,
	T&								value,
	bool							case_sensitive = false,
	int								skip = 0
	) const
{return parameter_value (std::string (pathname), value, case_sensitive, skip);}

/**	Set the value of a named parameter within the metadata parameters.

	This is a convenience method that overloads the equivalent method,
	but for use with a QString pathname.

	@see	parameter_value(const std::string&, T&, bool, int)
*/
template <typename T>
bool
parameter_value
	(
	const QString&					pathname,
	T&								value,
	bool							case_sensitive = false,
	int								skip = 0
	) const
{return parameter_value (pathname.toStdString (), value, case_sensitive, skip);}

/**	Provide an absolute pathname.

	If the pathname does not begin with a Parameter pathname delimiter
	one is prepended. An empty pathname results in an empty string.

	@param	pathname	A Parameter pathname string.
	@return	The pathname in absolute form.
*/
static std::string absolute_pathname (const std::string& pathname);

/**	Provide an absolute pathname.

	This is a convenience function that overloads the equivalent function,
	but for use with a char* pathname.

	@see	absolute_pathname(const std::string&)
*/
inline static std::string absolute_pathname (const char* const pathname)
{return PDS_Metadata::absolute_pathname (std::string (pathname));}

/**	Provide an absolute pathname.

	This is a convenience function that overloads the equivalent function,
	but for use with a QString pathname.

	@see	absolute_pathname(const std::string&)
*/
inline static QString absolute_pathname (const QString& pathname)
{return QString::fromStdString
	(PDS_Metadata::absolute_pathname (pathname.toStdString ()));}

/**	Get a PVL Lister appropriately configured for PDS metadata generation.

	The Lister will be configured to use strict mode except: Begin and
	End name markers on Aggregates will not be included, assignment
	value alignment will be enabled, array indenting will be enabled,
	single line commenting style will be used, and the statement end
	delimiter will not be used.

	@return	A pointer to a Lister that has been allocated on the heap.
		<b>N.B.</b>: Delete when no longer needed.
*/
static Lister* metadata_lister ();

/*==============================================================================
	Thread run
*/
//!	Flag values for the {@link fetch(const QUrl&, bool) fetch} method.
enum
	{
	ASYNCHRONOUS,
	SYNCHRONOUS
	};

/**	Fetch PDS metadata referenced by a URL.

	If a URL fetch is {@link fetching()const in progress}, or the
	specified URL path is empty, nothing is done and false is returned
	immediately.

	When a fetch is started the network request state is {@link reset()
	reset} to unset values with the {@link request_status() request
	status} set to {@link #IN_PROGRESS}. Also, any existing {@link
	metadata() metadata} parameters are deleted and the metadata is reset
	to NULL.

	Only an {@link is_HTTP_URL(const QUrl&) HTTP URL} or a {@link
	is_file_URL(const QUrl&) file URL}, including a plain filesystem
	pathname, will be accepted for fetching. Any other URLs are rejected
	and the {@link request_status() request status} value will be set to
	{@link #INVALID_URL}.

	For an HTTP URL a network get request will be made for the URL data
	content which will be parsed as it is received to produce PVL
	Parameter statements. Only the initial part of the URL content that
	contains PVL will be {@link idaeim::PVL::Parser parsed}; any
	additional content is ignored, and the network request after PVL
	parsing completes is canceled.

	<b>N.B.</b>: An HTTP network request that has been redirected by the
	server may not be automatically redirected to the new URL by the
	server. In this case the error message content delivered by the
	server will be parsed which is likely (but may not) result in a
	parsing failure. Check the request status, HTTP status, and/or
	redirected URL if in doubt. If the request fails, or the resulting
	metadata parameters appear to be incorrect, and a redirected URL is
	provided this new URL can be used to try the fetch again.

	A URL for a local filesystem pathname will result in the file being
	accessed directly for parsing. This is usually very fast, but can
	still be done asynchronously. The pathname must exist and be a
	readable file for the metadata fetch to succeed. If the source file
	does not exist the request status will be {@link #URL_NOT_FOUND}; if
	the source file is not readable the request status will be {@link
	#URL_ACCESS_DENIED}.

	A fetch may be synchronous or asynchronous. A synchronous fetch will
	wait - the thread of the caller will be blocked - until the fetch
	completes. The wait for the network request to complete will timeout
	if the {@link wait_time() wait time} is exceeded, in which case the
	network request will be {@link cancel() canceled} and the result
	value will be set to WAIT_TIMEOUT.

	Thread blocking can be avoided - this can be important, for example,
	when the fetch is being done from the thread running the main GUI
	event loop - by specifying an asynchronous fetch with a connection to
	the {@link fetched(Aggregate*) fetched} signal to obtain the results
	of the fetch.

	The {@link fetched(Aggregate*) fetched} signal is always emitted when
	the fetch completes regardless of whether the fetch is synchronous or
	asynchronous, or if the fetch was successful or not.

	The resulting metadata will be NULL if the data source could not be
	accessed or successfully parsed for PVL parameters. In this case
	check the request status for the failure reason. If the request
	status is {@link #NO_STATUS} the data content was not successfully
	parsed and the {@link metadata_parse_failure()const metadata parse
	failure} message will describe the reason for the failure.

	@param	URL	A QUrl that is to be used as the PDS metadata source.

		<b>>>> WARNING <<<</b> A filesystem pathname on MS/Windows that
		includes a drive specification - e.g. "D:" - will be interpreted
		as having a URL scheme specification that is the drive letter
		when a QUrl is constructed from such a pathname string. For this
		reason it is advisable that a {@link normalized_URL(const QUrl&)
		normalized URL} be used here.

	@param	synchronous	This argument is ignored unless the URL specifies
		an HTTP protocol. If synchronous is true the method will wait for
		the network fetch to complete or timeout before returning;
		otherwise the method will return without waiting for the network
		fetch to complete.
	@return	For a synchronous fetch: true if the fetch completed
		successfully; false otherwise. For an asynchronous fetch: true if
		the fetch has been started; false otherwise. Check the {@link
		Network_Status::request_status()const request status} to obtain
		the reason for a false return value.
	@see fetching()const
	@see fetched (Aggregate*)
*/
bool fetch (const QUrl& URL, bool synchronous = true);


protected:

/**	Begin running the network fetch thread.

	<b>N.B.</b>: Running the thread should be done by calling the start
	method. The thread will finish when the network fetch has finished.
*/
virtual void run ();

/*==============================================================================
	Qt signals
*/
public:

signals:

/**	Signals the result of a {@link fetch(const QUrl&, bool) URL fetch}.

	The signal is emitted whenever a URL fetch completes.

	@param	metadata	A pointer to the metadata that was fetched. This
		will be NULL if an error occurred while fetching the metadata, if
		the fetch was cancelled, or if the beginning of the source
		content did not contain any PVL.
	@see request_status()
*/
void fetched (Aggregate* metadata);

/*==============================================================================
	Data
*/
private:

Aggregate*
	Metadata;

QString
	Metadata_Parse_Failure;

QFile
	*Source_File;

};


}	//	namespace idaeim
#endif
