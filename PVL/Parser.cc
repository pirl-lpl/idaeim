/*	Parser

Copyright (C) 2002 - 2013  Bradford Castalia

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

CVS ID:	Parser.cc,v 1.43 2013/02/24 00:50:49 castalia Exp
*******************************************************************************/

#include	"Parser.hh"

#include	"../Strings/VMS_Records_Filter.hh"
#include	"../Strings/Line_Count_Filter.hh"

#include	<string>
using std::string;
#include	<istream>
using std::istream;
#include	<sstream>
using std::ostringstream;
#include	<algorithm>
#include	<stdexcept>
#include	<cstdlib>
using std::min;
#include	<cstring>
#include	<cerrno>




namespace idaeim
{
namespace PVL
{
/*******************************************************************************
	Parser
*/
/*==============================================================================
	Types:
*/
//	For convenience in disambiguating the two String types.
typedef	idaeim::Strings::String						iString;
typedef	idaeim::PVL::String							vString;

typedef idaeim::Strings::Sliding_String::Location	Location;

/*==============================================================================
	Constants:
*/
const char* const
	Parser::ID =
		"idaeim::PVL::Parser (1.43 2013/02/24 00:50:49)";

/*------------------------------------------------------------------------------
	PVL syntax elements
*/
const char
	*const Parser::RESERVED_CHARACTERS
		= "{}()[]<>&\"',=;#%~|+! \t\r\n\f\013";

//	Delimiter characters:
const char
	Parser::PARAMETER_NAME_DELIMITER			= '=',
	Parser::PARAMETER_VALUE_DELIMITER			= ',',
	Parser::TEXT_DELIMITER						= '"',
	Parser::SYMBOL_DELIMITER					= '\'',
	Parser::SET_START_DELIMITER					= '{',
	Parser::SET_END_DELIMITER					= '}',
	Parser::SEQUENCE_START_DELIMITER			= '(',
	Parser::SEQUENCE_END_DELIMITER				= ')',
	Parser::UNITS_START_DELIMITER				= '<',
	Parser::UNITS_END_DELIMITER					= '>',
	Parser::NUMBER_BASE_DELIMITER				= '#',
	Parser::STATEMENT_END_DELIMITER				= ';',
	Parser::STATEMENT_CONTINUATION_DELIMITER	= '&',
	Parser::STRING_CONTINUATION_DELIMITER		= '-',
	/*
		The COMMENT_LINE_DELIMITER must not be the same as 
		the first character of the COMMENT_START_DELIMITERS.
	*/
	Parser::COMMENT_LINE_DELIMITER				= '#';

//	Delimiter strings:
const char
	*const Parser::LINE_BREAK					= "\r\n",
	*const Parser::WHITESPACE					= " \t\r\n\f\v",
	*const Parser::COMMENT_START_DELIMITERS		= "/*",
	*const Parser::COMMENT_END_DELIMITERS		= "*/",
	*const Parser::DATE_TIME_DELIMITERS			= "-:",
	*const Parser::VERBATIM_STRING_DELIMITERS	= "\\v";

/**	The name of a root Aggregate containing all Paramters available
	from an input source.
*/
const char
	*const Parser::CONTAINER_NAME				= "The Container";

#ifndef DOXYGEN_PROCESSING
namespace
{
/*	For convenience.
*/
static const char
	/*
		LINE_BREAK + FF + VT
	*/
	*LINE_DELIMITERS = "\r\n\f\013",
	/*
		WHITESPACE +
		PARAMETER_NAME_DELIMITER +
		STATEMENT_END_DELIMITER
	*/
	*PARAMETER_NAME_DELIMITERS = " \t\r\n\f\013=;",
	/*
		WHITESPACE +
		PARAMETER_VALUE_DELIMITER +
		SET_START_DELIMITER +
		SET_END_DELIMITER +
		SEQUENCE_START_DELIMITER +
		SEQUENCE_END_DELIMITER +
		UNITS_START_DELIMITER +
		STATEMENT_END_DELIMITER
	*/
	*PARAMETER_VALUE_DELIMITERS = " \t\r\n\f\013,{}()<;";

string&
trim_trailing_whitespace
	(
	string&		a_string
	)
{
iString::Index
	last = a_string.find_last_not_of (Parser::WHITESPACE);
if (++last < a_string.length ())
	a_string.erase (last);
return a_string;
}

string&
trim_whitespace
	(
	string&		a_string
	)
{
iString::Index
	first = trim_trailing_whitespace (a_string)
		.find_first_not_of (Parser::WHITESPACE);
if (first < a_string.length ())
	a_string.erase (0, first);
return a_string;
}

}	//	Local namespace
#endif	//	DOXYGEN_PROCESSING

/*==============================================================================
	Constructors/destructor
*/
Parser::Parser
	(
	std::istream&			reader,
	Location				limit
	)
	:	Sliding_String		(reader, limit),
		Strict				(PARSE_STRICT),
		Verbatim_Strings	(PARSE_VERBATIM_STRINGS),
		Commented_Lines		(PARSE_COMMENTED_LINES),
		String_Continuation (PARSE_STRING_CONTINUATION),
		Warnings			(new Warnings_List ())
{
//	VMS binary record size filtering.
insert_filter (VMS_records_filter = new idaeim::Strings::VMS_Records_Filter ());
non_text_limit (4);
Index
	increment = size_increment (4);
slide ();	//	Slide the String forward the 4 characters.
if (VMS_records_filter->enabled ())
	{
	WARNING (new Exception ("VMS binary records detected.", ID));
	}
else
	{
	non_text_limit (1);
	remove_filter (VMS_records_filter);
	delete VMS_records_filter;
	VMS_records_filter = NULL;
	}
size_increment (increment);

//	Line counting.
insert_filter (Line_count_filter = new idaeim::Strings::Line_Count_Filter ());
Line_count_filter->filter (*this);
}

Parser::Parser
	(
	const std::string&		string_source
	)
	:	Sliding_String		(string_source),
		Strict				(PARSE_STRICT),
		Verbatim_Strings	(PARSE_VERBATIM_STRINGS),
		Commented_Lines		(PARSE_COMMENTED_LINES),
		String_Continuation (PARSE_STRING_CONTINUATION),
		VMS_records_filter	(NULL),
		Warnings			(new Warnings_List ())
{
//	Line counting.
insert_filter (Line_count_filter = new idaeim::Strings::Line_Count_Filter ());
Line_count_filter->filter (*this);
}

//	Destructor
Parser::~Parser ()
{
clear_warnings ();
delete Warnings;
delete VMS_records_filter;
delete Line_count_filter;
}

//	Assignment
Parser&
Parser::operator=
	(
	const Parser&	parser
	)
{
if (this != &parser)
	{
	Sliding_String::operator= (parser);
	Strict = parser.Strict;
	Verbatim_Strings = parser.Verbatim_Strings;
	Commented_Lines = parser.Commented_Lines;
	String_Continuation = parser.String_Continuation;
	clear_warnings ();
	}
return *this;
}

/*==============================================================================
	PVL Parser
*/
/*------------------------------------------------------------------------------
	Parameter
*/
Parameter*
Parser::get_parameters ()
{
Aggregate
	*The_Aggregate = new Aggregate (CONTAINER_NAME);
Parameter
	*The_Parameter = &add_parameters (*The_Aggregate);
	//	Note that add_parameters returns its aggregate reference.

if (string_source ())
	{
	/*
		Parameters read from a string only need a new aggregate to
		contain them when the string contains multiple, ungrouped
		parameters to be returned as a group. Otherwise the single
		parameter in the string representation is itself returned. If
		the string does not contain any recognizable parameters then
		NULL is returned.

		Parameters read from a file are guaranteed to be returned in a
		new aggregate named CONTAINER_NAME.
	*/
	if (The_Aggregate->empty ())
		{
		delete The_Aggregate;
		The_Parameter = NULL;
		}
	else if (The_Aggregate->size () == 1)
		{
		//	Just return the single parameter.
		The_Parameter = The_Aggregate->pull_back ();
		delete The_Aggregate;
		}
	}
return The_Parameter;
}

Aggregate&
Parser::add_parameters
	(
	Aggregate&	The_Aggregate
	)
{
ingest_parameters (The_Aggregate);
return The_Aggregate;
}

Parameter::Type
Parser::ingest_parameters
	(
	Aggregate&	The_Aggregate
	)
{
Parameter::Type
	parameter_type = Parameter::END;

//	Collect all the parameters:
while (! is_empty ())
	{
	//	Get the next parameter.
	Parameter
		*The_Parameter = get_parameter ();
	if (! The_Parameter)
		{
		//	Failed to get a parameter.
		parameter_type = Parameter::END;	//	Mark end of input.
		break;
		}

	/*--------------------------------------------------------------------------
		Check for END parameters:
	*/
	if ((parameter_type = special_type (The_Parameter->name ())) &&
		 parameter_type & Parameter::END)
		{
		delete The_Parameter;	//	Drop the END parameter.
		if ((parameter_type ^ Parameter::END)
				!= (The_Aggregate.type () ^ Parameter::AGGREGATE) &&
			(parameter_type ^ Parameter::END) && The_Aggregate.parent ())
			{
			/*	The Begin Aggregate parameter doesn't match
				the End Aggregate parameter.
			*/
			WARNING (new Aggregate_Closure_Mismatch
				(
				line_position (BEFORE_POSITION, next_location ())
				+ The_Aggregate.type_name () + " Parameter "
				+ The_Aggregate.name ()
				+ "\n  ends with an "
				+ Parameter::type_name (parameter_type) + " Parameter.",
				next_location (),
				ID
				));
			if (Strict)
				throw *Warnings->back ();
			}
		break;
		}

	//	Add the parameter to the aggregate's list.
	The_Aggregate.poke_back (The_Parameter);

	if (The_Parameter->is_Aggregate () &&
		//	Recursively ingest the Parameters for this Aggregate Parameter.
		(parameter_type
			= ingest_parameters (*static_cast<Aggregate*>(The_Parameter)))
			== Parameter::END)
		break;
	}
return parameter_type;
}

Parameter*
Parser::get_parameter
	(
	bool	Assignment_only
	)
{
if (is_empty ())
	return NULL;

Parameter
	*The_Parameter = NULL;
string
	//	Collect any leading comment before the parameter name.
	comment = get_comment (),
	name;
Location
	location,
	delimiter;
Index
	index;

//	Ignore any statement end delimiter.
if (is_end (location = next_location (skip_over
		(string (WHITESPACE) + STATEMENT_END_DELIMITER, next_location ()))))
	{
	return NULL;
	}

/*..............................................................................
	Get the parameter name:
*/
if (at (location) == TEXT_DELIMITER ||
	at (location) == SYMBOL_DELIMITER)
	{
	//	It's a quoted string.
	char quote = at (location);
	name = get_quoted_string ();
	WARNING (new Invalid_Syntax
		(
		line_position (AT_POSITION, location)
		+ "Quoted Parameter name - " + quote + name + quote,
		location,
		ID
		));
	if (Strict)
		throw *Warnings->back ();
	}
else
	{
	//	Find the parameter name's trailing delimiter.
	if ((delimiter = skip_until (PARAMETER_NAME_DELIMITERS, location))
			== NO_LIMIT)
		delimiter = end_location ();

	//	Get the parameter name substring.
	name = substring (location, delimiter);
	if ((index = name.find (COMMENT_START_DELIMITERS)) != EOS)
		{
		//	Only take the part up to the trailing comment.
		name.erase (index);
		delimiter = location + index;
		}

	//	Check for reserved characters in the parameter name.
	if ((index = reserved_character (name) != EOS))
		{
		iString
			printable (name);
		printable.special_to_escape ();
		ostringstream
			message;
		message << line_position (AT_POSITION, location + index)
				<< "At character " << index
				<< " of the parameter named \"" << printable << '"';
		WARNING (new Reserved_Character
			(message.str (), location + index, ID));
		if (Strict)
			throw *Warnings->back ();
		}
	if (! Verbatim_Strings)
		//	Translate escape sequences to special characters.
		translate_from_escape_sequences (name);
	next_location (delimiter);
	}

/*..............................................................................
	Create a new Parameter:
*/
//	Check for special Aggregate name.
Parameter::Type
	parameter_type = Parameter::TOKEN;	//	Avoid compiler warning.
if (! Assignment_only &&
	((parameter_type = special_type (name)) & Parameter::AGGREGATE))
	The_Parameter = new Aggregate (name, parameter_type);
else
	{
	The_Parameter = new Assignment (name);
	}

//	Attach the comment.
The_Parameter->comment (comment);

/*..............................................................................
	Get the Parameter value(s):
*/
/*	Find the parameter name delimiter character
	that separates the parameter name from the parameter values list.
*/
if (is_end (delimiter = next_location
		(skip_whitespace_and_comments (next_location ()))))
	{
	return The_Parameter;
	}

if (parameter_type != Parameter::END &&
	at (delimiter) == PARAMETER_NAME_DELIMITER)
	{
	// Values string starts.
	next_location (++delimiter);

	Value
		*value = get_value ();
	if (value)
		{
		if (The_Parameter->is_Aggregate ())
			{
			//	Set the Parameter name to the Value string.
			The_Parameter->name (*value);
			if (! value->is_String ())
				{
				//	The Value is inappropriate for an Aggregate.
				WARNING (new Invalid_Aggregate_Value
					(
					line_position (AT_POSITION, delimiter)
					+ The_Parameter->type_name () + " Parameter \"" + name
					+ "\" = " + value->type_name () + " Value.",
					delimiter,
					ID
					));
				if (Strict)
					{
					delete The_Parameter;
					delete value;
					throw *Warnings->back ();
					}
				}
			}
		else
			//	Assign the value to the parameter.
			*The_Parameter = *value;
		delete value;
		}
	}

//	Skip any trailing white space and statement end delimiters.
next_location (skip_over
	(string (WHITESPACE) + STATEMENT_END_DELIMITER,
		next_location ()));

return The_Parameter;
}

/*------------------------------------------------------------------------------
	Comment
*/
string
Parser::get_comment ()
{
string
	comments,
	comment;
if (is_empty ())
	return comments;
Location
	location = next_location (),
	comment_start,
	comment_end;

//	Accumulate all sequential comments.
while (true)
	{
	//	Find the beginning of the comment string.
	if (is_end (location = skip_commented_line (location)) ||
		! begins_with (COMMENT_START_DELIMITERS, location))
		break;
	comment_start = location + strlen (COMMENT_START_DELIMITERS);

	//	Find the end of the comment string.
	if ((comment_end = location_of (COMMENT_END_DELIMITERS, location))
		== NO_LIMIT)
		{
		//	No comment end.
		WARNING (new Missing_Comment_End
			(
			line_position (AT_POSITION, location)
			+ "For comment starting with \""
			+ substring (comment_start,
				min (comment_start + 20, end_location ()))
			+ "\" ..." + line_position (AT_POSITION, location),
			location,
			ID
			));
		if (Strict)
			throw *Warnings->back ();

		//	Assume it ends at the end of the line.
		if ((comment_end = skip_until
				(string (LINE_DELIMITERS) + STATEMENT_END_DELIMITER,
					location))
			== NO_LIMIT)
			location = end_location ();
		location = skip_over
			(string (LINE_DELIMITERS) + STATEMENT_END_DELIMITER,
				comment_end);
		}
	else
		location = comment_end + strlen (COMMENT_END_DELIMITERS);

	comment = substring (comment_start, comment_end);
	if (comment.find_first_of (LINE_BREAK) != EOS)
		{
		WARNING (new Multiline_Comment
			(
			line_position (AT_POSITION,
				comment_start - strlen (COMMENT_START_DELIMITERS))
			+ "For comment starting with \""
			+ substring (comment_start - strlen (COMMENT_END_DELIMITERS),
				min (comment_start + 20, end_location ()))
			+ "\" ...",
			comment_start - strlen (COMMENT_START_DELIMITERS),
			ID
			));
		if (Strict)
			throw *Warnings->back ();
		}
	if (! Verbatim_Strings)
		//	Clean up the comment String.
		translate_from_escape_sequences (comment);

	//	Append the comment to the comments.
	if (! comments.empty ())
		comments += '\n';
	comments += comment;
	}
next_location (location);

return comments;
}

/*------------------------------------------------------------------------------
	Value
*/
Value&
Parser::add_values
	(
	Array&		The_Array
	)
{
try
	{
	Value
		*value;
	while ((value = get_value ()))
		The_Array.poke_back (value);
	}
catch (Invalid_Syntax exception) {/* Expected at the end of Values */}
return The_Array;
}


Value*
Parser::get_value ()
{
if (is_empty ())
	return NULL;

Array
	*The_Array = new Array ();	//	Accumulate an Array of Values.
Value
	*value;						//	Each Value added to the Array.
Value::Type
	start_type = static_cast<Value::Type>(0),
	end_type   = static_cast<Value::Type>(0);
Location
	delimiter,
	location,
	array_start_location;

if (is_end (delimiter = next_location
		(skip_whitespace_and_comments (next_location ()))))
	{
	delete The_Array;
	return NULL;
	}

/*..............................................................................
	Check for an initial array start character.
*/
array_start_location = delimiter;
if (at (delimiter) == SET_START_DELIMITER)
	{
	The_Array->type (start_type = Value::SET);
	}
else if (at (delimiter) == SEQUENCE_START_DELIMITER)
	{
	The_Array->type (start_type = Value::SEQUENCE);
	}
if (start_type != 0)
	next_location (++delimiter);

/*..............................................................................
	Get as many values as possible.
*/
while (true)
	{
	//	Find the values string.
	if (is_end (delimiter = skip_whitespace_and_comments (next_location ())))
		break;

	//	Check for a valid values string:

	switch (at (delimiter))
		{
		/*	End of array cases:

			Note: An empty array is valid.
		*/
		case SET_END_DELIMITER:
			end_type = Value::SET;
		case SEQUENCE_END_DELIMITER:
			if (end_type == 0)
				{
				end_type = Value::SEQUENCE;
				}
			next_location (++delimiter);

			//	Get any units string for this array.
			The_Array->units (get_units ());

		case STATEMENT_END_DELIMITER:
			goto End_of_Values;

		//	Syntax error cases:
		case PARAMETER_NAME_DELIMITER:
		case PARAMETER_VALUE_DELIMITER:
		case UNITS_START_DELIMITER:
		case UNITS_END_DELIMITER:
		case NUMBER_BASE_DELIMITER:
			/*
				Note: It would be possible here to be VERY tolerant
				and just take these illegal characters as the first
				character of an identifier string, but this seems
				unlikely in any case.
			*/
			delete The_Array;
			throw Invalid_Syntax
				(
				line_position (AT_POSITION, delimiter)
				+ "Expected a value, but found '"
				+ at (delimiter) + "'.",
				delimiter,
				ID
				);

		//	Possible value:
		default:
			next_location (delimiter);
			break;
		}

	//	Get the value:

	if (at (delimiter) == SET_START_DELIMITER ||
		at (delimiter) == SEQUENCE_START_DELIMITER)
		{
		//	The value is an array of values.
		if (! (value = get_value ()))
			break;
		}
	else
		{
		//	Get the Value.
		if (! (value = get_datum ()))
			break;

		//	Get any units string for this datum.
		value->units (get_units ());
		}
	//	Add the value to the array.
	The_Array->poke_back (value);

	/*	Find the next word.

		Note: Leave the next_location before any comment in case it
		occurs as a lead-in to the next PVL statement (i.e. this may be
		the end of the current statement). Update the next_location
		when the new location is recognized as value syntax.
	*/
	if (is_end (location = skip_whitespace_and_comments (next_location ())))
		break;

	//	Check what comes next:

	switch (at (location))
		{
		//	Another datum is expected:

		case PARAMETER_VALUE_DELIMITER:
			next_location (++location);
			break;

		case SET_START_DELIMITER:
		case SEQUENCE_START_DELIMITER:
			WARNING (new Invalid_Syntax
				(
				line_position (AT_POSITION, location)
				+ "Expected another datum, but found character '"
				+ at (location) + "'.",
				location,
				ID
				));
			if (Strict)
				{
				delete The_Array;
				throw *Warnings->back ();
				}
			next_location (location);
			break;

		//	End of array cases:
		case SET_END_DELIMITER:
			end_type = Value::SET;

		case SEQUENCE_END_DELIMITER:
			if (end_type == 0)
				end_type = Value::SEQUENCE;
			if (start_type != end_type)
				{
				ostringstream
					message;
				message << line_position (AT_POSITION, location)
						<< "For Value array starting at location "
						<< array_start_location
						<< line_position (AT_POSITION, array_start_location)
						<< "\n  with \""
						<< substring (array_start_location,
							min (array_start_location + 20, end_location ()))
						<< "\" ...";
				WARNING (new Array_Closure_Mismatch
					(message.str (), location, ID));
				if (Strict)
					{
					delete The_Array;
					throw *Warnings->back ();
					}
				}
			next_location (++location);

			//	Get any units string for this array.
			The_Array->units (get_units ());
			goto End_of_Values;

		//	Syntax error cases:
		case PARAMETER_NAME_DELIMITER:
		case UNITS_START_DELIMITER:
		case UNITS_END_DELIMITER:
		case NUMBER_BASE_DELIMITER:
			delete The_Array;
			 throw Invalid_Syntax
				(
				line_position (AT_POSITION, location)
				+ "Expected another datum, but found '"
				+ at (location) + "'.",
				location,
				ID
				);

		default:
			//	Not a recognized value syntax.
			goto End_of_Values;
		}
	}
End_of_Values:

/*..............................................................................
	Check the results:
*/
value = The_Array;
if (! start_type &&	//	Not an explicit Array.
	! end_type &&
	The_Array->size () <= 1)
	{
	if (The_Array->empty ())
		{
		//	Didn't get anything.
		value = new Parameter::Default_Value ();
		}
	else
		{
		/*
			The_Array contains a single, non-Array (parsed from an
			undecorated string) Value. Just return the Value.

			Note that this case can only occur at the root level for
			a PVL statement with a single non-Array value.
		*/
		value = The_Array->pull_back ();
		}
	delete The_Array;
	}
return value;
}

/*------------------------------------------------------------------------------
	Datum
*/
Value*
Parser::get_datum ()
{
if (is_empty ())
	return NULL;

Location
	delimiter;
//	Find the beginning of the datum string.
if (is_end (delimiter = next_location
		(skip_whitespace_and_comments (next_location ()))))
	{
	return NULL;
	}

Value
	*The_Value = NULL;
Value::Type
	value_type = static_cast<Value::Type>(0);

//	Check the beginning of the datum string.
switch (at (delimiter))
	{
	//	End of PVL statement cases:
	case STATEMENT_END_DELIMITER:
		//	Nothing to get.
		break;

	//	Syntax error cases:
	case PARAMETER_NAME_DELIMITER:
	case PARAMETER_VALUE_DELIMITER:
	case SET_START_DELIMITER:
	case SET_END_DELIMITER:
	case SEQUENCE_START_DELIMITER:
	case SEQUENCE_END_DELIMITER:
	case UNITS_START_DELIMITER:
	case UNITS_END_DELIMITER:
	case NUMBER_BASE_DELIMITER:
		/*
			Note: It would be possible here to be VERY tolerant
			and just take these illegal characters as the first
			character of an identifier string, but this seems
			unlikely in any case.
		*/
		throw Invalid_Syntax
			(
			line_position (AT_POSITION, delimiter)
			+ "Expected a datum, but found '"
			+ at (delimiter) + "'.",
			delimiter,
			ID
			);

	//	Quoted string cases:
	case TEXT_DELIMITER:
		value_type = Value::TEXT;
	case SYMBOL_DELIMITER:
		if (! value_type)
			value_type = Value::SYMBOL;
		//	CAUTION: get_quoted_string will update the next_location.
		The_Value = new vString (get_quoted_string (), value_type);
		return The_Value;

	//	Numeric value or symbol:
	default:
		//	Find the value string delimiter.
		if ((delimiter =
					skip_until (PARAMETER_VALUE_DELIMITERS, next_location ()))
				== NO_LIMIT)
			//	Hit the end of input
			delimiter = end_location ();

		string
			datum = substring (next_location (), delimiter);
		Index
			end = datum.find (COMMENT_START_DELIMITERS);
		if (end != EOS)
			{
			//	Only take the part up to the comment.
			datum = datum.substr (0, end);
			delimiter = next_location () + end;
			}
		if (! Verbatim_Strings)
			//	Convert escape sequences to special characters.
			translate_from_escape_sequences (datum);

		/*......................................................................
			Try for an integer number.
		*/
		errno = 0;
		char
			*character;
		int
			base = 10;
		#if ! defined (NO_HEX_NOTATION)
		//	Allow 0[x|X]... hex notation.
		if (! Strict &&
		   (! datum.compare (0, 2, "0x") ||
			! datum.compare (0, 2, "0X")))
			base = 16;
		#endif

		//	Storage precision is determined in the Value.hh header.
		#ifdef idaeim_LONG_LONG_INTEGER
		#define STRTOL strtoll
		#else
		#define STRTOL strtol
		#endif
		Value::Integer_type
			integer = STRTOL (datum.c_str (), &character, base);
		if (! *character)
			{
			if (errno)
				throw Invalid_Value
					(
					line_position (AT_POSITION, next_location ())
					+ "For datum \"" + datum + "\": "
					+ strerror (errno) + '.',
					next_location (),
					ID
					);
			The_Value = new Integer (integer);
			}
		else if (*character == NUMBER_BASE_DELIMITER)
			{
			/*..................................................................
				Probable base notation datum ([sign]Base#[sign]Radix#).
			*/
			int
				sign = 1;
			base = integer;
			if (base < 0)
				{
				sign = -1;
				base = -base;
				}
			if (base < Integer::MIN_BASE ||
				base > Integer::MAX_BASE)
				{
				ostringstream
					message;
				message << line_position (AT_POSITION, next_location ())
						<< "For datum \"" << datum
						<< "\": The base must be in the range "
						<< Integer::MIN_BASE << " - " << Integer::MAX_BASE << '.';
				throw Invalid_Value
					(message.str (), next_location (), ID);
				}
			int
				digits;
			char
				*end_character;
			integer = STRTOL (++character, &end_character, base);
			digits = end_character - character;
			if (*end_character == NUMBER_BASE_DELIMITER &&
				! *++end_character)
				{
				if (errno)
					throw Invalid_Value
						(
						line_position (AT_POSITION, next_location ())
						+ "For datum \"" + datum + "\": "
						+ strerror (errno) + '.',
						next_location (),
						ID
						);
				The_Value = new Integer (sign * integer, base, digits);
				break;
				}
			}
		else
			{
			/*..................................................................
				Try for a real number.
			*/
			//	Storage precision is determined in the Value.hh header.
			#ifdef PVL_LONG_DOUBLE
			#define STRTOD strtold
			#else
			#define STRTOD strtod
			#endif
			Value::Real_type
				real = STRTOD (datum.c_str (), &character);
			if (! *character)
				{
				if (errno)
					throw Invalid_Value
						(
						line_position (AT_POSITION, next_location ())
						+ "For datum \"" + datum + "\": "
						+ strerror (errno) + '.',
						next_location (),
						ID
						);

				//	Precision; digits after decimal point.
				int
					precision = datum.find ('.');
				std::ios::fmtflags
					format = std::ios::fixed;
				if (precision != (int)string::npos)
					{
					int
						 end_index;
					if ((end_index = datum.find ('e')) == (int)string::npos &&
						(end_index = datum.find ('E')) == (int)string::npos)
						 end_index = datum.length ();
					else
						{
						format = std::ios::scientific;
						//	Adjust precision for an index 1 decimal point.
						precision = 1;
						}
					precision = --end_index - precision;
					}
				else
					precision = 0;
				Real
					*value = new Real (real, precision);
				value->format_flags (format | std::ios::showpoint);
				The_Value = value;
				}
			else
				{
				/*..............................................................
					Couldn't make a number. It's a string.
				*/
				The_Value = new vString (datum);

				/*	Check for possible date and time string.
					N.B.: This is a cursory check; it's not determinate.
				*/
				if (datum.find_first_of (DATE_TIME_DELIMITERS) != EOS)
					The_Value->type (Value::DATE_TIME);

				//	Check for reserved characters.
				if ((end = reserved_character (datum)) != EOS)
					{
					iString
						printable (datum);
					printable.special_to_escape ();
					ostringstream
						message;
					message << line_position (AT_POSITION, next_location ())
							<< "At character " << end
							<< " of datum \"" << printable << "\"";
					WARNING (new Reserved_Character
						(message.str (), next_location (), ID));
					if (Strict)
						{
						delete The_Value;
						throw *Warnings->back ();
						}
					}
				}
			}
	}
next_location (delimiter);
return The_Value;
}

/*------------------------------------------------------------------------------
	Units
*/
string
Parser::get_units ()
{
string
	units;
Location
	delimiter,
	end;

//	Find the beginning of the units string.
if (is_empty () ||
	is_end (delimiter = skip_whitespace_and_comments (next_location ())) ||
	at (delimiter) != UNITS_START_DELIMITER)
	{
	return units;
	}
delimiter++;

//	Find the end of the units string.
if ((end = location_of (UNITS_END_DELIMITER, delimiter)) == NO_LIMIT)
	{
	WARNING (new Missing_Units_End
		(
		line_position (AT_POSITION, delimiter - 1)
		+ "For value units starting with \""
		+ substring (delimiter - 1, min (delimiter + 19, end_location ()))
		+ "\" ...",
		delimiter - 1,
		ID
		));
	if (Strict)
		throw *Warnings->back ();

	/*	Lacking a formal units string end marker
		just find the next non-white_space parameter value delimiter.
	*/
	for (end = delimiter;
		(end = skip_until (PARAMETER_VALUE_DELIMITERS, end)) != NO_LIMIT;
		 end = skip_over  (WHITESPACE, end))
		if (end != skip_until (WHITESPACE, end))
			break;
	if (end == NO_LIMIT)
		end = end_location ();

	//	Get the units string.
	units = substring (delimiter, end);
	}
else
	units = substring (delimiter, end++);

//	Move the next_location to after the end of the units string.
next_location (end);

trim_whitespace (units);	//	Remove leading and trailing whitspace.

if (! Verbatim_Strings)
	{
	//	Collapse each comment sequence to a single space.
	Index
		first = 0,
		last;
	while ((first = units.find (COMMENT_START_DELIMITERS, first)) != EOS)
		{
		if ((last = units.find (COMMENT_END_DELIMITERS, first)) == EOS)
			last = units.length ();
		else
			last += strlen (COMMENT_END_DELIMITERS);
		units.replace (first, last - first, " ");
		}

	//	Replace all whitespace sequences with a single space.
	for (first = 0;
		 first < units.length () &&
		(first = units.find (WHITESPACE, first)) != EOS;
		 first = last)
		{
		if ((last = units.find_first_not_of (WHITESPACE, first)) == EOS)
			last = units.length ();
		units.replace (first, last - first, " ");
		}
	trim_whitespace (units);	//	Remove leading and trailing whitspace.
	
	/*	Convert all escape sequences to their special characters.
		Note: The escape sequences are converted last so any
		resulting whitespace will not be collapsed away.
	*/
	translate_from_escape_sequences (units);
	}

return units;
}

/*==============================================================================
	Utility Methods:
*/
string
Parser::get_quoted_string ()
{
if (is_empty ())
	return NULL;

Location
	location,
	start;

//	The first non-whitespace character is the quotation mark.
if (is_end (start = location = next_location
		(skip_over (WHITESPACE, next_location ()))))
	{
	return NULL;
	}

//	Find the matching closing quote.
while ((location = location_of (at (start), ++location)) != NO_LIMIT)
	//	Allow for escaped quotation mark.
	if (at (location - 1) != '\\')
		break;
if (location == NO_LIMIT)
	{
	//	No end quote.
	WARNING (new Missing_Quote_End
		(
		line_position (AT_POSITION, start)
		+ "For the quoted string starting with \""
		+ substring (start, min (start + 20, end_location ()))
		+ "\" ...",
		start,
		ID
		));
	if (Strict)
		throw *Warnings->back ();
	location = end_location ();
	}

//	Make a duplicate of the quoted string.
string
	quote (substring (++start, location));

//	Move the next_location past the quoted string.
next_location (location + 1);

if (! Verbatim_Strings)
	{
	/*	Compress out line wrap effects.
		Sections between VERBATIM_STRING_DELIMITERS are not modified.
	*/
	Index
		first = 0,
		last,
		delimiters_length = strlen (VERBATIM_STRING_DELIMITERS);
	iString
		section;
	for (last = quote.find (VERBATIM_STRING_DELIMITERS, first);
		 first < quote.length ();
		 last = quote.find (VERBATIM_STRING_DELIMITERS, first = last))
		{
		if (last == EOS)
			//	This section extends to the end of the string.
			last = quote.length ();
		section = quote.substr (first, last - first);

		Index
			begin = 0,
			index;
		while ((index = section.skip_until (LINE_DELIMITERS, begin)) != EOS)
			{
			//	Backup over any trailing white space.
			if ((begin = section.skip_back_over (WHITESPACE, index)) != EOS)
				{
				if (! String_Continuation ||
					section[begin] != STRING_CONTINUATION_DELIMITER)
					{
					if (section[begin] != STRING_CONTINUATION_DELIMITER ||
						//	Hyphen
						begin == 0 ||
						section[begin - 1] == ' ')
						{
						//	Allow one space.
						section[++begin] = ' ';
						}
					begin++;
					}
				}
			else
				begin = 0;

			/*	Skip over any whitespace leading the next line.

				Note that skipping over white space here will
				also skip over any redundant line breaks
				because WHITESPACE includes LINE_DELIMITERS.
			*/
			index = section.skip_over (WHITESPACE, index);

			//	Delete the extra whitespace.
			section.erase (begin, index - begin);
			}

		//	Convert escape sequences to special characters.
		section.escape_to_special ();

		//	Replace the section in the quote string.
		quote.replace (first, last - first, section);
		last = first + section.length ();

		//	Skip the next, verbatim, section.
		if ((last += delimiters_length) >= quote.length () ||
			(last = quote.find (VERBATIM_STRING_DELIMITERS, last)) == EOS ||
			(last += delimiters_length) == quote.length ())
			//	No more sections.
			break;
		}
	}
return quote;
}

Location
Parser::skip_whitespace_and_comments
	(
	Location	location
	)
{
Location
	comment_end;

while (true)
	{
	//	Find the beginning of the comment string.
	if (is_end (location = skip_commented_line (location)) ||
		! begins_with (COMMENT_START_DELIMITERS, location))
		break;
	//	Find the end of the comment string.
	location += strlen (COMMENT_START_DELIMITERS);
	if ((comment_end = location_of (COMMENT_END_DELIMITERS, location)) == EOS)
		{
		//	No comment end.
		WARNING (new Missing_Comment_End
			(
			line_position (AT_POSITION,
				location - strlen (COMMENT_START_DELIMITERS))
			+ "For comment starting with \""
			+ substring (location, min (location + 20, end_location ()))
			+ "\" ...",
			location - strlen (COMMENT_START_DELIMITERS),
			ID
			));
		if (Strict)
			throw *Warnings->back ();

		//	Assume it ends at the end of the line.
		if ((comment_end = skip_until
				(string (LINE_DELIMITERS) + STATEMENT_END_DELIMITER, location))
				== EOS)
			location = end_location ();
		location = skip_over
			(string (LINE_DELIMITERS) + STATEMENT_END_DELIMITER,
				comment_end);
		}
	else
		location = comment_end + strlen (COMMENT_END_DELIMITERS);
	}
return location;
}

Location
Parser::skip_commented_line
	(
	Location	location
	)
{
if (Strict || ! Commented_Lines)
	//	Just skip whitespace.
	location = skip_over
		(string (WHITESPACE) + STATEMENT_CONTINUATION_DELIMITER,
			location);
else
	{
	while (true)
		{
		//	Find the beginning of the comment string.
		if (is_end (location = skip_over
				(string (WHITESPACE) + STATEMENT_CONTINUATION_DELIMITER,
					location)) ||
			at (location) != COMMENT_LINE_DELIMITER)
			break;
		//	A crosshatch comment extends to the end of the line.
		location = skip_until (LINE_BREAK, location);
		if (location == NO_LIMIT)
			location = end_location ();
		}
	}
return location;
}

/*..............................................................................
	Warnings
*/
void
Parser::WARNING
	(
	Exception*	exception
	)
{
Warnings->push_back (exception);
}

void
Parser::clear_warnings ()
{
if (! Warnings->empty ())
	for (Parser::Warnings_List::iterator
			warning = Warnings->begin ();
		 warning != Warnings->end ();
		 ++warning)
		delete *warning;
Warnings->clear ();
}

string
Parser::line_position
	(
	bool		before,
	Location	location
	) const
{
if (Line_count_filter)
	{
	idaeim::Strings::Line_Count_Filter::Position
		a_position = Line_count_filter->position (location);
	if (a_position.character != string::npos)
		{
		ostringstream
			message;
		message << " " << (before ? "before" : "at")
				<< " character " << a_position.character
				<< " of line " << a_position.line << '\n';
		return message.str ();
		}
	}
return "";
}		

/*==============================================================================
	Helper Functions:
*/
#ifndef DOXYGEN_PROCESSING
namespace
{
/**	An association of an Aggregate Parameter name with its
	Parameter::Type.
*/
typedef struct
	{
	const char*	const	name;
	Parameter::Type		type;
	}
	Name_Type;


/**	Each reserved Parameter name is associated with a specific
	Parameter::Type in this list (used by #special_type).
*/
Name_Type
	name_types[] =
	{
	/*
		BEGIN_XXXX subtypes are identical to XXXX subtypes. Both forms
		must be in the list so they can be identified during parsing.
	*/
	{"BEGIN_GROUP",		Parameter::GROUP},
	{"BEGINGROUP",		Parameter::GROUP},
	{"GROUP",			Parameter::GROUP},
	{"BEGIN_OBJECT",	Parameter::OBJECT},
	{"BEGINOBJECT",		Parameter::OBJECT},
	{"OBJECT",			Parameter::OBJECT},
	{"END_GROUP",		Parameter::END_GROUP},
	{"ENDGROUP",		Parameter::END_GROUP},
	{"END_OBJECT",		Parameter::END_OBJECT},
	{"ENDOBJECT",		Parameter::END_OBJECT},
	{"END",				Parameter::END},
	{NULL,				Parameter::END}
	};
}	//	Local namespace
#endif	//	DOXYGEN_PROCESSING

Parameter::Type
special_type
	(
	const string&		name
	)
{
string
	uppercase_name (name);
for (string::iterator
		character  = uppercase_name.begin ();
		character != uppercase_name.end ();
	  ++character)
	*character = (char)toupper (*character);
const char
	*normalized_name = uppercase_name.c_str ();

for (Name_Type *name_type = name_types;
				name_type->name;
				name_type++)
	if (! strcmp (normalized_name, name_type->name))
		return name_type->type;
return static_cast<Parameter::Type>(0);
}

string&
translate_from_escape_sequences
	(
	string&				a_string
	)
{
if (a_string.empty ())
	return a_string;
iString
	source (a_string);
iString::Index
	first = 0,
	last = 0,
	delimiters_length = strlen (Parser::VERBATIM_STRING_DELIMITERS);

while (true)
	{
	//	Find sections delimited by VERBATIM_STRING_DELIMITERS.
	last = source.index_of (Parser::VERBATIM_STRING_DELIMITERS, first);
	//	Convert escape sequences to special characters.
	source.escape_to_special (first, last);
	//	Skip the next verbatim section.
	if (last == iString::EOS ||
		(first = source.index_of (Parser::VERBATIM_STRING_DELIMITERS,
			last += delimiters_length))
		== iString::EOS)
		break;
	first += delimiters_length;
	}
//	Remove all VERBATIM_STRING_DELIMITERS.
source.replace (Parser::VERBATIM_STRING_DELIMITERS, "");
a_string = source;
return a_string;
}

string&
translate_to_escape_sequences
	(
	string&				a_string
	)
{
if (a_string.empty ())
	return a_string;
iString
	source (a_string);
iString::Index
	first = 0,
	last,
	delimiters_length = strlen (Parser::VERBATIM_STRING_DELIMITERS);

while (true)
	{
	//	Find sections delimited by VERBATIM_STRING_DELIMITERS.
	last = source.index_of (Parser::VERBATIM_STRING_DELIMITERS, first);
	//	Convert special characters to escape sequences.
	source
		.special_to_escape
			(first, last)
		.replace
			(string (&Parser::TEXT_DELIMITER, 1),
			string ("\\") + Parser::TEXT_DELIMITER,
			first, last)
		.replace
			(string (&Parser::SYMBOL_DELIMITER, 1),
			string ("\\") + Parser::SYMBOL_DELIMITER,
			first, last);
	//	Skip the next verbatim section.
	if (last == iString::EOS ||
		(first = source.index_of (Parser::VERBATIM_STRING_DELIMITERS,
			last += delimiters_length))
		== iString::EOS)
		break;
	first += delimiters_length;
	}
a_string = source;
return a_string;
}

string::size_type
reserved_character
	(
	const string&		a_string
	)
{
string::size_type
	index = a_string.find_first_of (Parser::RESERVED_CHARACTERS);
if (index != string::npos)
	{
	return index;
	}
for (string::const_iterator position = a_string.begin ();
	 position != a_string.end ();
	 ++position)
	{
	if (! isprint (*position))
		{
		return position - a_string.begin ();
		}
	}
return string::npos;
}

}		//	namespace PVL
}		//	namespace idaeim
