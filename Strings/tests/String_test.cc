/*	String_test

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

CVS ID:	String_test.cc,v 1.17 2013/02/25 00:05:09 castalia Exp
*******************************************************************************/

#include	"String.hh"
using namespace idaeim::Strings;

#include	"Utility/Checker.hh"

#include	<iostream>
#include	<string>
#include	<cstdlib>
using namespace std;


int
main
	(
	int		count,
	char	**arguments
	)
{
cout << "String class test" << endl
	 << String::ID << endl << endl;

idaeim::Checker
	checker;
bool
	result;

if (count > 1)
	{
	if (*arguments[1] == '-')
		 arguments[1]++;
	if (*arguments[1] == 'v' ||
	 	*arguments[1] == 'V')
		checker.Verbose = true;
	}

//	Equality.

cout << "--- String comparisons" << endl;

if (checker.Verbose)
	cout << "String a_String (\"a_String\")" << endl
		 << "String b_String (\"a_String\")" << endl
		 << "a_String == b_String" <<endl;
String
	a_String ("a_String"),
	b_String ("a_String");
result = (a_String == b_String);
if (! checker.check ("String == String, identical Strings",
	true, result))
	exit (1);

checker.Compare_Equals = false;
if (checker.Verbose)
	cout << "b_String = \"b_String\"" << endl
		 << "b_String == a_String" << endl;
b_String = "b_String";
if (! checker.check ("String == String, dissimilar Strings",
	a_String, b_String))
	exit (1);

if (checker.Verbose)
	cout << "b_String = \"a_Str\"" << endl
		 << "b_String == a_String" << endl;
b_String = "a_Str";
if (! checker.check ("String == String, similar but different Strings",
	a_String, b_String))
	exit (1);

checker.Compare_Equals = true;
if (checker.Verbose)
	cout << "a_String.begins_with (b_String)" << endl;
checker.check ("begins_with, matching prefix",
	true, a_String.begins_with (b_String));

if (checker.Verbose)
	cout << "b_String = \"a_str\"" << endl
		 << "a_String.begins_with (b_String)" << endl;
b_String = "a_str";
checker.check ("begins_with, different case for prefix",
	false, a_String.begins_with (b_String));

if (checker.Verbose)
	cout << "a_String.begins_with (b_String, 0, false)" << endl;
checker.check ("begins_with, case insensitive with different case for prefix",
	true, a_String.begins_with (b_String, 0, false));

if (checker.Verbose)
	cout << "b_String = \"A_STRING\"" << endl
		 << "a_String.begins_with (b_String, 0, false)" << endl;
b_String = "A_STRING";
checker.check ("equals_ignore_case, only case is different",
	true, a_String.begins_with (b_String, 0, false));

//	Numerical representation.
cout << "--- Numerical representation, "
	 << (sizeof (String::Integer_type) << 3) << "-bit integers" << endl;

if (checker.Verbose)
	cout << "a_String = 57" << endl;
a_String = 57;
checker.check ("assignment of int",
	"57", a_String);

if (checker.Verbose)
	cout << "String::Integer_type number = 100" << endl
		 << "String *number_String = new String (number)" << endl
		 << "*number_String == \"100\"" << endl;
String::Integer_type
	number = 100;
String
	*number_String;
if (checker.Verbose)
	cout << "new String (" << number << ", 16)" << endl;
number_String = new String (number);
checker.check ("construct from Integer_type",
	"100", *number_String);
delete number_String;

if (checker.Verbose)
	cout << "new String (" << number << ", 16)" << endl;
number_String = new String (number, 16);
checker.check ("construct from int with base 16 representation",
	"64", *number_String);
delete number_String;

if (checker.Verbose)
	cout << "number_String = new String (" << number << ", 2)" << endl;
number_String = new String (number, 2);
checker.check ("construct from Integer_type with base 2 representation",
	"1100100", *number_String);
delete number_String;

number = -1;
string
	a_string;
if (sizeof (String::Integer_type) > 4)
	a_string = "FFFFFFFFFFFFFFFF";
else
	a_string = "FFFFFFFF";
if (checker.Verbose)
	cout << "new String (" << number << ", String::UNSIGNED, 16)" << endl;
number_String = new String (number, String::UNSIGNED, 16);
checker.check (
	"construct from Integer_type, UNSIGNED, with base 16 representation",
	a_string, *number_String);
delete number_String;

number = 5;
if (checker.Verbose)
	cout << "new String (" << number << ", 2, 5)" << endl;
number_String = new String (number, 2, 5);
checker.check (
	"construct from Integer_type, with base 2 5 digit representation",
	"00101", *number_String);
delete number_String;

if (sizeof (String::Integer_type) > 4)
	{
	number = ((long long)(1073741824) << 4);
	if (checker.Verbose)
		cout << "number_String = new String (" << number  << ')' << endl;
	number_String = new String (number);
	checker.check ("construct from Integer_type with big long long value",
		"17179869184", *number_String);
	delete number_String;
	}

//	Skipping and Indexing.
cout << "--- Skipping and Indexing" << endl;

a_String = "  This is the test String.  ";
if (checker.Verbose)
	cout << "a_String = \"" << a_String << '"' << endl
		 << "            0123456789012345678901234567" << endl
		 << "b_String = \" Tthise\"" << endl
		 << "a_String.skip_over (b_String)" << endl;
b_String = " Tthise";
checker.check ("skip_over",
	19, (int)a_String.skip_over (b_String));

if (checker.Verbose)
	cout << "a_String.skip_over (b_String, 20)" << endl;
checker.check ("skip_over from index, valid location",
	21, (int)a_String.skip_over (b_String, 20));

if (checker.Verbose)
	cout << "a_String.skip_back_over (b_String, 18)" << endl;
checker.check ("skip_back_over from index, invalid location",
	String::EOS, a_String.skip_back_over (b_String, 18));

if (checker.Verbose)
	cout << "b_String = \" ing.\"" << endl
		 << "a_String.skip_back_over (b_String)" << endl;
b_String = " ing.";
checker.check ("skip_back_over",
	21, (int)a_String.skip_back_over (b_String));

if (checker.Verbose)
	cout << "b_String = \"Sr\"" << endl
		 << "a_String.skip_until (b_String)" << endl;
b_String = "Sr";
checker.check ("skip_until",
	19, (int)a_String.skip_until (b_String));

if (checker.Verbose)
	cout << "a_String.skip_until (b_String, 22)" << endl;
checker.check ("skip_until from index, invalid location",
	String::EOS, a_String.skip_until (b_String, 22));

if (checker.Verbose)
	cout << "a_String.skip_back_until (b_String)" << endl;
checker.check ("skip_back_until",
	21, (int)a_String.skip_back_until (b_String));

if (checker.Verbose)
	cout << "a_String.skip_back_until (b_String, 18)" << endl;
checker.check ("skip_back_until, invalid location",
	String::EOS, a_String.skip_back_until (b_String, 18));

if (checker.Verbose)
	cout << "a_String.index_of ('S')" << endl;
checker.check ("index_of character",
	19, (int)a_String.index_of ('S'));

if (checker.Verbose)
	cout << "b_String = \"test\"" << endl
		 << "a_String.index_of (b_String)" << endl;
b_String = "test";
checker.check ("index_of String",
	14, (int)a_String.index_of (b_String));

//	Trimming.
cout << "--- Trimming" << endl;

if (checker.Verbose)
	cout << "b_String = a_String" << endl
		 << "a_String.trim_leading ()" << endl;
b_String = a_String;
checker.check ("trim_leading",
	"This is the test String.  ", a_String.trim_leading ());

if (checker.Verbose)
	cout << "b_String = a_String" << endl
		 << "a_String.trim_trailing ()" << endl;
a_String = b_String;
checker.check ("trim_trailing",
	"  This is the test String.", a_String.trim_trailing ());

if (checker.Verbose)
	cout << "b_String.trim ()" << endl;
checker.check ("trim",
	"This is the test String.", b_String.trim ());

//	Replacing.
cout << "--- Replacing" << endl;

if (checker.Verbose)
	cout << "a_String = \"the * * test*is    the* *best\"" << endl
		 << "a_String" << endl
		 << "  .replace (\"the\", \"these\")" << endl
		 << "  .replace (\"*is\", \"s*are\")" << endl
		 << "  .replace (\"these\", \"the\", 1)" << endl;
a_String = "the * * test*is    the* *best";
a_String
	.replace ("the", "these")
	.replace ("*is", "s*are")
	.replace ("these", "the", 1);
checker.check ("replace",
	"these * * tests*are    the* *best", a_String);

if (checker.Verbose)
	cout << "a_String.replace_span (\" *\", \" | \")" << endl;
a_String.replace_span (" *", " | ");
checker.check ("replace_span",
	"these | tests | are | the | best", a_String);

//	Escaped and Special characters
cout << "--- Escaped and Special characters" << endl;

if (checker.Verbose)
	cout << "a_String = \"HT:\\tampersand:\\046NL:\\n\"" << endl
		 << "a_String.escape_to_special ()" << endl;
a_String = "HT:\\tampersand:\\046NL:\\n";
a_String.escape_to_special ();
checker.check ("escape_to_special",
	"HT:\tampersand:&NL:\n", a_String);

int
	index = a_String.index_of ('&');
if (index != (int)String::EOS)
	{
	if (checker.Verbose)
		cout << "a_String.at (index) = 0231" << endl
			 << "a_String.replace (\"ampersand\", \"\\t\\octal\\0231\")" << endl
			 << "a_String.special_to_escape ()" << endl;
	a_String.at (index) = 0231;
	a_String.replace ("ampersand", "\\t\\octal\\0231");
	a_String.special_to_escape ();
	checker.check ("special_to_escape",
		"HT:\\t\\t\\\\octal\\0231:\\0231NL:\\n", a_String);
	}

//	RAD50
cout << "--- RAD50" << endl;

if (checker.Verbose)
	cout << "is_RAD50 ('A')" << endl;
checker.check ("is_RAD50 of valid char",
	true, is_RAD50 ('A'));

if (checker.Verbose)
	cout << "is_RAD50 ('a')" << endl;
checker.check ("is_RAD50 of invalid char",
	false, is_RAD50 ('a'));

if (checker.Verbose)
	cout << "to_RAD50 (string ())" << endl;
checker.check ("to_RAD50 of empty string",
	0, (int)to_RAD50 (string ()));

if (checker.Verbose)
	cout << "to_RAD50 (\"ABC\")" << endl;
checker.check ("to_RAD50 of valid string",
	1683, (int)to_RAD50 ("ABC"));

if (checker.Verbose)
	cout << "to_RAD50 (\"999\")" << endl;
checker.check ("to_RAD50 of max valid string",
	63999, (int)to_RAD50 ("999"));

if (checker.Verbose)
	cout << "to_RAD50 (\"AAa\")" << endl;
checker.check ("to_RAD50 of invalid string, encode_invalid true",
	1669, (int)to_RAD50 ("AAa", true));

if (checker.Verbose)
	cout << "to_RAD50 (\"AAa\")" << endl;
checker.check ("to_RAD50 of invalid string, encode_invalid false",
	65535, (int)to_RAD50 ("AAa", false));

if (checker.Verbose)
	cout << "from_RAD50 (0)" << endl;
checker.check ("from_RAD50 of zero",
	string ("   "), from_RAD50 (0));

if (checker.Verbose)
	cout << "from_RAD50 (1683)" << endl;
checker.check ("from_RAD50 of valid value",
	string ("ABC"), from_RAD50 (1683));

if (checker.Verbose)
	cout << "from_RAD50 (63999)" << endl;
checker.check ("from_RAD50 of max valid value",
	string ("999"), from_RAD50 (63999));

if (checker.Verbose)
	cout << "from_RAD50 (1669)" << endl;
checker.check ("from_RAD50 of invalid value, decode_invalid true",
	string ("AA?"), from_RAD50 (1669, true));

if (checker.Verbose)
	cout << "from_RAD50 (1669)" << endl;
checker.check ("from_RAD50 of invalid value, decode_invalid false",
	string (), from_RAD50 (1669, false));

cout << endl
	 << "Checks: " << checker.Checks_Total << endl
	 << "Passed: " << checker.Checks_Passed << endl;

exit ((checker.Checks_Total == checker.Checks_Passed) ? 0 : 1);
}
