/*	misc

Copyright (C) 2013-2014 Bradford Castalia, idaeim studio.

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

CVS ID: misc.cc,v 1.9 2014/02/19 02:31:11 castalia Exp
*******************************************************************************/

#include	"misc.hh"

#include	<QtCore/qnumeric.h>
#include	<QSize>

#include	<cmath>




namespace idaeim
{
const double INVALID_VALUE = qQNaN ();

bool
is_invalid
	(
	double	value
	)
{return qIsNaN (value);}


int
decimal_digits
	(
	unsigned long long	value
	)
{
int
	digits = 1;
while ((value /= 10))
	++digits;
return digits;
}


int
hex_digits
	(
	unsigned long long	value
	)
{
int
	digits = 1;
while ((value >>= 4))
	++digits;
return digits;
}


unsigned long long
bits_mask
	(
	unsigned int	bits,
	unsigned int	lsb
	)
{
unsigned long long
	mask = 0;
while (bits--)
	{
	mask <<= 1;
	mask  |= 1;
	}
mask <<= lsb;
return mask;
}


unsigned char*
swap_bytes
	(
	unsigned long long	groups,
	unsigned int		size,
	unsigned char*		data
	)
{
if (groups &&
	data)
	{
	if (size > 1)
		{
		register unsigned char
			datum,
			*first,
			*last;
		while (groups--)
			{
			first =
			last = data;
			last += size;
			while (first < --last)
				{
				datum    = *first;
				*first++ = *last;
				*last    = datum;
				}
			data += size;
			}
		}
	else
		data += groups * size;
	}
return data;
}


const bool
	HOST_IS_MSB = MSB_host ();


bool
MSB_host ()
{
union
	{
	unsigned char	bytes[sizeof (int)];
	unsigned int	integer;
	}
	value;
value.integer = 0;
value.bytes [0] = 1;
return (value.integer != 1);
}


double
scale_to_area
	(
	const QSize&		size,
	unsigned long long 	area
	)
{
double
	scaling = sqrt ((double)area / ((double)size.width () * size.height ()));
QSize
	scaled_size (size * scaling);
unsigned long long
	scaled_area =
		(unsigned long long)scaled_size.rwidth () * scaled_size.rheight ();
if (scaled_area > area)
	{
	/*	The scaling factor is too large, due to rounding.

		Reduce the scaled area by the smallest dimension that will bring
		the area under the limit. Then recalculate the scaling based on
		the adjusted dimension.
	*/
	scaled_area -= area;
	if (scaled_size.rwidth () < scaled_size.rheight ())
		{
		/*	If reducing the scaled area by a row width is sufficient
			to bring the area under the limit, then decrement the
			column height. Otherwise decrement the row width to reduce
			the scaled area by a column height.
		*/
		if ((unsigned long long)scaled_size.rwidth () >= scaled_area)
			{
			//	Reduce the area by a row width.
			scaled_size.rheight ()--;
			scaling = (double)scaled_size.rheight () / size.height ();
			}
		else
			{
			//	Reduce the area by a column height.
			scaled_size.rwidth ()--;
			scaling = (double)scaled_size.rwidth () / size.width ();
			}
		}
	else
		{
		if ((unsigned long long)scaled_size.rheight () >= scaled_area)
			{
			scaled_size.rwidth ()--;
			scaling = (double)scaled_size.rwidth () / size.width ();
			}
		else
			{
			scaled_size.rheight ()--;
			scaling = (double)scaled_size.rheight () / size.height ();
			}
		}
	}
return scaling;
}

}	//	namespace idaeim
