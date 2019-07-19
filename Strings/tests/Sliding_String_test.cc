/*	Sliding_String_test

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

CVS ID:	Sliding_String_test.cc,v 1.13 2013/02/25 00:04:51 castalia Exp
*******************************************************************************/

#include	"Sliding_String.hh"
using namespace idaeim::Strings;

#include	"Utility/Checker.hh"

#include	<iostream>
#include	<fstream>
#include	<iomanip>
#include	<cstdlib>
using namespace std;


int
main
	(
	int		count,
	char	**arguments
	)
{
cout << "Sliding_String class test" << endl
	 << Sliding_String::ID << endl << endl;

idaeim::Checker
	checker;

if (count > 1)
	{
	if (*arguments[1] == '-')
		 arguments[1]++;
	if (*arguments[1] == 'v' ||
		*arguments[1] == 'V')
		checker.Verbose = true;
	}

String
	a_String =
			"  This is the test String.  ";
if (checker.Verbose)
	cout <<	"String a_String = \"" << a_String << '"' << endl
		 << "                             111111111122222222" << endl
		 << "                   0123456789012345678901234567" << endl
		 << "fstream test_file (\"Sliding_String.test\", ios::in | ios::out | ios::trunc)" << endl
		 << "test_file << a_String << endl" << endl;

fstream
	test_file ("Sliding_String.test", ios::in | ios::out | ios::trunc);
if (! test_file)
	{
	cerr << "Unable to create the \"Sliding_String.test\" file." << endl;
	exit (EXIT_FAILURE);
	}
test_file << a_String << endl;

//	Non-text data.
char
	non_text[] = {28, 0, 28, 0, 28, 0, 28, 0};
if (checker.Verbose)
	cout << "char non_text[] = {28, 0, 28, 0, 28, 0, 28, 0}" << endl
		 << "test_file.write (non_text, 8)" << endl
		 << "test_file << \"Beyond non-text\" << endl" << endl
		 << "test_file.seekg (0)" << endl;
test_file.write (non_text, 8);
test_file << "Beyond non-text" << endl;
test_file.seekg (0);

if (checker.Verbose)
	cout << "Sliding_String sliding_string (test_file)" << endl
		 << "sliding_string.size_increment (10)" << endl;
Sliding_String
	sliding_string (test_file);
sliding_string.size_increment (10);

if (checker.Verbose)
	cout << "char character = sliding_string.at (2)" << endl;
char
	character = sliding_string.at (2);
checker.check ("at index character",
	'T', character);

if (checker.Verbose)
	cout << "sliding_string.location_of ('s', 0)" << endl;
checker.check ("location_of character",
	5, (int)sliding_string.location_of ('s', 0));

checker.check ("location_of string",
	14, (int)sliding_string.location_of ("test", 0));

if (checker.Verbose)
	cout << "sliding_string.location_of (\"String\", 0)" << endl;
checker.check ("location_of string, forward",
	19, (int)sliding_string.location_of ("String", 0));

if (checker.Verbose)
	cout << "a_String = sliding_string.substr (19, 6)" << endl;
a_String = sliding_string.substr (19, 6);
checker.check ("substr",
	"String", a_String);

if (checker.Verbose)
	cout << "sliding_string.skip_until (\"X\", 0)" << endl;
checker.check ("skip_until, invalid string",
	String::EOS, (String::Index)(sliding_string.skip_until ("X", 0)));

if (checker.Verbose)
	cout << "sliding_string.read_limit ()" << endl;
checker.check ("read_limit",
	29, (int)sliding_string.read_limit ());

if (checker.Verbose)
	cout << "sliding_string.total_read ()" << endl;
checker.check ("total_read",
	29, (int)sliding_string.total_read ());


cout << endl
	 << "Checks: " << checker.Checks_Total << endl
	 << "Passed: " << checker.Checks_Passed << endl;

exit ((checker.Checks_Total == checker.Checks_Passed) ? 0 : 1);
}
