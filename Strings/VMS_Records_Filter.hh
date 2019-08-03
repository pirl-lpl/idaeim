/*	VMS_Records_Filter

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

CVS ID: VMS_Records_Filter.hh,v 1.15 2013/02/24 00:50:13 castalia Exp
*******************************************************************************/

#ifndef	idaeim_Strings_VMS_Records_Filter_hh
#define	idaeim_Strings_VMS_Records_Filter_hh

#include	"../Strings/String_Filter.hh"


namespace idaeim
{
namespace Strings
{
/**	VMS record filtering for a Sliding_String.

	Binary data files produced by DEC VMS systems use a record
	structure composed of a leading binary size value (16-bits, LSB
	first) followed by size bytes of data. This is like Pascal strings
	(and just as cumbersome). In addition, if the size is odd then a
	zero-valued pad byte will be appended to the record (to force word
	alignment for all size values).

	Binary record size values are detected by testing, at the beginning
	of the VMS record structure (at initialization of the algorithm on
	first use of the #filter method and after re-enabling) the first
	two characters (from the start index) of the string for a value
	less than 8k. This test for the presence of the binary size value
	depends on a few assumptions: 1) the start index is at the
	beginning of a potential binary size value (this is probably the
	beginning of the stream), 2) the second character of normal
	(unfiltered) input will be printable (have an ASCII value >= ' '),
	and 3) the binary size values are less than 8k. Of course, this
	also requires that any character encoding preserve the values of
	bytes in the stream. If the #filter method ever detects a record
	size value >= 8k it assumes that the VMS binary record structure is
	no longer present in the string and turns off further filtering.
	Filtering can be restarted with the #enabled(bool) method.

	<b>WARNING</b>: Strings that have as their second character (after
	the start index on first use) such print control characters as a
	tab, or new-line will be incorrectly sensed as containing VMS
	binary record size values.

	<b>WARNING</b>: If the string length after the start index is less
	than 2 when an initialization test is to be done, then the test is
	presumed to fail and filtering will be disabled.

	The String if filtered by replacing record size bytes with a
	#LINE_BREAK (CR-LF) sequence, and any pad bytes will be replaced
	with a space character, thus providing a consistent character
	stream.

	@see		String_Filter
	@see		Sliding_String

	@author		Bradford Castalia
	@version	1.15 
*/
class VMS_Records_Filter
	:	public String_Filter
{
public:
/*==============================================================================
	Constants
*/
//!	Class identification.
static const char* const
	ID;

//!	Used to plug VMS binary record size value holes.
static const char
	LINE_BREAK[];

//!	Threshold for invalid record_size value.
static const int
	RECORD_SIZE_LIMIT;

/*==============================================================================
	Constructor
*/
/**	Constructs a VMS_Records_Filter in a newly enabled condition.
*/
VMS_Records_Filter ();

/*==============================================================================
	Accessors
*/
/**	Gets the class ID string.
*/
virtual std::string identification () const;

/*==============================================================================
	Filtering methods
*/
/**	Filters out VMS binary record size values from current contents of
	the string.

	This filter is expected to be applied to a Sliding_String. The
	start Index must be where the new data was added when at the last
	slide forward. The end Index is expected to be the end of the
	string.

	Each VMS record is preceeded by a 16-bit, LSB first, binary record
	size value. The record size value is the number of characters in
	the next record. This value does not include the two chars of the
	size value itself, nor a possible zero valued byte that is appended
	to the record whenever its size is odd. The record size bytes are
	replaced with LINE_BREAK (CR-LF) characters.  Since the pad byte is
	a non-text datum it is plugged with a space character whenever it
	occurs.

	To keep track of where the record size values are located in the
	string, the offset from the last data byte processed to the next
	record size value is recorded in the Record_Size value. It's
	possible for a size value to be split between (at the end of)
	filtering operations. In this case the Record_Size is set to -1,
	the first (LSB) byte of the size value is saved in the LSB value,
	and the start byte of the next filtering operation will be the
	value of the next (MSB) byte of record size value; except if the
	last record was padded, in which case it follows the pad byte in
	the start position.

	To keep track of odd-sized, and thus padded, records between
	Sliding_String slide operations, a Padded value is set at the end
	of each filtering operation: 1 if a pad byte is expected; 0
	otherwise. This allows the Record_Size value to be adjusted
	accordingly.

	<b>Note</b>: The size of the string is not changed and no
	characters are moved. Only record size/padding values are replaced.
	If the position input stream backing the Sliding_String is changed,
	or the string is modified in a way that changes character positions
	or the value of the record size bytes, the filtering algorithm will
	lose track of the VMS record structure resulting in undefined, and
	probably inappropriate, effects.

	@param	a_string	A string to be filtered.
	@param	start	The position in the string where filtering
		is to start. Nothing is done if the position is not within the
		length of the string [default = 0].
	@param	end		The end of the filtering range. This is expected to
		be the end of the string [default = std::string::npos]
*/
virtual void filter
	(
	std::string&	a_string,
	Index			start = 0,
	Index			end = std::string::npos
	);

/**	Enables or disables filtering.

	If the filter is being re-enabled after being disabled - either as
	a result of calling #enabled (false) or because the algorithm
	detected an invalid record size - then the algorithm will
	reinitialize the next time the #filter method is used. If the
	filter is currently in the same state as the enable argument,
	nothing is changed.

	@param	enable	If true filtering will be enabled; otherwise it
		is disabled.
	@return	The previous state of the filter.
*/
virtual bool enabled (bool enable);

/**	Tests if the filter is enabled or disabled.

	@return	The current state of the filter.
*/
virtual bool enabled () const;

/**	Converts two char values to a single index value, assuming that the
	first char value is the LSB and the second the MSB of a 16-bit
	integer.

	Bits 0-7 of the MSB are shifted left 8 bits and ORed with bits 0-7
	of the LSB to form the integer value.

	@param	LSB	The Least Significant Byte of the 16-bit record size value.
	@param	MSB	The Most Significant Byte of the 16-bit record size value.
	@return	A record size value (non-negative).
*/
static int record_size (char LSB, char MSB);


/*==============================================================================
	Private Data
*/
private:
int
	Record_Size,
	Padded;
char
	LSB;
};		//	End of VMS_Records_Filter class.


}		//	namespace Strings
}		//	namespace idaeim
#endif	//	VMS_Records_Filter_hh
