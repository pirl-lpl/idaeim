/*	Exception_test

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

CVS ID: Exception_test.cc,v 1.10 2013/02/24 22:27:12 castalia Exp
*******************************************************************************/

#include	"PVL_Exceptions.hh"
using namespace idaeim;
using namespace PVL;

#include	"Utility/Checker.hh"

#include	<iostream>
#include	<iomanip>
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
cout << "PVL::Exception class test" << endl
	 << Exception::ID << endl << endl;

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

string
	expected,
	obtained,
	report
		(
		"Exception_test message.\n"
		"  Multi-line report."
		);

if (checker.Verbose)
	cout << "--- report -" << endl
		 << report << '"' << endl
		 << "--- throw new Invalid_Argument (report)" << endl
		 << "    catch (Exception *except) {obtained = except->what ();}"
			<< endl;
expected = Exception::ID;
expected += "\nError: Invalid_Argument\n";
expected += report;
try {throw new Invalid_Argument (report);}
catch (Exception *except) {obtained = except->what ();}
catch (...) {obtained = "Unexpected exception";}
checker.check ("throw new Invalid_Argument, catch Exception*",
	expected, obtained);

if (checker.Verbose)
	cout << endl
		 << "--- throw Invalid_Syntax (report)" << endl
		 << "    catch (Invalid_Syntax except) {obtained = except->message ();}"
			<< endl;
expected = "Invalid PVL Syntax\n";
expected += report;
obtained.clear ();
try {throw Invalid_Syntax (report);}
catch (Invalid_Syntax except) {obtained = except.message ();}
catch (...) {obtained = "Unexpected exception";}
checker.check ("throw Invalid_Syntax, catch Invalid_Syntax",
	expected, obtained);


cout << endl
	 << "Checks: " << checker.Checks_Total << endl
	 << "Passed: " << checker.Checks_Passed << endl;

exit ((checker.Checks_Total == checker.Checks_Passed) ? 0 : 1);

}
