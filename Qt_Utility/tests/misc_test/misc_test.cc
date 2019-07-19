/*	misc_test

Copyright (C) 2013 Bradford Castalia, idaeim studio.

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

CVS ID: misc_test.cc,v 1.2 2014/02/06 02:12:20 castalia Exp
*******************************************************************************/

#include	"misc.hh"

#include	"Utility/Checker.hh"
using namespace idaeim;

#include	<QApplication>
#include	<QColor>

#include	<string>
#include	<iostream>
#include	<iomanip>
#include	<cstdlib>
using namespace std;


void
init_data
	(
	unsigned char*	data,
	unsigned int	amount
	)
{
for (unsigned int
		index = 0;
		index < amount;
		index++)
	data[index] = index;
}


int
main
	(
	int		count,
	char**	arguments
	)
{
QApplication
	application (count, arguments);

cout << "MSB_host = " << boolalpha << MSB_host () << endl;

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

checker.check ("is_invalid (0.0)",
	false, is_invalid (0.0));
checker.check ("is_invalid (1234.5678)",
	false, is_invalid (1234.5678));
checker.check ("is_invalid (INVALID_VALUE)",
	true, is_invalid (INVALID_VALUE));

checker.check ("decimal_digits (0)",
	1, decimal_digits (0));
checker.check ("decimal_digits (123456789)",
	9, decimal_digits (123456789));

checker.check ("hex_digits (0)",
	1, hex_digits (0));
checker.check ("hex_digits (0x1234)",
	4, hex_digits (0x1234));

checker.check ("bytes_for_bits (0)",
	(unsigned int)0, bytes_for_bits (0));
checker.check ("bytes_for_bits (1)",
	(unsigned int)1, bytes_for_bits (1));
checker.check ("bytes_for_bits (16)",
	(unsigned int)2, bytes_for_bits (16));
checker.check ("bytes_for_bits (17)",
	(unsigned int)3, bytes_for_bits (17));

checker.check ("bits_mask (8)",
	(unsigned long long)255, bits_mask (8));
checker.check ("bits_mask (8, 2)",
	(unsigned long long)(255 << 2), bits_mask (8, 2));

unsigned char
	data[12],
	expected_data[12];
init_data (data, sizeof (data));
init_data (expected_data, sizeof (data));
checker.check ("swap_bytes (12, 1, data) return address",
	(void*)(data + sizeof (data)), (void*)swap_bytes (12, 1, data));
checker.check ("swap_bytes (12, 1, data) values",
	expected_data, data, sizeof (data));

expected_data[0] = 3;
expected_data[1] = 2;
expected_data[2] = 1;
expected_data[3] = 0;
expected_data[4] = 7;
expected_data[5] = 6;
expected_data[6] = 5;
expected_data[7] = 4;
expected_data[8] = 11;
expected_data[9] = 10;
expected_data[10] = 9;
expected_data[11] = 8;
checker.check ("swap_bytes (3, 4, data) return address",
	(void*)(data + sizeof (data)), (void*)swap_bytes (3, 4, data));
checker.check ("swap_bytes (3, 4, data) values",
	expected_data, data, sizeof (data));

init_data (data, sizeof (data));
expected_data[0] = 2;
expected_data[1] = 1;
expected_data[2] = 0;
expected_data[3] = 5;
expected_data[4] = 4;
expected_data[5] = 3;
expected_data[6] = 8;
expected_data[7] = 7;
expected_data[8] = 6;
expected_data[9] = 11;
expected_data[10] = 10;
expected_data[11] = 9;
swap_bytes (4, 3, data);
checker.check ("swap_bytes (4, 3, data) values",
	expected_data, data, sizeof (data));

checker.Hex_Notation = true;
unsigned int
	color_value = qRgba (0, 1, 2, 0xFF);
checker.check ("qRgba (0, 1, 2, 0xFF)",
	(unsigned int)0xFF000102, color_value);
checker.check ("sample_value (qRgba (0, 1, 2, 0xFF), 0)",
	(unsigned char)0, sample_value (color_value, 0));
checker.check ("sample_value (qRgba (0, 1, 2, 0xFF), 2)",
	(unsigned char)2, sample_value (color_value, 2));
checker.check ("sample_value (qRgba (0, 1, 2, 0xFF), -1)",
	(unsigned char)0xFF, sample_value (color_value, -1));
checker.Hex_Notation = false;

cout << endl
	 << "Checks: " << checker.Checks_Total << endl
	 << "Passed: " << checker.Checks_Passed << endl;

exit (0);
}
