/*	Types

Copyright (C) 2007 - 2013  Bradford Castalia

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

CVS ID: Types.hh,v 1.7 2014/01/06 09:30:07 castalia Exp
*******************************************************************************/

#ifndef idaeim_Utility_Types_hh
#define idaeim_Utility_Types_hh


namespace idaeim
{
/*==============================================================================
	Types
*/
/**	Selection of high precision integer value type.

	Define the idaeim_LONG_LONG_INTEGER token to use the long long
	integer types; otherwise the long integer type will be used.

	On most platforms a long integer has 32-bit precision and a long long
	integer has 64-bit precision. For access to files that are larger
	than 2GB, for example, 64-bit precision is necessary; but this may
	not be sufficient if the filesystem is not 64-bit capable (this is
	usually determined by the precision of such system defined types as
	off_t and the C++ standard I/O library types std::ios::off_type and
	std::streamoff).

	>>> WARNING <<< The library MUST be rebuilt if this is changed.
	Various conditional code selection depends on this token.
*/
#define idaeim_LONG_LONG_INTEGER

#ifdef idaeim_LONG_LONG_INTEGER
typedef long long					High_Precision_Integer_type;
typedef unsigned long long			High_Precision_Unsigned_Integer_type;
#else
typedef	long						High_Precision_Integer_type;
typedef	unsigned long				High_Precision_Unsigned_Integer_type;
#endif


#ifdef DOXYGEN_PROCESSING
/**	Selection of high precision floating point value type.

	Define the idaeim_LONG_DOUBLE token to use the long double type;
	otherwise the double type will be used.

	On most platforms a double has 64-bit precision and a long double
	has 128-bit precision (96-bit precision is also used on some
	platforms). Use of long double can be significant in certain
	mathematical computations, especially for iterative calculations.
	It is not usually necessary.

	>>> WARNING <<< The library MUST be rebuilt if this is changed.
	Various conditional code selection depends on this token.
*/
#define idaeim_LONG_DOUBLE
//	Remove the line above to enable it.
#endif

#ifdef idaeim_LONG_DOUBLE
typedef long double					High_Precision_Floating_Point_type;
#else
typedef double						High_Precision_Floating_Point_type;
#endif

}

#endif
