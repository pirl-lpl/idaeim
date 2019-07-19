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

CVS ID: Value.hh,v 1.54 2013/09/17 03:52:51 castalia Exp
*******************************************************************************/

#ifndef idaeim_PVL_Value_hh
#define idaeim_PVL_Value_hh

#include	"Utility/Types.hh"
#include	"PVL/PVL_Exceptions.hh"
#include	"PVL/Vectal.hh"

#include	<string>
#include	<iostream>	//	Needed for default cin and cout arguments.


namespace idaeim
{
namespace PVL
{
//	Forward references.
class Array;
class Parser;

/*=*****************************************************************************
	Value
*/
/**	A <i>Value</i> is a general-purpose, abstract value entity.

	There are two Value categories: a primitive Value - <i>Integer</i>,
	<i>Real</i> or <i>String</i> - has a corresponding type of datum;
	an <i>Array</i> is a list of Values. A digits member is provided
	for use in creating a text representation of a primitive Value: the
	precision of a Numeric Value or the field width of a String. A base
	member is also provided for specifying the radix in creating a text
	representation of a numeric Value. A pointer to a parent value is
	provided for navigating Array hierachies; a Value with no parent is
	not a member of an Array. Arrays list may have Values of mixed
	types and contain Arrays of mixed sizes.

	Numeric Values can be directly converted to/from their primitive
	datum types. This enables them to be used in mathematical
	expressions. A String, when it represents a numerical value, can
	also be converted to an integer or real data type. And, of course, a
	String can be directly converted to/from a std::string. A set of
	logical operators is also provided for all Values so they can be
	meaningfully compared.

	<b>N.B.</b>: The storage precision of Integer and Real values is
	determined at the time the PVL library is compiled by the
	High_Precision_Integer_type (and
	High_Precision_Unsigned_Integer_type) and
	High_Precision_Floating_Point_type typedefs defined in the
	Utility/Types.hh file. These are expected to be either long or long
	long and double or long double, respectively. To ensure complete type
	conversion coverage is provided the idaeim_LONG_LONG_INTEGER is
	defined if long long is used and idaeim_LONG_DOUBLE is defined if
	long double is used. The storage precision of an Integer Value is
	independent of the storage precision of a Real Value.

	Any Value, including an Array, may have a units string that
	provides an arbitrary description for the Value. Provision is
	made for a Value to inherit the units of its parent.

	A <i>Parser</i> is used for interpreting the %Parameter %Value
	Language syntax from text into Value objects. A <i>Lister</i> is
	used for generating %Parameter %Value Language syntax from Value
	objects.

	@see		Integer
	@see		Real
	@see		String
	@see		Array
	@see		Parameter
	@see		Parser
	@see		Lister

	@author		Bradford Castalia
	@version	1.54 
*/
class Value
{
friend class Array;

public:
/*==============================================================================
	Types
*/
/**	Integer Value Datum type.

	@see	Types.hh
*/
typedef High_Precision_Integer_type				Integer_type;
typedef High_Precision_Unsigned_Integer_type	Unsigned_Integer_type;

/*	Real Value Datum type.

	@see	Types.hh
*/
typedef High_Precision_Floating_Point_type		Real_type;

//!	String Value datum type.
typedef	std::string								String_type;

/*==============================================================================
	Constants:
*/
//!	Class identification name with source code version and date.
static const char* const
	ID;

/**	Subtype identifiers.

	There are four specific implementations of Value: Integer, Real,
	String and Array. However, a String and Array can also be
	distinguished in the way they are represented with the PVL syntax.
	Since this distinction is not functional - in fact is a
	characteristic that can be readily changed within the specific type -
	additional subclasses are not used. Instead, a Type code is
	provided and used as a "subtype" characteristic, in addition to
	offering a convenient means of identifying the general type (class)
	of a Value.

	The STRING Type codes are:

	<dl compact>
	<dt>IDENTIFIER	<dd>Unquoted
	<dt>SYMBOL		<dd>Single-quoted (')
	<dt>TEXT		<dd>Double-quoted (")
	<dt>DATE_TIME	<dd>Contains a dash (-) or colon (:)
	</dl>

	The ARRAY Type codes are:

	<dl compact>
	<dt>SET			<dd>Enclosed by curly braces ('{' and '}')
						or no enclosing characters
	<dt>SEQUENCE	<dd>Enclosed by parentheses ('(' and ')')
	</dl>

	INTEGER and REAL Type codes are also provided for the Integer and
	Real classes for consistency. These latter two classes are both
	categorized as NUMERIC as an aid to the application in identifying
	how it will use a Value.

	Type specification codes are bit flags. These are organized such
	that the general Type bit is present for all specific members: Both
	INTEGER and REAL contain the NUMERIC bit; IDENTIFIER, SYMBOL, TEXT
	and DATE_TIME contain the STRING bit; and  both SET and SEQUENCE
	contain the ARRAY bit.

	<b>Note</b>: Value Type codes are guaranteed to be distinct from
	Parameter Type codes: they occupy completely separate bit fields
	and can not be accidently confused.
*/
enum Type
	{
	NUMERIC			= (1 << 3),		// Allows for up to 7 subtypes.
		INTEGER,
		REAL,

	STRING			= (NUMERIC << 1),
		IDENTIFIER,
		SYMBOL,
		TEXT,
		DATE_TIME,

