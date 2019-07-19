/*	Parser_test

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

CVS ID: Parser_test.cc,v 1.15 2013/02/24 22:27:12 castalia Exp
*******************************************************************************/

#include	"PVL.hh"
#include	"Utility/Checker.hh"
using namespace idaeim;
using namespace PVL;

#include	<string>
#include	<iostream>
#include	<fstream>
#include	<sstream>
#include	<cstdlib>
using namespace std;


int
main
	(
	int		count,
	char	**arguments
	)
{
cout << "*** Parser class test" << endl
	 << Parser::ID << endl << endl;

Checker
	checker;
checker.Reset_Values = false;

if (count > 1)
	{
	if (*arguments[1] == '-')
		 arguments[1]++;
	if (*arguments[1] == 'v' ||
		*arguments[1] == 'V')
		checker.Verbose = true;
	}

Parser
	*parser = NULL;
Parameter
	*aggregate = NULL;
ostringstream
	parameters,
	*expected[3],
	description;

try {
parameters
	<< "Group = First_Group"							<< endl
	<< "	Integer_Number = 3 <integer>"				<< endl
	<< "	Floating_Point = 1.23 <real>"				<< endl
	<< "	Identifier = IDENTIFIER <identifier>"		<< endl
	<< "	Text = \"A string of text\" <text>"			<< endl
	<< "	Wrapped_String = \"First line."				<< endl
	<< "		Second line.\""							<< endl
	<< "	Symbol = 'A Symbol' <symbol>"				<< endl
	<< "	Object = Second_Group"						<< endl
	<< "		/* Quoted Parameter name */"			<< endl
	<< "		\"Text\" = \"Another string of text\""	<< endl
	<< "		/* Inappropriate Group name */"			<< endl
	<< "		Begin_Group = 3"						<< endl
	<< "			Date = Aug-10-2002"					<< endl
	<< "		/* End type mismatch */"				<< endl
	<< "		End_Object"								<< endl
	<< "	End_Object"									<< endl
	<< "End_Group"										<< endl;
if (checker.Verbose)
	cout << "--- parameters -" << endl
		 << parameters.str ();
cout << "--- ofstream output (\"Parser_test.parameters\");" << endl;
ofstream
	output ("Parser_test.parameters");
if (! output)
	{
	cerr << "Unable to open the output file: Parser_test.parameters" << endl;
	exit (1);
	}
cout << "--- output << parameters.str ();" << endl;
output << parameters.str ();
output.close ();
cout << "--- ifstream input_file (\"Parser_test.parameters\");" << endl;
ifstream
	input_file ("Parser_test.parameters");
if (! input_file)
	{
	cerr << "Unable to open the input file: Parser_test.parameters" << endl;
	exit (1);
	}
cout << "--- Parser *parser = new Parser (input_file);" << endl;
parser = new Parser (input_file);

cout << "--- Parameter *parameter = parser->get_parameters ();" << endl;
aggregate = parser->get_parameters ();

expected[0] = new ostringstream;
*expected[0]
	<< Parser::ID << endl
	<< "Invalid PVL Syntax at Location 284" << endl
	<< " at character 2 of line 11" << endl
	<< "Quoted Parameter name - \"Text\"";
expected[1] = new ostringstream;
*expected[1]
	<< Parser::ID << endl
	<< "Invalid PVL Syntax at Location 366" << endl
	<< "Invalid PVL Aggregate Value" << endl
	<< " at character 15 of line 13" << endl
	<< "Group Parameter \"Begin_Group\" = Integer Value.";
expected[2] = new ostringstream;
*expected[2]
	<< Parser::ID << endl
	<< "Invalid PVL Syntax at Location 431" << endl
	<< "PVL Aggregate Closure Mismatch" << endl
	<< " before character 1 of line 17" << endl
	<< "Group Parameter 3" << endl
	<< "  ends with an End_Object Parameter.";
Parser::Warnings_List
	*warnings_list = parser->warnings ();
int
	expected_index,
	warning_index = -1,
	total = warnings_list->size ();
checker.check ("total warnings", 3, total);
while (++warning_index < total)
	{
	description.str ("");
	description << "warning " << warning_index;
	expected_index = -1;
	while (++expected_index < 3)
		{
		if (expected[expected_index] &&
			expected[expected_index]->str () ==
			   (*warnings_list)[warning_index]->message ())
			{
			checker.check (description.str (),
				expected[expected_index]->str (),
				(*warnings_list)[warning_index]->message ());
			delete expected[expected_index];
			expected[expected_index] = NULL;
			break;
			}
		}
	if (expected_index == 3)
		checker.check (description.str (),
			"Unexpected warning",
			(*warnings_list)[warning_index]->message ());
	}
expected_index = -1;
while (++expected_index < 3)
	{
	description.str ("");
	description << "expected warning " << expected_index;
	if (expected[expected_index])
		checker.check (description.str (),
			expected[expected_index]->str (), "No warning");
	}

if (checker.Verbose)
	cout << "--- parameters << *aggregate;" << endl;
parameters.str ("");
parameters << *aggregate;
expected[0] = new ostringstream;
*expected[0]
	<< "Group = First_Group" << endl
	<< "    Integer_Number = 3 <integer>" << endl
	<< "    Floating_Point = 1.23 <real>" << endl
	<< "    Identifier = IDENTIFIER <identifier>" << endl
	<< "    Text = \"A string of text\" <text>" << endl
	<< "    Wrapped_String = \"First line. Second line.\"" << endl
	<< "    Symbol = 'A Symbol' <symbol>" << endl
	<< "    Object = Second_Group" << endl
	<< "        /* Quoted Parameter name" << endl
	<< "        */" << endl
	<< "        Text = \"Another string of text\"" << endl
	<< "        /* Inappropriate Group name" << endl
 	<< "        */" << endl
	<< "        Group = 3" << endl
	<< "            Date = Aug-10-2002" << endl
	<< "        End_Group" << endl
	<< "    End_Object" << endl
	<< "End_Group" << endl
	<< "END" << endl;
checker.check ("parsed parameters",
	*expected[0], parameters);
}
catch (const Exception& except)
	{
	cout << except.message () << endl;
	checker.check (false);
	}
catch (out_of_range except)
	{
	cout << "out_of_range - " << endl << except.what () << endl;
	checker.check (false);
	}

cout << endl
	 << "Checks: " << checker.Checks_Total << endl
	 << "Passed: " << checker.Checks_Passed << endl;

exit ((checker.Checks_Total == checker.Checks_Passed) ? 0 : 1);
}
