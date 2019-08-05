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

CVS ID: Lister.hh,v 1.28 2013/02/24 00:50:49 castalia Exp
*******************************************************************************/

#ifndef idaeim_PVL_Lister_hh
#define idaeim_PVL_Lister_hh

#include	"Parameter.hh"
#include	"Value.hh"

#include	<string>
#include	<iosfwd>


namespace idaeim
{
namespace PVL
{
//	Forward references.
class Parameter;
class Assignment;
class Aggregate;
class Value;
class Integer;
class Real;
class String;
class Array;

/*******************************************************************************
	Lister
*/
/**	A <i>Lister</i> is used to manage the listing of PVL objects.

	A Lister controls the modes for writing %PVL syntax text for
	Parameter and Value objects. These control the use of strict
	%Parameter %Value Language syntax such as multiple or single line
	comments, the use of formatting escape sequences, the PVL statement
	end marker, and the end-of-line character sequence. In addition,
	line indenting and wrapping to a maximum page width are also
	managed.

	The Lister provides both global defaults and lister specific mode
	settings. These settings apply to each Parameter and Value listed.
	They supplement the Parameter specific syntax modes.

@see		Parameter
@see		Value
@see		Parser

@author		Bradford Castalia
@version	1.28 
*/
class Lister
{
public:
/*==============================================================================
	Constants:
*/
//!	Class identification name with source code version and date.
static const char* const
	ID;

//!	The indent level value for no indenting.
static const int
	NO_INDENT;

/*==============================================================================
	Constructors
*/
/**	Default constructor.

	All local data members are initialized from their static default
	counterparts.

	@param	out	The ostream for listing output.
*/
explicit Lister (std::ostream& out = std::cout);

/**	Copy constructor.

	@param	lister	The Lister to be copied.
*/
Lister (const Lister& lister);

/*==============================================================================
	Syntax
*/
/*------------------------------------------------------------------------------
	Modes
*/
//!	Mode selectors.
enum MODE
	{
	Begin_Aggregates,
	Uppercase_Aggregates,
	Name_End_Aggregates,
	Assign_Align,
	Array_Indenting,
	Use_Statement_Delimiter,
	Single_Line_Comments,
	Verbatim_Strings,
	TOTAL_MODES
	};

//!	Default strict mode.
#ifndef STRICT
#define STRICT							false
#endif

//!	Default Begin annotation for Aggregates.
#ifndef	BEGIN_AGGREGATES
#define	BEGIN_AGGREGATES				STRICT
#endif
#define	STRICT_BEGIN_AGGREGATES			true

//!	Default uppercase annotation for Aggregates.
#ifndef	UPPERCASE_AGGREGATES
#define	UPPERCASE_AGGREGATES			STRICT
#endif
#define	STRICT_UPPERCASE_AGGREGATES		true

//!	Default naming of Aggregate End annotations.
#ifndef	NAME_END_AGGREGATES
#define	NAME_END_AGGREGATES				STRICT
#endif
#define	STRICT_NAME_END_AGGREGATES		true

//!	Default alignment of Assignment Values.
#ifndef	ASSIGN_ALIGN
#define	ASSIGN_ALIGN					false
#endif
#define	STRICT_ASSIGN_ALIGN				false

//!	Default top level Array indenting.
#ifndef	ARRAY_INDENTING
#define	ARRAY_INDENTING					(! STRICT)
#endif
#define	STRICT_ARRAY_INDENTING			false

//!	Default statement end delimiter charater use.
#ifndef	USE_STATEMENT_DELIMITER
#define	USE_STATEMENT_DELIMITER			STRICT
#endif
#define	STRICT_USE_STATEMENT_DELIMITER	true

//!	Default single line comments presentation.
#ifndef	SINGLE_LINE_COMMENTS
#define	SINGLE_LINE_COMMENTS			STRICT
#endif
#define	STRICT_SINGLE_LINE_COMMENTS		true

//!	Default verbatim strings use.
#ifndef	LIST_VERBATIM_STRINGS
#define	LIST_VERBATIM_STRINGS			STRICT
#endif
#define	STRICT_LIST_VERBATIM_STRINGS	true

/**	Mode set selector.

	The <i>Which_Mode</i> class is provided to avoid type ambiguity if
	the arguments to the syntax methods where:

	<code>method (bool, bool = default)</code>

	is confused with

	<code>method (bool = default)</code>

	It is not sufficient to make the defaulted argument some other
	primitive type (e.g. an enum) since default conversions will still
	result in ambiguity.

	The solution is this trivial class (struct) which contains a state
	data member and two methods for the == and != operators. Now the
	defaulted argument is typed to this class and no ambiguity occurs.
*/
struct Which_Mode
	{
	char	state;

