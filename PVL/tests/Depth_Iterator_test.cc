/*	Depth_Iterator_test

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

CVS ID: Depth_Iterator_test.cc,v 1.8 2013/02/24 22:27:12 castalia Exp
*******************************************************************************/

#include	"Parameter.hh"
#include	"Parser.hh"
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
cout << "*** Aggregate::Depth_Iterator class test" << endl
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

ostringstream
	parameters,
	expected,
	listing;

try {
//	Construct an Aggregate from a Parser on a string.
parameters
	<< "    Group = First_Group"						<< endl
	<< "        Integer_Number = 3 <integer>"			<< endl
	<< "        Group = empty"							<< endl
	<< "        End_Group"								<< endl
	<< "        Floating_Point = 1.23 <real>"			<< endl
	<< "        Identifier = IDENTIFIER <identifier>"	<< endl
	<< "        Text = \"A string of text\" <text>"		<< endl
	<< "        Symbol = 'A Symbol' <symbol>"			<< endl
	<< "        Object = Second_Group"					<< endl
	<< "            Date = Jan-1-2001:00:00"			<< endl
	<< "            Object = Third_Group"				<< endl
	<< "                Date = Aug-10-2002"				<< endl
	<< "            End_Object"							<< endl
	<< "        End_Object"								<< endl
	<< "    End_Group"									<< endl;
if (checker.Verbose)
	cout << "--- parameters string -" << endl
		 << parameters.str ()
		 << "--- Parser parser (parameters.str ());" << endl;
Parser
	parser (parameters.str ());
if (checker.Verbose)
	cout << "--- Aggregate aggregate (parser, \"Parameters\");" << endl;
Aggregate
	aggregate (parser, "Parameters");

//	Listing of Aggregate.
if (checker.Verbose)
	cout << "--- listing << aggregate;" << endl;
listing << aggregate;
expected
	<< "Group = Parameters" << endl
	<< parameters.str ()
	<< "End_Group" << endl;
checker.check ("Listing of Aggregate constructed from Parser on string",
	expected, listing);

//	Depth_Iterator instantiation.
if (checker.Verbose)
	cout << "--- Aggregate::depth_iterator" << endl
		 << "        position = aggregate.begin_depth ()," << endl
		 << "        aggregate_end = aggregate.end_depth ();" << endl;
Aggregate::depth_iterator
	position = aggregate.begin_depth (),
	aggregate_end = aggregate.end_depth ();
if (! checker.check ("begin_depth != end_depth",
		true, (position != aggregate_end)))
	goto Done;

//	Dereference.
listing << (*position).pathname ();
expected << "/Parameters/First_Group";
if (! checker.check ("deference (*)",
		expected, listing))
	goto Done;

listing << position->pathname ();
expected << "/Parameters/First_Group";
if (! checker.check ("deference (->)",
		expected, listing))
	goto Done;

//	Pre-increment.
listing << *(++position);
expected << "Integer_Number = 3 <integer>" << endl;
if (! checker.check ("pre-increment (++) and dereference (*)",
		expected, listing))
	goto Done;

//	Pre-decrement.
listing << (--position)->pathname ();
expected << "/Parameters/First_Group";
if (! checker.check ("pre-decrement (--) and dereference (->)",
		expected, listing))
	goto Done;

if (! checker.check ("iterator moved to beginning == begin_depth",
		true, (position == aggregate.begin_depth ())))
	goto Done;

//	Post-increment.
if (checker.Verbose)
	cout << "--- while (position != aggregate_end)" << endl
		 << "        listing << (position++)->pathname () << endl;" << endl;
while (position != aggregate_end)
	//	Dereference (operator->).
	listing << (position++)->pathname () << endl;
expected
	<< "/Parameters/First_Group" << endl
	<< "/Parameters/First_Group/Integer_Number" << endl
	<< "/Parameters/First_Group/empty" << endl
	<< "/Parameters/First_Group/Floating_Point" << endl
	<< "/Parameters/First_Group/Identifier" << endl
	<< "/Parameters/First_Group/Text" << endl
	<< "/Parameters/First_Group/Symbol" << endl
	<< "/Parameters/First_Group/Second_Group" << endl
	<< "/Parameters/First_Group/Second_Group/Date" << endl
	<< "/Parameters/First_Group/Second_Group/Third_Group" << endl
	<< "/Parameters/First_Group/Second_Group/Third_Group/Date" << endl;
checker.check ("post-increment (++) and dereference (->)",
	expected, listing);

//	Post-decrement.
position--;
listing << *position;
expected << "Date = Aug-10-2002" << endl;
checker.check ("post-decrement (--) and dereference (*)",
	expected, listing);

//	find_if using Depth_Iterator for non-existant parameter.
if (checker.Verbose)
	cout << "--- position = find_if (aggregate.begin_depth (), aggregate_end,"
			<< endl
		 << "        bind2nd (mem_fun_ref (&Parameter::at_pathname), \"NONE\"));"
		 	<< endl;
position = find_if (aggregate.begin_depth (), aggregate_end,
	bind2nd (mem_fun_ref (&Parameter::at_pathname), "NONE"));
if (! checker.check ("find_if for non-existant parameter",
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
if (checker.check ("find_if for existing parameter",
		true, (position != aggregate_end)))
	checker.check ("result of find_if for existing parameter",
		"Second_Group", position->name ());

//	find_if from previous position using relative pathname.
if (checker.Verbose)
	cout << "--- position = find_if (position, aggregate_end," << endl
		 << "        bind2nd (mem_fun_ref (&Parameter::at_pathname), \"Third_Group/Date\"));"
		 << endl;
position = find_if (position, aggregate_end,
	bind2nd (mem_fun_ref (&Parameter::at_pathname), "Third_Group/Date"));
if (checker.check ("find_if from previous position using relative pathname",
		true, (position != aggregate_end)))
	checker.check ("result of find_if from previous position using relative pathname",
		true, position->at_pathname ("Third_Group/Date"));

//	find_if using absolute pathname.
expected << "/Parameters/First_Group/Second_Group/Third_Group/Date";
if (checker.Verbose)
	cout << "--- position = find_if (aggregate.begin_depth (), aggregate_end," << endl
		 << "        bind2nd (mem_fun_ref (&Parameter::at_pathname)," << endl
		 << "           \"" << expected << "\"));" << endl;
position = find_if (aggregate.begin_depth (), aggregate_end,
	bind2nd (mem_fun_ref (&Parameter::at_pathname), expected.str ()));
if (checker.check ("find_if using absolute pathname",
		true, (position != aggregate_end)))
	checker.check ("result of find_if using absolute pathname",
		expected.str (), position->pathname ());

} catch (Exception except)
	{
	cout << except.message () << endl;
	exit (1);
	}

Done:
cout << endl
	 << "Checks: " << checker.Checks_Total << endl
	 << "Passed: " << checker.Checks_Passed << endl;

exit ((checker.Checks_Total == checker.Checks_Passed) ? 0 : 1);
}
