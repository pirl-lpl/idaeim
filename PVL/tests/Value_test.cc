/*	Value_test

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

CVS ID: Value_test.cc,v 1.27 2013/02/24 22:27:12 castalia Exp
*******************************************************************************/

#include	"Value.hh"
#include	"Parser.hh"
#include	"Lister.hh"
#include	"Utility/Checker.hh"
using namespace idaeim;
using namespace PVL;

#include	<iostream>
#include	<sstream>
#include	<algorithm>
#include	<functional>
using namespace std;


int
main
	(
	int		count,
	char	**arguments
	)
{
cout << "*** Value class test" << endl
	 << Value::ID << endl;

Value::Integer_type
	expected_integer,
	obtained_integer;
short int
	short_integer;
int
	status = 0,
	integer;
long
	long_integer;
#ifdef idaeim_LONG_LONG_INTEGER
long long
	long_long_integer;
#define INTEGER_VALUE	(((Value::Integer_type)(0x8070605) << 32) | 0x4030201)
#define INTEGER_VALUE_STRING	"578437695752307201"
#else
#define INTEGER_VALUE			 32786
#define INTEGER_VALUE_STRING	"32786"
#endif

Value::Real_type
	expected_real,
	obtained_real;
string
	expected_string,
	obtained_string;
ostringstream
	expected,
	listing;
Checker
	checker;

if (count > 1)
	{
	if (*arguments[1] == '-')
		 arguments[1]++;
	if (*arguments[1] == 'v' ||
		*arguments[1] == 'V')
		checker.Verbose = true;
	}

try {

Lister::Default_Page_Width = 200;

//	Integer:

cout << "Integer precision: "
	 << (sizeof (Value::Integer_type) << 3) << "-bit" << endl;
#ifdef idaeim_LONG_LONG_INTEGER
if (sizeof (Value::Integer_type) != 8)
	cout << "WARNING: idaeim_LONG_LONG_INTEGER defined!" << endl;
#else
if (sizeof (Value::Integer_type) == 8)
	cout << "WARNING: idaeim_LONG_LONG_INTEGER not defined." << endl;
#endif

//	Construct.
if (checker.Verbose)
	cout << "--- Integer* Integer_Value = new Integer (INTEGER_VALUE)" << endl;
Integer*
	Integer_Value = new Integer (INTEGER_VALUE);

//	Type convert - constant int.
if (checker.Verbose)
	cout << "--- int number = *Integer_Value;" << endl;
expected_integer = INTEGER_VALUE;
obtained_integer = *Integer_Value;
if (! checker.check ("Integer, construct from constant integer; convert to Integer_type.",
	expected_integer, obtained_integer))
	exit (1);

//	Construct.
if (checker.Verbose)
	cout << "--- Integer* Integer_Value = new Integer ((Integer_type)INTEGER_VALUE)" << endl;
Integer_Value = new Integer (expected_integer = INTEGER_VALUE);

//	Type convert - constant int.
if (checker.Verbose)
	cout << "--- int number = *Integer_Value;" << endl;
obtained_integer = *Integer_Value;
if (! checker.check ("Integer, construct from Integer_type.",
	expected_integer, obtained_integer))
	exit (1);

//	Construct.
if (checker.Verbose)
	cout << "--- Integer* Integer_Value = new Integer ((short)INTEGER_VALUE)" << endl;
short_integer = (short int)INTEGER_VALUE;
expected_integer = short_integer;
Integer_Value = new Integer (short_integer);

//	Type convert - constant int.
if (checker.Verbose)
	cout << "--- int number = *Integer_Value;" << endl;
obtained_integer = *Integer_Value;
if (! checker.check ("Integer, construct from short.",
	expected_integer, obtained_integer))
	exit (1);

//	Construct.
if (checker.Verbose)
	cout << "--- Integer* Integer_Value = new Integer ((int)INTEGER_VALUE)" << endl;
integer = (int)INTEGER_VALUE;
expected_integer = (Value::Integer_type)integer;
Integer_Value = new Integer (integer);

//	Type convert - constant int.
if (checker.Verbose)
	cout << "--- int number = *Integer_Value;" << endl;
obtained_integer = *Integer_Value;
if (! checker.check ("Integer, construct from int.",
	expected_integer, obtained_integer))
	exit (1);

//	Construct.
if (checker.Verbose)
	cout << "--- Integer* Integer_Value = new Integer ((long)INTEGER_VALUE)" << endl;
long_integer = (long)INTEGER_VALUE;
expected_integer = (Value::Integer_type)long_integer;
Integer_Value = new Integer (long_integer);

//	Type convert - constant int.
if (checker.Verbose)
	cout << "--- int number = *Integer_Value;" << endl;
obtained_integer = *Integer_Value;
if (! checker.check ("Integer, construct from long.",
	expected_integer, obtained_integer))
	exit (1);

#ifdef idaeim_LONG_LONG_INTEGER
//	Construct.
if (checker.Verbose)
	cout << "--- Integer* Integer_Value = new Integer ((long long)INTEGER_VALUE)" << endl;
long_long_integer = INTEGER_VALUE;
expected_integer = (Value::Integer_type)long_long_integer;
Integer_Value = new Integer (long_long_integer);

//	Type convert - constant int.
if (checker.Verbose)
	cout << "--- int number = *Integer_Value;" << endl;
obtained_integer = *Integer_Value;
if (! checker.check ("Integer, construct from long long.",
	expected_integer, obtained_integer))
	exit (1);
#endif


if (checker.Verbose)
	cout << "--- Real_type = *Integer_Value;" << endl;
obtained_real = *Integer_Value;
expected_real = expected_integer;
if (! checker.check ("Integer, type convert to double.",
	expected_real, obtained_real))
	exit (1);

//	Type convert - string.
if (checker.Verbose)
	cout << "--- string a_string = *Integer_Value;" << endl;
expected_string = INTEGER_VALUE_STRING;
obtained_string = static_cast<string>(*Integer_Value);
if (! checker.check ("Integer, type convert to string.",
	expected_string, obtained_string))
	exit (1);

//	Base.
if (checker.Verbose)
	cout << "--- Integer_Value->base (16); Integer_Value->base ();" << endl;
Integer_Value->base (expected_integer = 16);
obtained_integer = Integer_Value->base ();
checker.check ("Integer, set base.",
	expected_integer, obtained_integer);

//	Units.
if (checker.Verbose)
	cout << "--- Integer_Value->units (\"integer\"); Integer_Value->units ();" << endl;
Integer_Value->units (expected_string = "integer");
obtained_string = Integer_Value->units ();
checker.check ("Integer, set units.",
	expected_string, obtained_string);

//	Assignment of hex string.
if (checker.Verbose)
	cout << "--- *Integer_Value = \"ABC\";" << endl;
*Integer_Value = "ABC";
expected_integer = 2748;
obtained_integer = *Integer_Value;
checker.check ("Integer, assignment of hex string.",
	expected_integer, obtained_integer);

//	Write.
if (checker.Verbose)
	cout << "--- Integer_Value->write (ostringstream);" << endl;
Integer_Value->write (listing);
expected << "16#ABC# <integer>";
checker.check ("Integer, write.",
	expected, listing);

//	String:

//	Construct w/ specific subtype.
if (checker.Verbose)
	cout << "--- String* String_Value = new String (\"Text string\", Value::TEXT);" << endl;
String*
	String_Value = new String ("Text string", Value::TEXT);
obtained_string = static_cast<string>(*String_Value);
expected_string = "Text string";
checker.check ("String, construct as TEXT, convert to string.",
	expected_string, obtained_string);

//	Operator<<.
if (checker.Verbose)
	cout << "--- ostringstream listing << *String_Value;" << endl;
listing << *String_Value;
expected << "\"Text string\"";
checker.check ("String, operator<<.",
	expected, listing);

//	Subtype.
if (checker.Verbose)
	cout << "--- String_Value->type (Value::SYMBOL); listing << *String_Value;" << endl;
String_Value->type (Value::SYMBOL);
listing << *String_Value;
expected << "'Text string'";
checker.check ("String, type set to SYMBOL.",
	expected, listing);

//	Type convert - int (exception)
if (checker.Verbose)
	cout << "--- try {number = *String_Value};" << endl;
try
	{
	obtained_integer = *String_Value;
	listing << obtained_integer;
	expected << "Exception";
	}
catch (Exception except)
	{expected << except.message (); listing << expected.str ();}
checker.check ("String, convert to int Exception.",
	expected, listing);

//	Assignment of int.
if (checker.Verbose)
	cout << "--- *String_Value = 23; a_string = *String_Value;" << endl;
*String_Value = 23;
obtained_string = static_cast<string>(*String_Value);
expected_string = "23";
checker.check ("String, assignment of int.",
	expected_string, obtained_string);

//	Type convert - int.
if (checker.Verbose)
	cout << "--- number = *String_Value;" << endl;
obtained_integer = *String_Value;
expected_integer = 23;
checker.check ("String, convert to int.",
	expected_integer, obtained_integer);

//	Copy constructor.
if (checker.Verbose)
	cout << "--- String String_number (*String_Value)" << endl
		 << "    String_number.type (Value::IDENTIFIER).units (\"number\");" << endl;
String
	String_number (*String_Value);
String_number.type (Value::IDENTIFIER).units ("number");
obtained_integer = String_number;
expected_integer = 23;
checker.check ("String, copy constructor.",
	expected_integer, obtained_integer);

//	Equality.
if (checker.Verbose)
	cout << "-- *String_Value == String_number;" << endl;
checker.check ("String, operator== with String copy.",
	true, *String_Value == String_number);

//	Real:

cout << "Real precision: "
	 << (sizeof (Value::Real_type) << 3) << "-bit" << endl;

//	Constructor.
if (checker.Verbose)
	cout << "--- Real pi (3.14159); Real_type = pi;" << endl;
Real
	pi (3.14159);
obtained_real = pi;
expected_real = 3.14159;
checker.check ("Real, constructor from double.",
	expected_real, obtained_real);

//	Write.
if (checker.Verbose)
	cout << "--- listing << pi" << endl;
listing << pi;
expected << "3.14159";
checker.check ("Real, operator<<.",
	expected, listing);

if (checker.Verbose)
	cout << "--- pi.write (listing);" << endl;
pi.write (listing);
expected << "3.14159";
checker.check ("Real, write.",
	expected, listing);

//	Construct from Integer.
if (checker.Verbose)
	cout << "--- Real Real_Value (*Integer_Value);" << endl;
Real
	Real_Value (*Integer_Value);
obtained_real = Real_Value;
expected_real = *Integer_Value;
checker.check ("Real, construct from Integer; convert to Real_type.",
	expected_real, obtained_real);

//	Assignment of string (exception).
if (checker.Verbose)
	cout << "--- try {pi = \"Text\";}" << endl;
try {pi = "Text"; listing << pi; expected << "Exception";}
catch (Exception except)
	{expected << except.message (); listing << expected.str ();}
checker.check ("Real, assign from string Exception.",
	expected, listing);

//	Compares:

//	Operator== - Integer == Real.
if (checker.Verbose)
	cout << "--- *Integer_Value |" << *Integer_Value << "| == Real_Value |" << Real_Value << "|;" << endl;
checker.check ("Integer == Real.",
	true, *Integer_Value == Real_Value);

//	Operator== - Real == Integer.
if (checker.Verbose)
	cout << "--- Real_Value |" << Real_Value << "| == *Integer_Value |" << *Integer_Value << "|;" << endl;
checker.check ("Real == Integer.",
	true, Real_Value == *Integer_Value);

//	Construct from String.
if (checker.Verbose)
	cout << "--- Real Real_number (*String_Value |" << *String_Value << "|);" << endl;
Real
	Real_number (*String_Value);
obtained_real = Real_number;
expected_real = 23;
checker.check ("Real, construct from String of number respresentation.",
	expected_real, obtained_real);

//	Operator== - Real == String.
if (checker.Verbose)
	cout << "--- Real_number |" << Real_number << "| == *String_Value |" << *String_Value << "|;" << endl;
checker.check ("Real == String of number representation.",
	true, Real_number == *String_Value);

//	Operator== - String == Real.
if (checker.Verbose)
	cout << "--- *String_Value |" << *String_Value << "| == Real_number |" << Real_number << "|;" << endl;
checker.check ("String of number representation == Real",
	false, *String_Value == Real_number);

//	Operator> - Integer > Real.
if (checker.Verbose)
	cout << "--- *Integer_Value |" << *Integer_Value << "| > Real_number |" << Real_number << "|;" << endl;
checker.check ("Integer > Real.",
	true, *Integer_Value > Real_number);

//	Real > String integer representation.
if (checker.Verbose)
	cout << "--- Real_Value |" << Real_Value << "| > *String_Value |" << *String_Value << "|;" << endl;
checker.check ("Real > String.",
	true, Real_Value > *String_Value);

//	Assign string to String.
if (checker.Verbose)
	cout << "--- String_Value->type (Value::IDENTIFIER);" << endl
		 << "--- *String_Value = \"Text\";" << endl;
String_Value->type (Value::IDENTIFIER);
*String_Value = "Text";
obtained_string = (string)*String_Value;
expected_string = "Text";
checker.check ("String, assign string to String.",
	expected_string, obtained_string);

if (checker.Verbose)
	cout << "--- *String_Value |" << *String_Value << "| == (String)\"Text\";" << endl;
checker.check ("String == (String)string.",
	true, *String_Value == (String)"Text");

//	Operator== - Real == String.
if (checker.Verbose)
	cout << "--- Real_Value |" << Real_Value << "| == *String_Value |" << *String_Value << "|;" << endl;
checker.check ("Real == String.",
	false, Real_Value == *String_Value);

//	Operator< - Real < String.
if (checker.Verbose)
	cout << "--- Real_Value |" << Real_Value << "| < *String_Value |" << *String_Value << "|;" << endl;
checker.check ("Real < String.",
	true, Real_Value < *String_Value);

//	Array:

//	Default TYPE.
if (checker.Verbose)
	cout << "--- Array::default_type (Value::SET);" << endl;
Array::default_type (Value::SET);
checker.check ("Array, default type SET",
	Value::SET, Array::default_type ());

//	Construct and Add.
if (checker.Verbose)
	cout << "--- Array list; list.add (Integer_Value);" << endl;
Array
	list;
list.add (Integer_Value);

//	Write.
if (checker.Verbose)
	cout << "--- number = list.write (listing);" << endl;
list.write (listing);
expected << "{16#ABC# <integer>}";
checker.check ("Array, add Integer, write.",
	expected, listing);

//	Add sequence.
if (checker.Verbose)
	cout << "--- list.add (String_Value)" << endl
		 << "        .add (String_number)" << endl
		 << "        .add (Real_Value)" << endl
		 << "        .add (Real_number);" << endl;
list.add (String_Value)
	.add (String_number)
	.add (Real_Value)
	.add (Real_number);
//	Size.
if (checker.Verbose)
	cout << "--- list.size ();" << endl;
obtained_integer = list.size ();
expected_integer = 5;
checker.check ("Array, array of one element, add two Strings and two Reals.",
	expected_integer, obtained_integer);

//	Operator<<.
if (checker.Verbose)
	cout << "--- listing << list;" << endl;
listing << list;
expected << "{16#ABC# <integer>, Text, 23 <number>, 2748.0 <integer>, 23.0}";
checker.check ("Array, operator<<.",
	expected, listing);

//	Assignment.
if (checker.Verbose)
	cout << "--- Array list_copy = list;" << endl;
Array
	list_copy = list;
listing << list_copy;
expected << "{16#ABC# <integer>, Text, 23 <number>, 2748.0 <integer>, 23.0}";
checker.check ("Array, copy constructor.",
	expected, listing);

//	Operator +=.
if (checker.Verbose)
	cout << "--- list_copy += *String_Value |" << *String_Value << "|;" << endl;
list_copy += *String_Value;
listing << list_copy;
expected << "{16#ABC# <integer>, Text, 23 <number>, 2748.0 <integer>, 23.0, Text}";
checker.check ("Array += String.",
	expected, listing);

if (checker.Verbose)
	cout << "--- list_copy.back () == (String)\"Text\";" << endl;
checker.check ("Array, get last element (back), compare to String.",
	true, list_copy.back () == (String)"Text");

//	Function sort.
if (checker.Verbose)
	cout << "--- sort (list_copy.begin (), list_copy.end ());" << endl;
sort (list_copy.begin (), list_copy.end ());
listing << list_copy;
expected << "{23 <number>, 23.0, 16#ABC# <integer>, 2748.0 <integer>, Text, Text}";
checker.check ("Array, sort.",
	expected, listing);

//	Algorithm count_if on Iterator.
if (checker.Verbose)
	cout << "--- number = count_if (list.begin (), list.end ()," << endl
		 << "        mem_fun_ref (&Value::is_Numeric));" << endl;
obtained_integer = count_if
	(list.begin (), list.end (), mem_fun_ref (&Value::is_Numeric));
expected_integer = 3;
if (! checker.check ("Array, count_if Value::is_Numeric.",
	expected_integer, obtained_integer) &&
	checker.Verbose)
	cout << "    Array: " << list << endl;

//	Algorithm find_if on Iterator.
if (checker.Verbose)
	cout << "--- Array::iterator element =" << endl
		 << "        find_if (list.begin (), list.end ()," << endl
		 << "            bind2nd (equal_to<string>(), \"Text\"));" << endl;
Array::iterator
	element = find_if (list.begin (), list.end (),
		bind2nd (equal_to<string>(), String ("Text")));
if (checker.check ("Array, find_if equal_to<string>() \"Text\"",
	true, element != list.end ()))
	{
	listing << *element;
	expected << "Text";
	if (! checker.check ("Array, result from find_if equal_to<string>() \"Text\"",
		expected, listing) &&
		checker.Verbose)
		cout << "    Array: " << list << endl;
	}
else if (checker.Verbose)
	cout << "    Array: " << list << endl;

//		Alternate value find.
if (checker.Verbose)
	cout << "--- for (Array::iterator element = list.begin (); element != list.end (); ++element)" << endl
		 << "        if ((string)(*element) == \"Text\") break;" << endl;
for (element = list.begin ();
	 element != list.end ();
   ++element)
	if ((string)(*element) == "Text")
		break;
if (checker.check ("Array, if ((string)(*element) == \"Text\"",
	true, element != list.end ()))
	{
	listing << *element;
	expected << "Text";
	if (! checker.check ("Array, result from if ((string)(*element) == \"Text\"",
		expected, listing) &&
		checker.Verbose)
		cout << "    Array: " << list << endl;
	}
else if (checker.Verbose)
	cout << "    Array: " << list << endl;

//	Construct from Parser on string.
string
	values_string = "(1.9, 1.0)";
if (checker.Verbose)
	cout << "--- string values_string = "
			<< '|' << values_string << '|' << endl
		 << "--- Parser parser (values_string);" << endl;
Parser
	a_parser (values_string);
if (checker.Verbose)
	cout << "--- Array *array = new Array (parser);" << endl;
Array
	*array = new Array (a_parser);
//	Size.
if (checker.Verbose)
	cout << "--- number = array->size ();" << endl;
obtained_integer = array->size ();
expected_integer = 2;
checker.check ("Array, constructed from Parser on string, size.",
	expected_integer, obtained_integer);

//	Write.
if (checker.Verbose)
	cout << "--- array->write (listing, -1);" << endl;
array->write (listing, -1);
expected << "(1.9, 1.0)";
checker.check ("Array, write, indenting off.",
	expected, listing);

values_string =
    "(0 <integer>,\n"
    "    (\"A single value\"),\n"
    "'NOTHING' <symbol>,\n"
    "    {1.9, 1.0,\n"
    "        () <empty>,\n"
    "    1.4} <reals>)";
if (checker.Verbose)
	cout << "--- string values_string =" << endl
		 << '|' << values_string << '|' << endl
		 << "--- Parser parser (values_string);" << endl;
Parser
	parser (values_string);
if (checker.Verbose)
	cout << "--- Array *array = new Array (parser);" << endl;
array = new Array (parser);
//	Size.
if (checker.Verbose)
	cout << "--- number = array->size ();" << endl;
obtained_integer = array->size ();
expected_integer = 4;
checker.check ("Array, constructed from Parser on string, size.",
	expected_integer, obtained_integer);

//	Write.
if (checker.Verbose)
	cout << "--- array->write (listing, -1);" << endl;
array->write (listing, -1);
expected << "(0 <integer>, (\"A single value\"), 'NOTHING' <symbol>, "
		 << "{1.9, 1.0, () <empty>, 1.4} <reals>)";
checker.check ("Array, write, indenting off.",
	expected, listing);

if (checker.Verbose)
	cout << "--- array->write (listing, 0);" << endl;
array->write (listing, 0);
expected.str (values_string);
checker.check ("Array, write, indending on.",
	expected, listing);

//	Copy.
if (checker.Verbose)
	cout << "--- Array array_copy (*array);" << endl;
Array
	array_copy (*array);
if (checker.Verbose)
	cout << "--- listing << array_copy;" << endl;
listing << array_copy;
expected.str (values_string);
checker.check ("Array, copy constructor.",
	expected, listing);

//	Equality.
if (checker.Verbose)
	cout << "--- array_copy == *array;" << endl;
checker.check ("Array, copy == Array.",
	true, array_copy == *array);

//	Compare to Value.
if (checker.Verbose)
	cout << "--- *array > *String_Value;" << endl;
checker.check ("Array > String.",
	true, *array > *String_Value);

if (checker.Verbose)
	cout << "--- *String_Value > *array;" << endl;
checker.check ("String > Array.",
	false, *String_Value > *array);

//	Compare to Array.
if (checker.Verbose)
	cout << "--- list > *array;" << endl;
if (! checker.check ("Array > Array.",
	true, list > *array) &&
	checker.Verbose)
	cout << "    Array 1: " << list << endl
		 << "    Array 2: " << *array << endl;

//	Operator[].
if (checker.Verbose)
	cout << "--- array_copy[3][1] = 0.1;" << endl;
expected_real = 0.1;
array_copy[3][1] = expected_real;
obtained_real = array_copy[3][1];
checker.check ("Array[3][1] assign of double.",
	expected_real, obtained_real);

//	Compare to Array.
if (checker.Verbose)
	cout << "--- array_copy < *array;" << endl;
if (! checker.check ("Array, copy < Array.",
	true, array_copy < *array) &&
	checker.Verbose)
	cout << "    Array 1: " << array_copy << endl
		 << "    Array 2: " << *array << endl;

//	Operator +=.
if (checker.Verbose)
	cout << "--- array_copy += list;" << endl;
array_copy += list;
listing << array_copy;
expected << "(0 <integer>," << endl
		 << "    (\"A single value\")," << endl
		 << "'NOTHING' <symbol>," << endl
		 << "    {1.9, 0.1," << endl
		 << "        () <empty>," << endl
		 << "    1.4} <reals>," << endl
		 << "16#ABC# <integer>, Text, 23 <number>, 2748.0 <integer>, 23.0)";
checker.check ("Array += Array.",
	expected, listing);

//	Depth_Iterator:

//	Postfix increment.
if (checker.Verbose)
	cout << "--- Array::depth_iterator" << endl
		 << "        position = array->begin_depth ()," << endl
		 << "        array_end = array->end_depth ();" << endl
		 << "    while (position != array_end)" << endl
		 << "        listing << *position++ << endl;" << endl;
expected << "0 <integer>" << endl
		 << "(\"A single value\")" << endl
		 << "\"A single value\"" << endl
		 << "'NOTHING' <symbol>" << endl
		 << "{1.9, 1.0," << endl
		 << "    () <empty>," << endl
		 << "1.4} <reals>" << endl
		 << "1.9" << endl
		 << "1.0" << endl
		 << "() <empty>" << endl
		 << "1.4" << endl;
Array::depth_iterator
	position = array->begin_depth (),
	array_end = array->end_depth ();
while (position != array_end)
	listing << *position++ << endl;
checker.check ("Array, Depth_Iterator.",
	expected, listing);

//	Function sort.
if (checker.Verbose)
	cout << "--- sort (array->begin_depth (), array->end_depth ());" << endl;
sort (array->begin_depth (), array->end_depth ());
listing << *array;
expected << "(0 <integer>, 'NOTHING' <symbol>," << endl
		 << "    (\"A single value\")," << endl
		 << "    {1.0, 1.4, 1.9," << endl
		 << "        () <empty>} <reals>)";
checker.check ("Array, sort.",
	expected, listing);


//	Function find.
if (checker.Verbose)
	cout << "--- position = find (array->begin_depth (), array->end_depth (), "
		 << "String (\"NOTHING\");" << endl;
position = find (array->begin_depth (), array->end_depth (), String ("NOTHING"));
if (checker.check ("Array, find String (\"NOTHING\")",
	true, position != array->end_depth ()))
	{
	listing << *position;
	expected << "'NOTHING' <symbol>";
	if (! checker.check ("Array, result from find String \"NOTHING\"",
		expected, listing) &&
		checker.Verbose)
		cout << "    Array: " << *array << endl;
	}
else if (checker.Verbose)
	cout << "    Array: " << *array << endl;

if (checker.Verbose)
	cout << "--- position = find (array->begin_depth (), array->end_depth (), "
		 << "Real (1.4);" << endl;
position = find (array->begin_depth (), array->end_depth (), Real (1.4));
if (checker.check ("Array, find Real (1.4)",
	true, position != array->end_depth ()))
	{
	listing << *position;
	expected << "1.4";
	if (! checker.check ("Array, result from find Real (1.4)",
		expected, listing) &&
		checker.Verbose)
		cout << "    Array: " << *array << endl;
	}
else if (checker.Verbose)
	cout << "    Array: " << *array << endl;

} catch (Exception except)
	{
	cout << except.message () << endl;
	status = 1;
	}

cout << endl
	 << "Checks: " << checker.Checks_Total << endl
	 << "Passed: " << checker.Checks_Passed << endl;

exit (status);
}
