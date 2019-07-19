/*	Line_Count_Filter

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

CVS ID: Line_Count_Filter.cc,v 1.16 2013/02/24 00:50:13 castalia Exp
*******************************************************************************/

#include	"Line_Count_Filter.hh"

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
	Line_Count_Filter::ID =
		"idaeim::Strings::Line_Count_Filter (1.16 2013/02/24 00:50:13)";

std::string
Line_Count_Filter::identification () const
	{return string (ID);}

namespace
{
//!	Interpretation of Enabled values.
enum
	{
	INITIALIZE	= -1,
	DISABLED	=  0,
	ENABLED		=  1
	};
}

/*==============================================================================
	Constructor
*/
Line_Count_Filter::Line_Count_Filter
	(
	const Position&		position
	)
	:	Enabled (INITIALIZE),
		Start_Position (position)
{}

/*==============================================================================
	Line counting.
*/
void
Line_Count_Filter::filter
	(
	std::string&		a_string,
	Index				start,
	Index				end
	)
{
if (! Enabled)
	return;	// Filter disabled.
if (Enabled == INITIALIZE)
	{
	Enabled = ENABLED;
	Line_Locations.clear ();
	Last_Location = start;
	}
if (end > a_string.length ())
	end = a_string.length ();
Index
	index = start;
while (index < end)
	if (a_string[index++] == '\n')
		{
		//	Record the Location of the beginning of the next line.
		Line_Locations.push_back (Last_Location + index - start);
		}
Last_Location += end - start;
}

Line_Count_Filter::Position
Line_Count_Filter::position
	(
	Location			position_location
	) const
{
Line_Count_Filter::Position
	index_position = Start_Position;
std::vector<Location>::const_iterator
	location = Line_Locations.begin ();
if (! Line_Locations.empty ())
	{
	while (position_location > *location &&
			++location != Line_Locations.end ()) ;
	if (location == Line_Locations.end ())
		{
		//	Beyond the last new-line.
		index_position.line += Line_Locations.size ();
		if (position_location > Last_Location)
			//	Beyond the end of the cumulative string.
			index_position.character = string::npos;
		else
			//	Within the last line.
			index_position.character = position_location - *(--location);
		}
	else
		{
		Index
			line = location - Line_Locations.begin ();
		if (position_location == *location)
			//	Beginning of the next line.
			line++;
		if (! line)
			//	Within the first line.
			index_position.character += position_location;
		else
			index_position.character = position_location - *(--location);
		index_position.line += line;
		}
	}
else
	//	No lines recorded.
	index_position.character += position_location;
return index_position;
}

bool
Line_Count_Filter::enabled
	(
	const Position&		position
	)
{
Start_Position = position;
return enabled (true);
}

bool
Line_Count_Filter::enabled
	(
	bool				enable
	)
{
bool
	enable_state = Enabled;
if (! enable)
	Enabled = DISABLED;
else if (! enable_state && enable)
	Enabled = INITIALIZE;
return enable_state;
}

bool
Line_Count_Filter::enabled () const
{return Enabled;}


}		//	namespace Strings
}		//	namespace idaeim
