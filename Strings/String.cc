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

CVS ID:	String.cc,v 1.23 2013/02/24 23:54:52 castalia Exp
*******************************************************************************/

#include	"String.hh"

#include	<string>
using std::string;
#include	<cctype>




namespace idaeim
{
namespace Strings
{
//	For notational convenience:
typedef	String::Index	Index;

/*==============================================================================
	Constants:
*/
const char* const
	String::ID =
		"idaeim::Strings::String (1.23 2013/02/24 23:54:52)";


const Index
	String::EOS			= (Index)-1;

const bool
	String::SIGNED		= true,
	String::UNSIGNED	= false;

/*==============================================================================
	Constructors
*/
String::String ()
	:	string ()
{}

String::String
	(
	const std::string&	a_string,
	Index				index,
	Index				length
	)
	:	string (a_string, index, length)
{}

String::String
	(
	const char		*characters
	)
	:	string (characters)
{}

String::String
	(
	const char		*characters,
	Index			length
	)
	:	string (characters, length)
{}

namespace
{
const char
	digits[] =
    	{
    	'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
    	'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J',
    	'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T',
    	'U', 'V', 'W', 'X', 'Y', 'Z'
    	};

void
integer_to_string
	(
	String&					a_String,
	String::Integer_type	value,
	bool					is_signed,
	int						base,
	int						digit_count
	)
{
if (base < 0)
	{
	base = -base;
	value = -value;
	}
if (base < 2 || base > 36)
	{
	return;		//	Empty String.
	}
if (is_signed)
	{
	if (value < 0)
		value = -value;
	else
		is_signed = false;
	}	

/*  Start at the end of the string (least significant digit)
	and work back to the beginning (most significant digit).
*/
String::Unsigned_Integer_type
	number = value;
string::iterator
	position = a_String.begin ();
do
	{
	position = a_String.insert (position, digits[number % base]);
	--digit_count;
	number /= base;
	}
	while (number);

//	Add zero-padding.
while (digit_count-- > 0)
	position = a_String.insert (position, '0');

if (is_signed)
	a_String.insert (position, '-');
}
}	//	local namespace

String::String
	(
	Integer_type	value,
	bool			is_signed,
	int				base,
	int				digit_count
	)
{
integer_to_string (*this, value, is_signed, base, digit_count);
}

String::String
	(
	Integer_type	value,
	int				base,
	int				digit_count
	)
{
integer_to_string (*this, value, SIGNED, base, digit_count);
}

#ifdef idaeim_LONG_LONG_INTEGER
String::String
	(
	long			value,
	bool			is_signed,
	int				base,
	int				digit_count
	)
{
integer_to_string (*this, (Integer_type)value, is_signed, base, digit_count);
}

String::String
	(
	long			value,
	int				base,
	int				digit_count
	)
{
integer_to_string (*this, (Integer_type)value, SIGNED, base, digit_count);
}


String::String
	(
	int				value,
	bool			is_signed,
	int				base,
	int				digit_count
	)
{
integer_to_string (*this, (Integer_type)value, is_signed, base, digit_count);
}

String::String
	(
	int				value,
	int				base,
	int				digit_count
	)
{
integer_to_string (*this, (Integer_type)value, SIGNED, base, digit_count);
}
#endif

/*==============================================================================
	Methods
*/
Index
String::skip_over
	(
	const std::string&	skip,
	Index				index
	)
{return (index < length ()) ? find_first_not_of (skip, index) : EOS;}

Index
String::skip_back_over
	(
	const std::string&	skip,
	Index				index
	)
{return find_last_not_of (skip, index < length () ? index : length () - 1);}

Index
String::skip_until
	(
	const std::string&	find,
	Index				index
	)
{
return (index < length ()) ? find_first_of (find, index) : EOS;
}

Index
String::skip_back_until
	(
	const std::string&	find,
	Index				index
	)
{return find_last_of (find, index < length () ? index : length () - 1);}

Index
String::index_of
	(
	char			character,
	Index			index
	)
{return (index < length ()) ? find (character, index) : EOS;}

Index
String::index_of
	(
	const std::string&	pattern,
	Index				index
	)
{return (index < length ()) ? find (pattern, index) : EOS;}

bool
String::begins_with
	(
	const std::string&	pattern,
	Index				index,
	bool				case_sensitive
	)
{
if (index > length () ||
	pattern.length () > (length () - index))
	return false;
String
	substring = substr (index, pattern.length ());
return case_sensitive ?
	! substring.compare (pattern) :
	substring.equals_ignore_case (pattern);
}

bool
String::equals_ignore_case
	(
	const std::string&	pattern
	)
{
if (length () != pattern.length ())
	return false;
for (Index index = 0;
	 index < length ();
	 index++)
	{
	if (toupper (at (index)) != toupper (pattern.at (index)))
		{
		return false;
		}
	}
return true;
}

String&
String::replace
	(
	const std::string&	old_string,
	const std::string&	new_string,
	Index				start,
	Index				end
	)
{
while ((start = index_of (old_string, start)) != EOS &&
		(start + old_string.length ()) <= end)
	{
	string::replace (start, old_string.length (), new_string);
	start += new_string.length ();
	}
return *this;
}

String&
String::replace_span
	(
	const std::string&	span,
	const std::string&	substitute,
	Index				start,
	Index				end
	)
{
Index
	next;
while ((start = skip_until (span, start)) < end)
	{
	next = skip_over (span, start);
	next = (next < end) ? next - start : end - start;
	string::replace (start, next, substitute);
	start += substitute.length ();
	}
return *this;
}

String&
String::trim
	(
	Index			start,
	Index			final
	)
{
if (start >= length ())
	start  = length () - 1;
if (final >= length () && final != EOS)
	final  = length ();
//	Determine the increment direction.
int
	increment = 1;
if (start > final)
	increment = -1;
//	Check for whitespace.
Index
	 index;
for (index  = start;
	 index != final;
	 index += increment)
	if (! isspace (at (index)))
		break;
//	The length of the whitespace substring.
final = increment * (static_cast<int>(index) - static_cast<int>(start));
if (final)
	{
	if (index < start)
		start = index + 1;
	erase (start, final);
	}
return *this;
}

String&
String::trim_leading ()
{return trim (0, EOS);}

String&
String::trim_trailing ()
{return trim (EOS, 0);}

String&
String::trim ()
{return trim (0, EOS).trim (EOS, 0);}

String&
String::escape_to_special
	(
	Index			start,
	Index			final
	)
{
Index
	index;
if (start > final)
	{
	//	Translate reverse indices to forward indices.
	index = start;
	start = final + 1;
	final = index + 1;
	}
if (start > length ())
	start = length ();
if (final > length ())
	final = length ();
while (start < final)
	{
	if (at (start) == '\\')
		{
		//	Delete the backslash.
		erase (start, 1);
		--final;
		//	Check the escaped character.
		switch (at (start))
			{
			case 'b':
				//	BS
				at (start) = '\b';
				break;
			case 't':
				//	HT
				at (start) = '\t';
				break;
			case 'n':
				//	NL
				at (start) = '\n';
				break;
			case 'f':
				//	FF
				at (start) = '\f';
				break;
			case 'r':
				//	CR
				at (start) = '\r';
				break;
			case '0':
				//	Octal or null value.
				char
					character;
				unsigned int
					new_character = 0;
				for (index = start + 1;
					 index < final &&
						(index - start) < 4;	//	No more that four digits.
					 index++)
					{
					//	Only octal digits are acceptable.
					if ((character = at (index)) > '7' ||
						 character < '0')
						break;
					new_character *= 8;
					new_character += character - 48;
					}
				index -= start;
				if (new_character < 256)
					{
					string::replace (start++, index, 1,
						static_cast<char>(new_character));
					final -= --index;
					}
			}
		}
	else
		start++;
	}
return *this;
}

String&
String::special_to_escape
	(
	Index			start,
	Index			final
	)
{
Index
	index;
if (start > final)
	{
	//	Translate reverse indices to forward indices.
	index = start;
	start = final + 1;
	final = index + 1;
	}
if (start > length ())
	start = length ();
if (final > length ())
	final = length ();
char
	character;
string
	escape;
while (start < final)
	{
	character = at (start);
	if (character < 32 ||
		character > 126)
		{
		//	Check the special character.
		switch (character)
			{
			case '\b':
				//	BS
				escape = 'b';
				break;
			case '\t':
				//	HT
				escape = 't';
				break;
			case '\n':
				//	NL
				escape = 'n';
				break;
			case '\f':
				//	FF
				escape = 'f';
				break;
			case '\r':
				//	CR
				escape = 'r';
				break;
			case 0:
				escape = '0';
				break;
			default:
				//	Octal value.
				String
					value (static_cast<Integer_type>(character & 0xFF), 8);
				escape = '0' + value;
			}
		at (start++) = '\\';
		insert (start, escape);
		start += escape.length ();
		final += escape.length ();
		}
	else if (character == '\\')
		{
		//	Backslash
		if (++start < final &&
		  ((character = at (start)) == '\\' ||
			character == 't' ||
			character == 'n' ||
			character == 'r' ||
			character == '0' ||
			character == 'b' ||
			character == 'f'))
			//	Escape sequence
			continue;
		//	Escape the backslash
		insert ((std::string::size_type)start++, 1, '\\');
		++final;
		}
	else
		start++;
	}
return *this;
}

/*==============================================================================
	Functions
*/

namespace
{
const char* const
	RAD50_CHARACTERS = " ABCDEFGHIJKLMNOPQRSTUVWXYZ$.?0123456789";
const int
	UNASSIGNED_INDEX = 29;

int
RAD50_code
	(
	const char&	character
	)
{
if (character != RAD50_CHARACTERS[UNASSIGNED_INDEX])
	for (const char*
			RAD50_character = RAD50_CHARACTERS;
		   *RAD50_character;
		  ++RAD50_character)
		if (*RAD50_character == character)
			return RAD50_character - RAD50_CHARACTERS;
return -1;
}
}

bool
is_RAD50
	(
	const char&	character
	)
{return (RAD50_code (character) >= 0);}

unsigned short int
to_RAD50
	(
	const std::string&	characters,
	bool				encode_invalid
	)
{
unsigned short int
	rad50 = 0;
const char
	*character = characters.c_str ();
int
	code;
for (int
		index = 0;
		index < 3;
	  ++index)
	{
	if (! *character)
		code = 0;
	else
		{
		code = RAD50_code (*character++);
		if (code < 0)
			{
			if (! encode_invalid)
				{
				rad50 = -1;
				break;
				}
			code = UNASSIGNED_INDEX;
			}
		}
	rad50 = rad50 * 40 + code;
	}
return rad50;
}

std::string
from_RAD50
	(
	unsigned short int	rad50,
	bool				decode_invalid
	)
{
char
	characters[4] = {0, 0, 0, 0};
int
	code,
	index = 3;
while (index--)
	{
	code = rad50 % 40;
	if (code == UNASSIGNED_INDEX &&
		! decode_invalid)
		break;
	rad50 = (rad50 - code) / 40;
	characters[index] = RAD50_CHARACTERS[code];
	}
return characters;
}


}		//	namespace Strings
}		//	namespace idaeim
