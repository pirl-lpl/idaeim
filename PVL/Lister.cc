/*	Lister

Copyright (C) 2004 - 2013  Bradford Castalia

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

CVS ID:	Lister.cc,v 1.29 2013/02/24 00:50:49 castalia Exp
*******************************************************************************/

#include	"Lister.hh"
#include	"Parser.hh"
#include	"PVL_Exceptions.hh"

#include	<string>
using std::string;
#include	<cstring>
#include	<sstream>
using std::ostringstream;
#include	<algorithm>
#include	<cstdlib>
#include	<cctype>

using std::ostream;
using std::istream;




namespace idaeim
{
namespace PVL
{
/*******************************************************************************
	Lister
*/
/*==============================================================================
	Types:
*/
typedef	string::size_type	size_type;

/*==============================================================================
	Constants:
*/
const char* const
	Lister::ID =
		"idaeim::PVL::Lister (1.29 2013/02/24 00:50:49)";

const int
	Lister::NO_INDENT				= -1;

#define EOS	std::string::npos

/*==============================================================================
	Constructors
*/
Lister::Lister
	(
	std::ostream&	out
	)
	:	Strict_NL_Characters (Default_Strict_NL_Characters),
		Informal_NL_Characters (Default_Informal_NL_Characters),
		Page_Width (Default_Page_Width),
		Indent_Width (Default_Indent_Width),
		Assign_Align_Depth (0),
		Output (&out),
		Total_Written (0)
{
strict (STRICT);
modes (modes (DEFAULT_MODE_SET));
}


Lister::Lister
	(
	const Lister& lister
	)
{
//	Copy the syntax modes.
modes (lister.modes ());
}

/*==============================================================================
	Syntax modes
*/
int
	Lister::Default_Page_Width			= PAGE_WIDTH,
	Lister::Default_Indent_Width		= INDENT_WIDTH;

bool
	Lister::Default_Modes[TOTAL_MODES]	=
		{
		BEGIN_AGGREGATES,
		UPPERCASE_AGGREGATES,
		NAME_END_AGGREGATES,
		ASSIGN_ALIGN,
		ARRAY_INDENTING,
		USE_STATEMENT_DELIMITER,
		SINGLE_LINE_COMMENTS,
		LIST_VERBATIM_STRINGS
		};

const bool
	Lister::STRICT_MODES[TOTAL_MODES]	=
		{
		STRICT_BEGIN_AGGREGATES,
		STRICT_UPPERCASE_AGGREGATES,
		STRICT_NAME_END_AGGREGATES,
		STRICT_ASSIGN_ALIGN,
		STRICT_ARRAY_INDENTING,
		STRICT_USE_STATEMENT_DELIMITER,
		STRICT_SINGLE_LINE_COMMENTS,
		STRICT_LIST_VERBATIM_STRINGS
		};

const Lister::Which_Mode
	Lister::DEFAULT_MODE_SET			= {0},
	Lister::LOCAL_MODE_SET				= {1};
	
const char
	*Lister::Default_Informal_NL_Characters	= INFORMAL_NL_STRING,
	*Lister::Default_Strict_NL_Characters	= STRICT_NL_STRING,
	*Lister::Default_New_Line				=
		STRICT ?
			Default_Strict_NL_Characters :
			Default_Informal_NL_Characters;


Lister&
Lister::modes
	(
	const bool*			mode_set,
	const Which_Mode&	which
	)
{
if (mode_set)
	{
	//	Apply the user's mode set.
	bool
		*set = (which == LOCAL_MODE_SET) ? Modes : Default_Modes;
	for (int selection = 0;
			 selection < TOTAL_MODES;
			 selection++)
		set[selection] = mode_set[selection];
	}
else if (which == LOCAL_MODE_SET)
	//	Apply the default modes to the local modes.
	for (int selection = 0;
			 selection < TOTAL_MODES;
			 selection++)
		Modes[selection] = Default_Modes[selection];
else
	//	Apply the built-in modes to the default modes.
	for (int selection = 0;
			 selection < TOTAL_MODES;
			 selection++)
		Default_Modes[selection] = (STRICT) ?
			STRICT_MODES[selection] : ! STRICT_MODES[selection];
return *this;
}


Lister&
Lister::strict
	(
	bool				enabled,
	const Which_Mode&	which
	)
{
if (which == LOCAL_MODE_SET)
	{
	for (int selection = 0;
			 selection < TOTAL_MODES;
			 selection++)
		Modes[selection] = enabled ?
			STRICT_MODES[selection] : ! STRICT_MODES[selection];
	New_Line = (enabled ?
		Strict_NL_Characters : Informal_NL_Characters);
	}
else
	default_strict (enabled);
return *this;
}

void
Lister::default_strict
	(
	const bool	enabled
	)
{
for (int selection = 0;
		 selection < TOTAL_MODES;
		 selection++)
	Default_Modes[selection] = enabled ?
		STRICT_MODES[selection] : ! STRICT_MODES[selection];
Default_New_Line = (enabled ?
	Default_Strict_NL_Characters : Default_Informal_NL_Characters);
}

bool
Lister::strict
	(
	const Which_Mode&	which
	)
	const
{
if (which == DEFAULT_MODE_SET)
	return default_strict ();
for (int selection = 0;
		 selection < TOTAL_MODES;
		 selection++)
	if (Modes[selection] != STRICT_MODES[selection])
		return false;
return New_Line == Strict_NL_Characters;
}

bool
Lister::default_strict ()
{
for (int selection = 0;
		 selection < TOTAL_MODES;
		 selection++)
	if (Default_Modes[selection] != STRICT_MODES[selection])
		return false;
return Default_New_Line == Default_Strict_NL_Characters;
}


Lister&
Lister::NL
	(
	const char*			new_line_characters,
	const Which_Mode&	which
	)
{
if (which == DEFAULT_MODE_SET)
	default_NL (new_line_characters);
else
	New_Line = strict () ?
		Strict_NL_Characters = new_line_characters :
		Informal_NL_Characters = new_line_characters;
return *this;
}

void
Lister::default_NL
	(
	const char*			new_line_characters
	)
{
Default_New_Line = default_strict () ?
	(Default_Strict_NL_Characters = new_line_characters) :
	(Default_Informal_NL_Characters = new_line_characters);
}

Lister&
Lister::informal_NL
	(
	const char*			new_line_characters,
	const Which_Mode&	which
	)
{
if ((which == LOCAL_MODE_SET))
	{
	const char
		*characters = Informal_NL_Characters;
	Informal_NL_Characters = new_line_characters;
	if (New_Line == characters)
		New_Line = Informal_NL_Characters;
	}
else
	{
	const char
		*characters = Default_Informal_NL_Characters;
	Default_Informal_NL_Characters = new_line_characters;
	if (Default_New_Line == characters)
		Default_New_Line = Default_Informal_NL_Characters;
	}
return *this;
}

Lister&
Lister::strict_NL
	(
	const char*			new_line_characters,
	const Which_Mode&	which
	)
{
if ((which == LOCAL_MODE_SET))
	{
	const char
		*characters = Strict_NL_Characters;
	Strict_NL_Characters = new_line_characters;
	if (New_Line == characters)
		New_Line = Strict_NL_Characters;
	}
else
	{
	const char
		*characters = Default_Strict_NL_Characters;
	Default_Strict_NL_Characters = new_line_characters;
	if (Default_New_Line == characters)
		Default_New_Line = Default_Strict_NL_Characters;
	}
return *this;
}

/*==============================================================================
	I/O
*/
/*------------------------------------------------------------------------------
	Utility functions
*/
#ifndef DOXYGEN_PROCESSING
namespace
{
string&
uppercase
	(
	string&			characters
	)
{
//	Uppercase the string in-place.
transform (characters.begin (), characters.end (),
	characters.begin (), toupper);
return characters;
}
}	//	local namespace
#endif	//	DOXYGEN_PROCESSING


Lister&
operator<<
	(
	ostream&		out,
	Lister&			lister
	)
{
lister.output_stream (out);
return lister;
}


Lister&
Lister::write
	(
	const std::string&	characters,
	int					wrap_depth
	)
{
Character_Buffer += characters;
if (characters.find_last_of ('\n') != EOS)
	write (wrap_depth);
return *this;
}


Lister&
Lister::write
	(
	const char		character,
	int				wrap_depth
	)
{
if (character == '\n')
	return new_line (wrap_depth);
Character_Buffer += character;
return *this;
}


Lister&
Lister::indent
	(
	int				depth
	)
{
if (indenting () && depth > 0)
	while (depth--)
		Character_Buffer += ' ';
return *this;
}


unsigned int
Lister::indent_depth
	(
	int				level
	)
	const
{
if (level > 0)
	level *= indent_width ();
if (level < 0)
	level = 0;
return level;
}

/*------------------------------------------------------------------------------
	Formatting
*/
unsigned int
Lister::write
	(
	int					wrap_depth,
	const std::string&	leading,
	const std::string&	trailing
	)
{
if (Character_Buffer.empty ())
	{
	return 0;
	}
unsigned int
	total_written = 0;
string
	section;
bool
	terminated_section = true;
int
	wrap_at = page_width ();
size_type
	leading_length = leading.length (),
	trailing_length = trailing.length ();
if (wrap_depth < 0)
	wrap_depth = 0;
size_type
	NL_length = strlen (NL ()),
	max_width = page_width () - NL_length,
	wrap_point,
	wrap_index,
	index;

if (wrap_at >= 0)
	{
	wrap_at -= trailing_length + NL_length;
	if (wrap_at < 0)
		wrap_at = 0;
	}
wrap_point = (size_type)wrap_at;

while (! Character_Buffer.empty ())
	{
	if ((index = Character_Buffer.find ('\n')) == EOS)
		{
		//	Wrap the remaining characters.
		index = Character_Buffer.length ();
		section = Character_Buffer;
		terminated_section = false;
		}
	else
		{
		//	Extract the next section (sans '\n') from the buffer.
		section = Character_Buffer.substr (0, index++);
		}
	//	Erase the section from the buffer.
	Character_Buffer.erase (0, index);

	while (section.length () > max_width)
		{
		//	Wrap the section.
		//	Search for whitespace before the wrap point.
		if ((wrap_index = section.find_last_of
				(Parser::WHITESPACE, wrap_point)) != EOS)
			{
			//	Find the beginning of the whitespace sequence.
			if ((index = section.find_last_not_of
					(Parser::WHITESPACE, wrap_index)) == EOS)
				{
				/*	All whitespace at the beginning of the section.
					Erase it all and try again.

					N.B.: This effectively wraps the previous section
					to the beginning of the next line.
				*/
				//	Search for the end of the whitespace.
				if ((index = section.find_first_not_of
						(Parser::WHITESPACE, wrap_index)) == EOS)
					{
					//	Blank.
					if (terminated_section)
						{
						//	Last section.
						break;
						}
					else
						//	Erase this section.
						index = wrap_index;
					}
				section.erase (0, index);
				continue;
				}
			else
				{
				//	Wrap at the beginning of the whitespace.
				if (section[index] == Parser::STRING_CONTINUATION_DELIMITER)
					{
					/*	SPECIAL CASE: String continuation delimiter at EOL
					
						When a string continuation delimiter character
						falls at the EOL, immediately preceding the
						end-of-line sequence, in a quoted PVL string the
						Parser will remove the delimiter character and
						all surrounding whitespace from the string
						(unless string continutation mode is disabled).
						Since it will only occur here at the end of a
						line by dint of the line wrapping operation it
						is clearly not the intention that the character
						be removed by a subsequent parsing of the PVL.
						By simply leaving the first whitespace character
						(forced to be a space character) before the EOL
						the string continuation feature of the PVL
						parser is elided.
					*/
					section[++index] = ' ';
					}
				wrap_index = ++index;
				}
			}
		//	Search for whitespace after the wrap point.
		else if ((wrap_index = section.find_first_of
					(Parser::WHITESPACE, wrap_point)) == EOS)
			{
			//	No whitespace; last section.
			wrap_index = section.length ();
			break;
			}

		//	Output the section.
		*Output << section.substr (0, wrap_index);
		total_written += wrap_index;

		if (trailing_length)
			{
			//	Add the trailing string.
			*Output << trailing;
			total_written += trailing_length;
			}

		//	Find the end of the section (beginning of the next section).
		if ((index = section.find_first_not_of
				(Parser::WHITESPACE, wrap_index)) == EOS)
			{
			//	Nothing left; last section.
			if (terminated_section)
				section.erase ();
			break;
			}
		//	Erase the section and the remainder of the break gap.
		section.erase (0, index);

		//	Add the NL to complete the previous section.
		*Output << NL ();
		total_written += NL_length;

		/*......................................................................
			Wrapped section
		*/
		if (wrap_depth)
			{
			//	Indent the next wrapped section.
			section.insert ((size_type)0, wrap_depth, ' ');
			}

		if (leading_length)
			{
			//	Add the leading string.
			section.insert (wrap_depth, leading);
			}
		}
	/*..........................................................................
		Last section
	*/
	if (terminated_section)
		{
		//	Trim off any trailing whitespace.
		if ((index = section.find_last_not_of (Parser::WHITESPACE)) == EOS)
			//	Only whitespace reamins.
			index = (size_type)-1;	//	Erase from the beginning of the section.
		if (++index != section.length ())
			{
			section.erase (index);
			}

		if (index)
			{
			//	Output the remainder of the section.
			*Output << section;
			total_written += index;
			}

		//	Add the final NL.
		*Output << NL ();
		total_written += NL_length;
		}
	else
		{
		//	Incomplete last section; put it back in the buffer.
		Character_Buffer = section;
		break;
		}
	}
Total_Written += total_written;
return total_written;
}


Lister&
Lister::write_comment
	(
	const std::string&	comment,
	int					depth
	)
{
if (comment.empty ())
	return *this;

string
	leading,
	trailing;
size_type
	comment_length = comment.length (),
	index,
	line_index = 0,
	line_length,
	trailing_blanks = 0;

leading = Parser::COMMENT_START_DELIMITERS;
if (comment.empty () ||
	(comment[0] != ' ' &&
	 comment[0] != '\t'))
	//	Add a space before a non-empty comment that doesn't begin with space.
	leading += ' ';
if (single_line_comments ())
	{
	int
		length = comment.length () - 1;
	if (length < 0 ||
		(comment[length] != ' ' &&
		 comment[length] != '\t'))
		//	Add a space after a non-empty comment that doesn't end with space.
		trailing = ' ';
	trailing += Parser::COMMENT_END_DELIMITERS;
	}

while (line_index < comment_length)
	{
	//	Find the next line section.
	if ((line_length = comment.find ('\n', line_index)) == EOS)
		//	Use the remainder of the characters.
		line_length = comment_length - line_index;
	else
		line_length -= line_index;

	if ((index = comment.find_first_not_of (Parser::WHITESPACE, line_index))
			> (line_index + line_length))
		{
		//	Blank line.
		if (! single_line_comments () && index == EOS)
			{
			//	Trailing blank(s).
			for (trailing_blanks = 1;	//	Implicit last EOL.
				 (line_index = comment.find ('\n', line_index)) != EOS;
				 ++line_index)
				{
				++trailing_blanks;
				}
			break;
			}
		line_index += line_length + 1;
		if (line_index < comment_length || depth >= 0)
			new_line ();
		continue;
		}
	//	Beginning of the next line (if there is one).
	index = line_index + line_length + 1;
	if (index == comment_length &&
		comment[index - 1] == '\n')
		{
		//	Single trailing blank line.
		++trailing_blanks;
		}

	//	Indent the line.
	indent (depth);

	if (! leading.empty ())
		{
		//	Comment start delimiters.
		write (leading, depth);
		if (! single_line_comments ())
			//	Don't use them anymore.
			leading.erase ();
		}

	//	Write the line section.
	write (comment.substr (line_index, line_length), depth);
	line_index = index;

	if (! trailing.empty ())
		//	Comment end delimiters.
		write (trailing, depth);

	if (line_index < comment_length || depth >= 0)
		//	Add a NL.
		Character_Buffer += '\n';

	//	List the comment line.
	single_line_comments () ?
		write (depth, leading, trailing) : write (depth);
	}

if (! single_line_comments ())
	{
	//	Add the comment end delimiters.
	indent (depth);
	if (depth < 0)
		write (' ', depth);
	write (Parser::COMMENT_END_DELIMITERS, depth);
	if (depth >= 0)
		new_line (depth);
	}

if (trailing_blanks)
	{
	//	Add trailing blank lines.
	if (--trailing_blanks || depth >= 0)
		new_line (depth);
	while (trailing_blanks--)
		new_line (depth);
	}
return *this;
}


unsigned int
Lister::flush
	(
	int		wrap_depth
	)
{
unsigned int
	total_written =  write (wrap_depth),
	more = Character_Buffer.length ();
//	Empty the character buffer.
if (more)
	{
	*Output << Character_Buffer;
	total_written += more;
	Character_Buffer.erase ();
	}
return total_written;
}


Lister&
Lister::new_line
	(
	int				wrap_depth
	)
{
Character_Buffer += NL ();
write (wrap_depth);
return *this;
}

/*------------------------------------------------------------------------------
	Parameter
*/
Lister&
Lister::write
	(
	const Parameter&	the_Parameter,
	int					wrap_depth
	)
{
if (the_Parameter.is_Aggregate ())
	return write (dynamic_cast<const Aggregate&>(the_Parameter), wrap_depth);
return write (dynamic_cast<const Assignment&>(the_Parameter), wrap_depth);
}


Lister&
Lister::write
	(
	const Aggregate&	the_Aggregate,
	int					wrap_depth
	)
{
//	Comment.
write_comment (the_Aggregate.comment (), wrap_depth);

if (! the_Aggregate.indenting () ||
	! indenting ())
	//	Aggregate indenting disabled.
	wrap_depth = NO_INDENT;
int
	parameter_depth = wrap_depth,
	previous_assign_align_depth = Assign_Align_Depth;

if (the_Aggregate.name () != string (Parser::CONTAINER_NAME))
	{
	indent (wrap_depth);

	//	Aggregate Type specification.
	string
		type_name;
	if (begin_aggregates ())
		type_name = "Begin_";
	type_name += the_Aggregate.type_name ();
	if (uppercase_aggregates ())
		uppercase (type_name);
	write (type_name);

	//	Aggregate name.
	write (' ');
	write (Parser::PARAMETER_NAME_DELIMITER);
	write (' ');
	parameter_name
		(the_Aggregate.name (), the_Aggregate.verbatim_strings ());

	//	PVL statement termination.
	if (use_statement_delimiter ())
		write (Parser::STATEMENT_END_DELIMITER);
	new_line (wrap_depth);

	if (wrap_depth >= 0)
		//	Indent the parameter list.
		parameter_depth += indent_depth (1);
	}

//	Aggregate List.
Aggregate::Parameter_List::const_iterator
	parameter;

if (assign_align () &&
	wrap_depth >= 0)
	{
	//	Set the new Assign_Align_Depth.
	Assign_Align_Depth = 0;
	int
		length;
	for (parameter = the_Aggregate.begin ();
		 parameter != the_Aggregate.end ();
		 ++parameter)
		if (! parameter->is_Aggregate () &&
			(length = parameter->name ().length ()) > Assign_Align_Depth)
			Assign_Align_Depth = length;
	Assign_Align_Depth += parameter_depth;
	}

for (parameter = the_Aggregate.begin ();
	 parameter != the_Aggregate.end ();
	 ++parameter)
	//	Write the Aggregate element Parameter.
	write (*parameter, parameter_depth);

//	The END.
if (the_Aggregate.name () != string (Parser::CONTAINER_NAME))
	{
	indent (wrap_depth);

	//	End of aggregate.
	string
		type_name ("End_");
	type_name += the_Aggregate.type_name ();
	if (uppercase_aggregates ())
		uppercase (type_name);
	write (type_name);

	//	Aggregate name at End.
	if (name_end_aggregates ())
		{
		write (' ');
		write (Parser::PARAMETER_NAME_DELIMITER);
		write (' ');
		parameter_name
			(the_Aggregate.name (), the_Aggregate.verbatim_strings ());
		}

	if (use_statement_delimiter ())
		//	PVL statement termination.
		write (Parser::STATEMENT_END_DELIMITER);
	new_line (wrap_depth);
	}
if (the_Aggregate.name () == string (Parser::CONTAINER_NAME))
	{
	indent (wrap_depth);

	//	END of Container.
	write ("END");
	if (use_statement_delimiter ())
		write (Parser::STATEMENT_END_DELIMITER);
	new_line (wrap_depth);
	}

//	Restore the assign align depth.
Assign_Align_Depth = previous_assign_align_depth;
return *this;
}


Lister&
Lister::write
	(
	const Assignment&	the_Assignment,
	int					wrap_depth
	)
{

//	Comment.
write_comment (the_Assignment.comment (), wrap_depth);

//	Name.
indent (wrap_depth);
int
	offset = parameter_name
		(the_Assignment.name (), the_Assignment.verbatim_strings ()),
	value_depth = wrap_depth;

//	Value.
if (! the_Assignment.is_Token ())
	{
	bool
		indented =
			wrap_depth >= 0 &&
			indenting (),
		freestanding = Assign_Align_Depth == 0;

	//	Assignment delimiter.
	if (indented &&
		assign_align ())
		{
		if (freestanding)
			//	Free-standing assignment; align here.
			Assign_Align_Depth = wrap_depth + offset;
		else
			{
			//	Align the assignment character.
			offset = Assign_Align_Depth - offset - wrap_depth;
			while (offset-- > 0)
				write (' ');
			}
		}
	write (' ');
	write (Parser::PARAMETER_NAME_DELIMITER);

	if (indented &&
		the_Assignment.value ().is_Array () &&
		the_Assignment.indenting () &&
		array_indenting () &&
		! assign_align ())
		{
		//	Top level array indented on the next line.
		new_line (wrap_depth);
		value_depth = wrap_depth + indent_depth (1);
		indent (value_depth);
		}
	else
		{
		//	Space after the assignment delimiter.
		write (' ');

		//	The depth for value wrapping.
		if (indented)
			{
			value_depth = assign_align () ?
				(Assign_Align_Depth + 3) :
				(wrap_depth + (int)indent_depth (1));
			if (the_Assignment.value ().is_Array () &&
				! the_Assignment.indenting ())
				//	Effective wrap_depth.
				value_depth = -value_depth - 2;
			}
		else
			value_depth = NO_INDENT;
		}
	write (the_Assignment.value (), value_depth);

	if (freestanding)
		Assign_Align_Depth = 0;
	}

//	PVL statement termination.
if (use_statement_delimiter ())
	write (Parser::STATEMENT_END_DELIMITER);
new_line (value_depth);
return *this;
}


int
Lister::parameter_name
	(
	const std::string&	name,
	bool				verbatim
	)
{
string
	the_name (name);
string::size_type
	index;

if (! verbatim)
	translate_to_escape_sequences (the_name);
if (the_name.empty ())
	{
	if (verbatim)
		throw Invalid_Syntax
			(
			string ("Empty Parameter name to list."),
			0,
			Lister::ID
			);
	//	Use an empty quoted string.
	the_name.append (2, Parser::TEXT_DELIMITER);
	}
else if ((index = the_name.find_first_of (Parser::RESERVED_CHARACTERS)) != EOS)
	{
	//	There's a reserved character in the String.
	if (verbatim)
		throw Reserved_Character
			(
			string ("In Parameter name \"") + the_name + "\".",
			index,
			Parameter::ID
			);
	//	Add quotes to protect the reserved characters.
	the_name.insert ((size_type)0, 1, Parser::TEXT_DELIMITER)
		.append (1, Parser::TEXT_DELIMITER);
	}
write (the_name);
return the_name.length ();
}

/*------------------------------------------------------------------------------
	Value
*/
Lister&
Lister::write
	(
	const Value&	the_Value,
	int				wrap_depth
	)
{
if (the_Value.is_Integer ())
	return write (dynamic_cast<const Integer&>(the_Value), wrap_depth);
if (the_Value.is_String ())
	return write (dynamic_cast<const String&>(the_Value), wrap_depth);
if (the_Value.is_Real ())
	return write (dynamic_cast<const Real&>(the_Value), wrap_depth);
return write (dynamic_cast<const Array&>(the_Value), wrap_depth);
}


Lister&
Lister::write
	(
	const Array&	the_Array,
	int				wrap_depth
	)
{
bool
	indented = indenting () && wrap_depth >= 0;
if (wrap_depth < 0 &&
	wrap_depth != NO_INDENT)
	//	Effective wrap_depth.
	wrap_depth = -wrap_depth - 2;

switch (the_Array.type ())
	{
	case Value::SET:
		write (Parser::SET_START_DELIMITER, wrap_depth); break;
	case Value::SEQUENCE:
		write (Parser::SEQUENCE_START_DELIMITER, wrap_depth); break;
	default: break;
	}
int
	value_depth;
Array::Value_List::const_iterator
	value = the_Array.begin ();
while (value != the_Array.end ())
	{
	value_depth = wrap_depth;
	if (indented &&
		value->is_Array ())
		{
		//	Indented sub-arrays.
		new_line (value_depth);
		value_depth += indent_depth (1);
		indent (value_depth);
		}

	//	Write the Array element Value.
	write (*value, value_depth);

	if (++value != the_Array.end ())
		{
		//	Value separation.
		write (Parser::PARAMETER_VALUE_DELIMITER);
		if (indented &&
			value_depth > wrap_depth &&
			! value->is_Array ())
			{
			//	Return from indented sub-array to non-array.
			new_line (wrap_depth);
			indent (wrap_depth);
			}
		else if (! indented ||
			! value->is_Array ())
			write (' ', wrap_depth);
		}
	}
switch (the_Array.type ())
	{
	case Value::SET:
		write (Parser::SET_END_DELIMITER, wrap_depth); break;
	case Value::SEQUENCE:
		write (Parser::SEQUENCE_END_DELIMITER, wrap_depth); break;
	default: break;
	}
value_units (the_Array.units ());
//	Wrap it all out.
write (wrap_depth);
return *this;
}


Lister&
Lister::write
	(
	const Integer&	the_Integer,
	int				wrap_depth
	)
{
Value::Integer_type
	value = the_Integer;
int
	base = the_Integer.base ();
if (base == 10)
	write (Value::integer_to_string
		(value, the_Integer.is_signed (), base, the_Integer.digits ()));
else
	{
	if (the_Integer.is_signed () && value < 0)
		{
		base = -base;
		value = -value;
		}
	write (Value::integer_to_string (base), wrap_depth);
	write (Parser::NUMBER_BASE_DELIMITER, wrap_depth);
	write (Value::integer_to_string (value,
		Value::UNSIGNED, base, the_Integer.digits ()), wrap_depth);
	write (Parser::NUMBER_BASE_DELIMITER, wrap_depth);
	}
value_units (the_Integer.units ());
//	Wrap it all out.
write (wrap_depth);
return *this;
}


Lister&
Lister::write
	(
	const Real&		the_Real,
	int				wrap_depth
	)
{
write (Value::real_to_string
	(static_cast<Value::Real_type>(the_Real),
		the_Real.format_flags (), the_Real.digits ()));
value_units (the_Real.units ());
//	Wrap it all out.
write (wrap_depth);
return *this;
}


Lister&
Lister::write
	(
	const String&	the_String,
	int				wrap_depth
	)
{
switch (the_String.type ())
	{
	case Value::TEXT:
		write (Parser::TEXT_DELIMITER);
		write (static_cast<Value::String_type>(the_String));
		write (Parser::TEXT_DELIMITER);
		break;
	case Value::SYMBOL:
		write (Parser::SYMBOL_DELIMITER);
		write (static_cast<Value::String_type>(the_String));
		write (Parser::SYMBOL_DELIMITER);
		break;
	default:
		write (static_cast<Value::String_type>(the_String));
	}
value_units (the_String.units ());
//	Wrap it all out.
write (wrap_depth);
return *this;
}


Lister&
Lister::value_units
	(
	const Value&	value
	)
{return value_units (value.nearest_units ());}


Lister&
Lister::value_units
	(
	const std::string&	units
	)
{
if (units.length ())
	{
	write (' ');
	write (Parser::UNITS_START_DELIMITER);
	write (units);
	write (Parser::UNITS_END_DELIMITER);
	}
return *this;
}

}		//	namespace PVL
}		//	namespace idaeim