	bool operator== (const Which_Mode& which) const
		{return state == which.state;}
	bool operator!= (const Which_Mode& which) const
		{return ! (*this == which);}
	};

//!	The syntax mode which argument for selecting local modes.
static const Which_Mode
	LOCAL_MODE_SET;

//!	The syntax mode which argument for selecting default modes.
static const Which_Mode
	DEFAULT_MODE_SET;


protected:

//!	Default (static) mode states.
static bool
	Default_Modes[TOTAL_MODES];

//!	Local (object) mode states.
bool
	Modes[TOTAL_MODES];

//!	Strict mode states.
static const bool
	STRICT_MODES[TOTAL_MODES];


public:

/**	Sets the state of a mode.

	@param	selection	The selected #MODE.
	@param	enabled	The new state of the selected mode.
	@param	which	If #DEFAULT_MODE_SET, use the #Default_Modes;
		if #LOCAL_MODE_SET use the local #Modes.
	@return	This Lister.
*/
Lister& mode
	(const MODE selection, const bool enabled,
		const Which_Mode& which = LOCAL_MODE_SET)
	{(which == LOCAL_MODE_SET) ?
		(Modes[selection] = enabled) : (Default_Modes[selection] = enabled);
	return *this;}

//!	Convenience function for static setting of default modes.
static void default_mode (const MODE selection, const bool enabled)
	{Default_Modes[selection] = enabled;}

/**	Gets the state of a mode.

	@param	selection	The selected #MODE.
	@param	which	If #DEFAULT_MODE_SET, use the #Default_Modes;
		if #LOCAL_MODE_SET use the local #Modes.
	@return	The state of the selected mode.
*/
bool mode
	(const MODE selection, const Which_Mode& which = LOCAL_MODE_SET) const
	{return (which == LOCAL_MODE_SET) ?
		Modes[selection] : Default_Modes[selection];}

//!	Convenience function for static testing of default modes.
static bool default_mode (const MODE selection)
	{return Default_Modes[selection];}

/**	Gets the array of syntax mode states.

	The mode states is an array of TOTAL_MODES bool values. The mode
	that each state represents is reflected in the #MODE enum list.

	@param	which	If #DEFAULT_MODE_SET, use the #Default_Modes;
		if #LOCAL_MODE_SET use the local #Modes.
	@return	A pointer to the array of mode states.
		<b>N.B.</b>: The pointer only remains valid for the life of the
		Lister object from which it is obtained.
*/
const bool* modes
	(const Which_Mode& which = LOCAL_MODE_SET) const
	{return (which == LOCAL_MODE_SET) ? Modes : Default_Modes;}

/**	Sets the state of all syntax modes.

	@param	mode_set	A pointer to an array of syntax modes. If NULL
		the modes will all be reset to their defaults.
	@param	which	If #DEFAULT_MODE_SET, use the #Default_Modes;
		if #LOCAL_MODE_SET use the local #Modes.
	@return	This Lister.
*/
Lister& modes
	(const bool* mode_set, const Which_Mode& which = LOCAL_MODE_SET);

/*..............................................................................
	Specific modes:
*/
/**	Enable or disable the BEGIN annotation for Aggregates.

	When enabled, a "BEGIN_" annotation will be written immediately
	preceeding the "GROUP" or "OBJECT" {@link Parameter::type_name()
	type_name} that introduces an Aggregate when it is written.

	@param	enabled true if the #mode is enabled; false if disabled.
	@param	which	If #DEFAULT_MODE_SET, use the #Default_Modes;
		if #LOCAL_MODE_SET use the local #Modes.
	@return	This Lister.
*/
Lister& begin_aggregates
	(const bool enabled, const Which_Mode& which = LOCAL_MODE_SET)
	{return mode (Begin_Aggregates, enabled, which);}

//!	Convenience function for static setting of default modes.
static void default_begin_aggregates (const bool enabled)
	{Default_Modes[Begin_Aggregates] = enabled;}

/**	Test if the BEGIN annotation for Aggregates is enabled.

	@param	which	If #DEFAULT_MODE_SET, use the #Default_Modes;
		if #LOCAL_MODE_SET use the local #Modes.
	@return	true if the #mode is enabled; false if disabled.
	@see	BEGIN_aggregate(bool, const Which_Mode&)
*/
bool begin_aggregates
	(const Which_Mode& which = LOCAL_MODE_SET) const
	{return mode (Begin_Aggregates, which);}

//!	Convenience function for static testing of default modes.
static bool default_begin_aggregates ()
	{return Default_Modes[Begin_Aggregates];}


/**	Enable or disable uppercase of Aggregate identifiers.

	When enabled, the "GROUP" or "OBJECT" {@link Parameter::type_name()
	type_name}, and its corresponding "END_GROUP" or "END_OBJECT",
	identifiers that start and end an Aggregate will be written in
	uppercase (this applies to the optional "BEGIN" prefix); otherwise
	they will be inital-capped (e.g. "Begin_Object").

	@param	enabled true if the #mode is enabled; false if disabled.
	@param	which	If #DEFAULT_MODE_SET, use the #Default_Modes;
		if #LOCAL_MODE_SET use the local #Modes.
	@return	This Lister.
	@see	BEGIN_aggregates(bool, const Which_Mode&)
*/
Lister& uppercase_aggregates
	(const bool enabled, const Which_Mode& which = LOCAL_MODE_SET)
	{return mode (Uppercase_Aggregates, enabled, which);}

//!	Convenience function for static setting of default modes.
static void default_uppercase_aggregates (const bool enabled)
	{Default_Modes[Uppercase_Aggregates] = enabled;}

/**	Test if the uppercase of Aggregate identifiers is enabled.

	@param	which	If #DEFAULT_MODE_SET, use the #Default_Modes;
		if #LOCAL_MODE_SET use the local #Modes.
	@return	true if the #mode is enabled; false if disabled.
	@see	Uppercase_aggregates(bool, const Which_Mode&)
*/
bool uppercase_aggregates
	(const Which_Mode& which = LOCAL_MODE_SET) const
	{return mode (Uppercase_Aggregates, which);}

//!	Convenience function for static testing of default modes.
static bool default_uppercase_aggregates ()
	{return Default_Modes[Uppercase_Aggregates];}


/**	Enable or disable the use of the Aggregate name in its end identifier.

	When enabled, an "End_OBJECT" or "END_GROUP" identifier will have
	the name of the Aggregate written after it separated by a
	Parser::PARAMETER_NAME_DELIMITER surrounded by space (' ')
	characters.

	@param	enabled true if the #mode is enabled; false if disabled.
	@param	which	If #DEFAULT_MODE_SET, use the #Default_Modes;
		if #LOCAL_MODE_SET use the local #Modes.
	@return	This Lister.
*/
Lister& name_end_aggregates
	(const bool enabled, const Which_Mode which = LOCAL_MODE_SET)
	{return mode (Name_End_Aggregates, enabled, which);}

//!	Convenience function for static setting of default modes.
static void default_name_end_aggregates (const bool enabled)
	{Default_Modes[Name_End_Aggregates] = enabled;}

/**	Test if the use of the Aggregate name in its end identifier is enabled.

	@param	which	If #DEFAULT_MODE_SET, use the #Default_Modes;
		if #LOCAL_MODE_SET use the local #Modes.
	@return	true if the #mode is enabled; false if disabled.
	@see	BEGIN_aggregate(bool)
*/
bool name_end_aggregates
	(const Which_Mode which = LOCAL_MODE_SET) const
	{return mode (Name_End_Aggregates, which);}

//!	Convenience function for static testing of default modes.
static bool default_name_end_aggregates ()
	{return Default_Modes[Name_End_Aggregates];}


/**	Enable or disable Assignment alignment.

	When enabled, each Assignment of an Aggregate has its
	Parser::PARAMETER_NAME_DELIMITER aligned horizontally so they all appear
	at the same line location. In addition, the Value's wrap_depth is
	set to the same location plus two (to allow for the following space
	character).

	<b>N.B.</b>: This mode has no effect if #indenting is not in
	effect.

	<b>N.B.</b>: The #array_indenting mode is effectively disabled
	(though not reset) when Assignment alignment is enabled.

	@param	enabled true if the #mode is enabled; false if disabled.
	@param	which	If #DEFAULT_MODE_SET, use the #Default_Modes;
		if #LOCAL_MODE_SET use the local #Modes.
	@return	This Lister.
	@see	write(const Assignement&, int)
	@see	indent_width(int, const Which_Mode&)
*/
Lister& assign_align
	(const bool enabled, const Which_Mode which = LOCAL_MODE_SET)
	{return mode (Assign_Align, enabled, which);}

//!	Convenience function for static setting of default modes.
static void default_assign_align (const bool enabled)
	{Default_Modes[Assign_Align] = enabled;}

/**	Test if Assignment alignment is enabled.

	@param	which	If #DEFAULT_MODE_SET, use the #Default_Modes;
		if #LOCAL_MODE_SET use the local #Modes.
	@return	true if the #mode is enabled; false if disabled.
	@see	BEGIN_aggregate(bool)
*/
bool assign_align
	(const Which_Mode& which = LOCAL_MODE_SET) const
	{return mode (Assign_Align, which);}

//!	Convenience function for static testing of default modes.
static bool default_assign_align ()
	{return Default_Modes[Assign_Align];}


/**	Enable or disable indenting of Assignment Array Values.

	When enabled, an Assignment with an Array Value begins on a new
	line with its indent depth incremented by one level, unless
	#indenting in general has been disabled. However, if the Assignment
	being written has its #indenting mode disabled, then no Array
	indenting will be applied.

	@param	enabled true if the #mode is enabled; false if disabled.
	@param	which	If #DEFAULT_MODE_SET, use the #Default_Modes;
		if #LOCAL_MODE_SET use the local #Modes.
	@return	This Lister.
	@see	write(const Assignement&, int)
	@see	indent_width(int, const Which_Mode&)
*/
Lister& array_indenting
	(const bool enabled, const Which_Mode which = LOCAL_MODE_SET)
	{return mode (Array_Indenting, enabled, which);}

//!	Convenience function for static setting of default modes.
static void default_array_indenting (const bool enabled)
	{Default_Modes[Array_Indenting] = enabled;}

/**	Test if indenting of Array Values is enabled.

	@param	which	If #DEFAULT_MODE_SET, use the #Default_Modes;
		if #LOCAL_MODE_SET use the local #Modes.
	@return	true if the #mode is enabled; false if disabled.
	@see	BEGIN_aggregate(bool)
*/
bool array_indenting
	(const Which_Mode& which = LOCAL_MODE_SET) const
	{return mode (Array_Indenting, which);}

//!	Convenience function for static testing of default modes.
static bool default_array_indenting ()
	{return Default_Modes[Array_Indenting];}


/**	Enable or disable the use of the statement end delimiter.

	The Parser::STATEMENT_END_DELIMITER will be written as
	the last character of each PVL statement, immediately preceeding
	a new-line sequence, when enabled.

	@param	enabled true if the #mode is enabled; false if disabled.
	@param	which	If #DEFAULT_MODE_SET, use the #Default_Modes;
		if #LOCAL_MODE_SET use the local #Modes.
	@return	This Lister.
*/
Lister& use_statement_delimiter
	(const bool enabled, const Which_Mode& which = LOCAL_MODE_SET)
	{return mode (Use_Statement_Delimiter, enabled, which);}

//!	Convenience function for static setting of default modes.
static void default_use_statement_delimiter (const bool enabled)
	{Default_Modes[Use_Statement_Delimiter] = enabled;}

/**	Test if the statement end delimiter character will be used.

	@param	which	If #DEFAULT_MODE_SET, use the #Default_Modes;
		if #LOCAL_MODE_SET use the local #Modes.
	@return	true if the #mode is enabled; false if disabled.
	@see	use_statement_delimiter(bool, const Which_Mode&)
*/
bool use_statement_delimiter
	(const Which_Mode& which = LOCAL_MODE_SET) const
	{return mode (Use_Statement_Delimiter, which);}

//!	Convenience function for static testing of default modes.
static bool default_use_statement_delimiter ()
	{return Default_Modes[Use_Statement_Delimiter];}


/**	Enable or disable single line comments.

	@param	enabled true if the #mode is enabled; false if disabled.
	@param	which	If #DEFAULT_MODE_SET, use the #Default_Modes;
		if #LOCAL_MODE_SET use the local #Modes.
	@return	This Lister.
	@see	write_comment(const std::string&, int)
*/
Lister& single_line_comments
	(const bool enabled, const Which_Mode& which = LOCAL_MODE_SET)
	{return mode (Single_Line_Comments, enabled, which);}

//!	Convenience function for static setting of default modes.
static void default_single_line_comments (const bool enabled)
	{Default_Modes[Single_Line_Comments] = enabled;}

/**	Test if single line comments will be used.

	@param	which	If #DEFAULT_MODE_SET, use the #Default_Modes;
		if #LOCAL_MODE_SET use the local #Modes.
	@return	true if the #mode is enabled; false if disabled.
	@see	single_line_comments_default(bool)
*/
bool single_line_comments
	(const Which_Mode& which = LOCAL_MODE_SET) const
	{return mode (Single_Line_Comments, which);}

//!	Convenience function for static testing of default modes.
static bool default_single_line_comments ()
	{return Default_Modes[Single_Line_Comments];}


/**	Enable or disable verbatim quoted strings.

	When disabled (the default initial condition) multi-line quoted
	strings in PVL statements have white space surrounding the line
	breaks compressed to a single space character - except when the
	last non-white space character on the line is a dash ("-"), in
	which case no space is included. This is because output formatting
	is expected to be controlled by embedded format characters which
	are processed by the Parameter's write method:

	\\n - line break.
	\\t - horizontal tab.
	\\f - form feed (page break).
	\\ - backslash character.
	\\v - verbatim (no formatting) till next \\v.

	When enabled all string values written as-is.

	@param	enabled true if the #mode is enabled; false if disabled.
	@param	which	If #DEFAULT_MODE_SET, use the #Default_Modes;
		if #LOCAL_MODE_SET use the local #Modes.
	@return	This Lister.
*/
Lister& verbatim_strings
	(const bool enabled, const Which_Mode& which = LOCAL_MODE_SET)
	{return mode (Verbatim_Strings, enabled, which);}

//!	Convenience function for static setting of default modes.
static void default_verbatim_strings (const bool enabled)
	{Default_Modes[Verbatim_Strings] = enabled;}

/**	Test if quoted strings will be written verbatim.

	@param	which	If #DEFAULT_MODE_SET, use the #Default_Modes;
		if #LOCAL_MODE_SET use the local #Modes.
	@return	true if the #mode is enabled; false if disabled.
	@see	verbatim_strings_default(bool, const Which_Mode&)
*/
bool verbatim_strings
	(const Which_Mode& which = LOCAL_MODE_SET) const
	{return mode (Verbatim_Strings, which);}

//!	Convenience function for static testing of default modes.
static bool default_verbatim_strings ()
	{return Default_Modes[Verbatim_Strings];}


/**	Enables or disables strict PVL syntax rules.

	Strict mode enables all syntax modes, uses the #strict_NL
	characters for the new-line sequence, and uses space characters for
	indenting.

	Informal mode disables all syntax modes and uses the informal_NL
	characters for the new-line sequence.

	@param	enabled	true if strict rules are applied; false otherwise.
	@param	which	If #DEFAULT_MODE_SET, use the #Default_Modes;
		if #LOCAL_MODE_SET use the local #Modes.
	@return	This Lister.
	@see	NL(bool)
	@see	informal_NL(bool, const Which_Mode&)
*/
Lister& strict
	(const bool enabled, const Which_Mode& which = LOCAL_MODE_SET);

//!	Convenience function for static setting of default modes.
static void default_strict (const bool enabled);

/**	Test if strict PVL syntax rules will be enforced.

	@param	which	If #DEFAULT_MODE_SET, use the #Default_Modes;
		if #LOCAL_MODE_SET use the local #Modes.
	@return	true if strict syntax rules will be enforced; false
		otherwise.
*/
bool strict
	(const Which_Mode& which = LOCAL_MODE_SET) const;

//!	Convenience function for static testing of default modes.
static bool default_strict ();

/*------------------------------------------------------------------------------
	Line Ending
*/
protected:

/**	The New_Line sequence is an aspect of Strict mode.

	When Strict mode is true the Strict_NL_Characters are used;
	when Strict mode is false the Informal_NL_Characters are used.
*/
const char
	*New_Line,
	*Strict_NL_Characters,
	*Informal_NL_Characters;

static const char
	*Default_New_Line;


public:

//!	Default informal (non-strict) NL (new-line) characters.
#ifndef INFORMAL_NL_STRING
#define INFORMAL_NL_STRING		"\n"
#endif
static const char*
	Default_Informal_NL_Characters;
	
//!	Default strict mode NL (new-line) characters.
#ifndef STRICT_NL_STRING
#define STRICT_NL_STRING		"\r\n"
#endif
static const char*
	Default_Strict_NL_Characters;

/**	Sets the new-line (end-of-line) character sequence.

	If strict is true, the strict_NL characters are reset to the
	new-line sequence; otherwise the informal_NL characters are set.

	@param	new_line_characters	A pointer to the new end-of-line characters.
	@param	which	If #DEFAULT_MODE_SET, use the #Default_Modes;
		if #LOCAL_MODE_SET use the local #Modes.
	@return	This Lister.
	@see	strict_default(bool)
*/
Lister& NL
	(const char* new_line_characters, const Which_Mode& which = LOCAL_MODE_SET);

//!	Convenience function for static setting of default modes.
static void default_NL (const char* new_line_characters);

/**	Gets the new-line (end-of-line) character sequence.

	@param	which	If #DEFAULT_MODE_SET, use the Default_New_Line;
		if #LOCAL_MODE_SET use the local #Modes.
	@return	A char pointer to the new-line character sequence. This
		will be the #strict_NL characters unless #strict mode is not in
		effect, in which case it will be the #informal_NL sequence.
	@see	strict(bool)
*/
const char* NL
	(const Which_Mode& which = LOCAL_MODE_SET) const
	{return (which == LOCAL_MODE_SET) ? New_Line : Default_New_Line;}

//!	Convenience function for static getting of default values.
static const char* default_NL ()
	{return Default_New_Line;}

/**	Sets the informal (strict = false) new-line character sequence.

	If strict is false, the NL characters are reset to the new
	sequence.

	@param	new_line_characters	A pointer to the new end-of-line characters.
	@param	which	If #DEFAULT_MODE_SET, use the
		Default_Informal_NL_Characters; if #LOCAL_MODE_SET use the
		local #Modes.
	@return	This Lister.
	@see	strict(bool)
*/
Lister& informal_NL
	(const char* new_line_characters, const Which_Mode& which = LOCAL_MODE_SET);

/**	Gets the informal (strict = false) new-line character sequence.

	@param	which	If #DEFAULT_MODE_SET, use the
		Default_Informal_NL_Characters; if #LOCAL_MODE_SET use the
		local #Modes.
	@return	The informal end-of-line characters.
	@see	strict(bool)
*/
const char* informal_NL
	(const Which_Mode& which = LOCAL_MODE_SET) const
	{return (which == LOCAL_MODE_SET) ?
		Informal_NL_Characters : Default_Informal_NL_Characters;}

/**	Gets the strict (strict = true) new-line character sequence.

	@param	which	If #DEFAULT_MODE_SET, use the
		Default_Strict_NL_Characters; if #LOCAL_MODE_SET use the local
		#Modes.
	@return	The strict end-of-line characters.
	@see	strict(bool)
*/
const char* strict_NL
	(const Which_Mode& which = LOCAL_MODE_SET) const
	{return (which == LOCAL_MODE_SET) ?
		Strict_NL_Characters : Default_Strict_NL_Characters;}

/**	Sets the strict (strict = true) new-line character sequence.

	If strict is true, the NL characters are reset to the new sequence.

	@param	new_line_characters	A pointer to the new strict end-of-line
		characters.
	@param	which	If #DEFAULT_MODE_SET, use the
		Default_Strict_NL_Characters; if #LOCAL_MODE_SET use the local
		#Modes.
	@return	This Lister.
	@see	strict(bool)
*/
Lister& strict_NL
	(const char* new_line_characters, const Which_Mode& which = LOCAL_MODE_SET);

/*------------------------------------------------------------------------------
	Page Formatting
*/
protected:

//!	The maximum width of a listing.
int
	Page_Width;

//!	The width of an indent level.
int
	Indent_Width;

//!	The Assign_Align depth.
int
	Assign_Align_Depth;

public:

//!	Default page width.
#ifndef PAGE_WIDTH
#define PAGE_WIDTH				80
#endif
static int
	Default_Page_Width;

/**	Sets the page width.

	Each PVL statement will be wrapped by replacing the Parser::WHITESPACE
	sequence preceding the page width minus the #NL width with the #NL
	character sequence. If there is no whitespace preceding the maximum
	page width, then the wrap will occur at the next whitespace
	sequence, if any.

	@param	width	The width of the listing in characters. If this
		value is negative no line wrapping will occur.
	@param	which	If #DEFAULT_MODE_SET, use the Default_Page_Width;
		if #LOCAL_MODE_SET use the local #Modes.
	@return	This Lister.
*/
Lister& page_width
	(const int width, const Which_Mode& which = LOCAL_MODE_SET)
	{(which == LOCAL_MODE_SET) ?
		(Page_Width = width) :
		(Default_Page_Width = width);
	return *this;}

//!	Convenience function for static setting of default modes.
static void default_page_width (const int width)
	{Default_Page_Width = width;}

/**	Gets the page width.

	@param	which	If #DEFAULT_MODE_SET, use the Default_Page_Width;
		if #LOCAL_MODE_SET use the local #Modes.
	@return	The width of a page at which line wrapping is to occur.
*/
int page_width
	(const Which_Mode& which = LOCAL_MODE_SET) const
	{return (which == LOCAL_MODE_SET) ? Page_Width : Default_Page_Width;}

//!	Convenience function for static getting of default values.
static int default_page_width ()
	{return Default_Page_Width;}

/*..............................................................................
	Indenting:
*/
//!	Default indent width.
#ifndef INDENT_WIDTH
#define INDENT_WIDTH			4
#endif
static int
	Default_Indent_Width;

/**	Sets the indenting width.

	<b>N.B.</b>: Use a width of #NO_INDENT
	to disable all indenting.

	@param	width	The number of indenting characters per indent level.
	@param	which	If #DEFAULT_MODE_SET, use the Default_Indent_Width;
		if #LOCAL_MODE_SET use the local #Modes.
	@return	This Lister.
*/
Lister& indent_width
	(const int width, const Which_Mode& which = LOCAL_MODE_SET)
	{(which == LOCAL_MODE_SET) ?
		(Indent_Width = width) :
		(Default_Indent_Width = width);
	return *this;}

//!	Convenience function for static setting of default modes.
static void default_indent_width (const int width)
	{Default_Indent_Width = width;}

/**	Gets the indenting width.

	@param	which	If #DEFAULT_MODE_SET, use the Default_Indent_Width;
		if #LOCAL_MODE_SET use the local #Modes.
	@return	The number of indent characters per indent level.
*/
int indent_width
	(const Which_Mode& which = LOCAL_MODE_SET) const
	{return (which == LOCAL_MODE_SET) ? Indent_Width : Default_Indent_Width;}

//!	Convenience function for static getting of default values.
static int default_indent_width ()
	{return Default_Indent_Width;}

/**	Calculates the character depth of an indent level.

	The depth of an indent level is the indent level times the
	#indent_width, but never less than zero. So even if the
	#indent_width has been set to a negative value the indent depth
	will always be zero regardless of the indent level.

	@param	level	The indent level.
	@return	The character depth of the indent level.
*/
unsigned int indent_depth (int level) const;

//!	Tests if indenting is enabled.
bool indenting () const
	{return Indent_Width > NO_INDENT;} 

/*==============================================================================
	I/O
*/
/**	Sets the output stream.

	@param	out	The ostream where output will be written.
	@return	This Lister.
*/
Lister& output_stream (std::ostream& out)
	{Output = &out; return *this;}

/**	Gets the output stream.

	@return	The ostream where output will be written.
*/
std::ostream& output_stream () const
	{return *Output;}

/**	Writes characters.

	@param	characters	A string of characters to be written.
	@param	wrap_depth	The indent depth for any wrapped lines.
	@return	This Lister.
*/
Lister& write (const std::string& characters, int wrap_depth = 0);

/**	Writes a character.

	@param	character	A character to be written.
	@param	wrap_depth	The indent depth for any wrapped lines.
	@return	This Lister.
*/
Lister& write (const char character, int wrap_depth = 0);

/**	Writes indenting characters.

	If #indenting is
	disabled, or the depth is not positive, nothing is written.

	@param	depth	The number of space (' ') characters to write.
	@return	This Lister.
	@see	indent_depth(int)
*/
Lister& indent (int depth);

/**	Adds an end-of-line marker to the characters pending output, which
	results in them being flushed to the output stream.

	@param	wrap_depth	The indent depth for any wrapped lines.
	@return	This Lister.
	@see	pending_characters()
	@see	flush()
	@see	output_stream()
*/
Lister& new_line (int wrap_depth = 0);

/**	Gets the total number of characters written.

	@return	The total number of characters sent to the {@link
		output_stream() output stream} since this Lister was constructed
		or the toal was {@link reset_total() reset}. <b>N.B.</b>: Any
		buffered {@link pending_characters() characters pending output}
		are not counted.
*/
unsigned int total () const
	{return Total_Written;}

/**	Resets the count of total characters output to zero.

	@return	This Lister.
	@see	total()
*/
Lister& reset_total ()
	{Total_Written = 0; return *this;}

/**	Gets the number of buffered characters pending output.

	@return	The number of characters buffered for output.
	@see	pending_characters()
*/
unsigned int pending () const
	{return Character_Buffer.length ();}

/**	Gets the characters pending output.

	@return	A string containing a copy of currently buffered characters.
	@see	pending()
*/
std::string pending_characters () const
	{return Character_Buffer;}

/*------------------------------------------------------------------------------
	Parameter
*/
/**	Writes a Parameter.

	@param	parameter	The Parameter to be written.
	@param	wrap_depth	The indent depth for any wrapped lines.
	@return	This Lister.
	@see	write(const Assignment&, int)
	@see	write(const Aggregate&, int)
*/
Lister& write (const Parameter& parameter, int wrap_depth = 0);

/**	Writes a Parameter.

	@param	parameter	The Parameter to be written.
	@return	The {@link output_stream() output stream} of this Lister.
	@see	write(const Parameter&, int)
*/
std::ostream& operator<< (const Parameter& parameter)
	{write (parameter); return *Output;}

/**	Writes an Assignment.

	The PVL syntax that is written follows this pattern:

	<i>Comment<br>
	Name</i> <b>=</b> <i>Value</i>

	The comment string, if there is one, is written by the
	#write_comment method.

	The #parameter_name is written on the next line {@link indent(int)
	indent}ed by the wrap_depth. <b>N.B.</b>: An Assignment indents
	itself.

	The Parameter's Value is written next, if the Parameter is not a
	TOKEN. It is preceeded by the Parser::PARAMETER_NAME_DELIMITER name
	delimiter character surrounded by space characters. The Value
	{@link write(const Value&, int) write} method is used to produce
	its PVL syntax representation.

	For Assignments with an Array Value, if the Assignment's
	#indenting, this Lister #indenting, and #array_indenting are
	enabled and the wrap_depth is positive then each Array, including
	elements of the Array that are themselves an Array, will be written
	on a new line and have their wrap_depth incremented by one indent
	level. However, if #array_indenting is disabled then the top level
	Array will be written on the same line as the Assignment and any
	contained Array elements will inherit all the other indenting
	modes; this includes the Assignment's indenting mode which, if
	disabled, overrides a positive wrap_depth causing all Array
	indenting to be disabled.

	The Parameter line is completed by writing a #new_line sequence.
	However, if strict PVL has been specified, then the PVL
	Parser::STATEMENT_END_DELIMITER character preceeds the end of line.

	<b>N.B.</b>: A #new_line is written last.

	@param	assignment	The Assignment to be written.
	@param	wrap_depth	The indent depth for any wrapped lines.
	@return	This Lister.
	@see	write_comment(const std::string&, int)
	@see	parameter_name(std::string, bool)
	@see	write(const Value&, int)
*/
Lister& write (const Assignment& assignment, int wrap_depth = 0);

/**	Writes an Aggregate.

	The PVL syntax that is written follows this pattern:

	<i>Comment<br>
	Type</i> <b>=</b> <i>Name<br>
	&nbsp;&nbsp;&nbsp;&nbsp;Parameter</i><br>
	&nbsp;&nbsp;&nbsp;&nbsp;...

	The comment string, if there is one, is written by the
	#write_comment method.

	The Aggregate's {@link Parameter::type_name() type_name} is written
	on the next line {@link indent(int) indent}ed by the wrap_depth.
	<b>N.B.</b>: An Aggregate indents itself. If #begin_aggregates is
	enabled the type name will be prepended with "Begin_". If
	#uppercase_aggregates is enabled the entire identifier will be
	written in uppercase.

	The Parameter's name is written next after the
	Parser::PARAMETER_NAME_DELIMITER character surrounded by space
	characters. Unless #verbatim_strings are enabled, escape sequences
	are substituted for special (format control) characters and
	Parser::TEXT_DELIMITER, Parser::SYMBOL_DELIMITER and backslash ('\\')
	characters are escaped (with a preceding backslash character). If the
	name contains Parser::WHITESPACE or Parser::RESERVED_CHARACTERS or is
	empty then it will be quoted (with Parser::TEXT_DELIMITER
	characters).

	The Aggregate identification line is completed by writing a
	#new_line sequence. However, if #use_statement_delimiter is enabled
	the PVL Parser::STATEMENT_END_DELIMITER character preceeds the end of
	line.

	<b>Note</b>: In the special case of an Aggregate used as a container
	for all of the Parameters read from a file (or a string defining
	multiple Parameters), the Aggregate identification is not written
	because it is only an artifact needed to contain the Parameter
	collection. This case is identified as an Aggregate named with the
	special Parser::CONTAINER_NAME.

	The Parameter list follows. If #indenting is enabled the wrap_depth
	is incremented by one indent level and then each Parameter in the
	list is {@link write(const Parameter&, int) write}n. This will
	recursively write any Aggregates in the Parameter list.

	At the end of the Parameter list, after decrementing any indenting,
	the Aggregate identification line will be repeated but with "End_"
	always used where "Begin_" would have been used (regardless of
	whether it was used or not). 

	For the special Parser::CONTAINER_NAME Aggregate an additional line
	containing just "END" (and the Parser::STATEMENT_END_DELIMITER when
	enabled) will be added to mark the end of the entire PVL sequence.
	<b>Note</b>: to turn a special container Aggregate into a regular
	Aggregate, just change its name; or <i>vice versa</i>.

	@param	aggregate	The Aggregate to be written.
	@param	wrap_depth	The indent depth for any wrapped lines.
	@return	This Lister.
	@see	write_comment(const std::string&, int)
	@see	parameter_name(const std::string, bool)
	@see	write (const Value&, int)
*/
Lister& write (const Aggregate& aggregate, int wrap_depth = 0);

/**	Writes a comment.

	A comment is preceeded by a Parser::COMMENT_START_DELIMITERS
	sequence. A single space character is added if the comment does not
	start with a space or horizonatal tab character. The comment is
	followed by aParser::COMMENT_END_DELIMITERS sequence. This is
	preceded by a single space if the comment does not end with a space
	or tab charater.

	When #single_line_comments mode is in effect, each line of the
	comment string is enclosed in comment delimiters.

	Comment lines - sections of the comment string terminated with an
	end-of-line ('\\n') character - are each written on their own line,
	depth {@link indent(int) indent}ed. The end of the comment string is
	implicitly an end-of-line, except if the depth is negative for
	in-line comments. Empty comment lines produce blank output lines;
	empty lines at the beginning of the string produce blank lines before
	the first Parser::COMMENT_START_DELIMITERS. sequence and empty lines
	at the end of the string produce blank lines after the last
	Parser::COMMENT_END_DELIMITERS sequence. For example, a single '\\n'
	character at the end of the string will produce one blank line after
	the end of the comment.

	@param	comment	The comment string to be written.
	@param	depth	The indent depth for all lines. Use a negative
		depth for in-line comments.
	@return	This Lister.
*/
Lister& write_comment (const std::string& comment, int depth = 0);

/**	Writes a Parameter name.

	If #verbatim_strings is true special characters (such as
	non-printing characters) are translated to escape sequences.

	The name is quoted with Parser::TEXT_DELIMITER characters if it is
	empty or contains characters otherwise reserved for the PVL syntax.

	@param	name	The name string to be written.
	@param	verbatim	Flag for verbatim treatment of characters.
	@return	The length of the name written.
	@throws	Invalid_Syntax		If the Parameter name is empty in
		verbatim mode.
	@throws	Reserved_Character	If the Parameter name contains any
		Parser::RESERVED_CHARACTERS in verbatim mode.
	@see	idaeim::PVL::translate_to_escape_sequences(std::string)
	@see	verbatim_strings(const Which_Mode&)
*/
int parameter_name (const std::string& name, bool verbatim = false);

/*------------------------------------------------------------------------------
	Value
*/
/**	Writes a Value.

	@param	value	The Value to be written.
	@param	wrap_depth	The indent depth for any wrapped lines.
	@return	This Lister.
	@see	write(Integer&, int)
	@see	write(Real&, int)
	@see	write(String&, int)
	@see	write(Array&, int)
*/
Lister& write (const Value& value, int wrap_depth = 0);

/**	Writes a Value.

	The Value representation is written and flushed to the {@link
	output_stream() output stream} of this Lister.

	@param	value	The Value to be written.
	@return	The output stream of this Lister.
	@see	write(const Value&, int)
*/
std::ostream& operator<< (const Value& value)
	{write (value); flush (); return *Output;}

/**	Writes an Integer Value.

	The integer representation is written in its base (radix) notation
	with at least the number of {@link Value::digits() digits} specified
	for the Value. If the base is not decimal, then the base value in
	decimal, followed by a Parser::NUMBER_BASE_DELIMITER, will preceed
	the integer representation, which will then be followed by another
	Parser::NUMBER_BASE_DELIMITER:

	<b><i>base</i>#<i>integer</i>#</b>

	<b>N.B.</b>: If the number of {@link Value::digits() digits} for the
	Value is greater than the number of digits required to represent the
	integer in its base notation, then the value will be padded with
	leading zeros.

	The units string, if any, will then be written.

	@param	integer	The Ingeter to be written.
	@param	wrap_depth	The indent depth for any wrapped lines.
	@return	This Lister.
	@see	Value::base()
	@see	Value::units()
	@see	value_units(const std::string)
*/
Lister& write (const Integer& integer, int wrap_depth = 0);

/**	Writes a Real Value.

	The real number representation is written using its {@link
	Real::format_flags() format flags} to control syntax with at least
	the {@link Value::digits() Value's digits} of precision.

	The units string, if any, will then be written.

	@param	real	The Real to be written.
	@param	wrap_depth	The indent depth for any wrapped lines.
	@return	This Lister.
	@see	Value::units()
	@see	value_units(const std::string)
*/
Lister& write (const Real& real, int wrap_depth = 0);

/**	Writes a String Value.

	If the String is a TEXT or SYMBOL {@link Value::Type Type} then it
	will be quoted with either Parser::TEXT_DELIMITER or
	Parser::SYMBOL_DELIMITER characters, respectively.

	The units string, if any, will then be written.

	@param	text	The String to be written.
	@param	wrap_depth	The indent depth for any wrapped lines.
	@return	This Lister.
	@see	Value::units()
	@see	value_units(const std::string)
*/
Lister& write (const String& text, int wrap_depth = 0);

/**	Writes an Array Value.

	Each Value of the Array is written separated by a
	Parser::PARAMETER_VALUE_DELIMITER and space (' '). Arrays of {@link
	Value::Type Type} SET are enclosed in a  Parser::SET_START_DELIMITER
	and Parser::SET_END_DELIMITER, while those of Type SEQUENCE SEQUENCE
	are enclosed in a Parser::SEQUENCE_START_DELIMITER and
	Parser::SEQUENCE_END_DELIMITER.

	When #indenting is true and the wrap_depth is positive each element
	Value that is itself an Array will be started on a new line with
	its indent depth incremented by one level. When the wrap_depth is
	negative and not #NO_INDENT then indenting of subarrays will not
	occur, but the effective wrap depth will be set to -wrap_depth - 2;
	this special value is used when writing an Assignment of an Array
	when #indenting is enabled but the Assignment's {@link
	Parameter::indenting() indenting} has been disabled.

	The units string, if any, will be written after the closing Array
	delimiter.

	@param	array	The Array to be written.
	@param	wrap_depth	The indent depth for any wrapped lines.
	@return	This Lister.
	@see	write(const Value&, int)
	@see	Value::units()
	@see	value_units(const std::string)
*/
Lister& write (const Array& array, int wrap_depth = 0);

/**	Writes a units string for a Value.

	The units string of the Value is written. However, if the units
	string is empty and the Value is a member of an Array, then the
	value_units of the parent Array will be used. The effect is to
	write the "nearest" non-empty units string.

	@param	value	The Value to have its units written.
	@return	This Lister.
	@see	value_units(const std::string&)
*/
Lister& value_units (const Value& value);

/**	Writes a Value units string.

	If the units is not empty a space (' ') character followed by the
	Parser::UNITS_START_DELIMITER character preceeds the string, and a
	Parser::UNITS_END_DELIMITER character follows it.

	@param	units	The units string to be written.
	@return	This Lister.
*/
Lister& value_units (const std::string& units);

/*------------------------------------------------------------------------------
	Special I/O methods
*/
/**	Writes any pending characters on the output stream of this Lister.

	Each line of the characters - where a line is terminated by a '\\n'
	character (which is replaced with a #NL sequence) - is wrapped to the
	#page_width into multiple output lines. A line longer than the
	maximum page width minus the width of the trailing string plus #NL
	sequence - the wrap point - is broken into multiple lines by
	replacing the last sequence of Parser::WHITESPACE characters
	preceeding the wrap point with the trailing string and then #NL
	sequence. This line is written to the output stream.

	<b>N.B.</b>: When a string continuation delimiter character ('-')
	falls at the end of a line, immediately preceding a #NL, in a quoted
	PVL string the Parser will remove the delimiter character and all
	surrounding whitespace from the string (unless string continutation
	mode is disabled). Since the delimiter is contained in the string
	being listed it is clearly not the intention that the character be
	removed by a subsequent parsing of the PVL. To elide this gotcha the
	first whitespace character (forced to be a space character) following
	a string delimiter character that would fall at the end of a line is
	retained.

	If the whitespace preceeding the wrap point starts at the beginning
	of the line, then all the leading whitespace is removed and the
	line wrap is tried again; this effectively wraps a line beginning
	with a word that is at or beyond the wrap point to the beginning of
	the line. If whitespace can not be found before the wrap point then
	the break will occur at the first whitespace that occurs in the
	line.

	Each wrapped line section - not the first section of a line - is
	preceeded by wrap_to space (' ') characters and then the leading
	string. Each wrapped line section is itself wrapped.

	<b>N.B.</b>: If the characters string does not end with a '\\n'
	none will be provided, but this unterminated line will still be
	output.

	@param	wrap_to	The number of spaces to preceed wrapped lines.
	@param	leading		A string to preceed each wrapped line (after any
		wrap_to spaces).
	@param	trailing	A string to follow each wrapped line.
	@return	The number of characters written.
	@see	pending_characters()
	@see	output_stream()
*/
unsigned int write (int wrap_to = 0,
	const std::string& leading = "", const std::string& trailing = "");

/**	Flushes any pending characters to the output stream of this Lister.

	<b>WARNING</b>: All characters, including those not terminated by
	an end-of-line, so that subsequent
	{@link write(int, const std::string&, const std::string&) write}
	operations may result in apparently improper line wrapping.

	@param	wrap_depth	The indent depth for any wrapped lines.
	@return	The number of characters written.
	@see	write(std::string,
		size_type, size_type, const std::string& const std::string&)
	@see	pending_characters()
	@see	output_stream()
*/
unsigned int flush (int wrap_depth = 0);

//!	Clears the pending characters buffer.
void clear ()
	{Character_Buffer.erase ();}

/*------------------------------------------------------------------------------
	Private members
*/
private:

//!	The output stream for writing characters.
std::ostream
	*Output;

//!	Buffer for characters pending output.
std::string
	Character_Buffer;

//!	Cummulative total count of characters sent to the Output stream.
int
	Total_Written;

};		//	Lister class

/**	Output operator.

	Used to bind a leading ostream to this Lister and a following
	Parameter or Value. For example:

	<code>
	cout << lister.single_line_comments (true).page_width (64) << parameter;
	</code>

	<b>N.B.</b>: The leading ostream, and any applied syntax modes,
	remain bound to the lister. Thus, after the example above:

	<code>
	lister << aggregate;
	</code>

	will list all elements of the aggregate to cout with single line
	comments and a page width of 64 characters.

	@param	out	The ostream to be bound to the lister.
	@param	lister	The Lister to be bound.
	@return This lister.
	@see	Lister::output_stream(std::ostream&)
	@see	Lister::operator<<(const Parameter&)
	@see	Lister::operator<<(const Value&)
*/
Lister& operator<< (std::ostream& out, Lister& lister);

}		//	namespace PVL
}		//	namespace idaeim
#endif	//	idaeim_PVL_Lister_hh
