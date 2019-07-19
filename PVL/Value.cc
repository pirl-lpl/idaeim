/*	Value

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

CVS ID: Value.cc,v 1.46 2013/09/17 03:52:51 castalia Exp
*******************************************************************************/

#include	"Value.hh"
#include	"Parser.hh"
#include	"Lister.hh"

//	For Integer to string in arbitrary base notation.
#include	"Strings/String.hh"

#include	<sstream>
using std::ostringstream;
#include	<iomanip>
using std::setw;
using std::setfill;
using std::endl;
#include	<stdexcept>
#include	<algorithm>
using std::min;
#include	<cstdlib>
#include	<cstring>
#include	<cerrno>

using std::string;
using std::ostream;
using std::istream;




namespace idaeim
{
namespace PVL
{
/*******************************************************************************
	Value
*/
/*==============================================================================
	Constants
*/
const char* const
	Value::ID =
		"idaeim::PVL::Value (1.46 2013/09/17 03:52:51)";

//	Equivalent values in idaeim::Strings::String
const bool
	Value::UNSIGNED		= false,
	Value::SIGNED		= true;

//	Accuracy of Real numbers when being compared.
#ifndef	REAL_ACCURACY
#define REAL_ACCURACY	1.0e-15
#endif

/*******************************************************************************
	Helper functions
*/
string
Value::integer_to_string
	(
	const Integer_type	value,
	const bool			is_signed,
	const int			base,
	const int			digits
	)
{
ostringstream
	representation;
representation << setw (digits) << setfill ('0')
	<< idaeim::Strings::String (value, is_signed, base ? base : 10);
return representation.str ();
}


Value::Integer_type
Value::string_to_integer
	(
	const std::string&	value,
	const int			base 
	)
{
errno = 0;
char*
	end_char;
#ifdef idaeim_LONG_LONG_INTEGER
#define STRTOL strtoll
#else
#define STRTOL strtol
#endif
Integer_type
	datum = STRTOL (value.c_str (), &end_char, base);
if (errno ||
	(*end_char &&
	 *end_char != '.'))
	{
	if (! errno)
		{
		//	Check for trailing whitespace.
		while (*end_char && isspace (*end_char++)) ;
		if (! *end_char)
			return datum;
		}
	ostringstream
		message;
	message << "Can't form an integer numeric value from the string\n"
			<< '"' << value.substr (0, min ((int)value.length (), 40))
			<< ((value.length () > 40) ? "\"...\n" : "\".\n")
			<< "  using base " << base << ".\n"
			<< "  ";
	if (errno)
		{
		message << strerror (errno) << '.';
		throw Out_of_Range (message.str (), Value::ID);
		}
	else
		{
		message << "Inappropriate character: " << *end_char
				<< " at index " << (unsigned int)(end_char - value.c_str ())
				<< '.';
		throw Invalid_Value (message.str (), -1, Value::ID);
		}
	}
return datum;
}


unsigned int
Value::string_numeric_base
	(
	const std::string&	value
	)
{
if (value.empty ())
	return 10;
unsigned int
	base = 10;	//	Assume decimal.
string::size_type
	index = value.find_first_not_of (Parser::WHITESPACE);
if (index == string::npos)
	//	Just whitespace.
	return 0;

if (value[index] == '-' ||
	value[index] == '+')
	{
	if (++index == value.length ())
		return 0;
	}
if (value[index] == '0')
	{
	//	Tentative octal.
	base = 8;
	if (++index == value.length ())
		//	Just 0; decimal.
		return 10;
	if (value[index] == 'x' ||
		value[index] == 'X')
		//	Hex notation.
		base = 16;
	}
try
	{
	if (base == 10)
		string_to_real (value);
	else
		string_to_integer (value, base);
		}
catch (...)
	{
	//	Not interpretable as numeric.
	return 0;
	}
return base;
}


string
Value::real_to_string
	(
	const Real_type		value,
	std::ios::fmtflags	format,
	unsigned int		precision
	)
{
ostringstream
	representation;
representation.flags (format | std::ios::showpoint);
if (precision)
	representation.precision (precision);
representation << value;
string
	value_string = representation.str ();
if (! precision &&
	! (format & (std::ios::fixed | std::ios::scientific)))
	{
	//	Remove trailing zeros.
	string::size_type
		index = value_string.find_last_not_of ('0');
	if (value_string[index] == '.')
		{
		//	Leave a single zero after the decimal point.
		if ((index += 2) < value_string.length ())
			//	Remove the excessive zero(s).
			value_string.erase (index);
		else if (--index == value_string.length ())
			//	Provide the missing zero.
			value_string += '0';
		}
	else if (++index < value_string.length ())
		//	Remove all trailing zeros.
		value_string.erase (index);
	}
return value_string;
}


Value::Real_type
Value::string_to_real
	(
	const std::string&	value
	)
{
errno = 0;
char*
	end_char;
#ifdef idaeim_LONG_DOUBLE
#define STRTOD strtold
#else
#define STRTOD strtod
#endif
Real_type
	datum = STRTOD (value.c_str (), &end_char);
if (errno || *end_char)
	{
	if (! errno)
		{
		//	Check for trailing whitespace.
		while (*end_char && isspace (*end_char++)) ;
		if (! *end_char)
			return datum;
		}
	ostringstream
		message;
	message << "Can't form a real numeric value from the string\n"
			<< '"' << value.substr (0, min ((int)value.length (), 40))
			<< ((value.length () > 40) ? "\"...\n" : "\".\n")
			<< "  ";
	if (errno)
		{
		message << strerror (errno) << '.';
		throw Out_of_Range (message.str (), Value::ID);
		}
	else
		{
		message << "Inappropriate character: " << *end_char
				<< " at index " << (unsigned int)(end_char - value.c_str ())
				<< '.';
		throw Invalid_Value (message.str (), -1, Value::ID);
		}
	}
return datum;
}

#ifndef DOXYGEN_PROCESSING
namespace
{
void
INVALID_OPERATION_exception
	(
	const string&		previous_message,
	const char* const	operation,
	const Value&		first,
	string				second
	)
{
ostringstream
	message;
unsigned int
	size = previous_message.size ();
if (size)
	{
	message << previous_message;
	if (previous_message[size - 1] != '\n')
		{
		message << endl;
		++size;
		}
	}
message << "Can't " << operation << ' '
		<< first.type_name () << " Value - " << first;
if (message.str ().size () > (size + 75))
	message.str (message.str ().substr (0, (size + 75)) + " ...");
if (! second.empty ())
	{
	if (second.size () > 75)
		second = second.substr (0, 75) + " ...";
	message << endl
			<< "  and " << second;
	}
throw Invalid_Argument (message.str (), Value::ID);
}

void
INVALID_OPERATION
	(
	const string&		previous_message,
	const char* const	operation,
	const Value&		first
	)
{
INVALID_OPERATION_exception
	(previous_message, operation, first, string ());
}

void
INVALID_OPERATION
	(
	const string&		previous_message,
	const char* const	operation,
	const Value&		first,
	const Value&		second
	)
{
ostringstream
	description;
description << second.type_name () << " Value - " << second;
INVALID_OPERATION_exception
	(previous_message, operation, first, description.str ());
}

void
INVALID_OPERATION
	(
	const string&		previous_message,
	const char* const	operation,
	const Value&		first,
	Value::String_type	second
	)
{
second = "string value - " + second;
INVALID_OPERATION_exception
	(previous_message, operation, first, second);
}

void
INVALID_OPERATION
	(
	const string&		previous_message,
	const char* const	operation,
	const Value&		first,
	Value::Real_type	second
	)
{
ostringstream
	description;
description << "real value - " << second;
INVALID_OPERATION_exception
	(previous_message, operation, first, description.str ());
}

void
INVALID_OPERATION
	(
	const string&		previous_message,
	const char* const	operation,
	const Value&		first,
	Value::Integer_type	second
	)
{
ostringstream
	description;
description << "integer value - " << second;
INVALID_OPERATION_exception
	(previous_message, operation, first, description.str ());
}

void
INVALID_OPERATION
	(
	const string&					previous_message,
	const char* const				operation,
	const Value&					first,
	Value::Unsigned_Integer_type	second
	)
{
ostringstream
	description;
description << "integer value - " << second;
INVALID_OPERATION_exception
	(previous_message, operation, first, description.str ());
}

}	//	Local namespace.
#endif	//	DOXYGEN_PROCESSING

/*******************************************************************************
	Value
*/
/*==============================================================================
	Accessors
*/
Value&
Value::operator[]
	(
	unsigned int
	)
{INVALID_OPERATION ("", "index ([])", *this); return *this;}

const Value&
Value::operator[]
	(
	unsigned int
	)
	const
{INVALID_OPERATION ("", "index ([])", *this); return *this;}

/*------------------------------------------------------------------------------
	Parent
*/
Array*
Value::root () const
{
Array
	*parent	= Parent,
	*root_array = NULL;
while (parent)
	{
	root_array = parent;
	parent = root_array->parent ();
	}
return root_array;
}

bool
Value::has_children () const
{return is_Array () && dynamic_cast<const Array*>(this)->size ();}

/*------------------------------------------------------------------------------
	Units
*/
std::string
Value::nearest_units () const
{
if (Units.empty () && Parent)
	return Parent->nearest_units ();
return Units;
}

/*------------------------------------------------------------------------------
	Subtype
*/
string
Value::type_name
	(
	const Type		type_code
	)
{
switch (type_code)
	{
	case NUMERIC:		return "Numeric";
		case INTEGER:		return "Integer";
		case REAL:			return "Real";

	case STRING:		return "String";
		case IDENTIFIER:	return "Identifier";
		case SYMBOL:		return "Symbol";
		case TEXT:			return "Text";
		case DATE_TIME:		return "Date_time";

	case ARRAY:			return "Array";
		case SET:			return "Set";
		case SEQUENCE:		return "Sequence";

	default:			return "Invalid";
	};
}

Value&
Value::type
	(
	const Type		type_code
	)
{
throw Invalid_Argument
	(
	type_name () + " Value can't be set to type " + type_name (type_code) + '.',
	ID
	);
}

/*------------------------------------------------------------------------------
	Base
*/
Value&
Value::base
	(
	const int	base
	)
{
if (base)
	{
	if (base < MIN_BASE ||
		base > MAX_BASE)
		{
		ostringstream
			message;
		message << type_name () << " Value base " << base
				<< " is outside of the valid range: "
				<< MIN_BASE << " - " << MAX_BASE;
		throw Out_of_Range (message.str (), ID);
		}
	Base = base;
	}
else if (is_String ())
	Base = base;
return *this;
}

/*==============================================================================
	I/O
*/
//	Write
ostream&
operator<<
	(
	std::ostream&	out,
	const Value&	value
	)
{
value.write (out, 0);
return out;
}

unsigned int
Value::write
	(
	std::ostream&	out,
	int				depth
	)
	const
{
Lister
	lister (out);
lister.write (*this, depth);
return lister.flush (depth);
}


//	Read

void
Value::read
	(
	std::istream&	in
	)
{
Parser
	parser (in);
read (parser);
}

void
Value::read
	(
	Parser&			parser
	)
{
Value
	*value = parser.get_value ();
if (value)
	{
	*this = *value;
	delete value;
	}
}


istream&
operator>>
	(
	std::istream&	in,
	Value&			value
	)
{
try {value.read (in);}
catch (Exception exception)
	{in.setstate (std::ios::failbit);}
return in;
}

/*******************************************************************************
	Integer
*/
/*------------------------------------------------------------------------------
	Constructors
*/
//	Type casting
//		integer
Integer::Integer
	(
	const Integer_type	datum,
	const bool			is_signed,
	const int			base,
	const int			digits
	)
	:	Value (digits, base),
		Datum (datum),
		Signed (is_signed)
{}

Integer::Integer
	(
	const Integer_type	datum,
	const int			base,
	const int			digits
	)
	:	Value (digits, base),
		Datum (datum),
		Signed (SIGNED)
{}

Integer::Integer
	(
	const Unsigned_Integer_type	datum,
	const bool					is_signed,
	const int					base,
	const int					digits
	)
	:	Value (digits, base),
		Datum (datum),
		Signed (is_signed)
{}

Integer::Integer
	(
	const Unsigned_Integer_type	datum,
	const int					base,
	const int					digits
	)
	:	Value (digits, base),
		Datum (datum),
		Signed (UNSIGNED)
{}

#ifdef idaeim_LONG_LONG_INTEGER
Integer::Integer
	(
	const long			datum,
	const bool			is_signed,
	const int			base,
	const int			digits
	)
	:	Value (digits, base),
		Datum (datum),
		Signed (is_signed)
{}

Integer::Integer
	(
	const long			datum,
	const int			base,
	const int			digits
	)
	:	Value (digits, base),
		Datum (datum),
		Signed (SIGNED)
{}

Integer::Integer
	(
	const unsigned long	datum,
	const bool			is_signed,
	const int			base,
	const int			digits
	)
	:	Value (digits, base),
		Datum (datum),
		Signed (is_signed)
{}

Integer::Integer
	(
	const unsigned long	datum,
	const int			base,
	const int			digits
	)
	:	Value (digits, base),
		Datum (datum),
		Signed (UNSIGNED)
{}
#endif

Integer::Integer
	(
	const int			datum,
	const bool			is_signed,
	const int			base,
	const int			digits
	)
	:	Value (digits, base),
		Datum (datum),
		Signed (is_signed)
{}

Integer::Integer
	(
	const int			datum,
	const int			base,
	const int			digits
	)
	:	Value (digits, base),
		Datum (datum),
		Signed (SIGNED)
{}

Integer::Integer
	(
	const unsigned int	datum,
	const bool			is_signed,
	const int			base,
	const int			digits
	)
	:	Value (digits, base),
		Datum (datum),
		Signed (is_signed)
{}

Integer::Integer
	(
	const unsigned int	datum,
	const int			base,
	const int			digits
	)
	:	Value (digits, base),
		Datum (datum),
		Signed (UNSIGNED)
{}

//		floating point

#ifdef idaeim_LONG_DOUBLE
Integer::Integer
	(
	const double		datum,
	const bool			is_signed,
	const int			base,
	const int			digits
	)
	:	Value (digits, base),
		Datum (static_cast<Integer_type>(datum)),
		Signed (is_signed)
{}

Integer::Integer
	(
	const double		datum,
	const int			base,
	const int			digits
	)
	:	Value (digits, base),
		Datum (static_cast<Integer_type>(datum)),
		Signed (SIGNED)
{}
#endif

Integer::Integer
	(
	const Real_type		datum,
	const bool			is_signed,
	const int			base,
	const int			digits
	)
	:	Value (digits, base),
		Datum (static_cast<Integer_type>(datum)),
		Signed (is_signed)
{}

Integer::Integer
	(
	const Real_type		datum,
	const int			base,
	const int			digits
	)
	:	Value (digits, base),
		Datum (static_cast<Integer_type>(datum)),
		Signed (SIGNED)
{}

//		string
Integer::Integer
	(
	const String_type&	datum,
	const bool			is_signed,
	const int			numeric_base,
	const int			digits
	)
	:	Value (digits, numeric_base),
		Signed (is_signed)
{
if (! numeric_base)
	//	Try to intuit the base from the string.
	base (string_numeric_base (datum));
Datum = string_to_integer (datum, Base);
}

Integer::Integer
	(
	const String_type&	datum,
	const int			numeric_base,
	const int			digits
	)
	:	Value (digits, numeric_base),
		Datum (string_to_integer (datum, numeric_base)),
		Signed (SIGNED)
{
if (! numeric_base)
	//	Try to intuit the base from the string.
	base (string_numeric_base (datum));
Datum = string_to_integer (datum, Base);
}

//	Copy
Integer::Integer
	(
	const Integer&		value
	)
	:	Value  (value),
		Datum  (value.Datum),
		Signed (value.Signed)
{}

//	Copy virtual
Integer::Integer
	(
	const Value&	value
	)
	:	Value (value),
		Signed (value.is_signed ())
{
try {Datum = static_cast<Integer_type>(value);}
catch (Invalid_Argument exception)
	{
	exception.message
		(
		exception.message () + '\n'
		+ value.type_name ()
		+ " Value can't be used to construct an Integer Value."
		);
	throw exception;
	}
}

/*------------------------------------------------------------------------------
	Type conversion
*/
Integer::operator Real_type () const
{
if (Signed)
	return static_cast<Real_type>(Datum);
return static_cast<Real_type>(static_cast<Unsigned_Integer_type>(Datum));
}

Integer::operator String_type () const
{return integer_to_string (Datum, Signed, Base, Digits);}

/*------------------------------------------------------------------------------
	Assignment operators
*/
Integer&
Integer::operator=
	(
	const Value&	value
	)
{
if (this != &value)
	{
	try {Datum = static_cast<Integer_type>(value);}
	catch (Exception exception)
		{INVALID_OPERATION (exception.message (), "assign (=)", *this, value);}
	Signed = value.is_signed ();
	Units = value.units ();
	Digits = value.digits ();
	if (! (Base = value.base ()) &&
		value.is_String ())
		Base = string_numeric_base (static_cast<String_type>(value));
	if (! Base)
		Base = 10;
	//	DO NOT assign the Parent!
	}
return *this;
}

Integer&
Integer::operator=
	(
	const String_type&	value
	)
{
int
	base = string_numeric_base (value);
if (! base)
	base = Base;
try {Datum = string_to_integer (value, base);}
catch (Exception exception)
	{INVALID_OPERATION (exception.message (), "assign (=)", *this, value);}
return *this;
}

Integer&
Integer::operator=
	(
	const char*		value
	)
{
if (value)
	return operator= (String_type (value));
else
	return operator= (String_type ());
}

/*------------------------------------------------------------------------------
	Addition operators
*/
Integer&
Integer::operator+=
	(
	const Value&	value
	)
{
string
	message;
if (! value.is_Array ())
	{
	try {Datum += static_cast<Integer_type>(value); return *this;}
	catch (Exception exception) {message = exception.message ();}
	}
INVALID_OPERATION (message, "add", *this, value);
return *this;
}

Integer
Integer::operator+
	(
	const Value&	value
	) const
{
Integer
	integer (*this);
integer += value;
return integer;
}

/*------------------------------------------------------------------------------
	Compare
*/
int
Integer::compare
	(
	const Value&	value
	) const
{
if (this == &value)
	return 0;
if (value.is_Array ())
	return -1;	//	Arrays values are always greater.

try
	{
	/*	Compare reals to correctly handle signed vs unsigned
		and in case the value is a String representing a real.
	*/
	Real_type
		comparison =
			static_cast<Real_type>(*this) - static_cast<Real_type>(value);
	if (comparison < -REAL_ACCURACY)
		return -1;
	if (comparison > REAL_ACCURACY)
		return 1;
	return 0;
	}
catch (Exception exception)
	{
	//	Lexicographically compare strings.
	return
		(static_cast<String_type>(*this))
			.compare (static_cast<String_type>(value));
	}
}

/*******************************************************************************
	Real
*/
#ifndef DEFAULT_FORMAT_FLAGS
#define DEFAULT_FORMAT_FLAGS	std::ios::showpoint
#endif
std::ios::fmtflags
	Real::Default_Format_Flags	= DEFAULT_FORMAT_FLAGS;

/*------------------------------------------------------------------------------
	Constructors
*/
#ifdef idaeim_LONG_DOUBLE
Real::Real
	(
	const double		datum,
	const int			digits
	)
	:	Value (digits, 10),
		Datum (static_cast<Real_type>(datum)),
		Format_Flags (Default_Format_Flags)
{}
#endif

Real::Real
	(
	const Real_type		datum,
	const int			digits
	)
	:	Value (digits, 10),
		Datum (datum),
		Format_Flags (Default_Format_Flags)
{}

//	Type casting
Real::Real
	(
	const String_type&	datum,
	const int			digits
	)
	:	Value (digits, 10),
		Datum (string_to_real (datum)),
		Format_Flags (Default_Format_Flags)
{}

//	Copy
Real::Real
	(
	const Real&		value
	)
	:	Value (value),
		Datum (value.Datum),
		Format_Flags (value.Format_Flags)
{}

//	Copy virtual
Real::Real
	(
	const Value&	value
	)
	:	Value (value)
{
try {Datum = static_cast<Real_type>(value);}
catch (Invalid_Argument exception)
	{
	exception.message
		(
		exception.message () + '\n'
		+ value.type_name ()
		+ " Value can't be used to construct an Integer Value."
		);
	throw exception;
	}
if (value.is_Real ())
	Format_Flags = (dynamic_cast<const Real&>(value)).Format_Flags;
else
	Format_Flags = Default_Format_Flags;
Base = 10;
}

/*------------------------------------------------------------------------------
	Type conversion
*/
Real::operator String_type () const
{return real_to_string (Datum, Format_Flags, Digits);}

/*------------------------------------------------------------------------------
	Assignment operators
*/
Real&
Real::operator=
	(
	const Value&	value
	)
{
if (this != &value)
	{
	try {Datum = static_cast<Real_type>(value);}
	catch (Invalid_Argument exception)
		{INVALID_OPERATION (exception.message (), "assign (=)", *this, value);}
	//	DO NOT assign the Parent!
	Units = value.units ();
	Digits = value.digits ();
	}
return *this;
}

Real&
Real::operator=
	(
	const String_type&	value
	)
{
try {Datum = string_to_real (value);}
catch (Exception exception)
	{INVALID_OPERATION (exception.message (), "assign (=)", *this, value);}
return *this;
}

Real&
Real::operator=
	(
	const char*		value
	)
{
if (value)
	return operator= (String_type (value));
else
	return operator= (String_type ());
}

/*------------------------------------------------------------------------------
	Addition operators
*/
Real&
Real::operator+=
	(
	const Value&	value
	)
{
string
	message;
if (! value.is_Array ())
	{
	try {Datum += static_cast<Real_type>(value); return *this;}
	catch (Exception exception) {message = exception.message ();}
	}
INVALID_OPERATION (message, "add", *this, value);
return *this;
}

Real
Real::operator+
	(
	const Value&	value
	) const
{
Real
	real (*this);
real += value;
return real;
}

/*------------------------------------------------------------------------------
	Compare
*/
int
Real::compare
	(
	const Value&	value
	) const
{
if (this == &value)
	{
	return 0;
	}
if (value.is_Array ())
	{
	//	Arrays values are always greater.
	return -1;
	}
try
	{
	//	Compare reals in case the value is a String representing a real.
	Real_type
		comparison = Datum - static_cast<Real_type>(value);
	if (comparison < -REAL_ACCURACY)
		return -1;
	if (comparison > REAL_ACCURACY)
		return 1;
	return 0;
	}
catch (Exception exception)
	{
	//	Lexicographically compare strings.
	return
		(static_cast<String_type>(*this))
			.compare (static_cast<String_type>(value));
	}
}

/*******************************************************************************
	String
*/
/*------------------------------------------------------------------------------
	Constructors
*/
String::String
	(
	const String_type&	datum
	)
	:	Datum (datum),
		//	Check, and undecorate, the string for its Type.
		Subtype (type (Datum))
{
Base = string_numeric_base (datum);
}

String::String
	(
	const String_type&	datum,
	const Type			type_code
	)
	//	Take the string as-is.
	:	Datum (datum)
{
type (type_code);
Base = string_numeric_base (datum);
}

//	Type casting
//		integer
String::String
	(
	const Integer_type	datum,
	const bool			is_signed,
	const int			base,
	const int			digits
	)
	:	Value (digits, (base < MIN_BASE || base > MAX_BASE) ? 10 : base),
		Datum (integer_to_string (datum, is_signed, Base, digits)),
		Subtype (IDENTIFIER)
{}

#ifdef idaeim_LONG_LONG_INTEGER
String::String
	(
	const long			datum,
	const bool			is_signed,
	const int			base,
	const int			digits
	)
	:	Value (digits, (base < MIN_BASE || base > MAX_BASE) ? 10 : base),
		Datum (integer_to_string
			(static_cast<Integer_type>(datum), is_signed, Base, digits)),
		Subtype (IDENTIFIER)
{}

String::String
	(
	const unsigned long	datum,
	const bool			is_signed,
	const int			base,
	const int			digits
	)
	:	Value (digits, (base < MIN_BASE || base > MAX_BASE) ? 10 : base),
		Datum (integer_to_string
			(static_cast<Integer_type>(datum), is_signed, Base, digits)),
		Subtype (IDENTIFIER)
{}
#endif

String::String
	(
	const int			datum,
	const bool			is_signed,
	const int			base,
	const int			digits
	)
	:	Value (digits, (base < MIN_BASE || base > MAX_BASE) ? 10 : base),
		Datum (integer_to_string
			(static_cast<Integer_type>(datum), is_signed, Base, digits)),
		Subtype (IDENTIFIER)
{}

String::String
	(
	const unsigned int	datum,
	const bool			is_signed,
	const int			base,
	const int			digits
	)
	:	Value (digits, (base < MIN_BASE || base > MAX_BASE) ? 10 : base),
		Datum (integer_to_string
			(static_cast<Integer_type>(datum), is_signed, Base, digits)),
		Subtype (IDENTIFIER)
{}

//		floating point
#ifdef idaeim_LONG_DOUBLE
String::String
	(
	const double		datum,
	const int			digits
	)
	:	Value (digits),
		Datum (real_to_string
			(static_cast<Real_type>(datum),
			Real::default_format_flags (), digits)),
		Subtype (IDENTIFIER)
{}
#endif

String::String
	(
	const Real_type		datum,
	const int			digits
	)
	:	Value (digits),
		Datum (real_to_string (datum, Real::default_format_flags (), digits)),
		Subtype (IDENTIFIER)
{}

//	Copy
String::String
	(
	const String&		value
	)
	:	Value (value),
		Datum (value.Datum),
		Subtype (value.Subtype)
{}

//	Copy virtual
String::String
	(
	const Value&		value
	)
	:	Value (value)
{
try {Datum = static_cast<String_type>(value);}
catch (Invalid_Argument exception)
	{
	exception.message
		(
		exception.message () + '\n'
		+ value.type_name ()
		+ " Value can't be used to construct a String Value."
		);
	throw exception;
	}
if (value.is_String ())
	Subtype = value.type ();
else
	Subtype = IDENTIFIER;	//	The default Type.
Base = value.base ();
}

/*------------------------------------------------------------------------------
	Type conversion
*/
String::operator Integer_type () const
{return string_to_integer (Datum, Base);}

#ifdef idaeim_LONG_DOUBLE
String::operator double () const
{return static_cast<double>(string_to_real (Datum));}
#endif

String::operator Real_type () const
{return string_to_real (Datum);}

/*------------------------------------------------------------------------------
	Subtype
*/
String&
String::type
	(
	const Type		type_code
	)
{
if (type_code == Value::IDENTIFIER ||
	type_code == Value::TEXT ||
	type_code == Value::SYMBOL ||
	type_code == Value::DATE_TIME)
	Subtype = type_code;
else
	{
	ostringstream
		message;
	message << "The " << Value::type_name (type_code)
			<< " Type (" << type_code
			<< ") is inappropriate for a String Value.";
	throw Invalid_Argument (message.str (), ID);
	}
return *this;
}

Value::Type
String::type
	(
	String_type&		characters
	)
{
Type
	type = IDENTIFIER;
if (characters.length ())
	{
	if (characters[0] == Parser::TEXT_DELIMITER)
		{
		type = TEXT;
		characters.erase (0, 1);
		if (characters[characters.length () - 1] == Parser::TEXT_DELIMITER)
			characters.erase (characters.length () - 1);
		}
	else if (characters[0] == Parser::SYMBOL_DELIMITER)
		{
		type = SYMBOL;
		characters.erase (0, 1);
		if (characters[characters.length () - 1] == Parser::SYMBOL_DELIMITER)
			characters.erase (characters.length () - 1);
		}
	else if (characters.find_first_of (Parser::WHITESPACE)
			!= string::npos)
		type = TEXT;
	}
return type;
}

/*------------------------------------------------------------------------------
	Assignment operators
*/
String&
String::operator=
	(
	const Value&	value
	)
{
if (this != &value)
	{
	try {Datum = static_cast<String_type>(value);}
	catch (Exception exception)
		{INVALID_OPERATION (exception.message (), "assign (=)", *this, value);}
	if (value.is_String ())
		Subtype = value.type ();
	else
		Subtype = IDENTIFIER;	//	The default Type.
	Units = value.units ();
	Digits = value.digits ();
	Base = value.base ();
	//	DO NOT assign the Parent!
	}
return *this;
}

String&
String::operator=
	(
	const Integer_type	value
	)
{
Datum = integer_to_string (value, SIGNED, Base, Digits);
return *this;
}

String&
String::operator=
	(
	const unsigned int	value
	)
{
Datum = integer_to_string
	(static_cast<Integer_type>(value), UNSIGNED, Base, Digits);
return *this;
}

String&
String::operator=
	(
	const Real_type		value
	)
{
Datum = real_to_string (value, Real::default_format_flags (), Digits);
Base = 10;
return *this;
}

String&
String::operator=
	(
	const String_type&	value
	)
{
Datum = value;
if (Subtype == IDENTIFIER)
	//	Possible unquoted Subtype promotion to quoted Subtype.
	Subtype = type (Datum);
return *this;
}

String&
String::operator=
	(
	const char*		value
	)
{
if (value)
	return operator= (String_type (value));
else
	return operator= (String_type ());
}

/*------------------------------------------------------------------------------
	Addition operators
*/
String&
String::operator+=
	(
	const Value&	value
	)
{
if (value.is_Array ())
	INVALID_OPERATION ("", "add (+=)", *this, value);
Datum += static_cast<String_type>(value);
return *this;
}

String
String::operator+
	(
	const Value&	value
	) const
{
String
	string_value (*this);
string_value += value;
return string_value;
}

/*------------------------------------------------------------------------------
	Compare
*/
int
String::compare
	(
	const Value&	value
	) const
{
if (this == &value)
	return 0;
if (value.is_Array ())
	return -1;	//	Arrays are always greater than any other type.
return Datum.compare (static_cast<String_type>(value));
}

/*******************************************************************************
	Array
*/
/*------------------------------------------------------------------------------
	Constructors
*/
Array::Array
	(
	const Type			type_code
	)
	:	Value (0, 0)
{
type (type_code);
}

//		Copy
Array::Array
	(
	const Array&		array
	)
	:	Value (array),
		Vectal<Value> (),
		Subtype (array.Subtype)
{
/*	Copy in the new Value List.

	This is done, rather than using the Value_List copy method, because
	the Parent of each cloned Value must be set as it is placed into
	the Value_List. Note that the cloning method will, itself, copy any
	children of arrays being cloned.
*/
copy_in (array);
}

//		Copy virtual
Array::Array
	(
	const Value&		value
	)
	:	Value (0, 0)
{
if (value.is_Array ())
	*this = value;
else
	{
	//	Make the value the first element of a default Array.
	Subtype = Default_Subtype;
	push_back (value);
	}
}

//		from Parser
Array::Array
	(
	Parser&				parser,
	const Type			type_code
	)
	:	Value (0, 0)
{
Value
	*value = parser.get_value ();

//	Default Array Type.
type (type_code);
if (value)
	{
	if (value->is_Array ())
		{
		//	Become the Subtype of the parsed array.
		type (value->type ());
		//	Move the Values into this Array.
		move_in (*(static_cast<Array*>(value)));
		delete value;
		}
	else
		//	Add the Value into this Array.
		poke_back (value);
	}
}

/*------------------------------------------------------------------------------
	Type conversion
*/
Array::operator Integer_type () const
{
throw Invalid_Argument
	(
	type_name () + " Value can't be used as an integer value.",
	Value::ID
	);
}

#ifdef idaeim_LONG_LONG_INTEGER
Array::operator long () const
{
throw Invalid_Argument
	(
	type_name () + " Value can't be used as an integer value.",
	Value::ID
	);
}
#endif

Array::operator int () const
{
throw Invalid_Argument
	(
	type_name () + " Value can't be used as an integer value.",
	Value::ID
	);
}

#ifdef idaeim_LONG_DOUBLE
Array::operator double () const
{
throw Invalid_Argument
	(
	type_name () + " Value can't be used as a double value.",
	Value::ID
	);
}
#endif

Array::operator Real_type () const
{
throw Invalid_Argument
	(
	type_name () + " Value can't be used as a real value.",
	Value::ID
	);
}

Array::operator String_type () const
{
throw Invalid_Argument
	(
	type_name () + " Value can't be used as a string value.",
	Value::ID
	);
}

/*------------------------------------------------------------------------------
	Subtype
*/
#ifndef DEFAULT_ARRAY_SUBTYPE
#define DEFAULT_ARRAY_SUBTYPE	SEQUENCE
#endif
Value::Type
	Array::Default_Subtype = Value::DEFAULT_ARRAY_SUBTYPE;

Value::Type
Array::default_type
	(
	const int		type_code
	)
{
if (type_code == static_cast<int>(Value::SET) ||
	type_code == static_cast<int>(Value::SEQUENCE))
	return Default_Subtype = static_cast<Value::Type>(type_code);
else
	{
	ostringstream
		message;
	message << "The " << Value::type_name
				(static_cast<Value::Type>(type_code))
			<< " Type (" << type_code
			<< ") is an inappropriate default for an Array Value.";
	throw Invalid_Argument (message.str (), Value::ID);
	}
}

Array&
Array::type
	(
	const Type		type_code
	)
{
if (type_code == Value::SET ||
	type_code == Value::SEQUENCE)
	Subtype = type_code;
else
	{
	ostringstream
		message;
	message << "The " << Value::type_name
				(static_cast<Value::Type>(type_code))
			<< " Type (" << type_code
			<< ") is inappropriate for an Array Value.";
	throw Invalid_Argument (message.str (), Value::ID);
	}
return *this;
}

/*------------------------------------------------------------------------------
	Assignment operators
*/
Array&
Array::operator=
	(
	const Value&		value
	)
{
if (this != &value)
	{
	if (value.is_Array ())
		{
		//	DO NOT assign the Parent!
		Units = value.units ();
		Subtype = value.type ();
		/*
			Remove the old Value List.
			Copy in the new Value List.

			This is done, rather than using the Value_List copy method,
			because the Parent of each cloned Value must be set as it
			is placed into the Value_List. Note that the cloning method
			will, itself, copy any children of arrays being cloned.

			The old list is set-aside without deleting it, as a copy
			of the Value pointers, in case the Array being
			assigned is enclosed within this Array.
		*/
		std::vector<Value*>
			list;
		list.reserve (size ());
		for (iterator
				first = begin (),
				last  = end ();
			 first != last;
			 ++first)
			list.push_back (peek (first));

		//	Remove these Value pointers.
		wipe ();
		//	Copy in those Values (clones).
		copy_in (static_cast<const Array&>(value));

		//	Delete the original Values.
		for (std::vector<Value*>::iterator
				first = list.begin (),
				last  = list.end ();
			 first != last;
			 ++first)
			delete *first;
		}
	else
		INVALID_OPERATION ("", "assign (=)", *this, value);
	}
return *this;
}

Array&
Array::operator=
	(
	const Integer_type	value
	)
{INVALID_OPERATION ("", "assign (=)", *this, value); return *this;}

Array&
Array::operator=
	(
	const Unsigned_Integer_type	value
	)
{INVALID_OPERATION ("", "assign (=)", *this, value); return *this;}

Array&
Array::operator=
	(
	const Real_type		value
	)
{INVALID_OPERATION ("", "assign (=)", *this, value); return *this;}

Array&
Array::operator=
	(
	const String_type&	value
	)
{INVALID_OPERATION ("", "assign (=)", *this, value); return *this;}

Array&
Array::operator=
	(
	const char*		value
	)
{
if (value)
	return operator= (String_type (value));
else
	return operator= (String_type ());
}

/*------------------------------------------------------------------------------
	Addition operators
*/
Array&
Array::operator+=
	(
	const Value&	value
	)
{
if (value.is_Array ())
	{
	Array::const_iterator
		element = (static_cast<const Array&>(value)).begin (),
		stop    = (static_cast<const Array&>(value)).end ();
	while (element != stop)
		{
		add (*element);
		++element;
		}
	}
else
	add (value);
return *this;
}

Array
Array::operator+
	(
	const Value&	value
	) const
{
Array
	array (*this);
array += value;
return array;
}

/*------------------------------------------------------------------------------
	Compare
*/
int
Array::compare
	(
	const Value&	value
	) const
{
if (this == &value)
	{
	return 0;
	}
if (! value.is_Array ())
	{
	//	Arrays are always greater than any other Value.
	return 1;
	}

//	Compare the lists.
const Array&
	array = static_cast<const Array&>(value);
int
	comparison = size () - array.size ();
if (comparison == 0)
	{
	//	Compare all elements.
	const_iterator
		stop (end ());
	for (const_iterator
			this_element (begin ()),
			that_element (array.begin ());
			this_element != stop;
			++this_element,
			++that_element)
		{
		comparison = this_element->compare (*that_element);
		if (comparison != 0)
			break;
		}
	}
return comparison;
}

/*------------------------------------------------------------------------------
	Convenient
*/
Array&
Array::copy_in
	(
	const Array&	array
	)
{
Value_List::const_iterator
	element  = array.begin (),
	stop     = array.end ();
while (element != stop)
	{
	push_back (*element);
	++element;
	}
return *this;
}

Array&
Array::move_in
	(
	Array&	array
	)
{
/*	Implementation note:

	The code would be more compact if the value pointers were moved
	in a peek, pull, poke_back loop. However, a pull instead of a
	pull_back would be used to maintain the order of the original
	values which would entail an unecessary and potentially
	expensive shift-up of all the remaining pointers in the list after
	each pull. To avoid this all the pointers are simply copied, wiped
	from the source array (which also avoids unecessary parent
	pointer NULL-ing) and then poke_back is used to move the copied
	pointers into this array list.
*/
//	Copy the new Value List (just the pointers).
std::vector<Value*>
	list (array.vector_base ());
//	Remove the pointers but don't delete the Values.
array.wipe ();
//	Move the value pointers into this Value List.
for (std::vector<Value*>::const_iterator
		value = list.begin (),
		last  = list.end ();
	 value != last;
	 ++value)
	{
	poke_back (*value);	//	Poke it into the destination (Parent reset).
	}
return *this;
}


Value*
Array::remove
	(
	iterator	position
	)
{return pull_out (position);}


Value*
Array::remove
	(
	size_type	index
	)
{return pull_out (index);}


Value*
Array::remove
	(
	Value	*value
	)
{
if (value)
	{
	for (iterator
			position  = begin (),
			last      = end ();
			position != last;
		  ++position)
		{
		if (peek (position) == value)
			{
			pull (position);
			return value;
			}
		}
	}
return NULL;
}


Value*
Array::remove_depth
	(
	Value	*value
	)
{
if (value)
	{
	for (Depth_Iterator
			position  = begin_depth (),
			last      = end_depth ();
			position != last;
		  ++position)
		{
		if (peek (position) == value)
			{
			position.array ()->pull (position);
			return value;
			}
		}
	}
return NULL;
}


/*------------------------------------------------------------------------------
	Vectal related methods.
*/
void
Array::entering
	(
	Value*	value
	)
{
Value
	*parent = Parent;
while (parent)
	{
	if (parent == value)
		{
		ostringstream
			message;
		message << "Can't enter Value \"" << *value << "\"";
		if (message.str ().size () > 75)
			message.str (message.str ().substr (0, 75) + " ...\"");
		message << endl
				<< "  into the Array \"" << *this << "\"";
		if (message.str ().size () > 150)
			message.str (message.str ().substr (0, 150) + " ...\"");
		message << endl
				<< "  because it is already in the Array's path.";
		throw Invalid_Argument (message.str (), Value::ID);
		}
	parent = parent->Parent;
	}
value->Parent = this;
}

void
Array::removing
	(
	Value*	value
	)
{
value->Parent = NULL;
}

Array::size_type
Array::total_size ()
{
int
	total = 0;
if (! empty ())
	{
	for (iterator
			first = begin (),
			last  = end ();
		first != last;
		++first,
		++total)
		if (first->is_Array ())
			total += (static_cast<Array&>(*first)).total_size ();
	}
return total;
}

/*******************************************************************************
	Depth_Iterator
*/
//	Array begin/end methods.

Array::Depth_Iterator
Array::begin_depth ()
{
return Depth_Iterator (*this);
}

Array::Depth_Iterator
Array::end_depth ()
{
Depth_Iterator
	depth_iterator = begin_depth ().end_depth ();
return depth_iterator;
}

/*------------------------------------------------------------------------------
	Constructors/Assignment
*/
Array::Depth_Iterator::Depth_Iterator
	(
	const Array&	array
	)
	:	Current_Position ((const_cast<Array&>(array)).begin ()),
		Current_Array (const_cast<Array*>(&array))
{
}

//	End.
Array::Depth_Iterator
Array::Depth_Iterator::end_depth () const
{
Array
	*top = root ();
if (! top)
	throw Error
		("Can't get the end of an invalid Depth_Iterator", Value::ID);
Depth_Iterator
	depth_iterator (*top);
depth_iterator.Current_Position = top->end ();
return depth_iterator;
}

//	Assignment from iterator.
Array::Depth_Iterator&
Array::Depth_Iterator::operator=
	(
	const Array::iterator&	list_iterator
	)
{
if (Current_Position != list_iterator)
	{
	if (Current_Array->end () == list_iterator)
		{
		Current_Position = list_iterator;
		if (! Previous_Position.empty ())
			{
			if (Current_Array->empty ())
				{
				//	Increment to a valid position.
				do
					{
					/*	Ascend to the Previous_Position in the parent Array.
						The pre-increment will then move to the next Value
						in the parent Array's Value_List.
					*/
					Current_Position = Previous_Position.back ();
					Previous_Position.pop_back ();
					if (! (Current_Array = Current_Position->parent ()))
						{
						ostringstream
							message;
						message << "Depth_Iterator increment found no parent for" << endl
								<< "  \"" << *Current_Position << "\".";
						if (message.str ().size () > 150)
							message.str (message.str ().substr (0, 150) + " ...\".");
						throw Error (message.str (), Value::ID);
						}
					}
					while (++Current_Position == Current_Array->end () &&
							! Previous_Position.empty ());
				}
			else
				//	Back off from the end.
				--Current_Position;
			}
		}
	else if (Current_Array == list_iterator->parent ())
		{
		Current_Position = list_iterator;
		}
	else
		{
		//	Try to find the list_iterator in the scope of this Depth_Iterator.
		Depth_Iterator
			position (*this);
		try
			{
			while (--*this != list_iterator) ;
			}
		catch (Out_of_Range out_of_range)
			{
			try
				{
				*this = position;
				while (++*this != list_iterator) ;
				}
			catch (Out_of_Range exception)
				{
				*this = position;
				ostringstream
					message;
				message << "Can't assign the Array iterator at \""
						<< *list_iterator << "\"";
				if (message.str ().size () > 75)
					message.str (message.str ().substr (0, 75) + " ...\"");
				message << endl
						<< "  to the Depth_Iterator at \""
						<< *Current_Position << "\".";
				if (message.str ().size () > 150)
					message.str (message.str ().substr (0, 150) + " ...\".");
				message << endl
						<< "  The former is not in the scope of the later.";
				throw Invalid_Argument (message.str (), Value::ID);
				}
			}
		}
	}
return *this;
}

/*------------------------------------------------------------------------------
	Accessors
*/
//	Root Array.
Array*
Array::Depth_Iterator::root () const
{
if (Previous_Position.empty ())
	return Current_Array;
else
	return Previous_Position[0]->parent ();
}

/*------------------------------------------------------------------------------
	Incrementors
*/
//	Prefix increment.
Array::Depth_Iterator&
Array::Depth_Iterator::operator++ ()
{
if (! Current_Array)
	throw Error ("Can't increment an invalid Depth_Iterator", Value::ID);
if (! Previous_Position.empty () ||
	Current_Position != Current_Array->end ())
	{
	if (Current_Position->is_Array () &&
		! (static_cast<Array&>(*Current_Position)).empty ())
		{
		/*	The current Value is a non-empty Array.
			Save the current position, then
			descend to the first Value in this Array.
		*/
		Previous_Position.push_back (Current_Position);
		Current_Array =
			static_cast<Array*>(Current_Position.operator-> ());
		Current_Position = Current_Array->begin ();
		}
	else
		{
		//	Pre-increment to the next position.
		while (++Current_Position == Current_Array->end () &&
				! Previous_Position.empty ())
			{
			/*	End of the current Value_List in a child Array.
				Ascend to the Previous_Position in the parent Array.
				The pre-increment will then move to the next Value
				in the parent Array's Value_List.
			*/
			Current_Position = Previous_Position.back ();
			Previous_Position.pop_back ();
			if (! (Current_Array = Current_Position->parent ()))
				{
				ostringstream
					message;
				message << "Depth_Iterator increment found no parent for" << endl
						<< "  \"" << *Current_Position << "\".";
				if (message.str ().size () > 150)
					message.str (message.str ().substr (0, 150) + " ...\".");
				throw Error (message.str (), Value::ID);
				}
			}
		}
	return *this;
	}
ostringstream
	message;
message << "Can't increment a Depth_Iterator beyond the end" << endl
		<< "  of its root Array - \"" << *Current_Array << "\".";
if (message.str ().size () > 150)
	message.str (message.str ().substr (0, 150) + " ...\".");
throw Out_of_Range (message.str (), Value::ID);
}


Array::Depth_Iterator&
Array::Depth_Iterator::next_sibling ()
{
if (! Current_Array)
	throw Error
		("No next siblilng for an invalid Depth_Iterator", Value::ID);
if (! Previous_Position.empty () ||
	Current_Position != Current_Array->end ())
	{
	//	Pre-increment to the next position.
	while (++Current_Position == Current_Array->end () &&
			! Previous_Position.empty ())
		{
		/*	End of the current Value_List in a child Array.
			Ascend to the Previous_Position in the parent Array.
			The pre-increment will then move to the next Value
			in the parent Array's Value_List.
		*/
		Current_Position = Previous_Position.back ();
		Previous_Position.pop_back ();
		if (! (Current_Array = Current_Position->parent ()))
			{
			ostringstream
				message;
			message << "Depth_Iterator next sibling found no parent for" << endl
					<< "  \"" << *Current_Position << "\".";
			if (message.str ().size () > 150)
				message.str (message.str ().substr (0, 150) + " ...\".");
			throw Error (message.str (), Value::ID);
			}
		}
	return *this;
	}
ostringstream
	message;
message << "A Depth_Iterator has no next sibling beyond the end" << endl
		<< "  of its root Array - \"" << *Current_Array << "\".";
if (message.str ().size () > 150)
	message.str (message.str ().substr (0, 150) + " ...\".");
throw Out_of_Range (message.str (), Value::ID);
}

//	Postfix increment.
Array::Depth_Iterator
Array::Depth_Iterator::operator++ (int)
{
Depth_Iterator
	depth_iterator = *this;
++*this;
return depth_iterator;
}

//	Prefix decrement.
Array::Depth_Iterator&
Array::Depth_Iterator::operator-- ()
{
if (! Current_Array)
	throw Error ("Can't decrement an invalid Depth_Iterator", Value::ID);
if (! Previous_Position.empty () ||
	Current_Position != Current_Array->begin ())
	{
	if (Current_Position == Current_Array->begin ())
		{
		/*	The current position is the beginning of a child array.
			Ascend to the previous position in the parent array.
		*/
		Current_Position = Previous_Position.back ();
		Previous_Position.pop_back ();
		if (! (Current_Array = Current_Position->parent ()))
			{
			ostringstream
				message;
			message << "Depth_Iterator decrement found no parent for" << endl
					<< "  \"" << *Current_Position << "\".";
			if (message.str ().size () > 150)
				message.str (message.str ().substr (0, 150) + " ...\".");
			throw Error (message.str (), Value::ID);
			}
		}
	else
		{
		//	Pre-decrement to the previous position.
		while ((*--Current_Position).is_Array () &&
				! (static_cast<Array&>(*Current_Position)).empty ())
			{
			/*	Backed onto a non-empty child Array.
				Save the current position, then
				descend to the end of the Value_List in this Array.
				The pre-decrement will back to the last Value in the
				Array's Value_List.
			*/
			Previous_Position.push_back (Current_Position);
			Current_Array =
				static_cast<Array*>(Current_Position.operator-> ());
			Current_Position = Current_Array->end ();
			}
		}
	return *this;
	}
ostringstream
	message;
message << "Can't decrement a Depth_Iterator beyond the beginning" << endl
		<< "  of its root Array - \"" << *Current_Array << "\".";
if (message.str ().size () > 150)
	message.str (message.str ().substr (0, 150) + " ...\".");
throw Out_of_Range (message.str (), Value::ID);
}


Array::Depth_Iterator&
Array::Depth_Iterator::previous_sibling ()
{
if (! Current_Array)
	throw Error
		("No previous sibling for an invalid Depth_Iterator", Value::ID);
if (! Previous_Position.empty () ||
	Current_Position != Current_Array->begin ())
	{
	//	Pre-decrement to the previous position.
	while (Current_Position == Current_Array->begin () &&
			! Previous_Position.empty ())
		{
		Current_Position = Previous_Position.back ();
		Previous_Position.pop_back ();
		if (! (Current_Array = Current_Position->parent ()))
			{
			ostringstream
				message;
			message << "Depth_Iterator previous sibling found no parent for" << endl
					<< "  \"" << *Current_Position << "\".";
			if (message.str ().size () > 150)
				message.str (message.str ().substr (0, 150) + " ...\".");
			throw Error (message.str (), Value::ID);
			}
		}
	return *this;
	}
ostringstream
	message;
message << "A Depth_Iterator has no previous sibling beyond the beginning" << endl
		<< "  of its root Array - \"" << *Current_Array << "\".";
if (message.str ().size () > 150)
	message.str (message.str ().substr (0, 150) + " ...\".");
throw Out_of_Range (message.str (), Value::ID);
}

//	Postfix decrement.
Array::Depth_Iterator
Array::Depth_Iterator::operator-- (int)
{
Depth_Iterator
	depth_iterator = *this;
--*this;
return depth_iterator;
}

//		Array-style indexing.
Value&
Array::Depth_Iterator::operator[]
	(
	int		offset
	) const
{return *(*this + offset);}

//	Offset.
Array::Depth_Iterator&
Array::Depth_Iterator::operator+=
	(
	int		distance
	)
{
if (distance > 0)
	while (distance--)
		++*this;
else
	while (distance++)
		--*this;
return *this;
}

Array::Depth_Iterator
Array::Depth_Iterator::operator+
	(
	int		distance
	) const
{
Depth_Iterator
	depth_iterator (*this);
return depth_iterator += distance;
}

Array::Depth_Iterator&
Array::Depth_Iterator::operator-=
	(
	int		distance
	)
{return operator+= (-distance);}

Array::Depth_Iterator
Array::Depth_Iterator::operator-
	(
	int		distance
	) const
{return operator+ (-distance);}

//	Distance.

int
Array::Depth_Iterator::operator-
	(
	const Array::iterator&	list_iterator
	) const
{
int
	distance;
Depth_Iterator
	position;
try
	{
	for (distance = 0,
		 position = *this;
		 position != list_iterator;
		 --position,
		 ++distance) ;
	}
catch (Out_of_Range exception)
	{
	for (distance = 0,
		 position = *this + 1;
		 position != list_iterator;
		 ++position,
		 --distance) ;
	}
return distance;
}

int
Array::Depth_Iterator::operator-
	(
	const Depth_Iterator&	depth_iterator
	) const
{return operator- (depth_iterator.Current_Position);}

/*------------------------------------------------------------------------------
	Logical operators
*/
bool
Array::Depth_Iterator::operator<
	(
	const Depth_Iterator& 	depth_iterator
	) const
{return *this < depth_iterator.Current_Position;}

bool
Array::Depth_Iterator::operator<
	(
	const Array::iterator&	list_iterator
	) const
{
if (*this == list_iterator)
	return false;
Depth_Iterator
	position (*this);
try
	{
	while (++position != list_iterator) ;
	return true;
	}
catch (Out_of_Range exception) {return false;}
}

/*******************************************************************************
	Utility functions
*/
void
sort
	(
	Array::iterator	first,
	Array::iterator	last
	)
{
if (first == last)
	{
	return;
	}
Array::Base_iterator
	bubble,
	this_value;
Value
	*value;
for (--last;
	 first != last;
	 --last)
	{
	bubble = first;
	do
		{
		this_value = bubble;
		 ++bubble;
		if (**bubble < **this_value)
			{
			//	Swap Value pointers.
			value = *bubble;
			*bubble = *this_value;
			*this_value = value;
			}
		}
		while (bubble != last);
	}
}

void
sort
	(
	Array::depth_iterator	first,
	Array::depth_iterator	last
	)
{
if (first == last)
	{
	return;
	}
if (first->parent () !=
	((static_cast<Array::iterator>(last)) - 1)->parent ())
	throw Invalid_Argument
		("Can't sort a range for depth_iterators in different Arrays",
		Value::ID);
sort (static_cast<Array::iterator>(first), static_cast<Array::iterator>(last));
while (first != last)
	{
	if (first->is_Array ())
		sort (
			(static_cast<Array&>(*first)).begin (),
			(static_cast<Array&>(*first)).end ());
	++first;
	}
}

}		//	namespace PVL
}		//	namespace idaeim
