/*	VMS_Records_Filter

CVS ID: VMS_Records_Filter.cc,v 1.16 2012/06/08 02:01:03 castalia Exp


Copyright (C) 2002 - 2012  Bradford Castalia

This file is part of the idaeim libraries.

The idaeim libraries are free software; you can redistribute them
and/or modify them under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation, either version 3 of
the License, or (at your option) any later version.

The idaeim libraries are distributed in the hope that they will be
useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser
General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with this program. If not, see <http://www.gnu.org/licenses/>.

------------------------------------------------------------------------------*/

#include	"VMS_Records_Filter.hh"

#include	<string>
using std::string;




namespace idaeim
{
namespace Strings
{
/*==============================================================================
	Constants
*/
const char* const
	VMS_Records_Filter::ID =
		"idaeim::Strings::VMS_Records_Filter (1.16 2012/06/08 02:01:03)";

string
VMS_Records_Filter::identification () const
{return string (ID);}

const char
	VMS_Records_Filter::LINE_BREAK[] = {'\r', '\n'};

const int
	VMS_Records_Filter::RECORD_SIZE_LIMIT = 8192;

namespace
{
/*	Interpretation of Record_Size values:

	-3 - Filtering disabled.
	-2 - Initialization needed.
	-1 - Split record size value;
			the next byte is MSB, or pad if Padded is true.
	non-negative - Offset from the next byte to the next record size
			value, or pad if Padded is true.

	A pad byte, when present, is always followed by a record size value.
*/
enum
	{
	DISABLED	= -3,
	INITIALIZE	= -2,
	SPLIT		= -1
	};
}	//	Local namespace.

/*==============================================================================
	Constructor
*/
VMS_Records_Filter::VMS_Records_Filter()
	:	Record_Size (INITIALIZE),
		Padded (0)
{}

/*==============================================================================
	VMS record filtering.
*/
void
VMS_Records_Filter::filter
	(
	std::string&	a_string,
	Index			index,
	Index			end
	)
{
if (Record_Size == DISABLED)
	return;
if (end > a_string.length ())
	end = a_string.length ();
if (index >= end)
	return;

//	Pick up where we left off.
int
	size = Record_Size;
if (size == INITIALIZE)
	{
	if ((end - index) < 2 ||
		record_size (a_string[index], a_string[index + 1]) >= RECORD_SIZE_LIMIT)
		{
		//	Initialization test failed to detect record size bytes.
		Record_Size = DISABLED;
		return;
		}
	size = 0;
	Padded = 0;
	}
else if (size == SPLIT)
	{
	//	Split record size value; reassemble it.
	size = record_size (LSB, a_string[index]);
	if (size >= RECORD_SIZE_LIMIT)
		{
		//	Invalid record size.
		if (index > 0)
			//	Restore the LSB byte.
			a_string[index - 1] = LSB;
		Record_Size = DISABLED;
		return;
		}
	//	Fill the MSB hole.
	a_string[index] = LINE_BREAK[1];
	}

//	Plug the holes in the sized records.
for (index += size;	//	Next (pad and) record size bytes.
	(index + Padded + 1) < end;
	 index += size)
	{
	if (Padded)
		{
		//	Patch the null pad byte with a space character.
		a_string[index++] = ' ';
		}
	//	Next record size.
	size = record_size (a_string[index], a_string[index + 1]);
	if (size >= RECORD_SIZE_LIMIT)
		{
		//	Invalid record size.
		Record_Size = DISABLED;
		return;
		}
	//	Replace the 2 record size chars with LINE_BREAK chars.
	a_string[index++] = LINE_BREAK[0];
	a_string[index++] = LINE_BREAK[1];
	Padded = size % 2;	//	Is there a pad byte?
	}

if (index < end &&
	Padded)
	{
	//	Trailing pad byte.
	a_string[index++] = ' ';
	}

if ((index + 1) == end)
	{
	//	Split size value.
	Record_Size = SPLIT;
	LSB = a_string[index];
	a_string[index] = LINE_BREAK[0];
	}
else 
	/*	Set the offset from the end index to where the filtering
		will pick up again after the string has been extended.
	*/
	Record_Size = index - end;

}

int
VMS_Records_Filter::record_size
	(
	char	LSB,
	char	MSB
	)
{return (Index)(((MSB & 0xFF) << 8) | (LSB & 0xFF));}

bool
VMS_Records_Filter::enabled
	(
	bool	enable
	)
{
bool
	enable_state = Record_Size != DISABLED;
if (! enable)
	Record_Size = DISABLED;
else if (Record_Size == DISABLED)
	Record_Size = INITIALIZE;
return enable_state;
}

bool
VMS_Records_Filter::enabled () const
{
return Record_Size != DISABLED;
}


}		//	namespace Strings
}		//	namespace idaeim
