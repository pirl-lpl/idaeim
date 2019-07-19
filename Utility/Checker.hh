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

CVS ID: Checker.hh,v 1.29 2014/02/06 02:22:31 castalia Exp
*******************************************************************************/

#ifndef idaeim_Utility_Checker_hh
#define idaeim_Utility_Checker_hh

#include	<string>
#include	<sstream>
#include	<iostream>
#include	<iomanip>


namespace idaeim
{
/**	A <i>Checker</i> is used to compare expected against obtained results
	for unit testing.

	Each check method compares the expected value against the obtained
	value. The comparison is done with the equality operator (==). For
	floating point values, however, a tolerance amount can be specified
	such that the obtained value must be less than or equal to the
	expected value plus the tolerance, and greater than or equal to the
	expected value minus the tolerance, for the comparison to be true.

	If {@link Compare_Equals} is false the result of the comparison is
	reversed: An otherwise true comparison becomes false, and a false
	comparison becomes true. This enables checks where the comparison
	result is expected to be false to pass the check.

	A message line is printed to cout of the form:

	PASS|FAIL: &lt;<i>description</i>&gt;

	where "PASS" is used if the check passed, "FAIL" otherwise.
	<b>N.B.</b>: A check passes if the result of the value comparison is
	the same as Compare_Equals; i.e. the value comparsion and
	Compare_Equals must be both true or both false for the check to pass.

	If the check fails, or {@link Verbose} is true, the expected and
	obtained values are printed on the following lines:

	====> expected|obtained [&lt;<i>length</i>&gt;&lt;NL&gt;]: &lt;<i>expected</i>&gt;|&lt;<i>obtained</i>&gt;

	If Hex_Notation is true integer values are reported using hexadecimal
	notation; otherwise decimal notation (the default) is used.

	For strings, and the strings contained in ostringstreams, their
	length is listed and they are started on a new line. Both an
	"expected" and "obtained" report are included. If Compare_Equals is
	false an additional line between the "expected" and "obtained"
	reports is included:

	====> compared !=

	<b>N.B.</b>: The contents of string and ostringstream arguments
	are reset after use if the arguments are not const values and
	{@link Reset_Values} is true.

	Each check method returns a passed or failed result.

	The total number of checks does is accumulated in the {@link
	Checks_Total} value. The {@link Checks_Passed} value is the number of
	checks that passed.

@author		Bradford Castalia
@version	1.29
*/
class Checker
{
public:
/*==============================================================================
	Constants:
*/
//!	Class identification name with source code version and date.
static const char* const
	ID;

/*==============================================================================
	Constructors
*/
Checker ();

/*==============================================================================
	Data members:
*/
/**	Verbose reporting flag.

	If true both the expected and obtained values; otherwise the expected
	and obtained values are only shown when the comparison is false.

	By default Verbose is false.
*/
bool
	Verbose;

/**	Comparison sense flag.

	If Compare_Equals is false the result of the expected and obtained
	value comparison is reversed. <b>N.B.</b>: The value comparsion and
	Compare_Equals must be both true or both false for a check to pass.

	By default Compare_Equals is true.

	@see	Checks_Passed
*/
bool
	Compare_Equals;

/**	Report values using hex notation.
*/
bool
	Hex_Notation;

/**	Reset string values after comparison flag.

	If true non-const expected and obtained string and ostringstream
	values will be reset (cleared) after the comparison; otherwise
	the values remain unchanged.

	By default Reset_Values is true.
*/
bool
	Reset_Values;

/**	Total number of checks that passed.

	@see	Compare_Equals
	@see	Checks_Total
*/
int
	Checks_Passed;

/**	Total number of checks performed.

	@see	Checks_Passed
*/
int
	Checks_Total;

/*==============================================================================
	Checks
*/
/**	Checks the contents of two ostringstream objects.

	<b>N.B.</b>: The expected and obtained results are const values so
	they will not be reset to empty regardles of the value of
	Reset_Values.

	@param	description	A brief description of what is being checked.
	@param	expected	The expected result.
	@param	obtained	The obtained result.
	@return	true if the check passed; false otherwise. The check
		accumulators will be {@link check(const bool&) updated} accordingly.
*/
bool
check
	(
	const std::string&			description,
	const std::ostringstream&	expected,
	const std::ostringstream&	obtained
	);

/**	Checks the contents of two ostringstream objects.

	<b>N.B.</b>: The expected and obtained results will be reset to empty
	if Reset_Values is true.

	@param	description	A brief description of what is being checked.
	@param	expected	The expected result.
	@param	obtained	The obtained result.
	@return	true if the check passed; false otherwise. The check
		accumulators will be {@link check(const bool&) updated} accordingly.
*/
bool
check
	(
	const std::string&	description,
	std::ostringstream&	expected,
	std::ostringstream&	obtained
	);

/**	Checks two string objects.

	<b>N.B.</b>: The expected and obtained results are const values so
	they will not be reset to empty regardles of the value of
	Reset_Values.

	@param	description	A brief description of what is being checked.
	@param	expected	The expected result.
	@param	obtained	The obtained result.
	@return	true if the check passed; false otherwise. The check
		accumulators will be {@link check(const bool&) updated} accordingly.
*/
bool
check
	(
	const std::string&	description,
	const std::string&	expected,
	const std::string&	obtained
	);

/**	Checks two string objects.

	<b>N.B.</b>: The expected and obtained results will be reset to empty
	if Reset_Values is true.

	@param	description	A brief description of what is being checked.
	@param	expected	The expected result.
	@param	obtained	The obtained result.
	@return	true if the check passed; false otherwise. The check
		accumulators will be {@link check(const bool&) updated} accordingly.
*/
bool
check
	(
	const std::string&	description,
	std::string&		expected,
	std::string&		obtained
	);

/**	Checks two addresses.

	@param	description	A brief description of what is being checked.
	@param	expected	The expected result.
	@param	obtained	The obtained result.
	@return	true if the check passed; false otherwise. The check
		accumulators will be {@link check(const bool&) updated} accordingly.
*/
bool
check
	(
	const std::string&	description,
	void*				expected,
	void*				obtained
	);

/**	Checks two bool values.

	@param	description	A brief description of what is being checked.
	@param	expected	The expected result.
	@param	obtained	The obtained result.
	@return	true if the check passed; false otherwise. The check
		accumulators will be {@link check(const bool&) updated} accordingly.
*/
bool
check
	(
	const std::string&	description,
	const bool&			expected,
	const bool&			obtained
	);

/**	Checks two char values.

	@param	description	A brief description of what is being checked.
	@param	expected	The expected result.
	@param	obtained	The obtained result.
	@return	true if the check passed; false otherwise. The check
		accumulators will be {@link check(const bool&) updated} accordingly.
*/
bool
check
	(
	const std::string&	description,
	const char&		expected,
	const char&		obtained
	);

/**	Checks two unsigned char values.

	@param	description	A brief description of what is being checked.
	@param	expected	The expected result.
	@param	obtained	The obtained result.
	@return	true if the check passed; false otherwise. The check
		accumulators will be {@link check(const bool&) updated} accordingly.
*/
bool
check
	(
	const std::string&	description,
	const unsigned char& expected,
	const unsigned char& obtained
	);

/**	Checks two byte arrays.

	@param	description	A brief description of what is being checked.
	@param	expected	The expected result.
	@param	obtained	The obtained result.
	@param	amount		The amount of data to check.
	@return	true if the check passed; false otherwise. The check
		accumulators will be {@link check(const bool&) updated} accordingly.
*/
bool
check
	(
	const std::string&	description,
	const unsigned char* expected,
	const unsigned char* obtained,
	unsigned int		 amount
	);

/**	Checks two short values.

	@param	description	A brief description of what is being checked.
	@param	expected	The expected result.
	@param	obtained	The obtained result.
	@return	true if the check passed; false otherwise. The check
		accumulators will be {@link check(const bool&) updated} accordingly.
*/
bool
check
	(
	const std::string&	description,
	const short&		expected,
	const short&		obtained
	);

/**	Checks two unsigned short values.

	@param	description	A brief description of what is being checked.
	@param	expected	The expected result.
	@param	obtained	The obtained result.
	@return	true if the check passed; false otherwise. The check
		accumulators will be {@link check(const bool&) updated} accordingly.
*/
bool
check
	(
	const std::string&		description,
	const unsigned short&	expected,
	const unsigned short&	obtained
	);

/**	Checks two int values.

	@param	description	A brief description of what is being checked.
	@param	expected	The expected result.
	@param	obtained	The obtained result.
	@return	true if the check passed; false otherwise. The check
		accumulators will be {@link check(const bool&) updated} accordingly.
*/
bool
check
	(
	const std::string&	description,
	const int&			expected,
	const int&			obtained
	);

/**	Checks two unsigned int values.

	@param	description	A brief description of what is being checked.
	@param	expected	The expected result.
	@param	obtained	The obtained result.
	@return	true if the check passed; false otherwise. The check
		accumulators will be {@link check(const bool&) updated} accordingly.
*/
bool
check
	(
	const std::string&	description,
	const unsigned int&	expected,
	const unsigned int&	obtained
	);

/**	Checks two long values.

	@param	description	A brief description of what is being checked.
	@param	expected	The expected result.
	@param	obtained	The obtained result.
	@return	true if the check passed; false otherwise. The check
		accumulators will be {@link check(const bool&) updated} accordingly.
*/
bool
check
	(
	const std::string&	description,
	const long&			expected,
	const long&			obtained
	);

/**	Checks two unsigned long values.

	@param	description	A brief description of what is being checked.
	@param	expected	The expected result.
	@param	obtained	The obtained result.
	@return	true if the check passed; false otherwise. The check
		accumulators will be {@link check(const bool&) updated} accordingly.
*/
bool
check
	(
	const std::string&		description,
	const unsigned long&	expected,
	const unsigned long&	obtained
	);

/**	Checks two long long values.

	@param	description	A brief description of what is being checked.
	@param	expected	The expected result.
	@param	obtained	The obtained result.
	@return	true if the check passed; false otherwise. The check
		accumulators will be {@link check(const bool&) updated} accordingly.
*/
bool
check
	(
	const std::string&	description,
	const long long&	expected,
	const long long&	obtained
	);

/**	Checks two unsigned long long values.

	@param	description	A brief description of what is being checked.
	@param	expected	The expected result.
	@param	obtained	The obtained result.
	@return	true if the check passed; false otherwise. The check
		accumulators will be {@link check(const bool&) updated} accordingly.
*/
bool
check
	(
	const std::string&			description,
	const unsigned long long&	expected,
	const unsigned long long&	obtained
	);

/**	Checks two double values.

	@param	description	A brief description of what is being checked.
	@param	expected	The expected result.
	@param	obtained	The obtained result.
	@param	tolerance	Comparison difference allowed.
	@return	true if the check passed; false otherwise. The check
		accumulators will be {@link check(const bool&) updated} accordingly.
*/
bool
check
	(
	const std::string&	description,
	const double&		expected,
	const double&		obtained,
	const double&		tolerance = 0.0
	);

/**	Checks two long double values.

	@param	description	A brief description of what is being checked.
	@param	expected	The expected result.
	@param	obtained	The obtained result.
	@param	tolerance	Comparison difference allowed.
	@return	true if the check passed; false otherwise. The check
		accumulators will be {@link check(const bool&) updated} accordingly.
*/
bool
check
	(
	const std::string&	description,
	const long double&	expected,
	const long double&	obtained,
	const long double&	tolerance = 0.0
	);

/**	Checks two objects.

	The objects being checked must provide operator== and operator<<
	methods. The results of operator== will determine if the check
	passes (true) or fails (false). If the test fails or Verbose is
	true operator<< will be used to list each OBJECT to cout (stdout).

	@param	description	A brief description of what is being checked.
	@param	expected	The expected OBJECT.
	@param	obtained	The obtained OBJECT.
	@return	true if the check passed; false otherwise. The check
		accumulators will be {@link check(const bool&) updated} accordingly.
*/
template<typename OBJECT>
bool check
	(
	const std::string&	description,
	const OBJECT&		expected,
	const OBJECT&		obtained
	)
{
bool
	passed = (obtained == expected);
if (! Compare_Equals)
	passed = ! passed;
std::cout << (passed ? "PASS" : "FAIL") << ": " << description << std::endl;
if (Verbose || ! passed)
	{
	std::cout
		<< "====> expected: " << expected << std::endl;
	if (! Compare_Equals)
	std::cout
		<< "====> compared !=" << std::endl;
	std::cout
		<< "====> obtained: " << obtained << std::endl;
	}
return check (passed);
}

/**	Updates the check accounting.

	The Checks_Total accumulator is incremented. The Checks_Passed
	accumulator is incremented if passed is true.

	@param	passed	true if the external test passed; false otherwise.
	@return	The passed value.
*/
bool
check (const bool& passed);

};		//	Checker class
}		//	namespace idaeim

#endif
