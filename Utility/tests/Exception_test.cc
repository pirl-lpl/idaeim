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

CVS ID: Exception_test.cc,v 1.2 2014/01/26 00:52:16 castalia Exp
*******************************************************************************/

#include	"Exception.hh"

#include	"Checker.hh"
using namespace idaeim;

#include	<stdexcept>
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
cout << "idaeim::Exception class test" << endl
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

Exception*
	an_exception;
string
	expected,
	obtained;

if (checker.Verbose)
	cout << "--- an_exception = new Exception ()" << endl;
an_exception = new Exception ();
checker.check ("Exception without message or caller ID",
	expected, an_exception->message ());

if (checker.Verbose)
	cout << "--- an_exception->what ()" << endl;
expected = Exception::ID;
checker.check ("what without message or caller ID",
	expected, an_exception->what ());
delete an_exception;

if (checker.Verbose)
	cout << "--- an_exception = new Exception (\"Exception message\")" << endl;
expected = "Exception message";
an_exception = new Exception ("Exception message");
checker.check ("Exception with only message",
	expected, an_exception->message ());
delete an_exception;

if (checker.Verbose)
	cout << "--- an_exception = new Exception (\"\", \"caller ID\")" << endl;
expected = "caller ID";
an_exception = new Exception ("", "caller ID");
checker.check ("Exception with only caller ID",
	expected, an_exception->message ());
delete an_exception;

if (checker.Verbose)
	cout << "--- an_exception = new Exception (\"Exception message\", \"caller ID\")"
			<< endl;
expected = "caller ID\nException message";
an_exception = new Exception ("Exception message", "caller ID");
checker.check ("Exception with message and caller ID",
	expected, an_exception->message ());

if (checker.Verbose)
	cout << "--- an_exception->what ()" << endl;
expected = Exception::ID;
expected += "\ncaller ID\nException message";
checker.check ("what with message and caller ID",
	expected, an_exception->what ());

Exception*
	an_exception_copy;
if (checker.Verbose)
	cout << "--- an_exception_copy = new Exception (*an_exception)" << endl;
an_exception_copy = new Exception (*an_exception);
checker.check ("Exception copy",
	an_exception->message (), an_exception_copy->message ());

if (checker.Verbose)
	cout << "--- *an_exception_copy = *an_exception" << endl;
*an_exception_copy = *an_exception;
checker.check ("Exception assignment",
	an_exception->message (), an_exception_copy->message ());

if (checker.Verbose)
	cout << "--- an_exception->add_message (\"added message\", \"added caller ID\")"
			<< endl;
expected = "caller ID\nadded caller ID\nadded message\nException message";
an_exception->add_message ("added message", "added caller ID");
checker.check ("add_message with message and caller ID",
	expected, an_exception->message ());

if (checker.Verbose)
	cout << "--- an_exception->add_message (\"\")" << endl;
an_exception->add_message ("");
checker.check ("add_message with no message or caller ID",
	expected, an_exception->message ());

if (checker.Verbose)
	cout << "--- an_exception->add_message (\"\", \"added caller ID\")" << endl;
an_exception->add_message ("", "added caller ID");
checker.check ("add_message with no message and duplicate caller ID",
	expected, an_exception->message ());

if (checker.Verbose)
	cout << "--- an_exception->add_message (\"new message\", \"added caller ID\")" << endl;
expected = "caller ID\nadded caller ID\nnew message\nadded message\nException message";
an_exception->add_message ("new message", "added caller ID");
checker.check ("add_message with new message and duplicate caller ID",
	expected, an_exception->message ());



cout << endl
	 << "Checks: " << checker.Checks_Total << endl
	 << "Passed: " << checker.Checks_Passed << endl;

exit ((checker.Checks_Total == checker.Checks_Passed) ? 0 : 1);

}
