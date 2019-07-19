/*	String_Filter

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

CVS ID: String_Filter.hh,v 1.13 2013/02/24 00:50:13 castalia Exp
*******************************************************************************/

#ifndef	idaeim_Strings_String_Filter_hh
#define	idaeim_Strings_String_Filter_hh

#include	<string>


namespace idaeim
{
namespace Strings
{
/**	A <i>String_Filter</i> filters the String contents of a
	Sliding_String during its slide forward.

	@see		Sliding_String
	@author		Bradford Castalia
	@version	1.13 
*/
class String_Filter
{
public:
/*==============================================================================
	Types
*/
//!	Index of character in a String.
typedef std::string::size_type	Index;

/*==============================================================================
	Constructors/destructor
*/
virtual ~String_Filter ()
	{}

/*==============================================================================
	Accessors
*/
/**	Gets the identification string for the filter.

	By default an empty string is provided.

	@return	A string that identifies the filter.
*/
virtual std::string identification () const
	{return "";}

/*==============================================================================
	Filtering methods
*/
/**	Applies the filtering algorithm to a string.

	An index range - [start, end) - of the string may be specified
	over which filtering is to be applied.

	@param	a_string	A string to be filtered.
	@param	start	The index in the string of the first character
		where the filtering is expected to start.
	@param	end		The index where filtering is expected to end; this
		character is not included in the filtering.
*/
virtual void filter
	(
	std::string&
	#ifdef DOXYGEN_PROCESSING
		a_string
	#endif
			,
	Index
	#ifdef DOXYGEN_PROCESSING
		start
	#endif
			= 0,
	Index
	#ifdef DOXYGEN_PROCESSING
		end
	#endif
			= std::string::npos
	)
	{
	}

/**	Enables or disables filtering.

	@param	enable	If true filtering will be enabled; otherwise it is
		disabled.
	@return	The previous state of the filter.
*/
virtual bool enabled (bool
	#ifdef DOXYGEN_PROCESSING
	enable
	#endif
	)
	{return true;}

/**	Tests if the filter is enabled or disabled.

	@return	The current state of the filter.
*/
virtual bool enabled () const
	{return true;}

};		//	End of String_Filter class.

}		//	namespace Strings
}		//	namespace idaeim
#endif	//	String_Filter_hh
