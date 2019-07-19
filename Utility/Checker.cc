/*	Checker

Copyright (C) 2004 - 2013  Bradford Castalia, idaeim studio.

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

CVS ID: Checker.cc,v 1.29 2014/02/06 02:22:31 castalia Exp
*******************************************************************************/

#include	"Checker.hh"

#include	<iostream>
using std::cout;
#include	<iomanip>
using std::endl;
using std::boolalpha;
using std::hex;
using std::setfill;
using std::setw;
using std::dec;
#include	<ios>
using std::ios;

namespace idaeim
{
/*==============================================================================
	Constants:
*/
const char* const
	Checker::ID =
		"idaeim::Checker (1.29 2014/02/06 02:22:31)";

/*==============================================================================
	Constructors
*/
Checker::Checker ()
	:	Verbose (false),
		Compare_Equals (true),
		Hex_Notation (false),
		Reset_Values (true),
		Checks_Passed (0),
		Checks_Total (0)
{}

/*==============================================================================
	Checkers
*/
bool
Checker::check
	(
	const std::string&			description,
	const std::ostringstream&	expected,
	const std::ostringstream&	obtained
	)
{
bool
	passed = (obtained.str () == expected.str ());
if (! Compare_Equals)
	passed = ! passed;
cout << (passed ? "PASS" : "FAIL") << ": " << description << endl;
if (Verbose || ! passed)
	{
	cout
		<< "====> expected " << expected.str ().length () << ':' << endl
		<< '"' << expected.str () << '"' << endl;
	if (! Compare_Equals)
	cout
		<< "====> compared !=" << endl;
	cout
		<< "====> obtained " << obtained.str ().length () << ':' << endl
		<< '"' << obtained.str () << '"' << endl;
	}
return check (passed);
}

bool
Checker::check
	(
	const std::string&	description,
	std::ostringstream&	expected,
	std::ostringstream&	obtained
	)
{
bool
	passed = (obtained.str () == expected.str ());
if (! Compare_Equals)
	passed = ! passed;
cout << (passed ? "PASS" : "FAIL") << ": " << description << endl;
if (Verbose || ! passed)
	{
	cout
		<< "====> expected " << expected.str ().length () << ':' << endl
		<< '"' << expected.str () << '"' << endl;
	if (! Compare_Equals)
	cout
		<< "====> compared !=" << endl;
	cout
		<< "====> obtained " << obtained.str ().length () << ':' << endl
	<< '"' << obtained.str () << '"' << endl;
	}
if (Reset_Values)
	{
	expected.str ("");
	obtained.str ("");
	}
return check (passed);
}

bool
Checker::check
	(
	const std::string&	description,
	const std::string&	expected,
	const std::string&	obtained
	)
{
bool
	passed = (obtained == expected);
if (! Compare_Equals)
	passed = ! passed;
cout << (passed ? "PASS" : "FAIL") << ": " << description << endl;
if (Verbose || ! passed)
	{
	cout
		<< "====> expected "
			<< expected.length () << " character string:" << endl
		<< '"' << expected << '"' << endl;
	if (! Compare_Equals)
	cout
		<< "====> compared !=" << endl;
	cout
		<< "====> obtained "
			<< obtained.length () << " character string:" << endl
		<< '"' << obtained << '"' << endl;
	}
return check (passed);
}

bool
Checker::check
	(
	const std::string&	description,
	std::string&		expected,
	std::string&		obtained
	)
{
bool
	passed = (obtained == expected);
if (! Compare_Equals)
	passed = ! passed;
cout << (passed ? "PASS" : "FAIL") << ": " << description << endl;
if (Verbose || ! passed)
	{
	cout
		<< "====> expected "
			<< expected.length () << " character string:" << endl
		<< '"' << expected << '"' << endl;
	if (! Compare_Equals)
	cout
		<< "====> compared !=" << endl;
	cout
		<< "====> obtained "
			<< obtained.length () << " character string:" << endl
		<< '"' << obtained << '"' << endl;
	}
if (Reset_Values)
	{
	expected = "";
	obtained = "";
	}
return check (passed);
}

bool
Checker::check
	(
	const std::string&	description,
	void*				expected,
	void*				obtained
	)
{
bool
	passed = (obtained == expected);
if (! Compare_Equals)
	passed = ! passed;
cout << (passed ? "PASS" : "FAIL") << ": " << description << endl;
if (Verbose || ! passed)
	{
	cout
		<< "====> expected: " << expected << endl;
	if (! Compare_Equals)
	cout
		<< "====> compared !=" << endl;
	cout
		<< "====> obtained: " << obtained << endl;
	}
return check (passed);
}

bool
Checker::check
	(
	const std::string&	description,
	const bool&			expected,
	const bool&			obtained
	)
{
bool
	passed = (obtained == expected);
if (! Compare_Equals)
	passed = ! passed;
cout << (passed ? "PASS" : "FAIL") << ": " << description << endl;
if (Verbose || ! passed)
	{
	ios::fmtflags
		flags = cout.flags ();
	cout << boolalpha
		<< "====> expected: " << expected << endl;
	if (! Compare_Equals)
	cout
		<< "====> compared !=" << endl;
	cout
		<< "====> obtained: " << obtained << endl;
	cout.flags (flags);
	}
return check (passed);
}

bool
Checker::check
	(
	const std::string&	description,
	const char&		expected,
	const char&		obtained
	)
{
bool
	passed = (obtained == expected);
if (! Compare_Equals)
	passed = ! passed;
cout << (passed ? "PASS" : "FAIL") << ": " << description << endl;
if (Verbose || ! passed)
	{
	if (Hex_Notation)
		cout << hex << setfill ('0');
	cout
		<< "====> expected: ";
	if (Hex_Notation)
		cout << "0x" << setw (sizeof (expected) << 1);
	cout << expected << endl;
	if (! Compare_Equals)
	cout
		<< "====> compared !=" << endl;
	cout
		<< "====> obtained: ";
	if (Hex_Notation)
		cout << "0x" << setw (sizeof (obtained) << 1);
	cout << obtained << endl;
	if (Hex_Notation)
		cout << dec << setfill (' ');
	}
return check (passed);
}

bool
Checker::check
	(
	const std::string&	description,
	const unsigned char& expected,
	const unsigned char& obtained
	)
{
bool
	passed = (obtained == expected);
if (! Compare_Equals)
	passed = ! passed;
cout << (passed ? "PASS" : "FAIL") << ": " << description << endl;
if (Verbose || ! passed)
	{
	if (Hex_Notation)
		cout << hex << setfill ('0');
	cout
		<< "====> expected: ";
	if (Hex_Notation)
		cout << "0x" << setw (sizeof (expected) << 1);
	cout << expected << endl;
	if (! Compare_Equals)
	cout
		<< "====> compared !=" << endl;
	cout
		<< "====> obtained: ";
	if (Hex_Notation)
		cout << "0x" << setw (sizeof (obtained) << 1);
	cout << obtained << endl;
	if (Hex_Notation)
		cout << dec << setfill (' ');
	}
return check (passed);
}

bool
Checker::check
	(
	const std::string&	description,
	const unsigned char* expected,
	const unsigned char* obtained,
	unsigned int		 amount
	)
{
bool
	passed = true;
unsigned int
	index = 0;
while (index < amount)
	{
	if (expected[index] != obtained[index])
		{
		passed = false;
		break;
		}
	++index;
	}
if (! Compare_Equals)
	passed = ! passed;
cout << (passed ? "PASS" : "FAIL") << ": " << description << endl;
if (Verbose || ! passed)
	{
	if (Hex_Notation)
		cout << hex << setfill ('0');
	cout
		<< "====> expected: ";
	if (Hex_Notation)
		cout << "0x" << setw (sizeof (char) << 1);
	cout << (unsigned int)expected[index] << endl;
	if (! Compare_Equals)
	cout
		<< "====> compared !=" << endl;
	cout
		<< "====> obtained: ";
	if (Hex_Notation)
		cout << "0x" << setw (sizeof (char) << 1);
	cout << (unsigned int)obtained[index];
	if (Hex_Notation)
		cout << dec << setfill (' ');
	cout << " at array index " << index << endl;
	}
return check (passed);
}

bool
Checker::check
	(
	const std::string&	description,
	const short&		expected,
	const short&		obtained
	)
{
bool
	passed = (obtained == expected);
if (! Compare_Equals)
	passed = ! passed;
cout << (passed ? "PASS" : "FAIL") << ": " << description << endl;
if (Verbose || ! passed)
	{
	if (Hex_Notation)
		cout << hex << setfill ('0');
	cout
		<< "====> expected: ";
	if (Hex_Notation)
		cout << "0x" << setw (sizeof (expected) << 1);
	cout << expected << endl;
	if (! Compare_Equals)
	cout
		<< "====> compared !=" << endl;
	cout
		<< "====> obtained: ";
	if (Hex_Notation)
		cout << "0x" << setw (sizeof (obtained) << 1);
	cout << obtained << endl;
	if (Hex_Notation)
		cout << dec << setfill (' ');
	}
return check (passed);
}

bool
Checker::check
	(
	const std::string&		description,
	const unsigned short&	expected,
	const unsigned short&	obtained
	)
{
bool
	passed = (obtained == expected);
if (! Compare_Equals)
	passed = ! passed;
cout << (passed ? "PASS" : "FAIL") << ": " << description << endl;
if (Verbose || ! passed)
	{
	if (Hex_Notation)
		cout << hex << setfill ('0');
	cout
		<< "====> expected: ";
	if (Hex_Notation)
		cout << "0x" << setw (sizeof (expected) << 1);
	cout << expected << endl;
	if (! Compare_Equals)
	cout
		<< "====> compared !=" << endl;
	cout
		<< "====> obtained: ";
	if (Hex_Notation)
		cout << "0x" << setw (sizeof (obtained) << 1);
	cout << obtained << endl;
	if (Hex_Notation)
		cout << dec << setfill (' ');
	}
return check (passed);
}

bool
Checker::check
	(
	const std::string&	description,
	const int&			expected,
	const int&			obtained
	)
{
bool
	passed = (obtained == expected);
if (! Compare_Equals)
	passed = ! passed;
cout << (passed ? "PASS" : "FAIL") << ": " << description << endl;
if (Verbose || ! passed)
	{
	if (Hex_Notation)
		cout << hex << setfill ('0');
	cout
		<< "====> expected: ";
	if (Hex_Notation)
		cout << "0x" << setw (sizeof (expected) << 1);
	cout << expected << endl;
	if (! Compare_Equals)
	cout
		<< "====> compared !=" << endl;
	cout
		<< "====> obtained: ";
	if (Hex_Notation)
		cout << "0x" << setw (sizeof (obtained) << 1);
	cout << obtained << endl;
	if (Hex_Notation)
		cout << dec << setfill (' ');
	}
return check (passed);
}

bool
Checker::check
	(
	const std::string&	description,
	const unsigned int&	expected,
	const unsigned int&	obtained
	)
{
bool
	passed = (obtained == expected);
if (! Compare_Equals)
	passed = ! passed;
cout << (passed ? "PASS" : "FAIL") << ": " << description << endl;
if (Verbose || ! passed)
	{
	if (Hex_Notation)
		cout << hex << setfill ('0');
	cout
		<< "====> expected: ";
	if (Hex_Notation)
		cout << "0x" << setw (sizeof (expected) << 1);
	cout << expected << endl;
	if (! Compare_Equals)
	cout
		<< "====> compared !=" << endl;
	cout
		<< "====> obtained: ";
	if (Hex_Notation)
		cout << "0x" << setw (sizeof (obtained) << 1);
	cout << obtained << endl;
	if (Hex_Notation)
		cout << dec << setfill (' ');
	}
return check (passed);
}

bool
Checker::check
	(
	const std::string&	description,
	const long&			expected,
	const long&			obtained
	)
{
bool
	passed = (obtained == expected);
if (! Compare_Equals)
	passed = ! passed;
cout << (passed ? "PASS" : "FAIL") << ": " << description << endl;
if (Verbose || ! passed)
	{
	if (Hex_Notation)
		cout << hex << setfill ('0');
	cout
		<< "====> expected: ";
	if (Hex_Notation)
		cout << "0x" << setw (sizeof (expected) << 1);
	cout << expected << endl;
	if (! Compare_Equals)
	cout
		<< "====> compared !=" << endl;
	cout
		<< "====> obtained: ";
	if (Hex_Notation)
		cout << "0x" << setw (sizeof (obtained) << 1);
	cout << obtained << endl;
	if (Hex_Notation)
		cout << dec << setfill (' ');
	}
return check (passed);
}

bool
Checker::check
	(
	const std::string&		description,
	const unsigned long&	expected,
	const unsigned long&	obtained
	)
{
bool
	passed = (obtained == expected);
if (! Compare_Equals)
	passed = ! passed;
cout << (passed ? "PASS" : "FAIL") << ": " << description << endl;
if (Verbose || ! passed)
	{
	if (Hex_Notation)
		cout << hex << setfill ('0');
	cout
		<< "====> expected: ";
	if (Hex_Notation)
		cout << "0x" << setw (sizeof (expected) << 1);
	cout << expected << endl;
	if (! Compare_Equals)
	cout
		<< "====> compared !=" << endl;
	cout
		<< "====> obtained: ";
	if (Hex_Notation)
		cout << "0x" << setw (sizeof (obtained) << 1);
	cout << obtained << endl;
	if (Hex_Notation)
		cout << dec << setfill (' ');
	}
return check (passed);
}

bool
Checker::check
	(
	const std::string&	description,
	const long long&	expected,
	const long long&	obtained
	)
{
bool
	passed = (obtained == expected);
if (! Compare_Equals)
	passed = ! passed;
cout << (passed ? "PASS" : "FAIL") << ": " << description << endl;
if (Verbose || ! passed)
	{
	if (Hex_Notation)
		cout << hex << setfill ('0');
	cout
		<< "====> expected: ";
	if (Hex_Notation)
		cout << "0x" << setw (sizeof (expected) << 1);
	cout << expected << endl;
	if (! Compare_Equals)
	cout
		<< "====> compared !=" << endl;
	cout
		<< "====> obtained: ";
	if (Hex_Notation)
		cout << "0x" << setw (sizeof (obtained) << 1);
	cout << obtained << endl;
	if (Hex_Notation)
		cout << dec << setfill (' ');
	}
return check (passed);
}

bool
Checker::check
	(
	const std::string&			description,
	const unsigned long long&	expected,
	const unsigned long long&	obtained
	)
{
bool
	passed = (obtained == expected);
if (! Compare_Equals)
	passed = ! passed;
cout << (passed ? "PASS" : "FAIL") << ": " << description << endl;
if (Verbose || ! passed)
	{
	if (Hex_Notation)
		cout << hex << setfill ('0');
	cout
		<< "====> expected: ";
	if (Hex_Notation)
		cout << "0x" << setw (sizeof (expected) << 1);
	cout << expected << endl;
	if (! Compare_Equals)
	cout
		<< "====> compared !=" << endl;
	cout
		<< "====> obtained: ";
	if (Hex_Notation)
		cout << "0x" << setw (sizeof (obtained) << 1);
	cout << obtained << endl;
	if (Hex_Notation)
		cout << dec << setfill (' ');
	}
return check (passed);
}

bool
Checker::check
	(
	const std::string&	description,
	const double&		expected,
	const double&		obtained,
	const double&		tolerance
	)
{
bool
	passed =
		(obtained <= (expected + tolerance)) &&
		(obtained >= (expected - tolerance));
if (! Compare_Equals)
	passed = ! passed;
cout << (passed ? "PASS" : "FAIL") << ": " << description << endl;
if (Verbose || ! passed)
	{
	cout
		<< "====> expected: " << expected << endl;
	if (! Compare_Equals)
	cout
		<< "====> compared !=" << endl;
	cout
		<< "====> obtained: " << obtained
			<< " with " << tolerance << " tolerance" << endl;
	}
return check (passed);
}

bool
Checker::check
	(
	const std::string&	description,
	const long double&	expected,
	const long double&	obtained,
	const long double&	tolerance
	)
{
bool
	passed =
		(obtained <= (expected + tolerance)) &&
		(obtained >= (expected - tolerance));
if (! Compare_Equals)
	passed = ! passed;
cout << (passed ? "PASS" : "FAIL") << ": " << description << endl;
if (Verbose || ! passed)
	{
	cout
		<< "====> expected: " << expected << endl;
	if (! Compare_Equals)
	cout
		<< "====> compared !=" << endl;
	cout
		<< "====> obtained: " << obtained
			<< " with " << tolerance << " tolerance" << endl;
	}
return check (passed);
}

bool
Checker::check
	(
	const bool&	passed
	)
{
Checks_Total++;
if (passed)
	++Checks_Passed;
return passed;
}

}		//	namespace idaeim
