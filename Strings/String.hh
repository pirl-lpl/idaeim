/*	String

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

CVS ID:	String.hh,v 1.22 2013/02/24 23:52:50 castalia Exp
*******************************************************************************/

#ifndef	idaeim_Strings_String_hh
#define	idaeim_Strings_String_hh

#include	"../Utility/Types.hh"

#include	<string>


namespace idaeim
{
/** The Strings Library.

	The Strings Library includes general purpose classes to
	aid in the manipulation of character strings.
*/
namespace Strings
{
/**	A <i>String</i> is a derivation of the standard string class.

	It contains miscellaneous convenience methods; i.e. it is the
	repository for string operations and manipulation capabilities that
	typically accumulate during most programming projects.

@see		std::string
@author		Bradford Castalia
@version	1.22 
*/
class String
:	public std::string
{
public:
/*==============================================================================
	Types
*/
//!	Index of character in a String.
typedef	std::string::size_type	Index;

/**	Integer value type.

	@see	Types.hh
*/
typedef High_Precision_Integer_type				Integer_type;
typedef High_Precision_Unsigned_Integer_type	Unsigned_Integer_type;

/*==============================================================================
	Constants
*/
//!	Class identification name with source code version and date.
static const char* const
	ID;

//!	End-Of-String (std::string::npos).
static const Index
	EOS;

//!	Integer to string representation is_signed true selection.
static const bool
	SIGNED;
//!	Integer to string representation is_signed false selection.
static const bool
	UNSIGNED;

/*==============================================================================
	Constructors
*/
/*	Constructs an empty String.
*/
String ();

/*	Constructs a String from the substring of a string.

	@param	a_string	The string to use in constructing the String.
	@param	index	The starting Index of the substring.
	@param	length	The length of the substring [Default: all
		characters from index to the end of str].
*/
String
	(
	const std::string&	a_string,
	Index				index = 0,
	Index				length = EOS
	);

/*	Constructs a String from a C-string (NULL terminated char*).

	@param	characters	The C-string to use in constructing the String.
*/
String
	(
	const char			*characters
	);

/*	Constructs a String from an array of characters.

	@param	characters The character array.
	@param	length	The length of the character array.
*/
String
	(
	const char			*characters,
	Index				length
	);

/*	Constructs a String that represents an integer value in some base
	notation.

	@param	value	The Integer_type value to represent.
	@param	is_signed	true if the value is signed, false otherwise.
	@param	base	The representation's base notation.
		If the base is outside the range 2-26, inclusive, an empty
		string will be produced.
	@param	digits	The number of digits in the representation. Extra
		digits will appear as zero-padding preceeding the default
		representation. In no case will the number of digits be less
		than required to correctly represent the value. Thus zero
		digits always selects the default representation.
*/
String
	(
	Integer_type		value,
	bool				is_signed = true,
	int					base = 10,
	int					digits = 0
	);

/*	Constructs a String that represents an integer value in some base
	notation.

	@param	value	The Integer_type value to represent.
	@param	base	The representation's base notation.
		If the base is outside the range 2-26, inclusive, an empty
		string will be produced.
	@param	digits	The number of digits in the representation. Extra
		digits will appear as zero-padding preceeding the default
		representation. In no case will the number of digits be less
		than required to correctly represent the value. Thus zero
		digits always selects the default representation.
*/
String
	(
	Integer_type		value,
	int					base,
	int					digits = 0
	);

#ifdef idaeim_LONG_LONG_INTEGER
/*
	These alternate constructors are provided for backwards compatibility
	to ensure that drop-in replacement of the library will continue to
	provide the expected interface signatures.
*/

/*	Constructs a String that represents an integer value in some base
	notation.

	@param	value	The long value to represent.
	@param	is_signed	true if the value is signed, false otherwise.
	@param	base	The representation's base notation.
		If the base is outside the range 2-26, inclusive, an empty
		string will be produced.
	@param	digits	The number of digits in the representation. Extra
		digits will appear as zero-padding preceeding the default
		representation. In no case will the number of digits be less
		than required to correctly represent the value. Thus zero
		digits always selects the default representation.
*/
String
	(
	long				value,
	bool				is_signed = true,
	int					base = 10,
	int					digits = 0
	);

/*	Constructs a String that represents an integer value in some base
	notation.

	@param	value	The long value to represent.
	@param	base	The representation's base notation.
		If the base is outside the range 2-26, inclusive, an empty
		string will be produced.
	@param	digits	The number of digits in the representation. Extra
		digits will appear as zero-padding preceeding the default
		representation. In no case will the number of digits be less
		than required to correctly represent the value. Thus zero
		digits always selects the default representation.
*/
String
	(
	long				value,
	int					base,
	int					digits = 0
	);


/*
	These alternate constructors are provided for to remove the ambiguity
	that can result from having two integer type converters from long and
	long long.
*/

/*	Constructs a String that represents an integer value in some base
	notation.

	@param	value	The int value to represent.
	@param	is_signed	true if the value is signed, false otherwise.
	@param	base	The representation's base notation.
		If the base is outside the range 2-26, inclusive, an empty
		string will be produced.
	@param	digits	The number of digits in the representation. Extra
		digits will appear as zero-padding preceeding the default
		representation. In no case will the number of digits be less
		than required to correctly represent the value. Thus zero
		digits always selects the default representation.
*/
String
	(
	int					value,
	bool				is_signed = true,
	int					base = 10,
	int					digits = 0
	);

/*	Constructs a String that represents an integer value in
	some base notation.

	@param	value	The long value to represent.
	@param	base	The representation's base notation.
		If the base is outside the range 2-26, inclusive, an empty
		string will be produced.
	@param	digits	The number of digits in the representation. Extra
		digits will appear as zero-padding preceeding the default
		representation. In no case will the number of digits be less
		than required to correctly represent the value. Thus zero
		digits always selects the default representation.
*/
String
	(
	int					value,
	int					base,
	int					digits = 0
	);

#endif

/*==============================================================================
	Methods
*/
/**	Skips over any and all characters in the skip String, starting with
	the character at the specified Index.

	@param	skip	The set of characters to be skipped.
	@param	index	The Index of the first character to test.
	@return	The Index of the character at or beyond the starting Index
		that is not in the skip String, or #EOS if all
		remaining characters from the starting Index are in the skip
		String.
	@see	string#find_first_not_of(string, int)
*/
Index
skip_over
	(
	const std::string&	skip,
	Index				index = 0
	);

/**	Skips back over any and all characters in the skip String, starting
	with the character at the specified Index.

	@param	skip	The set of characters to be skipped.
	@param	index	The Index of the first character to test.
		[Default: #EOS, i.e. the last character of the string]
	@return	The Index of the last character at or before the starting
		Index that is not in the skip String, or #EOS if
		all characters up to and including the starting Index are in
		the skip String.
	@see	string#find_last_not_of(string, int)
*/
Index
skip_back_over
	(
	const std::string&	skip,
	Index				index = EOS
	);

/**	Skips until a character also in the find String is found, starting
	with the character at the specified Index.

	@param	find	The set of potential characters to find.
	@param	index	The Index of the first character to test.
	@return	The Index of the character at or beyond the starting Index
		that is in the find String, or #EOS if all
		remaining characters from the starting Index are not in the
		find String.
	@see	string#find_first_of(string, int)
*/
Index
skip_until
	(
	const std::string&	find,
	Index				index = 0
	);

/**	Skips back until a character also in the find String is found,
	starting with the character at the specified Index.

	@param	find	The set of potential characters to find.
	@param	index	The Index of the first character to test.
		[Default: EOS, i.e. the last character of the string]
	@return	The Index of the last character at or before the starting
		Index that is in the find String, or #EOS if all
		characters up to and including the starting Index are not in
		the find String.
	@see	string#find_last_of(string, int)
*/
Index
skip_back_until
	(
	const std::string&	find,
	Index				index = EOS
	);

/**	Gets the Index of the next character matching the specified
	character, starting with the specified Index.

	@param	character	The character to match.
	@param	index	The Index of the first character to test
	@return	The Index of the next character at or beyond the starting
		Index that matches the specified character, or #EOS
		if all remaining characters to not match the specified
		character.
	@see	string#find(char, int)
*/
Index
index_of
	(
	char				character,
	Index				index = 0
	);

/**	Gets the Index of the next occurance of the substring matching the
	pattern string, starting with the specified Index.

	@param	pattern	The pattern string to match.
	@param	index	The starting Index from which to search for the
		pattern string.
	@return	The Index of the next substring at or beyond the starting
		Index that matches the pattern string, or #EOS if
		the pattern is not found.
	@see	string#find(string, int)
*/
Index
index_of
	(
	const std::string&	pattern,
	Index				index = 0
	);

/**	Tests if the pattern string is equal to the substring of the same length
	starting at the Index.

	@param	pattern	The pattern string to match.
	@param	index	The starting Index from which to test for the pattern
		string.
	@param	case_sensitive	true for case sensitive pattern matching;
		false otherwise.
	@return	true if the characaters starting at index equal all the
		characters of the pattern; false otherwise.
	@see	string#compare(int, int, string)
*/
bool
begins_with
	(
	const std::string&	pattern,
	Index				index = 0,
	bool				case_sensitive = true
	);

/**	Tests if the pattern string is a case insensitive match to this
	entire string.

	@param	pattern	The pattern string to match.
	@return	true if they are equal while ignoring case; false otherwise.
*/
bool
equals_ignore_case
	(
	const std::string&	pattern
	);

/**	Replaces all occurances of the old_string with the new_string
	over a substring range.

	Recursive replacements are avoided: If the new_string contains the
	old_string, the new old_string remains in its location within the
	new_string; the search for an old_string continues at the location
	after where the new_string has been inserted.

	@param	old_string	The substring to find.
	@param	new_string	The string to replace the old_string.
	@param	start	The starting Index from which to test for the
		old_string.
	@param	end		The Index of the end (last + 1) character of the
		substring.
	@return	This, possibly modified, String.
	@see	#index_of(const std::string&, Index)
	@see	string#replace(int, int, string)
*/
String& replace
	(
	const std::string&	old_string,
	const std::string&	new_string,
	Index				start = 0,
	Index				end = EOS
	);

/**	Replaces all sequences of any and all characters from the span
	string with the substitute string over a substring range.

	Each replacement occurs on the longest sequence of characters all
	from the span string set.

	Recursive replacements are avoided: If the substitute contains any
	characters from the span the substitute remains unaffected; the
	search for another span continues at the location after where the
	substitute has been inserted.

	@param	span	The set of potential characters for replacement.
	@param	substitute	The string to replace each span sequence.
	@param	start	The starting Index of the substring in which to
		test for span characters.
	@param	end		The Index of the end (last + 1) character of the
		substring.
	@return	This, possibly modified, String.
	@see	#skip_until(const std::string&, Index)
	@see	#skip_over(const std::string&, Index)
	@see	string#replace(int, int, string)
*/
String&
replace_span
	(
	const std::string&	span,
	const std::string&	substitute,
	Index				start = 0,
	Index				end = EOS
	);

/**	Trims whitespace from a substring of the String over a substring
	range.

	@param	start	The Index of the starting character of the substring.
	@param	end		The Index of the end (last + 1) character of the
		substring.
	@return	This, possibly modified, String.
	@see	string#erase(int, int)
	@see	isspace(char)
*/
String&
trim
	(
	Index			start,
	Index			end = EOS
	);

/**	Trims whitespace from the leading end of the String.

	@return	This, possibly modified, String.
	@see	#trim(Index, Index)
*/
String&
trim_leading ();

/**	Trims whitespace from the trailing end of the String.

	@return	This, possibly modified, String.
	@see	#trim(Index, Index)
*/
String&
trim_trailing ();

/**	Trims whitespace from both ends of the String.

	@return	This, possibly modified, String.
	@see	#trim(Index, Index)
*/
String&
trim ();

/**	Substitutes special characters for escape sequences.

	The following escape sequences, and their corresponding special
	characters, are recognized:

	\\b - Backspace (BS)<br>
	\\t - Horizontal tab (HT)<br>
	\\n - Newline (NL)<br>
	\\f - Form feed (FF)<br>
	\\r - Carriage return (CR)<br>
	\\X - The character X<br>
	\\0nnn - The character having the octal value nnn (0 <= nnn <= 377)<br>

	The escape sequences will be substituted for their corresponding
	special characters, which will shorten the length of the String.
	All backslash characters, except those that are themselves escaped,
	will be removed from the String.

	@param	start	The Index of the starting character of the
		substring.
	@param	final	The Index of the end (last + 1) character of the
		substring.
	@return	This, possibly modidifed, String.
*/
String&
escape_to_special
	(
	Index			start = 0,
	Index			final = EOS
	);

/**	Substitutes escape sequences for special characters.

	All control characters - less than ' ' (32), plus backslash (92) and
	DEL (127) - will be substituted with escape sequences:

	\\b - for backspace (BS, 8)<br>
	\\t - for horizontal tab (HT, 9)<br>
	\\n - for newline (NL, 10)<br>
	\\f - for form feed (FF, 12)<br>
	\\r - for carriage return (CR, 13)<br>
	\\0 - for the NULL character (NUL, 0)<br>
	\\ - for backslash ('\\', 92)<br>
	\\0nnn - for all other special characters where nnn is the octal
		value of the character.<br>

	The escape sequences will substitute for their corresponding
	special characters, which will increase the length of the String.

	@param	start	The Index of the starting character of the
		substring.
	@param	final	The Index of the end (last + 1) character of the
		substring.
	@return	This, possibly modidifed, String.
*/
String&
special_to_escape
	(
	Index			start = 0,
	Index			final = EOS
	);
};		//	End of String class.

/*==============================================================================
	Functions
*/
/**	Decodes a RAD50 value to its 3 character ASCII equivalent.

	RAD50 is a character encoding scheme as used by the Digital Equipment
	Corporation (DEC) PDP-11 and VAX 16-bit word size systems.

	@param	rad50	An unsigned short int value encoding three characters
		using the RAD50 scheme.
	@param	decode_invalid	If true, when an invalid decoding is
		encounterd the reserved character '?' is provided in its place;
		otherwise the empty string is returned in this case.
	@return	A three character string decoded from the rad50 value.
	@see	to_RAD50(const char*, bool)
*/
std::string
from_RAD50
	(
	unsigned short int	rad50,
	bool				decode_invalid = true
	);

/**	Encodes up to three characters into a RAD50 value.

	RAD50 is a character encoding scheme as used by the Digital Equipment
	Corporation (DEC) PDP-11 and VAX 16-bit systems.

	A 16-bit short integer encodes three characters:

	RAD50 = (C0 * 050 + C1) * 050 + C2

	where:

	RAD50 is the resulting 16-bit integer value.<br>
	050 is the octal value 50 (decimal 40).<br>
	C0, C1, and C2 are the character code values from this set:

	<table>
	<tr><th>Character		</th><th>Code (octal)</th></tr>
	<tr><td>space (' ')     </td><td>0           </td></tr>
	<tr><td>A - Z			</td><td>1 - 32      </td></tr>
	<tr><td>$				</td><td>33          </td></tr>
	<tr><td>.				</td><td>34          </td></tr>
	<tr><td>unasigned		</td><td>35          </td></tr>
	<tr><td>0 - 9			</td><td>36 - 47     </td></tr>
	</table>

	Only the first three characters are encoded. If there are less than
	three characters space characters are used implicitly as padding.

	@param	characters	A string of ASCII characters.
	@param	encode_invalid	If true, when an invalid character not in
		the RAD50 character set is encountered the unassigned code is
		used in its place; otherwise the value -1 is returned.
	@return	An unsigned short int value encoding three characters using
		the RAD50 scheme. 65535 (-1) will be returned if an invalid
		character (not in the RAD50 character set) is encountered and
		encode_invalid is false.
	@see	from_RAD50(unsigned short int, bool)
		
*/
unsigned short int
to_RAD50
	(
	const std::string&	characters,
	bool				encode_invalid = true
	);

/**	Test if a character is in the RAD50 character set.

	@param	character	The character to test.
	@return	true if the character is in the RAD50 set; false otherwise.
	@see	to_RAD50(const char*, bool)
*/
bool
is_RAD50
	(
	const char&	character
	);

}		//	namespace Strings
}		//	namespace idaeim
#endif	//	String_hh
