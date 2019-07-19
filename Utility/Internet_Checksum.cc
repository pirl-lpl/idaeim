/*	Internet_Checksum

Copyright (C) 2012 - 2013  Bradford Castalia, idaeim studio.

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

CVS ID: Internet_Checksum.cc,v 1.1 2013/02/24 00:31:06 castalia Exp
*******************************************************************************/

#include	"Internet_Checksum.hh"


unsigned short
Internet_Checksum
	(
	void*	data,
	long	amount
	)
{
register long
	sum = 0;
unsigned short*
	address = reinterpret_cast<unsigned short*>(data);
while (amount > 1)
	{
	sum += *address++;
	amount -= 2;
	}
if (amount > 0)
	//	Add the remaining odd byte.
	sum += *(unsigned char*)address;

//	Fold 32-bit sum to 16 bits.
while (sum >> 16)
	sum = (sum & 0xFFFF) + (sum >> 16);

return ~sum;
}
