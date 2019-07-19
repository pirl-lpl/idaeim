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

CVS ID: Internet_Checksum.hh,v 1.1 2013/02/24 00:31:06 castalia Exp
------------------------------------------------------------------------------*/

#ifndef Internet_Checksum_hh
#define Internet_Checksum_hh

/**	An implementation of the RFC1071 Internet Checksum algorithm.

	In outline, the Internet Checksum algorithm is very simple:

	Adjacent octets to be checksummed are paired to form 16-bit
	integers, and the 1's complement sum of these 16-bit integers is
	formed.

	To generate a checksum, the checksum field itself is cleared, the
	16-bit 1's complement sum is computed over the octets concerned,
	and the 1's complement of this sum is placed in the checksum field.

	To check a checksum, the 1's complement sum is computed over the
	same set of octets, including the checksum field. If the result is
	all 1 bits (-0 in 1's complement arithmetic), the check succeeds.

	Suppose a checksum is to be computed over the sequence of octets
	A, B, C, D, ... , Y, Z. Using the notation [a,b] for the 16-bit
	integer a * 256 + b, where a and b are bytes, then the 16-bit 1's
	complement sum of these bytes is given by one of the following:

<pre>
	[A,B] +' [C,D] +' ... +' [Y,Z]
	[A,B] +' [C,D] +' ... +' [Z,0]
</pre>

	where +' indicates 1's complement addition. These cases correspond
	to an even or odd count of bytes, respectively.

	On a 2's complement machine, the 1's complement sum must be
	computed by means of an "end around carry", i.e., any overflows
	from the most significant bits are added into the least significant
	bits.

	For details see: http://www.faqs.org/rfcs/rfc1071.html

	@param	data	A data address.
	@param	amount	The number of data bytes.
	@return	The checksum of the data as an unsigned 16-bit value.
*/
unsigned short Internet_Checksum (void* data, long amount);

#endif