	ARRAY			= (STRING << 1),
		SET,
		SEQUENCE
	};

//!	The range of allowable radix base values.
enum
	{
	MIN_BASE = 2,
	MAX_BASE = 36
	};

/*==============================================================================
	Constructors
*/
/**	Constructs a Value with optional units and digits specifications.

	@param	units	The units string.
	@param	digits	The precision of the value text representation.
		Zero means unpadded (6 digit floatingpoint).
	@param	base	The numeric base of value text representation.
		If the base is not in the range MIN_BASE (2) to MAX_BASE (36),
		inclusive, base 0 (no base) will be used.
*/
Value (const std::string& units = "", const int digits = 0, const int base = 10)
	:	Parent (NULL),
		Units (units),
		Digits (digits),
		Base ((base < MIN_BASE || base > MAX_BASE) ? 0 : base)
	{}

/**	Constructs a Value with specified representation digits.

	@param	digits	The precision of the value text representation.
		Zero means unpadded (6 digit floatingpoint).
	@param	base	The numeric base of value text representation.
		If the base is not in the range MIN_BASE (2) to MAX_BASE (36),
		inclusive, base 0 (no base) will be used.
*/
explicit Value (const int digits, const int base = 10)
	:	Parent (NULL),
		Units (""),
		Digits (digits),
		Base ((base < MIN_BASE || base > MAX_BASE) ? 0 : base)
	{}

/**	Constructs a Value as a copy of another Value.

	<b>N.B.</b>: The Parent is not copied; the new Value has no parent.

	@param	value	The Value to copy.
*/
Value (const Value& value)
	:	Parent (NULL),	// Never inherit the parent.
		Units (value.Units),
		Digits (value.Digits),
		Base (value.Base)
	{}

/**	Creates a Value clone of the Value.

	This method is required by the Array Vectal.

	@see	Vectal
*/
virtual Value* clone () const = 0;

//!	Virtual destructor.
virtual ~Value ()
	{}

/*==============================================================================
	Accessors
*/
/*------------------------------------------------------------------------------
	Parent
*/
/**	Gets the Value's parent.

	<B>Note</B>: The parent of a Value should only be set by entering
	the Value into an Array Value; it should only be cleared by
	removing the Value from its Array.

	@return	A pointer to the parent Array (NULL if the value
		has no parent).
	@see	Array::entering(Value*)
	@see	Array::removing(Value*)
*/
Array* parent () const
	{return Parent;}

/**	Tests if the Value has children Values.

	@return	true if the Value {@link is_Array() is an Array}
		containing one or more Values; false otherwise.
*/
bool has_children () const;

/**	Gets the root Array enclosing this Value.

	@return	A pointer to the root parent Array; i.e. the base of
		the parent hierarchy for this Value. This will be NULL if
		this Value has no parent.
*/
Array* root () const;

/*------------------------------------------------------------------------------
	Type conversion
*/
/**	Casts the Value to an int.

	@see	Integer::operator int
	@see	Real::operator int
	@see	String::operator int
	@see	Array::operator int
*/
virtual operator int () const = 0;

/**	Casts the Value to an unsigned int.

	@see	operator int()
*/
operator unsigned int () const
	{return static_cast<unsigned int>(operator int ());}

#ifdef idaeim_LONG_LONG_INTEGER
/**	Casts the Value to a long.

	@see	Integer::operator long
	@see	Real::operator long
	@see	String::operator long
	@see	Array::operator long
*/
virtual operator long () const = 0;

/**	Casts the Value to an unsigned long.

	@see	operator long()
*/
operator unsigned long () const
	{return static_cast<unsigned long>(operator long ());}
#endif

/**	Casts the Value to an Integer_type.

	@see	Integer::operator Integer_type
	@see	Real::operator Integer_type
	@see	String::operator Integer_type
	@see	Array::operator Integer_type
*/
virtual operator Integer_type () const = 0;

/**	Casts the Value to an Unsigned_Integer_type.

	@see	operator Integer_type()
*/
operator Unsigned_Integer_type () const
	{return static_cast<Unsigned_Integer_type>(operator Integer_type ());}

#ifdef idaeim_LONG_DOUBLE
/**	Casts the Value to a double type.

	@see	Integer::operator double
	@see	Real::operator double
	@see	String::operator double
	@see	Array::operator double
*/
virtual operator double () const = 0;
#endif

/**	Casts the Value to a Real_type.

	@see	Integer::operator Real_type
	@see	Real::operator Real_type
	@see	String::operator Real_type
	@see	Array::operator Real_type
*/
virtual operator Real_type () const = 0;

/**	Casts the Value to a String_type.

	@see	Integer::operator String_type
	@see	Real::operator String_type
	@see	String::operator String_type
	@see	Array::operator String_type
*/
virtual operator String_type () const = 0;

/*------------------------------------------------------------------------------
	Subtype
*/
/**	Tests if the Value is an Integer or Real.

	@return true if the implementing class is Numeric;
		false otherwise.
*/
virtual bool is_Numeric () const
	{return false;}

/**	Tests if the Value is an Integer type.

	@return true if the implementing class is an Integer;
		false otherwise.
*/
virtual bool is_Integer () const
	{return false;}

/**	Tests if the Value is a Real type.

	@return true if the implementing class is a Real;
		false otherwise.
*/
virtual bool is_Real () const
	{return false;}

/**	Tests if the Value is a String type.

	@return true if the implementing class is a String;
		false otherwise.
*/
virtual bool is_String () const
	{return false;}

/**	Tests if the Value is an IDENTIFIER String.

	@return true if the implementing class has an IDENTIFIER Type
		specification; false otherwise.
*/
virtual bool is_Identifier () const
	{return false;}

/**	Tests if the Value is a SYMBOL String.

	@return true if the implementing class has a SYMBOL Type
		specification; false otherwise.
*/
virtual bool is_Symbol () const
	{return false;}

/**	Tests if the Value is a TEXT String.

	@return true if the implementing class has a TEXT Type
		specification; false otherwise.
*/
virtual bool is_Text () const
	{return false;}

/**	Tests if the Value is a DATE_TIME String.

	@return true if the implementing class has a DATE_TIME Type
		specification; false otherwise.
*/
virtual bool is_Date_Time () const
	{return false;}

/**	Tests if the Value is an Array.

	@return true if the implementing class is an Array;
		false otherwise.
*/
virtual bool is_Array () const
	{return false;}

/**	Tests if the Value is a SET Array.

	@return true if the implementing class has a SET Type
		specification; false otherwise.
*/
virtual bool is_Set () const
	{return false;}

/**	Tests if the Value is a SEQUENCE Array.

	@return true if the implementing class has a SEQUENCE Type
		specification; false otherwise.
*/
virtual bool is_Sequence () const
	{return false;}

/**	Sets the Type of the Value.

	@param	type_code	The Type to be set.
	@return	This Value.
	@see	Integer::type(const Type)
	@see	Real::type(const Type)
	@see	String::type(const Type)
	@see	Array::type(const Type)
*/
virtual Value& type (const Type type_code);

/**	Gets the Type of the Value.

	@see	Integer::type()
	@see	Real::type()
	@see	String::type()
	@see	Array::type()
*/
virtual Type type () const = 0;

/**	Gets the name for the Type of the Value.

	@see	type_name(const Type)
*/
virtual std::string type_name () const = 0;

/**	Gets the name for a Type.

	The name of Type is identical to its enum symbol except only the
	first character is uppercase. If the argument is not a recognized
	Type, then the name returned will be "Invalid".

	@param	type_code	A Type code value.
	@return	A name string for the value.
*/
static std::string type_name (const Type type_code);

/*------------------------------------------------------------------------------
	Base
*/
/**	Gets the numeric base.

	@return The numeric base.
*/
virtual int base () const
	{return Base;}

/**	Sets the numeric base.

	As a special case, a base of 0 is accepted for a String. This is
	interpreted to mean that the base should be intuited from the
	string representation.

	@param	base	The numeric base value.
	@return	This Value.
	@throws Out_of_Range If the base argument is not within the
		allowable range of #MIN_BASE (2) to #MAX_BASE (36).
	@see	string_numeric_base(const string&)
*/
virtual Value& base (const int base);

/*------------------------------------------------------------------------------
	Signed
*/
//!	Convience constants for setting/testing is_signed.
static const bool
	UNSIGNED,
	SIGNED;

/**	Tests if the Value is signed.

	@return	true (#SIGNED) if the Value is to be treated as signed;
		false (#UNSIGNED) if the value is to be treated as unsigned.
		Unless a specific class overrides this method it will always
		return true.
*/
virtual bool is_signed () const
	{return true;}

/**	Sets the signed-ness of the Value.

	Unless a specific class overrides this method it does nothing.

	@param	signedness	true (#SIGNED) or false (#UNSIGNED).
	@return	This Value.
*/
virtual Value& is_signed (const bool
	#ifdef DOXYGEN_PROCESSING
	signedness
	#endif
	)
	{return *this;}

/*------------------------------------------------------------------------------
	Units
*/
/**	Gets the units description for the Value.

	@return	The units string for the Value. The string will
		be empty if the Value has no units associated with it.
*/
std::string units () const
	{return Units;}

/**	Gets the "nearest" units description for the Value.

	The units string of the Value is returned. However, if the units
	string is empty and the Value is a member of an Array, then the
	units of the parent Array will be recursively sought. The effect is
	to return the "nearest" non-empty units string, if there is one.

	@return	The units string for the Value or, recursively for the
		parent Array (if there is one) if the units string is empty.
		The string will be empty if, and only if, neither the Value nor
		any of its parents have units associated with it.
*/
std::string nearest_units () const;

/**	Sets the units description for the Value.

	@param	units	The units string to be associated with the
		Value. The string is copied into the Value.
	@return	This Value.
*/
Value& units (const std::string& units)
	{Units = units; return *this;}

/*------------------------------------------------------------------------------
	Digits
*/
/**	Gets the number of digits for the Value representation.

	@return	The number of digits.
*/
int digits () const
	{return Digits;}

/**	Sets the number of digits for the Value representation.

	How the number of digits is used is the responsibility of the
	specific type of Value.

	@param	digits	The number of Value digits.
	@return	This Value.
	@see	integer_to_string
	@see	real_to_string
*/
Value& digits (const int digits)
	{Digits = digits; return *this;}

/*==============================================================================
	Operators
*/
/**	Array notation accessor.

	@param	index	The Array element to reference.
	@return	A reference to the index-th Array element.
	@throws	Invalid_Argument	If the Value is not an Array.
	@see	Array::operator[]
*/
virtual Value& operator[] (unsigned int index);

/**	Array notation accessor.

	@param	index	The Array element to reference.
	@return	A const reference to the index-th Array element.
	@throws	Invalid_Argument	If the Value is not an Array.
	@see	Array::operator[]
*/
virtual const Value& operator[] (unsigned int index) const;

//	Assignments

/**	Assigns another Value to this Value.

	@param	value	The Value to be assigned.
	@see	Integer::operator=(const Value&)
	@see	Real::operator=(const Value&)
	@see	String::operator=(const Value&)
	@see	Array::operator=(const Value&)
*/
virtual Value& operator= (const Value& value) = 0;

/**	Assigns an int value to this Value.

	@param	value	The int value to be assigned.
	@see	Integer::operator=(const int)
	@see	Real::operator=(const int)
	@see	String::operator=(const int)
	@see	Array::operator=(const int)
*/
virtual Value& operator= (const int value) = 0;

#ifdef idaeim_LONG_LONG_INTEGER
/**	Assigns a long value to this Value.

	@param	value	The long value to be assigned.
	@see	Integer::operator=(const long)
	@see	Real::operator=(const long)
	@see	String::operator=(const long)
	@see	Array::operator=(const long)
*/
virtual Value& operator= (const long value) = 0;
#endif

/**	Assigns an Integer_type value to this Value.

	@param	value	The Integer_type value to be assigned.
	@see	Integer::operator=(const Integer_type)
	@see	Real::operator=(const Integer_type)
	@see	String::operator=(const Integer_type)
	@see	Array::operator=(const Integer_type)
*/
virtual Value& operator= (const Integer_type value) = 0;

/**	Assigns an unsigned int value to this Value.

	@param	value	The unsigned int value to be assigned.
	@see	Integer::operator=(const unsigned int)
	@see	Real::operator=(const unsigned int)
	@see	String::operator=(const unsigned int)
	@see	Array::operator=(const unsigned int)
*/
virtual Value& operator= (const unsigned int value) = 0;

#ifdef idaeim_LONG_DOUBLE
/**	Assigns a double value to this Value.

	@param	value	The double value to be assigned.
	@see	Integer::operator=(const double)
	@see	Real::operator=(const double)
	@see	String::operator=(const double)
	@see	Array::operator=(const double)
*/
virtual Value& operator= (const double value) = 0;
#endif

/**	Assigns a Real_type value to this Value.

	@param	value	The Real_type value to be assigned.
	@see	Integer::operator=(const Real_type)
	@see	Real::operator=(const Real_type)
	@see	String::operator=(const Real_type)
	@see	Array::operator=(const Real_type)
*/
virtual Value& operator= (const Real_type value) = 0;

/**	Assigns a String_type value to this Value.

	@param	value	The String_type value to be assigned.
	@see	Integer::operator=(const String_type)
	@see	Real::operator=(const String_type)
	@see	String::operator=(const String_type)
	@see	Array::operator=(const String_type)
*/
virtual Value& operator= (const String_type& value) = 0;

/**	Assigns a C-string (char*) value to this Value.

	@param	value	The C-string value to be assigned.
	@see	Integer::operator=(const char*)
	@see	Real::operator=(const char*)
	@see	String::operator=(const char*)
	@see	Array::operator=(const char*)
*/
virtual Value& operator= (const char* value) = 0;

//	Addition

/**	Adds another Value to this Value.

	The meaning of addition is the responsibility of the specific
	type of Value.

	@param	value	The Value to be added.
	@see	Integer::operator+=(const Value&)
	@see	Real::operator+=(const Value&)
	@see	String::operator+=(const Value&)
	@see	Array::operator+=(const Value&)
*/
virtual Value& operator+= (const Value& value) = 0;

//	Logical

/**	Logically compares another Value to this Value.

	The meaning of the comparison is the responsibility of the specific
	type of Value.

	@param	value	The Value to be compared against.
	@return	An int that is less than, equal to, or greater than 0 if
		this Value is logically less than, equal to, or greater
		than, respectively, the other Value.
	@see	Integer::compare(const Value&) const
	@see	Real::compare(const Value&) const
	@see	String::compare(const Value&) const
	@see	Array::compare(const Value&) const
*/
virtual int compare (const Value& value) const = 0;

/**	Test if this Value is logically equivalent to another Value.

	@param	value	The Value to compare against.
	@return	true or false based on how the Values compare.
	@see	compare(const Value&) const
*/
bool operator== (const Value& value) const
	{return compare (value) == 0;}

/**	Test if this Value is logically less than another Value.

	@param	value	The Value to compare against.
	@return	true or false based on how the Values compare.
	@see	compare(const Value&) const
*/
bool operator<  (const Value& value) const
	{return compare (value) <  0;}

/**	Test if a Value is logically not equivalent to another Value.

	@param	value	The Value to compare against.
	@return	true or false based on how the Values compare.
	@see	compare(const Value&) const
*/
bool operator!= (const Value& value) const
	{return compare (value) != 0;}

/**	Test if a Value is logically greater than another Value.

	@param	value	The Value to compare against.
	@return	true or false based on how the Values compare.
	@see	compare(const Value&) const
*/
bool operator>  (const Value& value) const
	{return compare (value) >  0;}

/**	Test if a Value is logically less than or equal to another Value.

	@param	value	The Value to compare against.
	@return	true or false based on how the Values compare.
	@see	compare(const Value&) const
*/
bool operator<= (const Value& value) const
	{return compare (value) <= 0;}

/**	Test if a Value is logically greater than or equal to another Value.

	@param	value	The Value to compare against.
	@return	true or false based on how the Values compare.
	@see	compare(const Value&) const
*/
bool operator>= (const Value& value) const
	{return compare (value) >= 0;}

/*==============================================================================
	I/O
*/
//	Write

/**	Writes the Value to an ostream.

	<b>N.B.</b>: The output is not terminated with an end-of-line. It is,
	however, flushed to the ostream.

	@param	out	The ostream to which the PVL syntax will be written.
	@param	depth	The indent depth to be applied to Array Values.
		A negative depth disables indenting.
	@return	The number of characters written.
	@see	Lister::write(const Value&, int)
*/
unsigned int write
	(std::ostream& out = std::cout, int depth = -1) const;

/**	Prints the Value ot an ostream.

	The Value is written to the stream with no indenting.

	@param	out	The ostream to which the PVL syntax will be written.
	@return	The ostream.
	@see	write(std::ostream&, int) const
*/
std::ostream& print (std::ostream& out) const
	{write (out); return out;}


//	Read

/**	Reads an input stream parsed as PVL syntax into this Value.

	A Parser is constructuced from the input stream and used to
	obtain the next Value interpreted from PVL syntax in the stream.

	@param	in	The istream to be parsed for PVL syntax.
	@see	read(Parser&)
*/
void read (std::istream& in = std::cin);

/**	Assigns the next Value from a Parser to this Value.

	The next Value interpreted from PVL syntax is obtained from the
	Parser. The Value obtained is assigned to this Value.

	<b>Warning</b>: Unless the next PVL item available to the Parser is
	a Value, this method will fail. Also, the type of Value obtained by
	the must be assignable to this Value or the method will fail. Any
	type of Value may be assigned to an Array.

	@param	parser	The Parser from which to obtain a Value.
	@see	operator=(const Value&)
	@see	Parser.get_value()
*/
void read (Parser& parser);

/*==============================================================================
	Helpers
*/
/**	Converts an integer value to its string representation.

	@param	value	The Integer_type value to be converted.
	@param	is_signed	Either #SIGNED or #UNSIGNED.
	@param	base	The numeric base interpretation of the string
		representation. A base of 0 is the same as base 10.
	@param	digits	The number of digits in the representation.
		The string will be padded with leading 0's as needed.
	@see	is_signed(bool)
	@see	idaeim::Strings::String(long, bool, int, int)
*/
static std::string integer_to_string
	(
	const Integer_type	value,
	const bool			is_signed = SIGNED,
	const int			base = 10,
	const int			digits = 0
	);

/**	Converts a string to the integer value it represents.

	If the numeric base is 0, it will be intuited from the string: a
	leading sign is ignored; a leading "0x" or "0X" indicates
	hexadecimal (base 16) notation, a leading '0' alone indicates octal
	(base 8) notation, while anything else is taken to be decimal
	notation. To prevent padding with leading '0' characters to be
	interpreted as octal specify the base.

	A conversion that succeeds up to a decimal point character ('.') is
	accepted as a real number representation truncated to an integer.
	Thus the conversion can be fooled by an otherwise invalid string.

	Leading and trailing whitespace in the string is ignored.

	@param	value	The string representation of the value to be
		converted.
	@param	base	The numeric base interpretation of the string
		representation.
	@return	The Integer_type that the string represents.
	@throws	Out_of_Range	If the value represented is out of the
		range of what can be represented by an Integer_type.
	@throws	Invalid_Value	If the value does not represent a
		numeric value.
	@see	string_numeric_base
*/
static Integer_type string_to_integer
	(
	const std::string&	value,
	const int			base = 0
	);

/**	Attemps to intuit the numeric base of string.

	Leading whitespace is ignored and, after any whitespace, a leading
	sign character ('+' or '-') is also ignored; a leading "0x" or "0X"
	indicates hexadecimal (base 16) notation, a leading '0' alone
	indicates octal (base 8) notation, while anything else is taken to
	be decimal notation.

	An internal test conversion based on the tentative surmise of the
	base representation is attempted. If the conversion fails a value
	of 0 is returned indicating that the string does not represent a
	numeric value. An empty string is defined as base 10.

	@param	value	The string to interpret.
	@return	The numeric base of the value the string represents,
		or 0 if the string does not represent a value.
	@see	string_to_integer
*/
static unsigned int string_numeric_base (const std::string& value);

/**	Converts a floating point Real_type value to its string
	representation using optional format controls.

	The real value representation is generated by an ostringstream. If
	format flags and/or a precision value is specified they are applied
	to the ostringstream. <b>N.B.</b>: The showpoint format flag is
	always applied. If a non-zero precision is specified, it is also
	applied; otherwise, if neither the fixed nor scientific format
	flags are specified, at least one digit after the decimal point
	will be used.

	@param	value	The Real_type value to be converted.
	@param	format	The fmtflags to be applied to the conversion.
	@param	precision	An unsigned int precision value to be applied
		to the conversion (0 implies default precision).
	@return	A string representation of the value.
*/
static std::string real_to_string
	(
	const Real_type		value,
	std::ios::fmtflags	format = std::ios::showpoint,
	unsigned int		precision = 0
	);

/**	Converts a string to the floating point value it represents.

	Leading and trailing whitespace in the string is ignored.

	@param	value	The string value to be converted.
	@return	The Real_type that the string represents.
	@throws	Out_of_Range	If the value represented is out of the
		range of what can be represented by an Real_type.
	@throws	Invalid_Value	If the value does not represent a
		numeric value.
*/
static Real_type string_to_real
	(
	const std::string&	value
	);

//..............................................................................
protected:

/**	The Array of which this Value is a member,
	or NULL if not a member of an Array.
*/
Array				*Parent;

/**	The units description string.

	@see	units(const std::string&)
*/
std::string			Units;

/**	The number of digits in the Value representation.

	@see	digits(const int)
*/
int					Digits;

/**	The numeric base of the Value.

	@see	base(const int)
*/
int					Base;

};	//	class Value

/*==============================================================================
	I/O operators
*/
/**	Writes the Value to an ostream.

	The Value is written with the indent depth set to 0.

	@param	out		The ostream to which the PVL syntax will be written.
	@param	value	The Value to be written.
	@return	The ostream.
	@see	Value::write(std::ostream&, int)
*/
std::ostream& operator<< (std::ostream& out, const Value& value);

/**	Reads an input stream parsed as PVL syntax into a Value.

	@param	in	The istream to be parsed for PVL syntax.
	@param	value	The Value to be assigned the Value definition
		that is read.
	@return	The in instream.
	@see	read(Parser&)
*/
std::istream& operator>> (std::istream& in, Value& value);

/*=*****************************************************************************
	Datum subclasses:
*/
/*==============================================================================
	Integer
*/
/**	An <i>Integer</i> implements a Value with an integer datum.

	An Integer may be treated as signed or unsigned.

	An Integer may always be cast to either a Real or String Value.
*/
class Integer
:	public Value
{
public:

//	Constructors

//		Type casting
//			Integer

/**	Constructs an Integer from an Integer_type datum.

	@param	datum	The Integer_type datum to be assigned the initial
		value of the Integer.
	@param	is_signed	true (#SIGNED) if the datum is to be treated as a
		signed value; false (#UNSIGNED) otherwise.
	@param	base	The numeric base to be used for a string representation
		of the Integer.
	@param	digits	The minimum number of digits to be used for a string
		representation; 0 padding is used.
*/
Integer (const Integer_type datum = 0,
	const bool is_signed = SIGNED, const int base = 10, const int digits = 0);

/**	Constructs an Integer from an Integer_type datum.

	The Integer is marked as #SIGNED.

	@param	datum	The Integer_type datum to be assigned the initial
		value of the Integer.
	@param	base	The numeric base to be used for a string representation
		of the Integer.
	@param	digits	The minimum number of digits to be used for a string
		representation; 0 padding is used.
*/
Integer (const Integer_type datum,
	const int base, const int digits = 0);

/**	Constructs an Integer from an Unsigned_Integer_type datum.

	@param	datum	The Unsigned_Integer_type datum to be assigned the initial
		value of the Integer.
	@param	is_signed	true (#SIGNED) if the datum is to be treated as a
		signed value; false (#UNSIGNED) otherwise.
	@param	base	The numeric base to be used for a string representation
		of the Integer.
	@param	digits	The minimum number of digits to be used for a string
		representation; 0 padding is used.
*/
Integer (const Unsigned_Integer_type datum,
	const bool is_signed = UNSIGNED, const int base = 10, const int digits = 0);

/**	Constructs an Integer from an Unsigned_Integer_type datum.

	The Integer is marked as #UNSIGNED.

	@param	datum	The Unsigned_Integer_type datum to be assigned the initial
		value of the Integer.
	@param	base	The numeric base to be used for a string representation
		of the Integer.
	@param	digits	The minimum number of digits to be used for a string
		representation; 0 padding is used.
*/
Integer (const Unsigned_Integer_type datum,
	const int base, const int digits = 0);

#ifdef idaeim_LONG_LONG_INTEGER
/**	Constructs an Integer from a long datum.

	@param	datum	The long datum to be assigned the initial value of
		the Integer.
	@param	is_signed	true (#SIGNED) if the datum is to be treated as a
		signed value; false (#UNSIGNED) otherwise.
	@param	base	The numeric base to be used for a string representation
		of the Integer.
	@param	digits	The minimum number of digits to be used for a string
		representation; 0 padding is used.
*/
Integer (const long datum,
	const bool is_signed = SIGNED, const int base = 10, const int digits = 0);

/**	Constructs an Integer from a long datum.

	The Integer is marked as #SIGNED.

	@param	datum	The long datum to be assigned the initial value of
		the Integer.
	@param	base	The numeric base to be used for a string representation
		of the Integer.
	@param	digits	The minimum number of digits to be used for a string
		representation; 0 padding is used.
*/
Integer (const long datum,
	const int base, const int digits = 0);

/**	Constructs an Integer from an unsigned long datum.

	@param	datum	The unsigned long datum to be assigned the initial
		value of the Integer.
	@param	is_signed	true (#SIGNED) if the datum is to be treated as a
		signed value; false (#UNSIGNED) otherwise.
	@param	base	The numeric base to be used for a string representation
		of the Integer.
	@param	digits	The minimum number of digits to be used for a string
		representation; 0 padding is used.
*/
Integer (const unsigned long datum,
	const bool is_signed = UNSIGNED, const int base = 10, const int digits = 0);

/**	Constructs an Integer from an unsigned long datum.

	The Integer is marked as #UNSIGNED.

	@param	datum	The unsigned long datum to be assigned the initial
		value of the Integer.
	@param	base	The numeric base to be used for a string representation
		of the Integer.
	@param	digits	The minimum number of digits to be used for a string
		representation; 0 padding is used.
*/
Integer (const unsigned long datum,
	const int base, const int digits = 0);
#endif

/**	Constructs an Integer from an int datum.

	@param	datum	The int datum to be assigned the initial value of
		the Integer.
	@param	is_signed	true (#SIGNED) if the datum is to be treated as a
		signed value; false (#UNSIGNED) otherwise.
	@param	base	The numeric base to be used for a string representation
		of the Integer.
	@param	digits	The minimum number of digits to be used for a string
		representation; 0 padding is used.
*/
Integer (const int datum,
	const bool is_signed = SIGNED, const int base = 10, const int digits = 0);

/**	Constructs an Integer from an int datum.

	The Integer is marked as #SIGNED.

	@param	datum	The int datum to be assigned the initial value of
		the Integer.
	@param	base	The numeric base to be used for a string representation
		of the Integer.
	@param	digits	The minimum number of digits to be used for a string
		representation; 0 padding is used.
*/
Integer (const int datum,
	const int base, const int digits = 0);

/**	Constructs an Integer from an unsigned int datum.

	@param	datum	The unsigned int datum to be assigned the initial
		value of the Integer.
	@param	is_signed	true (#SIGNED) if the datum is to be treated as a
		signed value; false (#UNSIGNED) otherwise.
	@param	base	The numeric base to be used for a string representation
		of the Integer.
	@param	digits	The minimum number of digits to be used for a string
		representation; 0 padding is used.
*/
Integer (const unsigned int datum,
	const bool is_signed = UNSIGNED, const int base = 10, const int digits = 0);

/**	Constructs an Integer from an unsigned int datum.

	The Integer is marked as #UNSIGNED.

	@param	datum	The unsigned int datum to be assigned the initial
		value of the Integer.
	@param	base	The numeric base to be used for a string representation
		of the Integer.
	@param	digits	The minimum number of digits to be used for a string
		representation; 0 padding is used.
*/
Integer (const unsigned int datum,
	const int base, const int digits = 0);

//			Real
#ifdef idaeim_LONG_DOUBLE
/**	Constructs an Integer from a double datum.

	@param	datum	The double datum, cast to an Integer_type, to be
		assigned the initial value of the Integer.
	@param	is_signed	true (#SIGNED) if the datum is to be treated as a
		signed value; false (#UNSIGNED) otherwise.
	@param	base	The numeric base to be used for a string representation
		of the Integer.
	@param	digits	The minimum number of digits to be used for a string
		representation; 0 padding is used.
*/
explicit Integer (const double datum,
	const bool is_signed = SIGNED, const int base = 10, const int digits = 0);

/**	Constructs an Integer from a Real_type datum.

	The Integer is marked as #SIGNED.

	@param	datum	The Real_type datum, cast to an Integer_type, to be
		assigned the initial value of the Integer.
	@param	base	The numeric base to be used for a string representation
		of the Integer.
	@param	digits	The minimum number of digits to be used for a string
		representation; 0 padding is used.
*/
explicit Integer (const double datum,
	const int base, const int digits = 0);
#endif

/**	Constructs an Integer from a Real_type datum.

	@param	datum	The Real_type datum, cast to an Integer_type, to be
		assigned the initial value of the Integer.
	@param	is_signed	true (#SIGNED) if the datum is to be treated as a
		signed value; false (#UNSIGNED) otherwise.
	@param	base	The numeric base to be used for a string representation
		of the Integer.
	@param	digits	The minimum number of digits to be used for a string
		representation; 0 padding is used.
*/
explicit Integer (const Real_type datum,
	const bool is_signed = SIGNED, const int base = 10, const int digits = 0);

/**	Constructs an Integer from a Real_type datum.

	The Integer is marked as #SIGNED.

	@param	datum	The Real_type datum, cast to an Integer_type, to be
		assigned the initial value of the Integer.
	@param	base	The numeric base to be used for a string representation
		of the Integer.
	@param	digits	The minimum number of digits to be used for a string
		representation; 0 padding is used.
*/
explicit Integer (const Real_type datum,
	const int base, const int digits = 0);

//			String

/**	Constructs an Integer from a String_type value representation.

	The intial value of the Integer will be taken from the conversion
	of the string representation to an integer.

	@param	datum	The String_type representation of the initial
		value to be assigned to the Integer.
	@param	is_signed	true (#SIGNED) if the datum is to be treated as a
		signed value; false (#UNSIGNED) otherwise.
	@param	base	The numeric base to be used for a string representation
		of the Integer. If this is 0 it will be intuited from the string
		representation.
	@param	digits	The minimum number of digits to be used for a string
		representation; 0 padding is used.
	@see	string_to_integer
	@see	string_numeric_base
*/
explicit Integer (const String_type& datum,
	const bool is_signed = SIGNED, const int base = 0, const int digits = 0);

/**	Constructs an Integer from a String_type value representation.

	The intial value of the Integer will be taken from the conversion
	of the string representation to an integer.

	The Integer is marked as #SIGNED.

	@param	datum	The String_type value representation.
	@param	base	The numeric base to be used for a string representation
		of the Integer. If this is 0 it will be intuited from the string
		representation.
	@param	digits	The minimum number of digits to be used for a string
		representation; 0 padding is used.
	@see	string_to_integer
	@see	string_numeric_base
*/
explicit Integer (const String_type& datum,
	const int base, const int digits = 0);

//		Copy

/**	Constructs an Integer as a copy of another Integer.

	The parent is not copied; the new Integer has no parent.

	@param	value	The Integer to copy.
*/
Integer (const Integer& value);

//		Copy virtual

/**	Constructs an Integer from another Value.

	All characteristics of the Value, except its parent, are copied.
	The initial value of the Integer is the Value cast to an Integer_type.

	@param	value	The Value to be used to derive this Integer.
	@throws	Invalid_Argument	If the Value can not be cast to an
		Integer_type.
*/
Integer (const Value& value);

//		Clone

/**	Constructs a clone of this Integer.

	A clone is a copy constructed on the heap.

	@return	A pointer to the new Integer clone.
*/
Integer* clone () const
	{return new Integer (*this);}

//	Type conversion

/**	Casts the Integer to an int type.

	@return	The datum int value.
*/
operator int () const
	{return static_cast<int>(Datum);}

#ifdef idaeim_LONG_LONG_INTEGER
/**	Casts the Integer to a long type.

	@return	The datum cast to a long value.
*/
operator long () const
	{return static_cast<long>(Datum);}
#endif

/**	Casts the Integer to an Integer_type.

	@return	The datum Integer_type value.
*/
operator Integer_type () const
	{return Datum;}

#ifdef idaeim_LONG_DOUBLE
/**	Casts the Integer to a double type.

	If the Integer is unsigned the double value will be cast from
	the Integer cast to an unsigned Integer_type.

	@return	The datum cast to a double value.
	@see	is_signed()
*/
operator double () const
	{return static_cast<double>(Datum);}
#endif

/**	Casts the Integer to a Real_type.

	If the Integer is unsigned the Real_type value will be cast from
	the Integer cast to an unsigned Integer_type.

	@return	The datum cast to a Real_type value.
	@see	is_signed()
*/
operator Real_type () const;

/**	Casts the Integer to a String_type.

	The String_type representation of the datum is produced. Whether
	the value is_signed, its radix base and the number of digits for
	the representation are used.

	@return	A String_type representation of the datum.
	@see	integer_to_string
*/
operator String_type () const;

//	Signed

/**	Tests if the Integer is to be treated as a signed value.

	@return	true if the Integer is signed; false otherwise.
*/
bool is_signed () const
	{return Signed;}

/**	Sets the signed-ness of the Value.

	@param	sign	true (#SIGNED) if the Value is signed; false
		(#UNSIGNED) if unsigned.
	@return	This Integer.
*/
Integer& is_signed (const bool sign)
	{Signed = sign; return *this;}

//	Subtype

/**	Asserts that the Integer is a NUMERIC Type.

	@return	Always returns true.
*/
bool is_Numeric () const
	{return true;}

/**	Asserts that the Integer is an INTEGER Type.

	@return	Always returns true.
*/
bool is_Integer () const
	{return true;}

/**	Gets the INTEGER Type code.

	@return	Always returns the INTEGER Type code.
*/
Type type () const
	{return Value::INTEGER;}

/**	Gets the name string for the INTEGER Type.

	@return	The {@link Value::type_name(const Type) type_name} of an
		INTEGER Type.
*/
std::string type_name () const
	{return Value::type_name (Value::INTEGER);}

//	Operators

//		Assignment

/**	Assigns a Value to this Integer.

	The datum for this Integer is the Value cast to an Integer_type.

	If the base of the Value is 0 and the Value is a String, then the
		base is intuited from the String characters.

	All other characteristics of the Value, except its parent, are
	assigned directly.

	@param	value	The Value to be assigned.
	@return	This Integer.
	@throws	Invalid_Argument	If the Value can not be cast to an
		Integer_Type.
	@see	string_numeric_base
*/
Integer& operator= (const Value& value);

/**	Assigns an int type value to this Integer.

	The datum for this Integer is assigned the int value.

	@param	value	The int value to be assigned.
	@return	This Integer.
*/
Integer& operator= (const int value)
	{Datum = value; return *this;}

/**	Assigns an unsigned int type value to this Integer.

	The datum for this Integer is assigned the unsigned int value.

	This Integer is marked as #UNSIGNED.

	@param	value	The unsigned int value to be assigned.
	@return	This Integer.
*/
Integer& operator= (const unsigned int value)
	{Datum = value; Signed = UNSIGNED; return *this;}

/**	Assigns an Integer_type value to this Integer.

	The datum for this Integer is assigned the Integer_type value.

	@param	value	The Integer_type value to be assigned.
	@return	This Integer.
*/
Integer& operator= (const Integer_type value)
	{Datum = value; return *this;}

#ifdef idaeim_LONG_LONG_INTEGER
/**	Assigns a long type value to this Integer.

	The datum for this Integer is assigned the long value.

	@param	value	The long value to be assigned.
	@return	This Integer.
*/
Integer& operator= (const long value)
	{Datum = value; return *this;}

/**	Assigns an unsigned long type value to this Integer.

	The datum for this Integer is assigned the unsigned long value.

	This Integer is marked as #UNSIGNED.

	@param	value	The unsigned long value to be assigned.
	@return	This Integer.
*/
Integer& operator= (const unsigned long value)
	{Datum = value; Signed = UNSIGNED; return *this;}
#endif

#ifdef idaeim_LONG_DOUBLE
/**	Assigns a double value to this Integer.

	The datum for this Integer is assigned the double value
	cast to an Integer_type.

	@param	value	The double value to be assigned.
	@return	This Integer.
*/
Integer& operator= (const double value)
	{Datum = static_cast<Integer_type>(value); return *this;}
#endif

/**	Assigns a Real_type value to this Integer.

	The datum for this Integer is assigned the Real_type value
	cast to an Integer_type.

	@param	value	The Real_type value to be assigned.
	@return	This Integer.
*/
Integer& operator= (const Real_type value)
	{Datum = static_cast<Integer_type>(value); return *this;}

/**	Assigns a string representation of a value to this Integer.

	If the base of the string representation can be intuited, it is
	assigned as the Base of this Integer.

	The datum for this Integer is assigned the value of the string
	representation parsed as an integer value.

	@param	value	The String_type value to be assigned.
	@return	This Integer.
	@throws	Invalid_Argument	If the string representation can not be 
		parsed as an integer value.
	@see	string_numeric_base
	@see	string_to_integer
*/
Integer& operator= (const String_type& value);

/**	Assigns a string representation of a value to this Integer.

	If the character string is non-NULL, an assignment of the
	character string as a String_type is done. Otherwise an
	assignment of an emtpy String_type is done.

	@param	value	A pointer to a character string (C-string).
	@return	This Integer.
	@see	operator=(const String_type&)
*/
Integer& operator= (const char* value);

//		Addition

/**	Adds another Value to this Value.

	The Value cast to an Integer_type is added to the datum of
	this Integer.

	@param	value	The Value to be added to this Integer.
	@return	This Integer.
	@throws	Invalid_Argument	If the Value can not be cast to an
		Integer_type.
*/
Integer& operator+= (const Value& value);

/**	Adds this Integer and another Value.

	This Integer is copied, onto the stack, and the Value is added to it.
	The temporary copy of the stack value that is returned is expected
	to be immediately assigned or otherwise used.

	@param	value	The Value to be used in the addition operation.
	@return	A temporary Integer.
	@see	operator+=(const Value&)
*/
Integer  operator+  (const Value& value) const;

//		Logical

/**	This Integer is compared to another Value.

	An attempt is made to compare the Values numerically by casting
	both to Real_type values. If that fails, because the other Value
	can not be cast to a Real_type, then the comparison is made
	lexicographically by casting both Values to a String_type.

	<b>Note</b>: An Array always compares greater than other types of
	Values.

	@param	value	The Value to be compared against.
	@return	An int that is less than, equal to, or greater than 0 if
		this Integer is logically less than, equal to, or greater
		than, respectively, the other Value.
*/
int compare (const Value& value) const;

//..............................................................................
private:

//!	The integer datum.
Integer_type		Datum;
//!	Flag for treating the datum as signed.
bool				Signed;

};	//	class Integer

/*==============================================================================
	Real
*/
/**	A <i>Real</i> implements a Value with a floating point datum.

	A Real may always be cast to either an Integer or String Value.
*/
class Real
:	public Value
{
public:

//	Constructors

#ifdef idaeim_LONG_DOUBLE
/**	Constructs a Real from a double datum.

	@param	datum	The double datum to be assigned the initial
		value of the Real.
	@param	digits	The number of digits of precision to use for a
		string representation of the Real.
*/
Real (const double datum = 0, const int digits = 0);
#endif

/**	Constructs a Real from a Real_type datum.

	@param	datum	The Real_type datum to be assigned the initial
		value of the Real.
	@param	digits	The number of digits of precision to use for a
		string representation of the Real.
*/
Real (const Real_type datum = 0, const int digits = 0);

/**	Constructs a Real from a String_type value representation.

	@param	datum	The String_type representation of the initial value
		to be assigned to the Real.
	@param	digits	The number of digits of precision to use for a
		string representation of the Real.
	@see	string_to_real
*/
explicit Real (const String_type& datum, const int digits = 0);

//		Copy

/**	Constructs a Real as a copy of another Real.

	The parent is not copied; the new Real has no parent.

	@param	value	The Real to copy.
*/
Real (const Real& value);

//		Copy virtual

/**	Constructs a Real from another Value.

	All characteristics of the Value, except its parent, are copied.
	The initial value of the Real is the Value cast to a Real_type.

	@param	value	The Value to be used to derive this Real.
*/
Real (const Value& value);

//		Clone

/**	Constructs a clone of this Real.

	A clone is a copy constructed on the heap.

	@return	A pointer to the new Real clone.
*/
Real* clone () const
	{return new Real (*this);}

//	Type conversion

/**	Casts the Real to an Integer_type type.

	@return	The datum cast to an Integer_type value.
*/
operator Integer_type () const
	{return static_cast<Integer_type>(Datum);}

#ifdef idaeim_LONG_LONG_INTEGER
/**	Casts the Real to a long type.

	@return	The datum cast to a long value.
*/
operator long () const
	{return static_cast<long>(Datum);}
#endif

/**	Casts the Real to an int type.

	@return	The datum cast to an int value.
*/
operator int () const
	{return static_cast<int>(Datum);}

#ifdef idaeim_LONG_DOUBLE
/**	Casts the Real to a double type.

	@return	The datum double value.
*/
operator double () const
	{return static_cast <double>(Datum);}
#endif

/**	Casts the Real to a Real_type.

	@return	The datum Real_type value.
*/
operator Real_type () const
	{return Datum;}

/**	Casts the Real to a String_type.

	The String_type representation of the datum is produced. The number
	of precision digits and the format flags for the representation are
	used.

	@return	A String_type representation of the datum.
	@see	real_to_string
	@see	format_flags(std::ios::fmtflags)
*/
operator String_type () const;

//	Base

/**	Prevents the base from being changed.

	A Real Value can only be base 10.

	@return	This Real.
*/
Real& base (const int)
	{return *this;}

//	Subtype

/**	Asserts that the Real is a NUMERIC Type.

	@return	Always returns true.
*/
bool is_Numeric () const
	{return true;}

/**	Asserts that the Real is a REAL Type.

	@return	Always returns true.
*/
bool is_Real () const
	{return true;}

/**	Gets the REAL Type code.

	@return	Always returns the REAL Type code.
*/
Type type () const
	{return Value::REAL;}

/**	Gets the name string for the REAL Type.

	@return	The {@link Value::type_name(const Type) type_name} of a REAL Type.
*/
std::string type_name () const
	{return Value::type_name (Value::REAL);}

//	Operators

//		Assignment

/**	Assigns a Value to this Real.

	The datum for this Real is the Value cast to a Real_type.

	All other characteristics of the Value, except its parent, are
	assigned directly.

	@param	value	The Value to be assigned.
	@return	This Real.
	@throws	Invalid_Argument	If the Value can not be cast to a
		Real_Type.
*/
Real& operator= (const Value& value);

/**	Assigns an int type value to this Real.

	The datum for this Real is assigned the int value.

	@param	value	The int value to be assigned.
	@return	This Real.
*/
Real& operator= (const int value)
	{Datum = value; return *this;}

/**	Assigns an unsigned int type value to this Real.

	The datum for this Real is assigned the unsigned int value.

	@param	value	The unsigned int value to be assigned.
	@return	This Real.
*/
Real& operator= (const unsigned int value)
	{Datum = value; return *this;}

#ifdef idaeim_LONG_LONG_INTEGER
/**	Assigns a long type value to this Real.

	The datum for this Real is assigned the long value.

	@param	value	The long value to be assigned.
	@return	This Real.
*/
Real& operator= (const long value)
	{Datum = value; return *this;}
#endif

/**	Assigns an Integer_type value to this Real.

	The datum for this Real is assigned the Integer_type value.

	@param	value	The Integer_type value to be assigned.
	@return	This Real.
*/
Real& operator= (const Integer_type value)
	{Datum = static_cast<Real_type>(value); return *this;}

#ifdef idaeim_LONG_DOUBLE
/**	Assigns a double value to this Real.

	The datum for this Real is assigned the double value.

	@param	value	The double value to be assigned.
	@return	This Real.
*/
Real& operator= (const double value)
	{Datum = value; return *this;}
#endif

/**	Assigns a Real_type value to this Real.

	The datum for this Real is assigned the Real_type value.

	@param	value	The Real_type value to be assigned.
	@return	This Real.
*/
Real& operator= (const Real_type value)
	{Datum = value; return *this;}

/**	Assigns a string representation of a value to this Real.

	The datum for this Real is assigned the value of the string
	representation parsed as a floating point value.

	@param	value	The String_type value to be assigned.
	@return	This Real.
	@throws	Invalid_Argument	If the string representation can not be 
		parsed as a floating point value.
	@see	string_to_real
*/
Real& operator= (const String_type& value);

/**	Assigns a string representation of a value to this Integer.

	If the character string is non-NULL, an assignment of the
	character string as a String_type is done. Otherwise an
	assignment of an emtpy String_type is done.

	@param	value	A pointer to a character string (C-string).
	@return	This Real.
	@see	operator=(const String_type&)
*/
Real& operator= (const char* value);

//		Addition

/**	Adds another Value to this Value.

	The Value cast to a Real_type is added to the datum of
	this Real.

	@param	value	The Value to be added to this Real.
	@return	This Real.
	@throws	Invalid_Argument	If the Value can not be cast to a
		Real_type.
*/
Real& operator+= (const Value& value);

/**	Adds this Real and another Value.

	This Real is copied, onto the stack, and the Value is added to it.
	The temporary copy of the stack value that is returned is expected
	to be immediately assigned or otherwise used.

	@param	value	The Value to be used in the addition operation.
	@return	A temporary Real.
	@see	operator+=(const Value&)
*/
Real  operator+  (const Value& value) const;

//		Logical

/**	This Real is compared to another Value.

	An attempt is made to compare the Values numerically by casting
	both to Real_type values. If that fails, because the other Value
	can not be cast to a Real_type, then the comparison is made
	lexicographically by casting both Values to a String_type.

	<b>Note</b>: An Array always compares greater than other types of
	Values.

	@param	value	The Value to be compared against.
	@return	An int that is less than, equal to, or greater than 0 if
		this Real is logically less than, equal to, or greater
		than, respectively, the other Value.
*/
int compare (const Value& value) const;

//	Format flags

/**	Gets the string representation format flags.

	@return	The std::ios::fmtflags in effect.
	@see	format_flags(std::ios::fmtflags)
*/
std::ios::fmtflags format_flags () const
	{return Format_Flags;}

/**	Sets the string representation format flags.

	Typical format flags are fixed or scientific.

	@param	flags	The std::ios::fmtflags to be used when producing a
		string representation of this Real.
	@return	This Real.
	@see	real_to_string
*/
Real& format_flags (std::ios::fmtflags flags)
	{Format_Flags = flags; return *this;}

/**	Gets the default format flags for a new Real.

	<b>Note</b>: The initial default format flags are
	std::ios::showpoint; however, this setting may be changed at
	compile time by the definition of DEFAULT_FORMAT_FLAGS.

	@return	The current default std::ios::fmtflags value.
*/
static std::ios::fmtflags default_format_flags ()
	{return Default_Format_Flags;}

/**	Sets the default format flags for a new Real.

	@param	flags	The std::ios::fmtflags to be used when constructing
		a new Real.
	@return	The flags.
*/
static std::ios::fmtflags default_format_flags
	(std::ios::fmtflags	flags)
	{return Default_Format_Flags = flags;}

//..............................................................................
private:

//!	The floating point datum.
Real_type					Datum;

//!	The string representation formatting flags.
std::ios::fmtflags			Format_Flags;
static std::ios::fmtflags	Default_Format_Flags;

};	//	class Real

/*==============================================================================
	String
*/
/**	A <i>String</i> implements a Value with a character string datum.
*/
class String
:	public Value
{
public:

//	Constructors

/**	Constructs a String from a String_type datum.

	The Type of the String is intuited by examining the String_type datum.

	The numeric base for a String that represents an integer value
	will be intuited be examining the String_type datum.

	@param	datum	The String_type datum to be assigned the initial
		value of the String.
	@see	type(String_type&)
	@see	string_numeric_base
*/
String (const String_type& datum = "");

/**	Constructs a String from a String_type datum.

	The numeric base for a String that represents an integer value
	will be intuited be examining the String_type datum.

	@param	datum	The String_type datum to be assigned the initial
		value of the String.
	@param	type	The specific Type of the String.
	@see	type(const Type)
	@see	string_numeric_base
*/
String (const String_type& datum, const Type type);

//		Type casting

/**	Constructs a String from an Integer_type value.

	The intial value of the String will be the string representation of
	the integer value.

	@param	datum	The Integer_type value to be used in creating the string
		representation to be assigned to the String.
	@param	is_signed	true (#SIGNED) if the datum is to be treated as a
		signed value; false (#UNSIGNED) otherwise.
	@param	base	The numeric base to be used for the string representation
		of the datum. If this is 0 decimal (10) will be used.
	@param	digits	The minimum number of digits to be used for the string
		representation; 0 padding is used.
	@see	integer_to_string
*/
String (const Integer_type datum,
	const bool is_signed = SIGNED, const int base = 10, const int digits = 0);

/**	Constructs a String from an int value.

	The intial value of the String will be the string representation of
	the integer value.

	@param	datum	The int value to be used in creating the string
		representation to be assigned to the String.
	@param	is_signed	true (#SIGNED) if the datum is to be treated as a
		signed value; false (#UNSIGNED) otherwise.
	@param	base	The numeric base to be used for the string representation
		of the datum. If this is 0 decimal (10) will be used.
	@param	digits	The minimum number of digits to be used for the string
		representation; 0 padding is used.
	@see	integer_to_string
*/
String (const int datum,
	const bool is_signed = SIGNED, const int base = 10, const int digits = 0);

/**	Constructs a String from an unsigned int value.

	The intial value of the String will be the string representation of
	the integer value.

	@param	datum	The unsigned int value to be used in creating the string
		representation to be assigned to the String.
	@param	is_signed	true (#SIGNED) if the datum is to be treated as a
		signed value; false (#UNSIGNED) otherwise.
	@param	base	The numeric base to be used for the string representation
		of the datum. If this is 0 decimal (10) will be used.
	@param	digits	The minimum number of digits to be used for the string
		representation; 0 padding is used.
	@see	integer_to_string
*/
String (const unsigned int datum,
	const bool is_signed = UNSIGNED, const int base = 10, const int digits = 0);

#ifdef idaeim_LONG_LONG_INTEGER
/**	Constructs a String from a long value.

	The intial value of the String will be the string representation of
	the integer value.

	@param	datum	The long value to be used in creating the string
		representation to be assigned to the String.
	@param	is_signed	true (#SIGNED) if the datum is to be treated as a
		signed value; false (#UNSIGNED) otherwise.
	@param	base	The numeric base to be used for the string representation
		of the datum. If this is 0 decimal (10) will be used.
	@param	digits	The minimum number of digits to be used for the string
		representation; 0 padding is used.
	@see	integer_to_string
*/
String (const long datum,
	const bool is_signed = SIGNED, const int base = 10, const int digits = 0);

/**	Constructs a String from an unsigned long value.

	The intial value of the String will be the string representation of
	the integer value.

	@param	datum	The unsigned long value to be used in creating the string
		representation to be assigned to the String.
	@param	is_signed	true (#SIGNED) if the datum is to be treated as a
		signed value; false (#UNSIGNED) otherwise.
	@param	base	The numeric base to be used for the string representation
		of the datum. If this is 0 decimal (10) will be used.
	@param	digits	The minimum number of digits to be used for the string
		representation; 0 padding is used.
	@see	integer_to_string
*/
String (const unsigned long datum,
	const bool is_signed = UNSIGNED, const int base = 10, const int digits = 0);
#endif

#ifdef idaeim_LONG_DOUBLE
/**	Constructs a String from a double value.

	The intial value of the String will be the string representation of
	the floating point value.

	@param	datum	The double value to be used in creating the string
		representation to be assigned to the String.
	@param	digits	The number of precision digits to be used for the string
		representation. The default_format_flags will also be applied.
	@see	real_to_string
*/
String (const double datum, const int digits = 0);
#endif

/**	Constructs a String from a Real_type value.

	The intial value of the String will be the string representation of
	the floating point value.

	@param	datum	The Real_type value to be used in creating the string
		representation to be assigned to the String.
	@param	digits	The number of precision digits to be used for the string
		representation. The default_format_flags will also be applied.
	@see	real_to_string
*/
String (const Real_type datum, const int digits = 0);

//		Copy

/**	Constructs a String as a copy of another String.

	The parent is not copied; the new String has no parent.

	@param	value	The String to copy.
*/
String (const String& value);

//		Copy virtual

/**	Constructs a String from another Value.

	All characteristics of the Value, except its parent, are copied.
	If the Value is not a String its Type is set to IDENTIFIER.

	The initial value of the String is the Value cast to a String_type.

	@param	value	The Value to be used to derive this String.
	@throws	Invalid_Argument	If the Value can not be cast to a
		String_type.
*/
String (const Value& value);

//		Clone

/**	Constructs a clone of this String.

	A clone is a copy constructed on the heap.

	@return	A pointer to the new String clone.
*/
String* clone () const
	{return new String (*this);}

//	Type conversion

/**	Casts the String to an int.

	@return	The int value of the string datum representation.
	@see	operator Integer_type() const
*/
operator int () const
	{return static_cast<int>(operator Integer_type ());}

#ifdef idaeim_LONG_LONG_INTEGER
/**	Casts the String to a long.

	@return	The long value of the string datum representation.
	@see	operator Integer_type() const
*/
operator long () const
	{return static_cast<long>(operator Integer_type ());}
#endif

/**	Casts the String to an Integer_type.

	The numeric base will be used when converting the String
	representation. A base of 0 impliles that it should be
	intuited from the representation.

	A floating point number representation has its decimal portion
	truncated.

	@return	The Integer_type value of the string datum representation.
	@see	string_to_integer
*/
operator Integer_type () const;

#ifdef idaeim_LONG_DOUBLE
/**	Casts the String to a double type.

	@return	The double value of the string datum representation.
	@see	string_to_real
*/
operator double () const;
#endif

/**	Casts the String to a Real_type.

	@return	The Real_type value of the string datum representation.
	@see	string_to_real
*/
operator Real_type () const;

/**	Casts the String to a String_type.

	@return	The string datum.
*/
operator String_type () const
	{return Datum;}

//	Subtype

/**	Asserts that the String is a STRING Type.

	@return	Always returns true.
*/
bool is_String () const
	{return true;}

/**	Tests if the String is an IDENTIFIER Type.

	@return	true if the Type of this String is IDENTIFIER.
*/
bool is_Identifier () const
	{return Subtype == Value::IDENTIFIER;}

/**	Tests if the String is an SYMBOL Type.

	@return	true if the Type of this String is SYMBOL.
*/
bool is_Symbol () const
	{return Subtype == Value::SYMBOL;}

/**	Tests if the String is an TEXT Type.

	@return	true if the Type of this String is TEXT.
*/
bool is_Text () const
	{return Subtype == Value::TEXT;}

/**	Tests if the String is an DATE_TIME Type.

	<b>N.B.</b>: Having a DATE_TIME Type does not guarantee that the
	String contents complies with any particular format requirements.
	This is considered to be an application issue.

	@return	true if the Type of this String is DATE_TIME.
*/
bool is_Date_Time () const
	{return Subtype == Value::DATE_TIME;}

/**	Gets the specific String Type.

	@return	The Type code of this String.
*/
Type type () const
	{return Subtype;}

/**	Sets the specific String Type.

	<b>Note</b>: A String may be set to the DATE_TIME Type without
	any format requirements on the String contents. This is considered
	to be an application issue.

	@param	type_code	A STRING Type code, which must be one of
		IDENTIFIER, TEXT, SYMBOL or DATE_TIME.
	@throws	Invalid_Argument if the type_code is not one of the STRING
		Type values.
*/
String& type (const Type type_code);

/**	Gets the name string for the String Type.

	@return	The {@link Value::type_name(const Type) type_name} of the
		String Type.
*/
std::string type_name () const
	{return Value::type_name (Subtype);}

/**	Determines the String Type of a string.

	The string is checked for special characters associated with a
	specific String Value Type, and any enclosing quotes are removed.
	<b>N.B.</b>: Using this function will modify its argument if it is
	found to have enclosing quotes.

	If the string begins with the Parser::TEXT_DELIMITER character then
	the Value Type is TEXT. If the string begins with the
	Parser::SYMBOL_DELIMITER character then the Value Type is SYMBOL.
	In either of these cases the leading and trailing (if present)
	delimiter characters are removed from the string. By default the
	Value Type is IDENTIFIER.

	@param	characters	The String_type characters to be examined.
		<b>N.B.</b>: May be modified.
	@return	The STRING Type.
*/
static Type type (String_type& characters);

//	Operators

//		Assignment

/**	Assigns a Value to this String.

	The datum for this String is the Value cast to a String_type.

	All other characteristics of the Value, except its parent, are
	assigned directly.

	@param	value	The Value to be assigned.
	@return	This String.
	@throws	Invalid_Argument	If the Value can not be cast to a
		String_Type.
*/
String& operator= (const Value& value);

/**	Assigns an int value to this String.

	The datum for this String is assigned the string representation of
	the signed integer value. The representation will be in the base of
	the String and will be padded with 0's, if needed, to produce a
	representation at least digits in length.

	@param	value	The int value to be assigned.
	@return	This String.
	@see	operator=(Integer_type)
*/
String& operator= (const int value)
	{return operator= (static_cast<const Integer_type>(value));}

/**	Assigns an unsigned int value to this String.

	The datum for this String is assigned the string representation of
	the unsigned integer value. The representation will be in the base of
	the String and will be padded with 0's, if needed, to produce a
	representation at least digits in length.

	@param	value	The unsigned int value to be assigned.
	@return	This String.
	@see	integer_to_string
*/
String& operator= (const unsigned int value);

#ifdef idaeim_LONG_LONG_INTEGER
/**	Assigns a long value to this String.

	The datum for this String is assigned the string representation of
	the signed integer value. The representation will be in the base of
	the String and will be padded with 0's, if needed, to produce a
	representation at least digits in length.

	@param	value	The long value to be assigned.
	@return	This String.
	@see	operator=(Integer_type)
*/
String& operator= (const long value)
	{return operator= (static_cast<const Integer_type>(value));}

/**	Assigns an unsigned long value to this String.

	The datum for this String is assigned the string representation of
	the unsigned integer value. The representation will be in the base of
	the String and will be padded with 0's, if needed, to produce a
	representation at least digits in length.

	@param	value	The unsigned long value to be assigned.
	@return	This String.
	@see	integer_to_string
*/
String& operator= (const unsigned long value)
	{return operator= (static_cast<const Integer_type>(value));}
#endif

/**	Assigns an Integer_type value to this String.

	The datum for this String is assigned the string representation of
	the signed integer value. The representation will be in the base of
	the String and will be padded with 0's, if needed, to produce a
	representation at least digits in length.

	@param	value	The Integer_type value to be assigned.
	@return	This String.
	@see	integer_to_string
*/
String& operator= (const Integer_type value);

#ifdef idaeim_LONG_DOUBLE
/**	Assigns a double value to this String.

	The datum for this String is assigned the string representation of
	the double value. The digits of the String determines its precision.
	The default_format_flags are also applied.

	@param	value	The double value to be assigned.
	@return	This String.
	@see	real_to_string
*/
String& operator= (const double value)
	{return operator= (static_cast<Real_type>(value));}
#endif

/**	Assigns a Real_type value to this String.

	The datum for this String is assigned the string representation of
	the Real_type value. The digits of the String determines its
	precision. The default_format_flags are also applied.

	@param	value	The Real_type value to be assigned.
	@return	This String.
	@see	real_to_string
*/
String& operator= (const Real_type value);

/**	Assigns a String_type value to this String.

	The datum for this String is assigned the String_type value.

	If this String is an IDENTIFIER Type the new datum is examined
	for special characters and its Type changed if they are present.
	<b>Note</b>: To assign a quoted string to a String and retain the
	enclosing quote characters, first set the Type of the String to
	TEXT (to encapsulate a single quoted string) or SYMBOL (to
	encapsulate a double quoted string) before assigning the quoted
	string.

	@param	value	The String_type value to be assigned.
	@return	This String.
	@see	type(String_type&)
*/
String& operator= (const String_type& value);

/**	Assigns a character string to this String.

	The datum for this String is assigned the String_type value.

	If this String is an IDENTIFIER Type the new datum is examined
	for special characters and its Type changed if they are present.
	<b>Note</b>: To assign a quoted string to a String and retain the
	enclosing quote characters, first set the Type of the String to
	TEXT (to encapsulate a single quoted string) or SYMBOL (to
	encapsulate a double quoted string) before assigning the quoted
	string.

	@param	value	A pointer to a character string (C-string).
	@return	This String.
	@see	operator=(const String_type&)
*/
String& operator= (const char* value);

//		Addition

/**	Adds another Value to this Value.

	The Value cast to a String_type is appended to the datum of
	this String.

	@param	value	The Value to be appended to this String.
	@return	This String.
	@throws	Invalid_Argument	If the Value can not be cast to a
		String_type.
*/
String& operator+= (const Value& value);

/**	Adds this String and another Value.

	This String is copied, onto the stack, and the Value is appended to it.
	The temporary copy of the stack value that is returned is expected
	to be immediately assigned or otherwise used.

	@param	value	The Value to be used in the addition operation.
	@return	A temporary String.
	@see	operator+=(const Value&)
*/
String  operator+  (const Value& value) const;

//		Logical

/**	This String is compared to another Value.

	The comparison is made lexicographically by casting both Values to
	a String_type.

	<b>Note</b>: An Array always compares greater than other types of
	Values.

	@param	value	The Value to be compared against.
	@return	An int that is less than, equal to, or greater than 0 if
		this String is logically less than, equal to, or greater
		than, respectively, the other Value.
*/
int compare (const Value& value) const;

//..............................................................................
private:

//!	The string datum.
String_type			Datum;

//!	The specific Type of Value.
Type				Subtype;

};		//	End of String class.

/*=*****************************************************************************
	Array subclass:
*/
/**	An <i>Array</i> implements a Value that is a list of Values.

	An Array provides for the hierarchical structuring of Values.

	The list is provided by subclassing a <i>Vectal</i> of Values. A
	Vectal is a <b>vect</b>or of virtu<b>al</b> objects that are stored
	as pointers but for which reference semantics are offered in the
	access interface. However, the normal value functionality is also
	provided by copying (actually cloning) objects into the list and
	deleting them on removal. An Array ensures that, normally, whenever
	a Value is entered into its list the parent of the Value is set to
	the Array. Thus the hierarchical relationships are automatically
	maintained. Provision is also made for pulling a Value from an
	Array's list without deleting it, but also automatically resetting
	its parent to NULL to correctly reflect that it is no longer a
	member of the Array hierarchy.

	In addition to the normal vector interators for traversing an
	Array's Value list, specialized Depth_Iterators are provided that
	descend into Arrays encountered in a Value list. They continue
	iterating the parent list where they left off after completing the
	interation of a child Array. They also remember the root Array
	where they started, which is where they will end. These iterators
	are bidirectional with some random access capabilities.

	@see	Vectal
*/
class Array
:	public Value,
	public Vectal<Value>
{
public:
//	Types

//!	Array as a Vectal (vector) of Values.
typedef Vectal<Value>			Value_List;

//!	Identify the iterator for compilers that need help.
typedef Value_List::iterator	iterator;
//!	Identify the iterator for compilers that need help.
typedef Value_List::const_iterator	const_iterator;

class Depth_Iterator;
//!	A specialized iterator that descends into Arrays in the Value list.
typedef Depth_Iterator			depth_iterator;

//	Constructors

/**	Constructs an empty Array.

	@param	type	The Type specification for an Array. This must
		be either SET or SEQUENCE.
	@see	type(Type)
*/
explicit Array (const Type type = Default_Subtype);

/**	Constructs an Array as a copy of another Array.

	The list of the Array being copied is replicated (cloned).

	@param	array	The Array to copy.
*/
Array (const Array& array);

/**	Constructs an Array from another Value.

	If the Value being copied is an Array, it is assigned to the new
	Array. Otherwise the Value is copied as the first element in the
	new Array's Value list.

	@param	value	The Value to copy.
*/
Array (const Value& value);

/**	Construct an Array from a Parser.

	The next Value obtained from the Parser is used as the contents of
	the new Array. If the Parser Value is an Array, then all of its
	elements are moved into the new Array which is set to the same
	Type, and the original Array is deleted. Otherwise the Value
	becomes the only element of the new Array which is set to the
	specified Type.

	@param	parser	The Parser from which to construct the Value.
	@param	type	The default Type of the Array to construct if the
		Parser does not provide an Array.
	@see	Parser#get_value()
*/
Array (Parser& parser, const Type type = Default_Subtype);

//		Clone

/**	Constructs a clone of this Array.

	A clone is a copy constructed on the heap.

	@return	A pointer to the new Array clone.
*/
Array* clone () const
	{return new Array (*this);}

//	Type conversion

//!	@throws	Invalid_Argument	An Array can not be cast to a primitive type.
operator Integer_type () const;
#ifdef idaeim_LONG_LONG_INTEGER
//!	@throws	Invalid_Argument	An Array can not be cast to a primitive type.
operator long () const;
#endif
//!	@throws	Invalid_Argument	An Array can not be cast to a primitive type.
operator int () const;
#ifdef idaeim_LONG_DOUBLE
//!	@throws	Invalid_Argument	An Array can not be cast to a primitive type.
operator double () const;
#endif
//!	@throws	Invalid_Argument	An Array can not be cast to a primitive type.
operator Real_type () const;
//!	@throws	Invalid_Argument	An Array can not be cast to a primitive type.
operator String_type () const;

//	Base

/**	Prevents the base from being changed.

	An Array does not have a numeric base.

	@return	This Array.
*/
Array& base (const int)
	{return *this;}

//	Subtype

/**	Asserts that the Array is an ARRAY Type.

	@return	Always returns true.
*/
bool is_Array () const
	{return true;}

/**	Tests if the Array is a SET Type.

	@return	true if the Type of this String is SET.
*/
bool is_Set () const
	{return Subtype == Value::SET;}

/**	Tests if the Array is a SEQUENCE Type.

	@return	true if the Type of this String is SEQUENCE.
*/
bool is_Sequence () const
	{return Subtype == Value::SEQUENCE;}

/**	Gets the specific Array Type.

	@return	The Type code of this Array.
*/
Type type () const
	{return Subtype;}

/**	Sets the specific Array Type.

	@param	type_code	An Array Type code, which must be one of
		SET or SEQUENCE.
	@throws	Invalid_Argument if the type_code is not one of the ARRAY
		Type values.
*/
Array& type (const Type type_code);

/**	Gets the name string for the Array Type.

	@return	The {@link Value::type_name(const Type) type_name} of the
		Array Type.
*/
std::string type_name () const
	{return Value::type_name (Subtype);}

/**	Gets the default Type for an Array.

	@return	The default ARRAY Type.
*/
static Type default_type ()
	{return Default_Subtype;}

/**	Sets the default Type to use when constructing an Array.

	@param	type_code	An Array Type code, which must be one of
		SET or SEQUENCE.
	@return	The type_code.
	@throws	Invalid_Argument if the type_code is not one of the ARRAY
		Type values.
*/
static Type default_type (const int type_code);

//	Operators

//	Array notation accessor.

/**	Gets the Value at an index of the Array list.

	This form of the operator is used with an assignable Value (lvalue)
	in an expression.

	@param	index	The index of a Value in the Array list.
	@return	The Value reference at the index position.
*/
Value& operator[] (unsigned int index)
	{return Value_List::operator[] (index);}

/**	Gets the Value at an index of the Array list.

	This form of the operator produces an unassignable (read only)
	Value.

	@param	index	The index of a Value in the Array list.
	@return	The Value reference at the index position.
*/
const Value& operator[] (unsigned int index) const
	{return Value_List::operator[] (index);}

//		Assignment

/**	Assigns a Value to this Array.

	The Value being assigned must be an Array. Use the #add (or
	push_back) method to add a Value to the Array list.

	The existing Value list is set aside without deleting it in case
	the Value being assigned is contained with this Array. The Value
	list from the assigned Value is 
	{@link copy_in(const Array&) copied into} this Array's now empty
	Value list. The copied Values are
	cloned in depth. Finally the original Value list is deleted.

	@param	value	The Value to be assigned.
	@return	This Array.
	@throws	Invalid_Argument	If the assigned Value is not an Array.
*/
Array& operator= (const Value& value);

//!	@throws	Invalid_Argument	An Array can not be assigned a primitive type.
Array& operator= (const Integer_type value);
//!	@throws	Invalid_Argument	An Array can not be assigned a primitive type.
Array& operator= (const Unsigned_Integer_type value);
//!	@throws	Invalid_Argument	An Array can not be assigned a primitive type.
Array& operator= (const int value)
	{return operator= (static_cast<const Integer_type>(value));}
//!	@throws	Invalid_Argument	An Array can not be assigned a primitive type.
Array& operator= (const unsigned int value)
	{return operator= (static_cast<const Unsigned_Integer_type>(value));}
#ifdef idaeim_LONG_LONG_INTEGER
//!	@throws	Invalid_Argument	An Array can not be assigned a primitive type.
Array& operator= (const long value)
	{return operator= (static_cast<const Integer_type>(value));}
//!	@throws	Invalid_Argument	An Array can not be assigned a primitive type.
Array& operator= (const unsigned long value)
	{return operator= (static_cast<const Unsigned_Integer_type>(value));}
#endif
//!	@throws	Invalid_Argument	An Array can not be assigned a primitive type.
Array& operator= (const Real_type value);
#ifdef idaeim_LONG_DOUBLE
//!	@throws	Invalid_Argument	An Array can not be assigned a primitive type.
Array& operator= (const double value)
	{return operator= (static_cast<Real_type>(value));}
#endif
//!	@throws	Invalid_Argument	An Array can not be assigned a primitive type.
Array& operator= (const String_type& value);
//!	@throws	Invalid_Argument	An Array can not be assigned a primitive type.
Array& operator= (const char* value);

//		Addition

/**	Adds a Value to this Array.

	If the value is an Array each Value in its list is {@link
	add(const Value&) add}ed to this Array's Value list. Otherwise
	the value itself is added to this Array's Value list.

	@param	value	The Value to be added to this Array.
	@return	This Array.
	@see	add(const Value&)
*/
Array& operator+= (const Value& value);

/**	Adds this Array and a Value.

	Adds the Value to a copy of this Array.

	@param	value	The Value to be added to this Array.
	@return	A copy of this Array with the Value added.
	@see	operator+=(const Value&)
*/
Array  operator+  (const Value& value) const;

//		Logical

/**	This Array is compared to another Value.

	An Array always compares greater than other types of Values.

	When the other Value is an Array the Value list sizes are compared.
	If the list sizes are the same then each Value in the list is
	compared and the first non-equal Values determine the comparison.

	@param	value	The Value to be compared against.
	@return	An int that is less than, equal to, or greater than 0 if
		this String is logically less than, equal to, or greater
		than, respectively, the other Value.
*/
int compare (const Value& value) const;

//	Conceptually convenient.

/**	Adds a clone of a Value to the end of the Array's Value list.

	<b>Note</b>: If the Value being added is an Array the Array
	itself, not its Value list, is added. This is different than
	operator+=().

	@param	value	The Value to add.
	@return	This Array.
	@see	Vectal::push_back(reference)
*/
Array& add (const Value& value)
	{push_back (value); return *this;}

/**	Adds a Value to the end of the Array's Value list.

	<b>Note</b>: The Value is not cloned; The Value pointer is placed
	directly in the Value list backing store. The parent of the Value
	is changed to this Array.

	@param	value	The Value to add.
	@return	This Array.
	@see	Vectal::poke_back(pointer)
*/
Array& add (Value* value)
	{poke_back (value); return *this;}

/**	Copies the Value list in an Array into this Array's list.

	Each Value in the source Array Value list is added to
	the end of this Array's Value list.

	@param	array	The source Array.
	@return	This Array.
	@see	add(const Value&)
*/
Array& copy_in (const Array& array);

/**	Moves the Value list in an Array into this Array's list.

	Each Value in the source Array Value list is moved to
	the end of this Array's Value list. As a result the source
	Array will be emptied.

	@param	array	The source Array.
	@return	This Array.
	@see	add(const Value*)
*/
Array& move_in (Array& array);

/**	Removes	a Value at an iterator position from the Array's
	Value list.

	@param	position	A valid iterator for the Array's Value
		list.
	@return	A pointer to the Value that was removed.
*/
Value* remove (iterator position);

/**	Removes	a Value at an element index of the Array's
	Value list.

	@param	index	An index of the Array's Value list.
	@return	A pointer to the Value that was removed. This
		will be NULL if the index is not within the range of
		existing elements of the Array's Value list.
	@see	pull_out(size_type)
*/
Value* remove (size_type index);

/**	Removes a Value from the Array's Value list.

	@param	value	A pointer to the Value to be removed.
	@return	A pointer to the Value that was removed, or NULL if
		the Value was not found in the Array's Value list.
*/
Value* remove (Value *value);

/**	Removes a Value from the Array's Value list or any
	of its sub-Array ists.

	@param	value	A pointer to the Value to be removed. If
		NULL nothing is done and NULL is returned.
	@return	A pointer to the Value that was removed, or NULL if
		the Value was not found.
*/
Value* remove_depth (Value *value);

/*------------------------------------------------------------------------------
	Vectal related methods.
*/
//	Overloads for setting the Parent.

/**	Sets the parent of a Value to this Array.

	This method overrides the Vectal's method that is
	invoked whenever a Value is entered into the Value list. It
	is not intended for normal use since it affects the logical
	relationship of Values in an Array hierarchy.

	The uniquness of the Value in the path hierachy is ensured;
	loops are prevented.

	@param	value	The Value whose parent is to be changed.
	@throws	Invalid_Argument	If the value is in the Array's path;
		i.e. the Value is already in the parent hierachy of the
		Array, or is the Array itself.
*/
void entering (Value* value);

/**	Resets the parent of a Value to NULL.

	This method overrides the Vectal's method that is
	invoked whenever a Value is removed from the Value list. It
	is not intended for normal use since it affects the logical
	relationship of Values in an Array hierarchy.

	@param	value	The Value whose parent is to be changed.
*/
void removing (Value* value);

/**	Counts the number of Values in the Array's Value list
	and, recursively, the number of Values in the Value list
	of each Array in the list.

	@return	The accumulated total number of Values enclosed by
		this Array.
*/
size_type total_size ();

/*==============================================================================
	Depth_Iterator
*/
/**	A <i>Depth_Iterator</i> is a bidirectional iterator over an
	Array's Value list and, depth-first, over the Value list
	of each Array encountered.

	The Depth_Iterator operates like a normal breadth-only interator
	for a Value list until it encounters an Array in the list.
	When it is incremented from an Array position it decends into
	the Array, and only returns to the next Value in the
	current list when the end of the Value list of the Array
	has been reached. This process is recursive resulting in a
	depth-first traversal of the entire Array Value hierarchy.
	The mechanism operates similarly when decrementing by descending
	into each Array and reverse iterating its Value list, then
	returning to the current Value list when the beginning of the
	Array's list has been reached.

	A Depth_Iterator is associated with a specific Array which it
	knows as its root Array. It will not ascend into the Value
	list of the root Array's parent. Like other container
	iterators, all positions are expected to be for valid Values
	except the final end position of the root Array; reverse
	iteration will not go beyond the initial begin position of the
	root Array.

	Random-access operators are also provided. But because they are not
	truely random access, and do not operate in constant time, the
	Depth_Iterator is tagged as only bidirectional.
*/
class Depth_Iterator
:	public std::iterator<std::bidirectional_iterator_tag, Value>
{
public:

//	Constructors

/**	Constructs a Depth_Iterator without a root Array.

	This Depth_Iterator is invalid until it is assigned a valid
	iterator.
*/
Depth_Iterator ()
	:	Current_Array (NULL)
	{}

/**	Constructs a Depth_Iterator for an Array.

	The initial position of this Depth_Iterator is the beginning
	of the Array's Value list.

	@param	array	The root Array.
	@see	Vectal::begin()
*/
explicit Depth_Iterator (const Array& array);

/**	Construct a Depth_Iterator from a normal Array iterator.

	<B>Warning</B>: The iterator must be valid; i.e. it must refer to a
	Value. An end iterator is not valid. Use the end_depth method to
	get the iterator for the end of a Depth_Iterator.
*/
Depth_Iterator (const Array::iterator& list_iterator)
	:	Current_Position (list_iterator),
		Current_Array ((*list_iterator).parent ())
	{}

/**	Converts the Depth_Iterator to a Value_List iterator.

	@return An iterator at the current position of the Depth_Iterator.
*/
operator Array::iterator ()
	{return Current_Position;}

/**	Gets the root Array.

	@return	A pointer to the root Array for this Depth_Iterator.
*/
Array* root () const;

/**	Gets the current Array context.

	@return	A pointer to the Array containing the Value list
		for the current iterator position.
*/
Array* array () const
	{return Current_Array;}

/**	Gets a Depth_Iterator positioned at the end of the traversal
	for this Depth_Iterator.

	This is the same as the end iterator for the root Array.

	@return	A Depth_Iterator positioned at the end of this Depth_Iterator.
	@throws	Error	If this Depth_Iterator is invalid (it doesn't
		have a root Array).
	@see	Vectal::end()
*/
Depth_Iterator end_depth () const;

/**	Assigns a Value list iterator to this Depth_Iterator.

	If the Depth_Iterator is currently positioned at the same
	Value as the list iterator nothing happens.

	If the Depth_Iterator and list iterator are positioned in the
	Value list of the same Array, then the current position of
	the Depth_Iterator is simply moved to the list iterator position.
	However, if the list iterator position is the end of the current
	Value list, and the list is not in the root Array, then the
	current position is moved to the position preceeding the end;
	unless the list has become empty (due to some other user action on
	the list) in which case the Depth_Iterator is incremented forward
	to the next valid position.

	When the Depth_Iterator and list iterator are not positioned in
	the same Array, then a search is made for the list iterator
	position within the scope of the Depth_Iterator. First a search
	is made from the current Depth_Iterator position towards the
	beginning of its scope. If this fails a search is made
	from the current Depth_Iterator position towards the end of its
	scope. The current position of the Depth_Iterator becomes list
	iterator position if it is found.

	@param	list_iterator	The Value_List iterator position
		to be assigned to this Depth_Iterator.
	@return	This Depth_Iterator.
	@throws	Invalid_Argument	If the list iterator is not in the scope
		of this Depth_Iterator.
	@throws	Error				If on attempting to ascend from an Array
		hierarchy, a non-root Array is encountered without a parent (this
		should not happen unless the Array hierachy was inappropriately
		modified during traversal).
*/
Depth_Iterator& operator= (const Array::iterator& list_iterator);

/**	Gets a reference to the Value at the current position.

	@return	The reference to the Value at the current position.
*/
Value& operator* () const
	{return *Current_Position;}

/**	Gets a pointer to the Value at the current position.

	@return	The pointer to the Value at the current position.
*/
Value* operator-> () const
	{return Current_Position.operator-> ();}

//	Repositioning.

/**	Pre-increments forward to the next Value position.

	If the Value at the current position is a non-empty Array,
	the current position is pushed onto a LIFO (last-in-first-out)
	stack and then the current position moves to the first entry in the
	child Array's list.

	If the Value at the current position is an Assignment or an
	empty Array, then the current position is moved forward to the
	next position in the Value list. If this position is the end of
	the Value list, and the list is not for the root Array,
	then the previous position in the parent's Value list is popped
	from the stack to the current position and it is moved forward
	again. Ascending up the Array hierarchy continues in this way
	until a non-end list position is found, or the end of the root
	Array is encountered.

	<b>Caution</b>: Like any iterator, care must be taken not to
	inappropriately modify the container structure being traversed or the
	integrity of the Depth_Iterator will be lost. The Array hierachy of
	the Depth_Iterator's current position is the primary issue. Each
	iterator, saved on the stack, to a previous position in the Array
	heirarchy must remain consistent; the location of its Array in the
	Value list must remain unchanged and the parent of the Array must
	remain unchanged. Changes to the current Value list may be made as
	long as the current position of the Depth_Iterator remains valid;
	e.g. assigning back to the Depth_Iterator the iterator returned from
	an insert (or poke) or erase (or pull) using the current
	Depth_Iterator. Modfications anywhere in the Depth_Iterator scope
	beyond any current hierachy path position is safe; e.g. push_back or
	poke_back is always OK. But modifications at or before any current
	hierachy path position is almost always going to disrupt a
	Depth_Iterator.

	@return	This Depth_Iterator.
	@throws	Out_of_Range	If the current position is the end of the
		root Array.
	@throws	Error			If the Depth_Iterator is invalid (doesn't
		have a root Array). An Error will also be thrown if, on attempting
		to ascend the Array hierarchy, a non-root Array is encountered
		without a parent (this should not happen unless the Array
		hierachy was inappropriately modified during traversal).
*/
Depth_Iterator& operator++ ();

/*	Moves to the next sibling Value position.

	If the Parameter at the current position is a non-empty Array the
	Depth_Iterator does not descend into its Value list. This is the
	same as the pre-increment operator of a normal interator.

	If the current position is the end of an Array list then the
	Depth_Iterator will ascend to the next available Value or the
	end of the root Array.

	@return	This Depth_Iterator.
	@throws	Out_of_Range	If the current position is the end of the
		root Array.
	@throws	Error			If the Depth_Iterator is invalid (doesn't
		have a root Array). An Error will also be thrown if, on
		attempting to ascend the Array hierarchy, a non-root
		Array is encountered without a parent (this should not happen
		unless the Array hierachy was inappropriately modified during
		traversal).
	@see	operator++();
*/
Depth_Iterator& next_sibling ();

/**	Post-increments forward to the next Value position.

	@return A copy of the Depth_Iterator before it was incremented.
	@see	operator++();
*/
Depth_Iterator  operator++ (int);

/**	Pre-decrements backward to the previous Value position.

	If the current position is not the beginning of a Value list,
	the current position moves to the previous Value in the list.
	If this Value is a non-empty Array, the current position is
	pushed onto a LIFO (last-in-first-out) stack and then the current
	position moves to the last entry (the Value immediately
	preceeding the end position) in the child Array's list. If this
	Value is a non-empty Array then the currrent positionis
	pushed onto the stack again and the current position becomes the
	last entry in this Array. This back-down cycle continues until
	an Assignment, or empty Array, Value is encountered.

	If the current position is the beginning of a Value list, and
	it is not the root Array's Value list,	then the previous
	position in the parent's Value list is popped from the stack to
	the current position.

	@return	This Depth_Iterator.
	@throws	Out_of_Range	If the current position is the beginning of
		the root Array.
	@throws	Error			If the Depth_Iterator is invalid (doesn't
		have a root Array). An Error will also be thrown if, on attempting
		to ascend the Array hierarchy, a non-root Array is encountered
		without a parent (this should not happen unless the Array
		hierachy was inappropriately modified during traversal).
	@see	operator++()
*/
Depth_Iterator& operator-- ();

/*	Moves to the previous sibling Value position.

	If the Parameter at the previous position is a non-empty Array
	the Depth_Iterator does not descend into its Value list. This is
	the same as the pre-decrement operator of a normal interator.

	If the current position is the beginning of an Array list then the
	Depth_Iterator will ascend to the previously available Value or
	the beginning of the root Array.

	@return	This Depth_Iterator.
	@throws	Out_of_Range	If the current position is the beginning of the
		root Array.
	@throws	Error			If the Depth_Iterator is invalid (doesn't
		have a root Array). An Error will also be thrown if, on
		attempting to ascend the Array hierarchy, a non-root
		Array is encountered without a parent (this should not happen
		unless the Array hierachy was inappropriately modified during
		traversal).
	@see	operator--();
*/
Depth_Iterator& previous_sibling ();

/**	Post-decrements backward to the previous Value position.

	@return A copy of the Depth_Iterator before it was incremented.
	@see	operator--();
*/
Depth_Iterator  operator-- (int);

/**	Array-style access to the Value at an offset from
	the current position.

	The Depth_Iterator is copied and offset to the new position,
	and then dereferenced.

	@param	offset	The position of the Value relative (positive
		or negative) to the current position.
	@return	The Value reference at the offset position.
	@see	operator+(int)
*/
Value&          operator[] (int offset) const;

/**	Relocates the current position by the specified distance.

	The Depth_Iterator is incremented for a positive distance,
	or decremented for a negative distance, repeatedly the
	absolute amount of the distance.

	<b>Note</b>: This is a random-access iterator operator
	that does not occur in constant time.

	@param	distance	The number of increment (positive) or
		decrement (negative) operations to apply.
	@return	This Depth_Iterator.
	@see	operator++()
	@see	operator--()
*/
Depth_Iterator& operator+= (int distance);

/**	Gets a Depth_Iterator for a position at some distance from
	the current position.

	A copy of this Depth_Iterator is offset the distance.

	@param	distance	The offset of the new Depth_Iterator.
	@return	A Depth_Iterator at the new position.
	@see	operator+=(int)
*/
Depth_Iterator  operator+  (int distance) const;

/**	Relocates the current position by the specified distance.

	@param	distance	The number of increment (negative) or
		decrement (positive) operations to apply.
	@return	This Depth_Iterator.
	@see	operator+=()
*/
Depth_Iterator& operator-= (int distance);

/**	Gets a Depth_Iterator for a position at some distance from
	the current position.

	@param	distance	The offset of the new Depth_Iterator.
	@return	A Depth_Iterator at the new position.
	@see	operator+(int)
*/
Depth_Iterator  operator-  (int distance) const;

/**	Gets the distance between this and another Depth_Iterator.

	@param	depth_iterator	A Depth_Iterator position to find.
	@return	The distance, in terms of decrement (positive) or
		increment (negative) count, to the depth_iterator position.
	@throw	Out_of_Range	If the depth_iterator is not within
		the scope of this Depth_Iterator.
	@see	operator-(const Array::iterator&)
*/
int operator- (const Depth_Iterator& depth_iterator) const;

/**	Get the distance between this Depth_Iterator and an
	Array list iterator.

	A search is made for the list iterator position in the
	scope of this Depth_Iterator; first backwards (positive
	distance), and then forwards (negative distance).

	<b>Note</b>: This is a random-access iterator operator
	that does not occur in constant time.

	@param	list_iterator	An Array::iterator position
		to find.
	@return	The distance, in terms of decrement (positive) or
		increment (negative) count, to the list_iterator position.
	@throw	Out_of_Range	If the list_iterator is not within
		the scope of this Depth_Iterator.
*/
int operator- (const Array::iterator& list_iterator) const;

//		Logical operators.

/**	Tests if the current positions of this Depth_Iterator and
	another Depth_Iterator are equivalent.

	@param	depth_iterator	The Depth_Iterator to compare against.
	@return	true if the current positions are identical; false otherwise.
*/
bool operator== (const Depth_Iterator& depth_iterator) const
	{
	return
		/*	Safety check:

			The comparison of two iterators on separate containers
			can be unsafe for the MS/Windows MSVC implementation.
		*/
		Current_Array    == depth_iterator.Current_Array &&
		Current_Position == depth_iterator.Current_Position;}

/**	Tests if the current positions of this Depth_Iterator and
	another Depth_Iterator are not equivalent.

	@param	depth_iterator	The Depth_Iterator to compare against.
	@return	false if the current positions are identical; true otherwise.
*/
bool operator!= (const Depth_Iterator& depth_iterator) const
	{return ! (*this == depth_iterator);}

/**	Tests if the current position of this Depth_Iterator and
	an Array list iterator are equivalent.

	<b>WARNING</b>: This method may result in undefined behaviour if the
	list_iterator argument is not bound to the same Array vector that the
	Depth_Iterator {@link Array::iterator() current position
	iterator} is bound to. If this is possible a Depth_Iterator should
	first be constructed from the list_iterator before the equality test
	is made.

	@param	list_iterator	The Array::iterator to compare against.
	@return	true if the positions are identical; false otherwise.
*/
bool operator== (const Array::iterator& list_iterator) const
	{return Current_Position == list_iterator;}

/**	Tests if the current position of this Depth_Iterator and
	an Array list iterator are not equivalent.

	@param	list_iterator	The Array::iterator to compare against.
	@return	false if the positions are identical; true otherwise.
*/
bool operator!= (const Array::iterator& list_iterator) const
	{return ! (*this == list_iterator);}

/**	Tests if the position of this Depth_Iterator is less than the
	position of another Depth_Iterator.

	@param	depth_iterator	The Depth_Iterator to compare against.
	@return	true if this Depth_Iterator preceeds the position of the
		other Depth_Iterator; flase otherwise.
	@see	operator<(const Array::iterator&)
*/
bool operator<  (const Depth_Iterator& depth_iterator) const;

/**	Tests if the current position of this Depth_Iterator is
	less than the position of an Array list iterator.

	A copy of this Depth_Iterator is incremented forward searching
	for the list iterator position.

	<b>Note</b>: This is a random-access iterator operator that does
	not occur in constant time. This is always more, often much more,
	expensive that the == (or !=) test. The latter should be used
	whenever possible (e.g. in loop conditions).

	@param	list_iterator	The Array::iterator to compare against.
	@return	true if the list_iterator position is found; false otherwise.
*/
bool operator<  (const Array::iterator& list_iterator) const;

/**	Tests if the position of this Depth_Iterator is greater than the
	position of another Depth_Iterator.

	@param	depth_iterator	The Depth_Iterator to compare against.
	@return	true if the other Depth_Iterator is less than 
		this Depth_Iterator; false otherwise.
	@see	operator<(const Array::iterator&)
*/
bool operator>  (const Depth_Iterator& depth_iterator) const
	{return depth_iterator < *this;}

/**	Tests if the position of this Depth_Iterator is less than or
	equal to the position of another Depth_Iterator.

	@param	depth_iterator	The Depth_Iterator to compare against.
	@return	true if the other Depth_Iterator is not less than this
		Depth_Iterator; false otherwise.
	@see	operator<(const Array::iterator&)
*/
bool operator<= (const Depth_Iterator& depth_iterator) const
	{return ! (depth_iterator < *this);}

/**	Tests if the position of this Depth_Iterator is greater than or
	equal to the position of another Depth_Iterator.

	@param	depth_iterator	The Depth_Iterator to compare against.
	@return	true if this Depth_Iterator is less than the other
		Depth_Iterator; false otherwise.
	@see	operator<(const Array::iterator&)
*/
bool operator>= (const Depth_Iterator& depth_iterator) const
	{return ! (*this < depth_iterator);}

/**	Tests if the position of this Depth_Iterator is greater than the
	position of an Array list iterator.

	@param	list_iterator	The Array::iterator to compare against.
	@return	true if this Depth_Iterator is not less than or equal to
		the list_terator; false otherwise.
	@see	operator<(const Array::iterator&)
*/
bool operator>  (const Array::iterator& list_iterator) const
	{return ! (*this >= list_iterator);}

/**	Tests if the position of this Depth_Iterator is less than or
	equal to the position of an Array list iterator.

	@param	list_iterator	The Array::iterator to compare against.
	@return	true if this Depth_Iterator is eqivalent to, or less than, the
		the list_iterator; false otherwise.
	@see	operator<(const Array::iterator&)
*/
bool operator<= (const Array::iterator& list_iterator) const
	{return ! (*this > list_iterator);}

/**	Tests if the position of this Depth_Iterator is greater than or
	equal to the position of an Array list iterator.

	@param	list_iterator	The Array::iterator to compare against.
	@return	true if this Depth_Iterator is not less than the
		list iterator; false otherwise.
	@see	operator<(const Array::iterator&)
*/
bool operator>= (const Array::iterator& list_iterator) const
	{return ! (*this < list_iterator);}

//..............................................................................
private:

Array::iterator
	Current_Position;
Array
	*Current_Array;
std::vector<Array::iterator>
	Previous_Position;
};		//	End of Depth_Iterator class.

/**	Gets a Depth_Iterator positioned at the beginning of this Array.

	@return	A Depth_Iterator.
*/
Depth_Iterator begin_depth ();

/**	Gets a Depth_Iterator positioned at the end of this Array.

	@return	A Depth_Iterator.
*/
Depth_Iterator end_depth ();

//..............................................................................
private:

Type		Subtype;
static Type	Default_Subtype;

};		//	End of Array class.

//	Non-member functions.

/**	Tests if the position of an Array list iterator is less than
	the positon of a Depth_Iterator.

	@param	list_iterator	The Array::iterator to compare against.
	@param	depth_iterator	The Depth_Iterator to compare against.
	@return	true if the Depth_Iterator is not greater than or equal to
		the list iterator; false otherwise.
	@see	operator<(const Array::iterator&)
*/
inline bool operator<
	(
	const Array::iterator&			list_iterator,
	const Array::Depth_Iterator&	depth_iterator
	)
{return ! (depth_iterator >= list_iterator);}

/*=*****************************************************************************
	Utility functions
*/
/**	Sorts an Array's list iterator range.

	A bubble sort is applied, in place, using the less than (<)
	comparison for the Values in the interator range [begin, end).

	@param	begin	The Array::iterator for the first Value
		where sorting begins.
	@param	end		The Array::iterator for the position after the
		last Value where sorting ends.
	@see	Value::operator<(const Value&) const
*/
void sort (Array::iterator begin, Array::iterator end);

/**	Sorts the Values in the iterator range, and all Values
	in all Arrays contained within the range.

	This is a "deep" sort in that not only are the Values in the
	Array of the iterator range sorted, but each Array within
	the range has all of its Values sorted. Each Array has its
	Values sorted independently (Values never leave their
	containing Array). Though the iterator range is defined by a
	pair of Depth_Iterators, which are used to traverse the entire
	Array hierarchy, they must both be positioned in the Value
	list of the same Array which thus defines the range of
	Values to be sorted in the starting Array.

	@param	begin	The Array::Depth_Iterator for the first Value
		where sorting begins.
	@param	end		The Array::Depth_Iterator for the position after the
		last Value where sorting ends.
	@throws	Invalid_Argument	If the begin and end iterators are
		not positioned in the same Value list.
	@see	sort(Array::iterator, Array::iterator)
*/
void sort (Array::depth_iterator begin, Array::depth_iterator end);

}		//	namespace PVL
}		//	namespace idaeim
#endif	//	idaeim_PVL_Value_hh
