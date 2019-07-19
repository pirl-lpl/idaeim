/*	Parameter_test

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

CVS ID: Parameter_test.cc,v 1.22 2013/09/17 04:01:22 castalia Exp
*******************************************************************************/

#include	"Parameter.hh"
#include	"Value.hh"
#include	"Parser.hh"
#include	"Utility/Checker.hh"
using namespace idaeim;
using namespace PVL;
typedef idaeim::PVL::Aggregate::Selection Selection;

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
cout << "*** Parameter class test" << endl
	 << Parameter::ID << endl << endl;

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

bool
	result;
int
	number;
ostringstream
	expected,
	listing,
	parameters;

try {

//	Assignment:

//	Construct.
if (checker.Verbose)
	cout << "--- Assignment assignment (\"The_Parameter\");" << endl;
Assignment
	assignment ("The_Parameter");

//	Name.
if (checker.Verbose)
	cout << "--- assignment.name () == \"The_Parameter\";" << endl;
checker.check ("Assignment name",
	"The_Parameter", assignment.name ());

//	Assign Real.
if (checker.Verbose)
	cout << "--- assignment = Real (-32.1);" << endl;
assignment = Real (-32.1);
checker.check ("Assignment = Real; Assignment value",
	-32.1, (double)assignment.value ());

//	Write. 
if (checker.Verbose)
	cout << "--- assignment.write (ostringstream);" << endl;
number = assignment.write (listing);
checker.check ("Assignment write amount",
	22, number);
expected << "The_Parameter = -32.1" << endl;
checker.check ("Assignment write listing",
	expected, listing);

//	Copy.
if (checker.Verbose)
	cout << "--- Assignment assignment_copy (assignment);" << endl;
Assignment
	assignment_copy (assignment);
checker.check ("Assignment copy name",
	"The_Parameter", assignment_copy.name ());
checker.check ("Assignment copy value",
	-32.1, (double)assignment_copy.value ());

//	Equality.
if (checker.Verbose)
	cout << "--- assignment == assignment_copy;" << endl;
result = (assignment == assignment_copy);
checker.check ("Assignment == Assignment copy",
	true, result);

//	Aggregate:

//	Construct.
if (checker.Verbose)
	cout << "--- root_aggregate = new Aggregate (\"Aggregate_Name\");"
			<< endl;
Aggregate
	aggregate ("Aggregate_Name");

//	Name.
if (checker.Verbose)
	cout << "--- aggregate.name ();" << endl;
if (! checker.check ("Aggregate name",
	"Aggregate_Name", aggregate.name ()))
	goto Done;
if (checker.Verbose)
	cout << "--- aggregate.name (Parser::CONTAINER_NAME);" << endl;
aggregate.name (Parser::CONTAINER_NAME);
if (! checker.check ("Aggregate name set",
	Parser::CONTAINER_NAME, aggregate.name ()))
	goto Done;

if (checker.Verbose)
	cout << "--- aggregate.has_children ();" << endl;
checker.check ("Aggregate has_children",
	false, aggregate.has_children ());

//	Subtype.
if (checker.Verbose)
	cout << "--- aggregate.type (Parameter::GROUP);" << endl;
aggregate.type (Parameter::GROUP);
checker.check ("Aggregate type set and get",
	Parameter::GROUP, aggregate.type ());

//	Write.
if (checker.Verbose)
	cout << "--- aggregate.write (ostringstream);" << endl;
number = aggregate.write (listing);
checker.check ("Aggregate write amount",
	4, number);
expected << "END" << endl;
checker.check ("Aggregate write listing",
	expected, listing);

//	Add Assignment.
if (checker.Verbose)
	cout << "--- aggregate.add (assignment);" << endl;
aggregate.add (assignment);
if (! checker.check ("Aggregate add; Aggregate size",
	1, (int)aggregate.size ()))
	goto Done;

//	Array index.
if (checker.Verbose)
	cout << "--- aggregate[0].name ();" << endl;
checker.check ("Aggregate[0] name",
	"The_Parameter", aggregate[0].name ());

//	Parent.
if (checker.Verbose)
	cout << "--- &aggregate == aggregate[0].parent ();" << endl;
checker.check ("Aggregate[0] parent",
	(void*)&aggregate, (void*)(aggregate[0].parent ()));

if (checker.Verbose)
	cout << "--- aggregate.has_children ();" << endl;
checker.check ("Aggregate has_children",
	true, aggregate.has_children ());

//	Copy.
if (checker.Verbose)
	cout << "--- Aggregate aggregate_copy (aggregate);" << endl;
Aggregate
	aggregate_copy (aggregate);
checker.check ("Aggregate copy name",
	Parser::CONTAINER_NAME, aggregate_copy.name ());
checker.check ("Aggregate copy size",
	1, (int)aggregate_copy.size ());

//	Operator<<.
if (checker.Verbose)
	cout << "--- ostringstream << aggregate_copy;" << endl;
listing << aggregate_copy;
expected
	<< "The_Parameter = -32.1" << endl
	<< "END" << endl;
checker.check ("Aggregate copy value from operator<<",
	expected, listing);

//	Equality.
if (checker.Verbose)
	cout << "--- aggregate == aggregate_copy" << endl;
result = (aggregate == aggregate_copy);
checker.check ("Aggregate == Aggregate copy",
	true, result);

//	Parent of copy.
if (checker.Verbose)
	cout << "--- &aggregate_copy == aggregate_copy[0].parent ();" << endl;
checker.check ("Aggregate copy[0] parent",
	(void*)&aggregate_copy, (void*)(aggregate_copy[0].parent ()));

//	Construct Aggregate from Parser on string.
parameters
	<< "Group = First_Group"						<< endl
	<< "    Integer_Number = 3 <integer>"			<< endl
	<< "    Group = empty"							<< endl
	<< "    End_Group"								<< endl
	<< "    Floating_Point = 1.23 <real>"			<< endl
	<< "    Identifier = IDENTIFIER <identifier>"	<< endl
	<< "    Text = \"A string of text\" <text>"		<< endl
	<< "    Symbol = 'A Symbol' <symbol>"			<< endl
	<< "    Object = Second_Group"					<< endl
	<< "        Date = Jan-1-2001:00:00"			<< endl
	<< "        Object = Third_Group"				<< endl
	<< "            Date = Aug-10-2002"				<< endl
	<< "        End_Object"							<< endl
	<< "    End_Object"								<< endl
	<< "End_Group"									<< endl;
if (checker.Verbose)
	cout << "--- parameters string -" << endl
		 << parameters.str ()
		 << "--- Parser parser (parameters.str ());" << endl;
Parser
	parser (parameters.str ());
if (checker.Verbose)
	cout << "--- Aggregate *container = "
			"new Aggregate (parser, Parser::CONTAINER_NAME);" << endl;
Aggregate
	*container = new Aggregate (parser, Parser::CONTAINER_NAME);

//	Operator<<.
if (checker.Verbose)
	cout << "--- listing << *container;" << endl;
listing << *container;
expected
	<< parameters.str ()
	<< "END" << endl;
if (! checker.check ("Aggregate from Parser on string; value from operator<<",
		expected, listing))
	goto Done;

//	Total size.
if (checker.Verbose)
	cout << "--- container->total_size ();" << endl;
if (! checker.check ("Aggregate total_size",
	11, (int)container->total_size ()))
	goto Done;

//	Copy array indexed child.
if (checker.Verbose)
	cout << "--- aggregate = (*container)[0];" << endl;
aggregate = (*container)[0];
listing << aggregate;
expected << parameters.str ();
checker.check ("Aggregate assigned from indexed child aggregate",
	expected, listing);

//	Equality.
if (checker.Verbose)
	cout << "--- aggregate == (*container)[0];" << endl;
checker.check ("Aggregate child copy == indexed child aggregate",
	(*container)[0], static_cast<Parameter&>(aggregate));

//	Delete.
delete container;

//	Iterator:

//	Algorithm find_if using Iterator with at_pathname member function.
if (checker.Verbose)
	cout << "--- Aggregate::iterator element = "
		 << "find_if (aggregate.begin (), aggregate.end ()," << endl
		 << "        bind2nd (mem_fun_ref (&Parameter::at_pathname), \"Symbol\"));"
			<< endl;
Aggregate::iterator
	element = find_if (aggregate.begin (), aggregate.end (),
		bind2nd (mem_fun_ref (&Parameter::at_pathname), "Symbol"));
if (checker.check ("Aggregate iterator find_if for existing parameter",
		true, (element != aggregate.end ())))
	checker.check ("Aggregate iterator result of find_if for existing parameter",
		"Symbol", element->name ());

//	Depth_Iterator:

//	Post-increment.
if (checker.Verbose)
	cout << "--- Aggregate::depth_iterator" << endl
		 << "        position = aggregate.begin_depth ()," << endl
		 << "        aggregate_end = aggregate.end_depth ();" << endl
		 << "    while (position != aggregate_end)" << endl
		 << "        cout << (position++)->pathname () << endl;" << endl;
Aggregate::depth_iterator
	position = aggregate.begin_depth (),
	aggregate_end = aggregate.end_depth ();
while (position != aggregate_end)
	//	Dereference (operator->).
	listing << (position++)->pathname () << endl;
expected
	<< "/First_Group/Integer_Number" << endl
	<< "/First_Group/empty" << endl
	<< "/First_Group/Floating_Point" << endl
	<< "/First_Group/Identifier" << endl
	<< "/First_Group/Text" << endl
	<< "/First_Group/Symbol" << endl
	<< "/First_Group/Second_Group" << endl
	<< "/First_Group/Second_Group/Date" << endl
	<< "/First_Group/Second_Group/Third_Group" << endl
	<< "/First_Group/Second_Group/Third_Group/Date" << endl;
checker.check ("Aggregate depth_iterator post-increment (++) and dereference (->)",
	expected, listing);

//	find_if using Depth_Iterator for non-existant parameter.
if (checker.Verbose)
cout << "--- position = find_if (aggregate.begin_depth (), aggregate_end,"
		<< endl
	 << "        bind2nd (mem_fun_ref (&Parameter::at_pathname), \"NONE\"));"
		<< endl;
position = find_if (aggregate.begin_depth (), aggregate_end,
	bind2nd (mem_fun_ref (&Parameter::at_pathname), "NONE"));
if (! checker.check ("Aggregate depth_iterator find_if for non-existant parameter",
		false, (position != aggregate_end)))
	cout << "    found: " << position->pathname ();

//	find_if to existing parameter name.
if (checker.Verbose)
	cout << "--- position = find_if (aggregate.begin_depth (), aggregate_end,"
			<< endl
		 << "        bind2nd (mem_fun_ref (&Parameter::at_pathname), \"Second_Group\"));"
		 	<< endl;
position = find_if (aggregate.begin_depth (), aggregate_end,
	bind2nd (mem_fun_ref (&Parameter::at_pathname), "Second_Group"));
if (checker.check ("Aggregate depth_iterator find_if for existing parameter",
		true, (position != aggregate_end)))
	checker.check ("Aggregate depth_iterator result of find_if for existing parameter",
		"Second_Group", position->name ());

//	find case insensitive any type.
if (checker.Verbose)
	cout << "--- Parameter *parameter = aggregate.find (\"date\")" << endl;
Parameter
	*parameter = aggregate.find ("date");
if (checker.check ("Aggregate find case insensitive any type",
		true, (parameter != NULL)))
	checker.check ("Aggregate find case insensitive any type result",
		"/First_Group/Second_Group/Date", parameter->pathname ());

//	find case sensitive, skip 1, ASSIGNMENT type.
if (checker.Verbose)
	cout << "--- parameter = "
		 << "aggregate.find (\"Date\", true, 1, Parameter::ASSIGNMENT)"
		 << endl;
parameter = aggregate.find ("Date", true, 1, Parameter::ASSIGNMENT);
if (checker.check ("Aggregate find case sensitive, skip 1, ASSIGNMENT type",
		true, (parameter != NULL)))
	checker.check ("Aggregate find case sensitive, skip 1, ASSIGNMENT type result",
		"/First_Group/Second_Group/Third_Group/Date", parameter->pathname ());

//	find case sensitive, skip 0, AGGREGATE type, non-existant.
if (checker.Verbose)
	cout << "--- parameter = aggregate.find (\"Date\", true, 0, Parameter::AGGREGATE)" << endl;
parameter = aggregate.find ("Date", true, 0, Parameter::AGGREGATE);
if (! checker.check ("Aggregate find case sensitive, skip 0, AGGREGATE type for non-existant parameter",
		false, (parameter != NULL)))
	cout << "    found: " << parameter->pathname () << endl;

//	find case sensitive, skip 3, ASSIGNMENT type, non-existant.
if (checker.Verbose)
	cout << "--- Parameter *parameter = aggregate.find (\"Date\", true, 3, Parameter::ASSIGNMENT)" << endl;
parameter = aggregate.find ("Date", true, 3, Parameter::ASSIGNMENT);
if (! checker.check ("Aggregate find case sensitive, skip 3, ASSIGNMENT type for non-existant parameter",
		false, (parameter != NULL)))
	cout << "    found: " << parameter->pathname () << endl;

//	find case sensitive, skip 0, ASSIGNMENT type, non-matching case.
if (checker.Verbose)
	cout << "--- Parameter *parameter = aggregate.find (\"date\", true, 0, Parameter::ASSIGNMENT)" << endl;
parameter = aggregate.find ("date", true, 0, Parameter::ASSIGNMENT);
if (! checker.check ("Aggregate find case sensitive, skip 0, ASSIGNMENT type for non-matching case parameter",
		false, (parameter != NULL)))
	cout << "    found: " << parameter->pathname () << endl;

//	Insert assignment at iterator begin.
if (checker.Verbose)
	cout << "--- aggregate.insert (aggregate.begin (), assignment);" << endl;
number = aggregate.size ();
aggregate.insert (aggregate.begin (), assignment);
checker.check ("Aggregate insert Assignment at iterator begin; Aggregate size",
	(number + 1), (int)aggregate.size ());
checker.check ("Aggregate insert Assignment at iterator begin; inserted parameter name",
	assignment.name (), aggregate[0].name ());

//	Remove (pull_out) at depth iterator begin_depth.
if (checker.Verbose)
	cout << "--- parameter = aggregate.remove (aggregate.begin_depth ());"
			<< endl;
parameter = aggregate.remove (aggregate.begin_depth ());
checker.check ("Aggregate remove at depth iterator begin_depth",
	assignment.name (), parameter->name ());

//	Insert parameter at element index.
if (checker.Verbose)
	cout << "--- aggregate.insert (3, *parameter);" << endl;
aggregate.insert (3, *parameter);
if (! checker.check ("Aggregate insert at element index",
	parameter->name (), aggregate[3].name ()))
	goto Done;

//	Remove parameter at element index.
if (checker.Verbose)
	cout << "--- Parameter *removed = aggregate.remove (3);" << endl;
Parameter
	*removed = aggregate.remove (3);
if (! checker.check ("Aggregate removed parameter at element index",
	true, (bool)removed))
	goto Done;
if (! checker.check ("Aggregate removed parameter at element index, value",
	*parameter, *removed))
	goto Done;

//	poke_back.
if (checker.Verbose)
	cout << "--- aggregate.poke_back (parameter);" << endl;
number = aggregate.size ();
aggregate.poke_back (parameter);
checker.check ("Aggregate poke_back; Aggregate size",
	(number + 1), (int)aggregate.size ());
checker.check ("Aggregate poke_back; inserted parameter name",
	parameter->name (), aggregate[number].name ());

//	Remove parameter.
if (checker.Verbose)
	cout << "--- Parameter *removed = aggregate.remove (parameter);" << endl;
removed = aggregate.remove (parameter);
checker.check ("Aggregate remove parameter, address",
	(void*)parameter, (void*)removed);
if (! checker.check ("Aggregate remove parameter, value",
	*parameter, *removed))
	goto Done;

//	parent check.
if (checker.Verbose)
	cout << "--- removed->parent () == NULL;" << endl;
if (! checker.check ("Aggregate removed parameter has no parent",
		(void*)NULL, (void*)(removed->parent ())))
	cout << "    found: " << removed->parent ()->pathname () << endl;

//	Depth_Iterator operator+ and operator+=; erase at Depth_Iterator.
if (checker.Verbose)
	cout << "--- position = aggregate.begin ();" << endl
		 << "    Aggregate::iterator" << endl
		 << "        location = position + 2;" << endl
		 << "    location = aggregate.erase (location);" << endl
		 << "    position += 2;" << endl;
position = aggregate.begin_depth ();
Aggregate::depth_iterator
	location = position + 2;
location = aggregate.erase (location);
position += 2;
checker.check ("Aggregate erase at depth_iterator with constant offset",
	"/First_Group/Identifier", position->pathname ());

//	Depth_Iterator operator==.
if (checker.Verbose)
	cout << "--- position == location;" << endl;
checker.check ("Aggregate erased depth_iterator == offset depth_iterator",
	true, (position == location));

//	Replace.
if (checker.Verbose)
	cout << "--- parameter = aggregate.replace (position, parameter);" << endl;
parameter = aggregate.replace (position, parameter);
checker.check ("Aggregate replace at depth_iterator, returned parameter",
	"Identifier", parameter->name ());
checker.check ("Aggregate replace at depth_iterator, resulting depth_iterator",
	"/First_Group/The_Parameter", position->pathname ());

//	Parent check.
if (checker.Verbose)
	cout << "--- parameter->parent () == NULL;" << endl;
if (! checker.check ("Aggregate replace at depth_iterator, returned parameter parent",
		(void*)NULL, (void*)(parameter->parent ())))
	cout << "    found: " << parameter->parent ()->pathname () << endl;
if (checker.Verbose)
	cout << "--- position->parent () == &aggregate;" << endl;
if (! checker.check ("Aggregate replace at depth_iterator, resulting depth_iterator parent",
		(void*)&aggregate, (void*)(position->parent ())) &&
	position->parent ())
	cout << "    found: " << position->parent ()->pathname () << endl;

//	Sort.
if (checker.Verbose)
	cout << "--- sort (aggregate.begin (), aggregate.end ());" << endl;
sort (aggregate.begin (), aggregate.end ());
listing << aggregate;
expected
	<< "Group = First_Group" << endl
	<< "    Integer_Number = 3 <integer>" << endl
	<< "    Object = Second_Group" << endl
	<< "        Date = Jan-1-2001:00:00" << endl
	<< "        Object = Third_Group" << endl
	<< "            Date = Aug-10-2002" << endl
	<< "        End_Object" << endl
	<< "    End_Object" << endl
	<< "    Symbol = 'A Symbol' <symbol>" << endl
	<< "    Text = \"A string of text\" <text>" << endl
	<< "    The_Parameter = -32.1" << endl
	<< "    Group = empty" << endl
	<< "    End_Group" << endl
	<< "End_Group" << endl;
checker.check ("Aggregate sort",
	expected, listing);

//	Find and remove_depth parameter.
if (checker.Verbose)
	cout << "--- aggregate.find (\"Third_Group\");" << endl;
parameter = aggregate.find ("Third_Group");
if (! checker.check ("Aggregate find nested parameter group",
	true, (bool)parameter))
	goto Done;

if (checker.Verbose)
	cout << "--- aggregate.remove_depth (parameter);" << endl;
removed = aggregate.remove_depth (parameter);
if (! checker.check ("Aggregate remove_depth",
	true, (bool)removed))
	goto Done;
if (! checker.check ("Aggregate remove_depth parameter",
	*parameter, *removed))
	goto Done;

listing << aggregate;
expected
	<< "Group = First_Group" << endl
	<< "    Integer_Number = 3 <integer>" << endl
	<< "    Object = Second_Group" << endl
	<< "        Date = Jan-1-2001:00:00" << endl
	<< "    End_Object" << endl
	<< "    Symbol = 'A Symbol' <symbol>" << endl
	<< "    Text = \"A string of text\" <text>" << endl
	<< "    The_Parameter = -32.1" << endl
	<< "    Group = empty" << endl
	<< "    End_Group" << endl
	<< "End_Group" << endl;
checker.check ("Aggregate after remove_depth",
	expected, listing);


//	Selection
parameters.str ("");
parameters
	<< "Integer = 3" << endl
	<< "Real = 1.23" << endl
	<< "String = \"A string\"" << endl
	<< "Integer_Array = {1, 2, 3}" << endl
	<< "Real_Array = {1.0, 2.1, 3.2}" << endl
	<< "String_Array = {first, second, third}" << endl;
if (checker.Verbose)
	cout << "--- Aggregate *container = "
			"new Parserparameters.str ()).get_parameters ();" << endl;
container =
	dynamic_cast<Aggregate*>(Parser (parameters.str ()).get_parameters ());
if (checker.Verbose)
	cout << "--- parameters -" << endl
	<< *container;

Value::Integer_type
	integer = -10,
	integers[4] = {-1, -2, -3, -4};
Value::Real_type
	real = -10.101,
	reals[4] = {-1.0, -2.1, -3.2, -4.3};
Value::String_type
	a_string ("not set"),
	strings[4] = {"aaa", "bbb","ccc","ddd"};
Selection
	selections[] =
	{
	Selection ("Integer", integer),
	Selection ("Real", real),
	Selection ("String", a_string),
	Selection ("Integer_Array", *integers, 3),
	Selection ("Real_Array", *reals, 3),
	Selection ("String_Array", *strings, 3),
	Selection ()
	};
if (checker.Verbose)
	for (int
			index = 0;
			selections[index].Type;
			index++)
		cout << index << ": " << selections[index];
if (checker.Verbose)
	cout << "--- container->select (selections)" << endl;
container->select (selections);
checker.check ("Aggregate select, integer",
	(Value::Integer_type)3, integer);
checker.check ("Aggregate select, real",
	(Value::Real_type)1.23, real);
checker.check ("Aggregate select, string",
	Value::String_type ("A string"), a_string);
for (int
		index = 0;
		index < 4;
		index++)
	{
	listing.str ("");
	listing << index;
	checker.check ("Aggregate select, integer array entry " + listing.str (),
		((index == 3) ? -1 : 1)
			* ((Value::Integer_type)index + 1),
		integers[index]);
	}
for (int
		index = 0;
		index < 4;
		index++)
	{
	listing.str ("");
	listing << index;
	checker.check ("Aggregate select, real array entry " + listing.str (),
		((index == 3) ? -1 : 1)
			* ((Value::Real_type)(index + 1) + index / 10.0),
		reals[index]);
	}
for (int
		index = 0;
		index < 4;
		index++)
	{
	if (index == 0)
		checker.check ("Aggregate select, string array entry 0",
			Value::String_type ("first"), strings[index]);
	if (index == 1)
		checker.check ("Aggregate select, string array entry 1",
			Value::String_type ("second"), strings[index]);
	if (index == 2)
		checker.check ("Aggregate select, string array entry 2",
			Value::String_type ("third"), strings[index]);
	if (index == 3)
		checker.check ("Aggregate select, string array unset entry 3",
			Value::String_type ("ddd"), strings[index]);
	}
}
catch (Exception except)
	{
	cout << except.message () << endl;
	checker.check (false);
	}

Done:
cout << endl
	 << "Checks: " << checker.Checks_Total << endl
	 << "Passed: " << checker.Checks_Passed << endl;

exit ((checker.Checks_Total == checker.Checks_Passed) ? 0 : 1);
}
