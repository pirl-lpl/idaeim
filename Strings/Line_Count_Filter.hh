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

CVS ID: Line_Count_Filter.hh,v 1.13 2013/02/24 00:50:13 castalia Exp
*******************************************************************************/

#ifndef	idaeim_Strings_Line_Count_Filter_hh
#define	idaeim_Strings_Line_Count_Filter_hh

#include	"Strings/String_Filter.hh"
#include	"Strings/Sliding_String.hh"

#include	<vector>


namespace idaeim
{
namespace Strings
{
/**	Line counting for a Sliding_String.

	As a Sliding_String is extended during a Sliding_String::slide, this
	filter keeps track of the #Location of the beginning of each line in
	the cumulative string. A Location is the cumulative index of all
	sequential strings that are filtered. Lines are counted at each
	occurance of a new-line ('\n') character in the string. The first
	line (line 1) is the portion of the string up to and including the
	first new-line. A line begins with the next character after the
	new-line.

	When a Line_Count_Filter is created it is initialized with the
	beginning Position of the first string to be filtered. A Position
	contains line count and character offset members. By default the
	initial Position is line 1, character 0. The filter records the
	Location of the beginning of each line after the first line.

	At any time the Position of any Location in the cumulative string
	may be found.

	<b>N.B.</b>: The Location of a character in the cumulative string
	is equivalent, but not necessarily identical, to the Location of
	the same character in the corresponding Sliding_String. Locations
	of the Line_Count_Filter are for the string at the time it was
	presented for filtering. Locations of the Sliding_String may be
	affected by application changes to the contents of the string. If
	the relative location of characters in the Sliding_String do not
	change, then both Location concepts are identical. Changes of
	new-line characters in the Sliding_String may, of course, affect
	the validity of line counts if the changes happen after the
	Line_Count_Filter has been applied.

	@see		String_Filter
	@see		Sliding_String

	@author		Bradford Castalia
	1.13 
*/
class Line_Count_Filter
	:	public String_Filter
{
public:
/*==============================================================================
	Types
*/
//!	A virtual location in a Sliding_String.
typedef Sliding_String::Location		Location;

//!	A line count and character offset Position.
struct Position
	{
	//!	Line count.
	Index	line;
	//!	Character offset.
	Index	character;
	/**	Creates a Position with an initial line count and
		character offset.
	
		@param	initial_line		The initial line count.
		@param	initial_character	The initial character offset.
	*/
	Position (Index initial_line = 1, Index initial_character = 0)
		:	line (initial_line), character (initial_character)
		{}
	};

/*==============================================================================
	Constants
*/
//!	Class identification.
static const char* const
	ID;

/*==============================================================================
	Constructor
*/
/**	Constructs a Line_Count_Filter in a newly enabled condition.

	An initial Position may be specified.

	@param	position	The initial Position of the line counter.
		[Default: line 1, character offset 0]
*/
Line_Count_Filter (const Position& position = Position (1, 0));

/*==============================================================================
	Accessors
*/
/**	Gets the class ID string.
*/
std::string identification () const;

/*==============================================================================
	Filtering methods
*/
/**	The line counters are updated based on the contents of a string.

	The Position of the start and end indices in the string are
	determined. The Position of the start index is the Position of the
	end index from the previous filter operation (when an initial
	Position is set, it is the end index Position that is set). To
	determine the new end index Position the character offset counter
	is incremented until a new-line character ('\\n') is encountered,
	which increments the line counter and resets the character offset
	counter. The search for new-lines is over the Index range (start,
	end] within the string. The end Index is always set to the length
	of the string. It is assumed that the start Index corresponds to
	the same character as the end Index of the previous filter
	operation (as is the case for a Sliding_String::slide),
	otherwise the Position values are likely to be invalid.

	@param	a_string	The string to be examined.
	@param	start		The starting Index in the string from which
		to search for new-line deliminters.
	@param	end			The end of the range over which to search.
*/
void filter
	(
	std::string&	a_string,
	Index			start = 0,
	Index			end = std::string::npos
	);

/**	Returns the Position corresponding to the Index in the last string
	filtered.

	If the Index is beyond the end of the last string, the value
	returned is the end Position with a character offset of
	std::string::npos. If the string has been altered since the last
	#filter operation, the Position may not be valid.

	<b>Note</b>: A Position can be determined even if the filter has
	been disabled. But the caveat about modifiying the last filtered
	string apply.

	@param	location	The Index for which to find a Position.
	@return	The Position of the index.
	@see	filter(std::string&, Index, Index)
*/
Position position (Location location) const;

/**	Enables the line counter with a new initial Position.

	@param	position	The initial Position of the line counter.
	@return	The previous state of the filter.
*/
bool enabled (const Position& position); 

/**	Enables or disables filtering.

	If the filter is being re-enabled after being disabled then the
	initial Position will reset to (0, 0). If the filter is
	currently in the same state as the enable argument, nothing is
	changed. Using

	<code><i>filter</i>.enabled (<i>filter</i>.enabled (false))</code>

	will not change the current enabled state of the filter, but
	will reset the line counter if the filter is enabled.

	@param	enable	If true filtering will be enabled; otherwise it
		is disabled.
	@return	The previous state of the filter.
*/
bool enabled (bool enable);

/**	Tests if the filter is enabled or disabled.

	@return	The current state of the filter.
*/
bool enabled () const;

/*==============================================================================
	Private Data
*/
private:

int
	Enabled;

//!	Location of the character after each new-line in line number order.
std::vector<Location>
	Line_Locations;

//!	Location of the last end index of the cumulative string.
Location
	Last_Location;

//!	The start Position of the cumulative string.
Position
	Start_Position;

};		//	End of Line_Count_Filter class.


}		//	namespace Strings
}		//	namespace idaeim
#endif	//	Line_Count_Filter_hh
