/*	Lister_test

Copyright (C) 2004 - 2013  Bradford Castalia

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

CVS ID: Lister_test.cc,v 1.17 2013/02/24 22:27:12 castalia Exp
*******************************************************************************/

#include	<iostream>
#include	<sstream>
#include	<algorithm>
#include	<functional>

#include	"Lister.hh"
#include	"Parameter.hh"
#include	"Value.hh"
#include	"Parser.hh"
#include	"Utility/Checker.hh"

using namespace std;
using namespace idaeim;
using namespace PVL;



int
main
	(
	int		count,
	char	**arguments
	)
{
cout << "*** Lister class test" << endl
	 << Lister::ID << endl
	 << endl;

int
	status = 0;
Checker
	checker;
ostringstream
	expected,
	listing;

if (count > 1)
	{
	if (*arguments[1] == '-')
		 arguments[1]++;
	if (*arguments[1] == 'v' ||
		*arguments[1] == 'V')
		checker.Verbose = true;
	}

try {

//	Construct.
if (checker.Verbose)
	cout << "--- Lister lister (listing);" << endl;
Lister
	lister (listing);

//	Values:

//		Integer.

cout << "Integer precision: "
	 << (sizeof (Value::Integer_type) << 3) << "-bit" << endl;

if (checker.Verbose)
	cout << "--- Integer integer (255, 16, 3); integer.units (\"hex\");" << endl;
Integer
	integer (255, 16, 3);
integer.units ("hex");
if (checker.Verbose)
	cout << "--- lister.write (integer);" << endl;
lister.write (integer);
if (checker.Verbose)
	cout << "--- lister.new_line ();" << endl;
lister.new_line ();
expected << "16#0FF# <hex>" << endl;
if (! checker.check ("Value, Integer.",
	expected, listing))
	exit (1);

//	Output binding.
if (checker.Verbose)
	cout << "--- listing << lister << integer << endl;" << endl;
listing << lister << integer << endl;
expected << "16#0FF# <hex>" << endl;
if (! checker.check ("Output binding.",
	expected, listing))
	exit (1);

//		String.
if (checker.Verbose)
	cout << "--- String text (\"Text...\", Value::TEXT);" << endl;
String
	text ("Text...", Value::TEXT);
if (checker.Verbose)
	cout << "--- lister << text << endl;" << endl;
lister << text << endl;
expected << "\"Text...\"" << endl;
if (! checker.check ("Value, String.",
	expected, listing))
	exit (1);

//		Real.

cout << "Real precision: "
	 << (sizeof (Value::Real_type) << 3) << "-bit" << endl;

if (checker.Verbose)
	cout << "--- Real real (123.45, 7);" << endl;
Real
	real (123.45, 7);
if (checker.Verbose)
	cout << "--- lister << real << endl;" << endl;
lister << real << endl;
expected << "123.4500" << endl;
if (! checker.check ("Value, Real.",
	expected, listing))
	exit (1);

//		Array.

if (checker.Verbose)
	cout << "--- Array::default_type (Value::SET);" << endl;
Array::default_type (Value::SET);
if (! checker.check ("Array default type SET.",
	Value::SET, Array::default_type ()))
	exit (1);

if (checker.Verbose)
	cout << "--- Array array; array.add (integer);" << endl;
Array
	array;

//			Single value.
array.add (integer);
if (checker.Verbose)
	cout << "--- lister << array << endl;" << endl;
lister << array << endl;
expected << "{16#0FF# <hex>}" << endl;
if (! checker.check ("Value, Array, single element.",
	expected, listing))
	exit (1);

//			Multiple values.
if (checker.Verbose)
	cout << "--- array.add (text)" << endl
		 << "         .add (real)" << endl;
array.add (text)
	 .add (real);
if (checker.Verbose)
	cout << "--- lister << array << endl;" << endl;
lister << array << endl;
expected << "{16#0FF# <hex>, \"Text...\", 123.4500}" << endl;
if (! checker.check ("Value, Array, multiple elements.",
	expected, listing))
	exit (1);

//			Multi-array.
if (checker.Verbose)
	cout << "--- Array multi_array = array;" << endl
		 << "    multi_array.add (array.type (Value::SEQUENCE));" << endl;
Array
	multi_array = array;
multi_array.add (array.type (Value::SEQUENCE));
if (checker.Verbose)
	cout << "--- lister.indent_width (Lister::NO_INDENT)" << endl
		 << "          .write (multi_array)" << endl
		 << "          .new_line ();" << endl;
lister.indent_width (Lister::NO_INDENT)
	  .write (multi_array)
	  .new_line ();
expected << "{16#0FF# <hex>, \"Text...\", 123.4500, "
		 << "(16#0FF# <hex>, \"Text...\", 123.4500)}" << endl;
if (! checker.check ("Value, Array, with Array element.",
	expected, listing))
	exit (1);

//	Page width for wrap.
if (checker.Verbose)
	cout << "--- lister.indent_width (Lister::default_indent_width ());" << endl;
lister.indent_width (Lister::default_indent_width ());
if (checker.Verbose)
	cout << "--- lister.page_width (32) << multi_array << endl;" << endl;
lister.page_width (32) << multi_array << endl;
expected << "{16#0FF# <hex>, \"Text...\"," << endl
		 << "123.4500," << endl
		 << "    (16#0FF# <hex>, \"Text...\"," << endl
		 << "    123.4500)}" << endl;
if (! checker.check ("Value, Array, with Array element, page width 32.",
	expected, listing))
	exit (1);
if (checker.Verbose)
	cout << "--- lister.page_width (80);" << endl;
lister.page_width (80);

//	Parameters:

//	Assignment -

if (checker.Verbose)
	cout << "--- Assignment assignment;" << endl;
Assignment
	assignment;

//		Of Integer.
if (checker.Verbose)
	cout << "--- assignment.name (\"Integer\") = integer;" << endl;
assignment.name ("Integer") = integer;
if (checker.Verbose)
	cout << "--- lister << assignment;" << endl;
lister << assignment;
expected << "Integer = 16#0FF# <hex>" << endl;
if (! checker.check ("Assignment of Integer.",
	expected, listing))
	exit (1);

if (checker.Verbose)
	cout << "--- Aggregate aggregate (\"Aggregate\");" << endl;
Aggregate
	aggregate ("Aggregate");
if (checker.Verbose)
	cout << "--- aggregate.add (assignment);" << endl;
aggregate.add (assignment);

//		Of Array, no indenting.
if (checker.Verbose)
	cout << "--- assignment.name (\"Array\") = array;" << endl;
assignment.name ("Array") = array;
if (checker.Verbose)
	cout << "--- assignment.indenting (false);" << endl;
assignment.indenting (false);
if (checker.Verbose)
	cout << "--- lister << assignment;" << endl;
lister << assignment;
expected << "Array = (16#0FF# <hex>, \"Text...\", 123.4500)" << endl;
if (! checker.check ("Assignment of Array, assignment indenting off.",
	expected, listing))
	exit (1);

//		Of Array, indented.
if (checker.Verbose)
	cout << "--- assignment.indenting (true);" << endl;
assignment.indenting (true);
if (checker.Verbose)
	cout << "--- lister << assignment;" << endl;
lister << assignment;
expected << "Array =" << endl
		 << "    (16#0FF# <hex>, \"Text...\", 123.4500)" << endl;
if (! checker.check ("Assignment of Array, assignment indenting on.",
	expected, listing))
	exit (1);

//		Of Array, assign aligned and wrapped.
if (checker.Verbose)
	cout << "--- lister.assign_align (true).page_width (27) << assignment;" << endl;
lister.assign_align (true).page_width (27) << assignment;
expected << "Array = (16#0FF# <hex>," << endl
		 << "        \"Text...\"," << endl
		 << "        123.4500)" << endl;
if (! checker.check ("Assignment of Array, assign align on, page width 27.",
	expected, listing))
	exit (1);
if (checker.Verbose)
	cout << "--- lister.assign_align (false);" << endl
		 << "--- lister.page_width (lister.default_page_width ());" << endl;
lister.assign_align (false);
lister.page_width (lister.default_page_width ());

if (checker.Verbose)
	cout << "--- aggregate.add (assignment);" << endl;
aggregate.add (assignment);

//		Of multi-array, indented.
if (checker.Verbose)
	cout << "--- assignment.name (\"multi-Array\") = multi_array;" << endl;
assignment.name ("multi-Array") = multi_array;
if (checker.Verbose)
	cout << "--- lister << assignment;" << endl;
lister << assignment;
expected << "multi-Array =" << endl
		 << "    {16#0FF# <hex>, \"Text...\", 123.4500," << endl
		 << "        (16#0FF# <hex>, \"Text...\", 123.4500)}" << endl;
if (! checker.check ("Assignment of Array with Array element, assignment indenting on.",
	expected, listing))
	exit (1);

//	Aggregate -

if (checker.Verbose)
	cout << "--- aggregate.add (assignment);" << endl;
aggregate.add (assignment);

if (checker.Verbose)
	cout << "--- assignment.name (\"String\") = text;" << endl;
assignment.name ("String") = text;
if (checker.Verbose)
	cout << "--- aggregate.add (assignment);" << endl;
aggregate.add (assignment);
if (checker.Verbose)
	cout << "--- assignment.name (\"Real\") = real;" << endl;
assignment.name ("Real") = real;
if (checker.Verbose)
	cout << "--- aggregate.add (assignment);" << endl;
aggregate.add (assignment);
if (checker.Verbose)
	cout << "--- lister << aggregate;" << endl;
lister << aggregate;
expected << "Group = Aggregate" << endl
		 << "    Integer = 16#0FF# <hex>" << endl
		 << "    Array =" << endl
		 << "        (16#0FF# <hex>, \"Text...\", 123.4500)" << endl
		 << "    multi-Array =" << endl
		 << "        {16#0FF# <hex>, \"Text...\", 123.4500," << endl
		 << "            (16#0FF# <hex>, \"Text...\", 123.4500)}" << endl
		 << "    String = \"Text...\"" << endl
		 << "    Real = 123.4500" << endl
		 << "End_Group" << endl;
if (! checker.check ("Aggregate.",
	expected, listing))
	exit (1);

if (checker.Verbose)
	cout << "--- lister.begin_aggregates (true) << aggregate;" << endl;
lister.begin_aggregates (true) << aggregate;
expected << "Begin_Group = Aggregate" << endl
		 << "    Integer = 16#0FF# <hex>" << endl
		 << "    Array =" << endl
		 << "        (16#0FF# <hex>, \"Text...\", 123.4500)" << endl
		 << "    multi-Array =" << endl
		 << "        {16#0FF# <hex>, \"Text...\", 123.4500," << endl
		 << "            (16#0FF# <hex>, \"Text...\", 123.4500)}" << endl
		 << "    String = \"Text...\"" << endl
		 << "    Real = 123.4500" << endl
		 << "End_Group" << endl;
if (! checker.check ("Aggregate, begin on.",
	expected, listing))
	exit (1);

if (checker.Verbose)
	cout << "--- lister.name_end_aggregates (true) << aggregate;" << endl;
lister.name_end_aggregates (true) << aggregate;
expected << "Begin_Group = Aggregate" << endl
		 << "    Integer = 16#0FF# <hex>" << endl
		 << "    Array =" << endl
		 << "        (16#0FF# <hex>, \"Text...\", 123.4500)" << endl
		 << "    multi-Array =" << endl
		 << "        {16#0FF# <hex>, \"Text...\", 123.4500," << endl
		 << "            (16#0FF# <hex>, \"Text...\", 123.4500)}" << endl
		 << "    String = \"Text...\"" << endl
		 << "    Real = 123.4500" << endl
		 << "End_Group = Aggregate" << endl;
if (! checker.check ("Aggregate, begin and name end on.",
	expected, listing))
	exit (1);

if (checker.Verbose)
	cout << "--- lister.uppercase_aggregates (true) << aggregate;" << endl;
lister.uppercase_aggregates (true) << aggregate;
expected << "BEGIN_GROUP = Aggregate" << endl
		 << "    Integer = 16#0FF# <hex>" << endl
		 << "    Array =" << endl
		 << "        (16#0FF# <hex>, \"Text...\", 123.4500)" << endl
		 << "    multi-Array =" << endl
		 << "        {16#0FF# <hex>, \"Text...\", 123.4500," << endl
		 << "            (16#0FF# <hex>, \"Text...\", 123.4500)}" << endl
		 << "    String = \"Text...\"" << endl
		 << "    Real = 123.4500" << endl
		 << "END_GROUP = Aggregate" << endl;
if (! checker.check ("Aggregate, begin, name end and uppercase on.",
	expected, listing))
	exit (1);

if (checker.Verbose)
	cout << "--- lister.begin_aggregates (false) << aggregate;" << endl;
lister.begin_aggregates (false) << aggregate;
expected << "GROUP = Aggregate" << endl
		 << "    Integer = 16#0FF# <hex>" << endl
		 << "    Array =" << endl
		 << "        (16#0FF# <hex>, \"Text...\", 123.4500)" << endl
		 << "    multi-Array =" << endl
		 << "        {16#0FF# <hex>, \"Text...\", 123.4500," << endl
		 << "            (16#0FF# <hex>, \"Text...\", 123.4500)}" << endl
		 << "    String = \"Text...\"" << endl
		 << "    Real = 123.4500" << endl
		 << "END_GROUP = Aggregate" << endl;
if (! checker.check ("Aggregate, name end and uppercase on, begin off.",
	expected, listing))
	exit (1);

//		Assignment alignment.
if (checker.Verbose)
	cout << "--- text =" << endl
		 << "    \"Now is the time for all good men to come to the aid of their country. \"" << endl
		 << "    \"Now is the time for all good men to come to the aid of their country. \"" << endl
		 << "    \"Now is the time for all good men to come to the aid of their country.\";" << endl
		 << "--- assignment.name (\"Description\") = text;" << endl;
text =
	"Now is the time for all good men to come to the aid of their country. "
	"Now is the time for all good men to come to the aid of their country. "
	"Now is the time for all good men to come to the aid of their country.";
assignment.name ("Description") = text;
if (checker.Verbose)
	cout << "--- aggregate.add (assignment);" << endl;
aggregate.add (assignment);
if (checker.Verbose)
	cout << "--- lister.assign_align (true) << aggregate;" << endl;
lister.assign_align (true) << aggregate;
expected << "GROUP = Aggregate" << endl
		 << "    Integer     = 16#0FF# <hex>" << endl
		 << "    Array       = (16#0FF# <hex>, \"Text...\", 123.4500)" << endl
		 << "    multi-Array = {16#0FF# <hex>, \"Text...\", 123.4500," << endl
		 << "                      (16#0FF# <hex>, \"Text...\", 123.4500)}" << endl
		 << "    String      = \"Text...\"" << endl
		 << "    Real        = 123.4500" << endl
		 << "    Description = \"Now is the time for all good men to come to the aid of their" << endl
		 << "                  country. Now is the time for all good men to come to the aid" << endl
		 << "                  of their country. Now is the time for all good men to come to" << endl
		 << "                  the aid of their country.\"" << endl
		 << "END_GROUP = Aggregate" << endl;
if (! checker.check ("Aggregate, name end and uppercase on, begin off, align assign on.",
	expected, listing))
	exit (1);

if (checker.Verbose)
	cout << "--- for (Aggregate::iterator parameter = aggregate.begin ();" << endl
		 << "        parameter != aggregate.end ();" << endl
		 << "        ++parameter)" << endl
		 << "        parameter->indenting (false);" << endl
		 << "    lister.page_width (58) << aggregate;" << endl;
for (Aggregate::iterator
		parameter = aggregate.begin ();
	 parameter != aggregate.end ();
	 ++parameter)
	parameter->indenting (false);
lister.page_width (58) << aggregate;
expected << "GROUP = Aggregate" << endl
		 << "    Integer     = 16#0FF# <hex>" << endl
		 << "    Array       = (16#0FF# <hex>, \"Text...\", 123.4500)" << endl
		 << "    multi-Array = {16#0FF# <hex>, \"Text...\", 123.4500," << endl
		 << "                  (16#0FF# <hex>, \"Text...\", 123.4500)}" << endl
		 << "    String      = \"Text...\"" << endl
		 << "    Real        = 123.4500" << endl
		 << "    Description = \"Now is the time for all good men to" << endl
		 << "                  come to the aid of their country. Now" << endl
		 << "                  is the time for all good men to come to" << endl
		 << "                  the aid of their country. Now is the" << endl
		 << "                  time for all good men to come to the" << endl
		 << "                  aid of their country.\"" << endl
		 << "END_GROUP = Aggregate" << endl;
if (! checker.check ("Aggregate, assignment indenting off, align assign on.",
	expected, listing))
	exit (1);

if (checker.Verbose)
	cout << "--- lister.assign_align (false);" << endl
		 << "--- lister.page_width (Lister::default_page_width ());" << endl;
lister.assign_align (false);
lister.page_width (Lister::default_page_width ());

if (checker.Verbose)
	cout << "--- aggregate.clear ();" << endl;
aggregate.clear ();
if (checker.Verbose)
	cout << "--- Aggregate object (\"Integer Object\", Parameter::OBJECT);" << endl;
Aggregate
	object ("Integer Object", Parameter::OBJECT);
if (checker.Verbose)
	cout << "--- assignment.name (\"Integer\") = integer;" << endl;
assignment.name ("Integer") = integer;
if (checker.Verbose)
	cout << "--- object.add (assignment);" << endl;
object.add (assignment);
if (checker.Verbose)
	cout << "--- aggregate.add (object);" << endl;
aggregate.add (object);
if (checker.Verbose)
	cout << "--- assignment.name (\"Real\") = real;" << endl;
assignment.name ("Real") = real;
if (checker.Verbose)
	cout << "--- object.add (assignment);" << endl;
object.add (assignment);
if (checker.Verbose)
	cout << "--- aggregate.add (object.name (\"Numbers\"));" << endl;
aggregate.add (object.name ("Numbers"));
if (checker.Verbose)
	cout << "--- Aggregate group (\"Empty\");" << endl;
Aggregate
	group ("Empty");
if (checker.Verbose)
	cout << "--- object.add (group);" << endl;
object.add (group);
if (checker.Verbose)
	cout << "--- aggregate.add (object.name (\"Collection\"));" << endl;
aggregate.add (object.name ("Collection"));
if (checker.Verbose)
	cout << "--- aggregate.add (group);" << endl;
aggregate.add (group);
if (checker.Verbose)
	cout << "--- lister << aggregate;" << endl;
lister << aggregate;
expected << "GROUP = Aggregate" << endl
		 << "    OBJECT = \"Integer Object\"" << endl
		 << "        Integer = 16#0FF# <hex>" << endl
		 << "    END_OBJECT = \"Integer Object\"" << endl
		 << "    OBJECT = Numbers" << endl
		 << "        Integer = 16#0FF# <hex>" << endl
		 << "        Real = 123.4500" << endl
		 << "    END_OBJECT = Numbers" << endl
		 << "    OBJECT = Collection" << endl
		 << "        Integer = 16#0FF# <hex>" << endl
		 << "        Real = 123.4500" << endl
		 << "        GROUP = Empty" << endl
		 << "        END_GROUP = Empty" << endl
		 << "    END_OBJECT = Collection" << endl
		 << "    GROUP = Empty" << endl
		 << "    END_GROUP = Empty" << endl
		 << "END_GROUP = Aggregate" << endl;
if (! checker.check ("Aggregates with aggregate elements, name end and uppercase on, begin off.",
	expected, listing))
	exit (1);

if (checker.Verbose)
	cout << "--- lister << object.name (Parser::CONTAINER_NAME);" << endl;
lister << object.name (Parser::CONTAINER_NAME);
expected << "Integer = 16#0FF# <hex>" << endl
		 << "Real = 123.4500" << endl
		 << "GROUP = Empty" << endl
		 << "END_GROUP = Empty" << endl
		 << "END" << endl;
checker.check ("Aggregate with Parser::CONTAINER_NAME.",
	expected, listing);

if (checker.Verbose)
	cout << "--- Parameter* parameter (aggregate.find (\"Collection\"));" << endl;
Parameter
	*parameter (aggregate.find ("Collection"));
if (parameter &&
	parameter->parent ())
	{
	if (checker.Verbose)
		cout << "--- lister << *(parameter->name (Parser::CONTAINER_NAME));" << endl;
	lister << object.name (Parser::CONTAINER_NAME);
	expected << "Integer = 16#0FF# <hex>" << endl
			 << "Real = 123.4500" << endl
			 << "GROUP = Empty" << endl
			 << "END_GROUP = Empty" << endl
			 << "END" << endl;
	checker.check ("Aggregate from nested list set to Parser::CONTAINER_NAME.",
		expected, listing);
	}

//	Comments:

if (checker.Verbose)
	cout << "--- string comment (\"Comment.\");" << endl;
string
	comment ("Comment.");

//	One single line.
if (checker.Verbose)
	cout << "--- lister.single_line_comments (true)" << endl
		 << "          .write_comment (comment);" << endl;
lister.single_line_comments (true)
	  .write_comment (comment);
expected << "/* Comment. */" << endl;
if (! checker.check ("Comments, one line, single line on.",
	expected, listing))
	exit (1);

//	One multi-line.
if (checker.Verbose)
	cout << "--- lister.single_line_comments (false)" << endl
		 << "          .write_comment (comment);" << endl;
lister.single_line_comments (false)
	  .write_comment (comment);
expected << "/* Comment." << endl
		 << "*/" << endl;
if (! checker.check ("Comments, one line, single line off.",
	expected, listing))
	exit (1);

//	Multiple single line.
if (checker.Verbose)
	cout << "--- comment += \"\\n0123456789 123456789 123456789\";" << endl;
comment += "\n0123456789 123456789 123456789";
if (checker.Verbose)
	cout << "--- lister.single_line_comments (true)" << endl
		 << "          .write_comment (comment);" << endl;
lister.single_line_comments (true)
	  .write_comment (comment);
expected << "/* Comment. */" << endl
		 << "/* 0123456789 123456789 123456789 */" << endl;
if (! checker.check ("Comments, two lines, single line on.",
	expected, listing))
	exit (1);

//	Multiple multi-line.
if (checker.Verbose)
	cout << "--- lister.single_line_comments (false)" << endl
		 << "          .write_comment (comment);" << endl;
lister.single_line_comments (false)
	  .write_comment (comment);
expected << "/* Comment." << endl
		 << "0123456789 123456789 123456789" << endl
		 << "*/" << endl;
if (! checker.check ("Comments, two lines, single line off.",
	expected, listing))
	exit (1);

//	Multiple single line wrapped.
if (checker.Verbose)
	cout << "--- lister.single_line_comments (true)" << endl
		 << "          .page_width (19)" << endl
		 << "          .write_comment (comment);" << endl;
lister.single_line_comments (true)
	  .page_width (19)
	  .write_comment (comment);
expected << "/* Comment. */" << endl
		 << "/* 0123456789 */" << endl
		 << "/* 123456789 */" << endl
		 << "/* 123456789 */" << endl;
if (! checker.check ("Comments, two lines, single line on, page width 19.",
	expected, listing))
	exit (1);

//	Mulitple multi-line wrapped.
if (checker.Verbose)
	cout << "--- lister.single_line_comments (false)" << endl
		 << "          .write_comment (comment);" << endl;
lister.single_line_comments (false)
	  .write_comment (comment);
expected << "/* Comment." << endl
		 << "0123456789" << endl
		 << "123456789" << endl
		 << "123456789" << endl
		 << "*/" << endl;
if (! checker.check ("Comments, two lines, single line off, page width 19.",
	expected, listing))
	exit (1);

if (checker.Verbose)
	cout << "--- lister.page_width (lister.default_page_width ());" << endl;
lister.page_width (lister.default_page_width ());

//	Attached to Assignment, single line.
if (checker.Verbose)
	cout << "--- assignment.comment (\"Comment.\");" << endl;
assignment.comment ("Comment.");
if (checker.Verbose)
	cout << "--- lister.single_line_comments (true) << assignment;" << endl;
lister.single_line_comments (true) << assignment;
expected << "/* Comment. */" << endl
		 << "Real = 123.4500" << endl;
if (! checker.check ("Assignment with comments, single line on.",
	expected, listing))
	exit (1);

//	Attached to Assignment, multi-line.
if (checker.Verbose)
	cout << "--- lister.single_line_comments (false) << assignment;" << endl;
lister.single_line_comments (false) << assignment;
expected << "/* Comment." << endl
		 << "*/" << endl
		 << "Real = 123.4500" << endl;
if (! checker.check ("Assignment with comments, single line off.",
	expected, listing))
	exit (1);

//	With blank lines, indent 3, multi-line.
if (checker.Verbose)
	cout << "--- assignment.comment (\"\\nComment.    \\n\\nFourth line.\\n\");" << endl;
assignment.comment ("\nComment.    \n\nFourth line.\n");
if (checker.Verbose)
	cout << "--- lister.write (assignment, 3);" << endl;
lister.write (assignment, 3);
expected << endl
		 << "   /* Comment." << endl
		 << endl
		 << "   Fourth line." << endl
		 << "   */" << endl
		 << endl
		 << "   Real = 123.4500" << endl;
if (! checker.check ("Comments, single line off, indent 3, with blanks pre/mid/post.",
	expected, listing))
	exit (1);

//	With blank lines, indent 3, single line.
if (checker.Verbose)
	cout << "--- lister.single_line_comments (true)" << endl
		 << "          .write (assignment, 3);" << endl;
lister.single_line_comments (true)
	  .write (assignment, 3);
expected << endl
		 << "   /* Comment.     */" << endl
		 << endl
		 << "   /* Fourth line. */" << endl
		 << endl
		 << "   Real = 123.4500" << endl;
if (! checker.check ("Comments, single line on, indent 3, with blanks pre/mid/post.",
	expected, listing))
	exit (1);

//	With blank lines, indent 3, multi-line.
if (checker.Verbose)
	cout << "--- assignment.comment (\"\\n\\nComment.    \\n\\n\\nFourth line.\\n\\n\\n\");" << endl;
assignment.comment ("\n\nComment.    \n\n\nFourth line.\n\n\n");
if (checker.Verbose)
	cout << "--- lister.single_line_comments (false)" << endl
		 << "          .write (assignment, 3);" << endl;
lister.single_line_comments (false)
	  .write (assignment, 3);
expected << endl
		 << endl
		 << "   /* Comment." << endl
		 << endl
		 << endl
		 << "   Fourth line." << endl
		 << "   */" << endl
		 << endl
		 << endl
		 << endl
		 << "   Real = 123.4500" << endl;
if (! checker.check ("Comments, single line off, indent 3, with multiple blanks pre/mid/post.",
	expected, listing))
	exit (1);

//	String continuation elide.
text =
"This is a very long line which is expected to be wrapped by the lister when it is output as PVL.";
expected << '"'
		 << "This is a very long line which is expected to be wrapped by the lister when it" << endl
		 << "is output as PVL."
		 << '"';
lister << text;
checker.check ("String wrap Value.",
	expected, listing);

assignment.name ("String_Wrap").comment ("") = text;
expected << "String_Wrap = \""
		 << "This is a very long line which is expected to be wrapped by the" << endl
		 << "    lister when it is output as PVL.\"" << endl;
lister << assignment;
checker.check ("String wrap Parameter.",
	expected, listing);

text =
"This is a long line which is expected to be wrapped - with the hyphen retained.";
expected << "String_Continuation = \""
		 << "This is a long line which is expected to be wrapped - " << endl
		 << "    with the hyphen retained.\"" << endl;
assignment.name ("String_Continuation") = text;
lister << assignment;
checker.check ("String continuation elided.",
	expected, listing);

text =
"This is a rather long line with a word that is hyphen-\nated at the line wrap point.";
expected << "String_Continuation = \""
		 << "This is a rather long line with a word that is hyphen-" << endl
		 << "ated at the line wrap point.\"" << endl;
assignment.name ("String_Continuation") = text;
lister << assignment;
checker.check ("String continuation retained.",
	expected, listing);


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
