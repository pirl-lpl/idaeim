/*	Qrounders

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

CVS ID: Qrounders.cc,v 1.3 2014/01/15 05:23:15 castalia Exp


This file contains code forked from the UA HiRISE Project source code
produced by Bradford Castalia at the HiRISE Operations Center:

HiROC CVS ID: HiView_Utilities.cc,v 1.1 2012/10/04 02:11:11 castalia Exp

Copyright (C) 2009-2011  Arizona Board of Regents on behalf of the
Planetary Image Research Laboratory, Lunar and Planetary Laboratory at
the University of Arizona.

This library is free software; you can redistribute it and/or modify it
under the terms of the GNU Lesser General Public License, version 2.1,
as published by the Free Software Foundation.

This library is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

*******************************************************************************/

#include	"Qrounders.hh"


namespace idaeim
{
double
round_to
	(
	double	value,
	int		decimal_places
	)
{
if (decimal_places == 0)
	{
	value += 0.5;
	value = static_cast<int>(value);
	}
else
	{
	double
		places = 1;
	if (decimal_places > 0)
		while (decimal_places--)
			places *= 10;
	else
		while (decimal_places++)
			places /= 10;
	value *= places;
	value += 0.5;
	value = (int)value / places;
	}
return value;
}

}	//	namespace idaeim
