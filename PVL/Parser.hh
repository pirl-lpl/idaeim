/*	Parser

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

CVS ID: Parser.hh,v 1.30 2013/02/24 00:50:49 castalia Exp
*******************************************************************************/

#ifndef idaeim_PVL_Parser_hh
#define idaeim_PVL_Parser_hh

#include	"Parameter.hh"
#include	"Value.hh"
#include	"PVL_Exceptions.hh"
#include	"../Strings/Sliding_String.hh"


namespace idaeim
{
//	Forward references.
namespace Strings {
class VMS_Records_Filter;
class Line_Count_Filter;
}

namespace PVL
{
/**	A <i>Parser</i> interprets the characters of a
	<i>Sliding_String</i> as a sequence of %Parameter %Value Language
	(PVL) syntax statements.

	This Parser implements the syntax of the PVL used by the Planetary
	Data System (PDS) as specified by the
	<A HREF="http://www.ccsds.org" TARGET="_top">
	Consultative Committee for Space Data Systems</A> in the
	<A HREF="http://www.ccsds.org/blue_books.html" TARGET="_top">
	Blue Book</A>
	"Parameter Value Language Specification (CCSD0006,8)",
	June 2000 [CCSDS 641.0-B-2] and
	<A HREF="http://www.ccsds.org/green_books.html" TARGET="_top">
	Green Book</A>
	"Parameter Value Language - A Tutorial",
	June 2000 [CCSDS 641.0-G-1] documents.
	PVL has been accepted by the
	<A HREF="http://www.iso.ch" TARGET="_top">
	International Standards Organization (ISO)</A>,
	as a Final Draft International Standard (ISO 14961:2002) keyword
	value type language for naming and expressing data values.

	The PVL syntax defines a <i>Parameter</i> with this basic format:

	[<i>Comment</i>]<br>
	<i>Name</i> [<b>=</b> <i>Value</i>][<b>;</b>]

	The optional Comment is enclosed in C-style delimiters.

	The PVL syntax for a <i>Value</i> follows this format:

	[<b>(</b>|<b>{</b>]<i>Datum</i> [<b>&lt;</b><i>Units</i><b>&gt;</b>][<b>,</b> <i>Datum</i>[...]][<b>)</b>|<b>}</b> [<b>&lt;</b><i>Units</i><b>&gt;</b>]]

	The purpose of a Parser object is to assemble Parameter and Value
	objects using the PVL statements obtained from the associated
	Sliding_String. The Parser inserts a VMS_Records_Filter into
	the Sliding_String to automatically handle the special binary record
	size values if this (now obsolete) data source is encountered. A
	set of syntax modes are provided to control how strictly the
	PVL syntax will be applied. Normally the Parser is very tolerant
	with the goal of providing parameters if at all possible, and when
	what might be a syntax violation is encountered but can be elided
	the exception that would have been thrown is saved in a stack of
	{@link warnings() warnings} for possible consideration.

	The class methods that perform the parsing of the character source
	are organized into a hierarchy:

	<ul type="disc">
	<li>#get_parameters
	<li>#add_parameters
		<ul type="disc">
		<li>#get_parameter
			<ul type="disc">
			<li>#get_comment
			<li>#add_values
			<li>#get_value
				<ul type="disc">
				<li>#get_datum
					<ul type="disc">
					<li>#get_quoted_string
					</ul>
				<li>#get_units
				</ul>
			<li>#add_values
			</ul>
		</ul>
	</ul>

	Higher level methods utilize lower level methods to assemble their
	constituent parts. At the top level an Aggregate of all Parameters
	that can be interpreted from the input will be collected by getting
	as many Parameters as possible. A Parameter is produced from the
	input stream by getting any comment, a name String, and a Value. A
	Value includes as many datum and optional units descriptions that
	can be sequentially found in the input stream. A datum is composed
	from primitive syntactic elements, including integer or real number
	representations or character strings which may be quoted (a
	Parameter name may also be a quoted string). Typically applications
	will only use the top method(s). Applications needing finer grained
	control over input stream parsing may, of course, use the lower
	level methods directly, however it is much easier to just get all
	of the Parameters from an input source and then manipulate the
	Parameter-Value object hierarchy.

	Each method that parses the input stream interprets the contents of
	the logical Sliding_String which presents the entire virtual
	contents of the stream from the current Location onwards. Except
	for the top level which does not interpret the character source
	directly, these methods first seek forward from the current
	Location to the beginning of a potentially relevant syntactic
	character sequence. If the sequence is recognized as suitable for
	the item the method is responsible for interpreting, then the
	appropriate end of the sequence is found and the characters it
	contains are parsed to construct the corresponding object class
	variable. If the parsing of the element is successful then the
	logical next_location of the Sliding_String is moved forward to the
	end of the sequence, thus consuming the character sequence. If,
	however, the beginning of a recognizable syntactic sequence is not
	found, or the parsing of a sequence fails, then the method returns
	empty handed, and without having advanced the logical
	Sliding_String, to the invoking method which may invoke a different
	method in an attempt to get a different item or itself discontinue
	its efforts to assemble an item. Thus each parsing method either
	gets its item and advances the current Location in the character
	stream, or does not get its item nor advance the stream; i.e. the
	PVL statements encountered in the character source are sequentially
	parsed at the same time that the input stream is incrementally
	moved forward.

	@see		Sliding_String
	@see		Parameter
	@see		Value

	@author		Bradford Castalia
	@version	1.30
*/
class Parser
:	private idaeim::Strings::Sliding_String
{
public:
/*==============================================================================
	Types
*/
//!	Vector of warning Exception pointers.
typedef	std::vector<Exception*>			Warnings_List;

//!	String_Filters applied to the Sliding_String.
typedef idaeim::Strings::String_Filter	Filter;

/*==============================================================================
	Constants:
*/
//!	Class identification.
static const char* const
	ID;

/*------------------------------------------------------------------------------
	PVL syntax elements
*/
#ifndef PARSE_STRICT
#define PARSE_STRICT	false
#endif

#ifndef PARSE_VERBATIM_STRINGS
#define PARSE_VERBATIM_STRINGS PARSE_STRICT
#endif

#ifndef PARSE_COMMENTED_LINES
#define PARSE_COMMENTED_LINES !PARSE_STRICT
#endif

#ifndef PARSE_STRING_CONTINUATION
#define PARSE_STRING_CONTINUATION true
#endif


/**	Characters reserved by the PVL syntax.

	Some of these characters have special meanings in specific contexts
	as delimiters of PVL items.
*/
static const char* const
	RESERVED_CHARACTERS;

//	Delimiter characters:

//!	Delimits a Parameter name from its Value.
static const char
	PARAMETER_NAME_DELIMITER;

//!	Delimits elements of an ARRAY Value.
static const char
	PARAMETER_VALUE_DELIMITER;

//!	Encloses a TEXT STRING Value.
static const char
	TEXT_DELIMITER;

//!	Encloses a SYMBOL STRING Value.
static const char
	SYMBOL_DELIMITER;

//!	Marks the start of a SET ARRAY Value.
static const char
	SET_START_DELIMITER;

//!	Marks the end of a SET ARRAY Value.
static const char
	SET_END_DELIMITER;

//!	Marks the start of a SEQUENCE ARRAY Value.
static const char
	SEQUENCE_START_DELIMITER;

//!	Marks the end of a SEQUENCE ARRAY Value.
static const char
	SEQUENCE_END_DELIMITER;

//!	Marks the start of a Value units description.
static const char
	UNITS_START_DELIMITER;

//!	Marks the end of a Value units description.
static const char
	UNITS_END_DELIMITER;

//!	Encloses the datum of a Value in radix base notation.
static const char
	NUMBER_BASE_DELIMITER;

//!	Marks the end of a PVL statement.
static const char
	STATEMENT_END_DELIMITER;

//!	Indicates that the statement continues in the next record.
static const char
	STATEMENT_CONTINUATION_DELIMITER;

//!	Indicates that the quoted string continues unbroken in the next record.
static const char
	STRING_CONTINUATION_DELIMITER;

//!	Begins a comment that extends to the end of the line.
static const char
	COMMENT_LINE_DELIMITER;
/*	The comment line delimiter must not be the same as 
	the first character of COMMENT_START_DELIMITERS.
*/

//	Delimiter strings:

//!	Character sequence that separates PVL statement lines.
static const char* const
	LINE_BREAK;

//!	Set of "whitespace" characters between PVL tokens.
static const char* const
	WHITESPACE;

//!	Marks the start of a comment string.
static const char* const
	COMMENT_START_DELIMITERS;

//!	Marks the end of a comment string.
static const char* const
	COMMENT_END_DELIMITERS;

//!	Set of characters that suggests a DATE_TIME type of STRING Value.
static const char* const
	DATE_TIME_DELIMITERS;

//!	Encloses a verbatim (uninterpreted) string.
static const char* const
	VERBATIM_STRING_DELIMITERS;

/**	The default name of the Aggregate to contain all Parameters when
	get_parameters() finds more than one Parameter or the
	source is an istream.
*/
static const char* const
	CONTAINER_NAME;

/*==============================================================================
	Constructors
*/
/**	Creates a Parser with no source of PVL statements.
*/
Parser ()
	:	Sliding_String		(),
		Strict				(PARSE_STRICT),
		Verbatim_Strings	(PARSE_VERBATIM_STRINGS),
		Commented_Lines		(PARSE_COMMENTED_LINES),
		String_Continuation	(PARSE_STRING_CONTINUATION),
		Warnings			(new Warnings_List ())
{}

/*------------------------------------------------------------------------------
	File input
*/
/**	Creates a Parser using an istream as the source of PVL statements,
	and sets a limit on the amount to read.

	@param	reader	The istream to use as the source of characters.
	@param	limit	The maximum amount to read.
	@see	Sliding_String::Sliding_String(const istream&, Location)
*/
Parser (std::istream& reader, Location limit = Sliding_String::NO_LIMIT);

/*------------------------------------------------------------------------------
	String input
*/
/**	Creates a Parser using a String as the source of PVL statements.

	@param	string_source	The string to use as the source of characters.
	@see	Sliding_String::Sliding_String(const String&)
*/
Parser (const std::string& string_source);

/*------------------------------------------------------------------------------
	Copy
*/
Parser
	(
	const Parser&			parser
	)
	:	Sliding_String		(parser),
		Strict				(parser.Strict),
		Verbatim_Strings	(parser.Verbatim_Strings),
		Commented_Lines		(parser.Commented_Lines),
		String_Continuation	(parser.String_Continuation),
		VMS_records_filter	(parser.VMS_records_filter),
		Line_count_filter	(parser.Line_count_filter),
		Warnings			(new Warnings_List ())
{}

/*------------------------------------------------------------------------------
	Destructor
*/
virtual ~Parser ();

/*------------------------------------------------------------------------------
	Assignment
*/
Parser& operator= (const Parser& parser);

/*==============================================================================
	Accessors:
*/
/*------------------------------------------------------------------------------
	Modes:
*/
/**	Enables or disables strict PVL syntax rules in the Parser.

	Normally the Parser is tolerant.

	@param	strict	true if strict rules are applied; false otherwise.
	@return	This Parser.
*/
Parser& strict (bool strict)
	{Strict = strict; return *this;}

/**	Tests if the Parser will enforce strict PVL syntax rules.

	@return	true if the Parser will enforce strict syntax rules;
		false otherwise.
*/
bool strict ()
	{return Strict;}

/**	Enable or disable verbatim quoted strings.

	With format control (verbatim_strings disabled) multi-line quoted
	strings in PVL statements have white space surrounding the line
	breaks compressed to a single space character - except when the
	last non-white space character on the line is a dash ("-"), in
	which case no space is included. This is because output formatting
	is expected to be controlled by embedded format characters which
	are processed by the Write method:

	<dl compact>
	<dt>\\n		<dd>line break.
	<dt>\\t		<dd>horizontal tab.
	<dt>\\f		<dd>form feed (page break).
	<dt>\\		<dd>backslash character.
	<dt>\\v		<dd>verbatim (no formatting) till the next \\v.
	</dl>

	Without format control (verbatim_strings enabled) all
	string values are taken as-is.

	@param	verbatim	true if quoted strings in PVL statements are to
		be taken verbatim, without format control.
	@return	This Parser.
	@see	get_parameter()
	@see	get_quoted_string()
	@see	get_comment()
	@see	get_datum()
	@see	get_units()
*/
Parser& verbatim_strings (bool verbatim)
	{Verbatim_Strings = verbatim; return *this;}

/**	Tests if the Parser will handle quoted strings verbatim.

	@return	true if quoted strings are taken verbatim; false otherwise.
	@see	Verbatim_Strings(boolean)
*/
bool verbatim_strings ()
	{return Verbatim_Strings;}

/**	Enable or disable recognition of single commented lines.

	A commented line begins with the #COMMENT_LINE_DELIMITER character
	and extends to the end of the current line (marked by a #LINE_BREAK
	character). Commented lines are never recognized in {@link strict(bool)
	strict mode}.

	<b>N.B.</b>: Commented lines are not recognized in the PVL syntax
	specification. Because of their common use in configuration files,
	this special extension is provided to accommodate such
	applications. Be default commented lines are not recognized.

	@param	allow	true if crosshatch comments are to be recognized.
	@return	This Parser.
	@see	get_quoted_string()
*/
Parser& commented_lines (bool allow)
	{Commented_Lines = allow; return *this;}

/**	Tests if commented lines will be recognized.

	Regardless of this test, commented lines are never recognized
	in {@link strict(bool) strict mode}.

	@return	true if commented lines will be recognized; false otherwise.
	@see	commented_lines(bool)
*/
bool commented_lines ()
	{return Commented_Lines;}

/**	Enable or disable recognition of the string continuation delimiter.

	Normally, unless {@link verbatim_strings(bool) verbatim strings} is
	enabled, when a quoted string continues across multiple lines the end
	of line sequence and all surrounding white space are collapsed to a
	single space character (' '). When string continuation is enabled
	(the default) and {@link verbatim_strings(bool) verbatim strings} is
	disabled (the default) the occurrance in a quoted string of a {@link
	#STRING_CONTINUATION_DELIMITER} as the last character before a new
	line sequence causes the string continuation delimiter and all
	characters up to the next non-whitespace character to be removed from
	the string; i.e. the string continues on the next line after any
	whitespace. However, if the string continuation delimiter is
	preceeded by a space character, or the delimiter is the first
	character of the string, the delimiter is retained but the normal
	collapsing of whitespace around the line wrap to a single space is
	still done.

	By default string continuation is enabled.

	@param	enable	true if string continuation is to be enabled;
		false otherwise.
	@return	This Parser.
	@see	get_quoted_string()
	@see	verbatim_strings(bool)
*/
Parser& string_continuation (bool enable)
	{String_Continuation = enable; return *this;}

/**	Tests if the string continuation delimiter will be recognized.

	@return	true if the continuation delimiter will be recognized; false
		otherwise.
	@see	string_continuation(bool)
*/
bool string_continuation ()
	{return String_Continuation;}

/*..............................................................................
	Warnings
*/
/**	Gets the current vector of warning conditions.

	When conditions are encountered that are unusual enough to warrant
	attention, but not an error condition that would prevent successful
	processing which would cause an exception to be thrown, a warning
	condition is registered. The warning is in the form of a pointer to
	an Exception that was not thrown. Each new warning is appended to
	the vector of Warnings maintained for the Parser.

	<b>N.B.</b>: The warnings are pointers to Exception objects created
	with new. <b>DO NOT delete the #Warnings_List</b>; use
	clear_warnings() instead.

	@return	A pointer to the current list of warning pointers.
	@see	clear_warnings()
*/
Warnings_List *warnings ()
	{return Warnings;}

/**	Clears all warnings.

	Each warning pointer in the list first has its Exception object
	deleted, then all of the now invalid warning pointers are deleted.
	This leaves a clean, empty warnings list.

	@see	warnings()
*/
void clear_warnings ();

/*==============================================================================
	PVL parser
*/
/*------------------------------------------------------------------------------
	Parameter
*/
/**	Gets as many Parameters as possible.

	When the source of PVL statements is a Reader, then an aggregate
	Parameter named #CONTAINER_NAME will be provided to contain all the
	Parameters found (zero or more). If this Parser was created with a
	string as the source of PVL statements, then a container Parameter
	will only be provided if more than one Parameter is found;
	otherwise the single Parameter found is returned. If nothing is
	found then NULL is returned.

	@return	A Parameter containing everything found from the input
		source, or NULL if nothing was found.
	@throws	Invalid_Syntax	If an unrecoverable problem occurred while
		parsing the input source.
	@see	add_parameters(Aggregate&)
*/
Parameter* get_parameters ();

/**	Adds to an Aggregate all Parameters found from the input source.

	While the source of PVL statements is not empty and #get_parameter
	can assemble a Parameter from the source, each new Parameter is
	added to the specified Parameter's aggregate list. <b>N.B.</b>:
	Parameters having the END Parameter::Type are not added, instead
	they stop the addition of Parameters to the current aggregate list;
	additions will continue with the parent of the current aggregate on
	returning from recursive method invocations. However, if an END
	Parameter is encountered no more Parameters will be added
	regardless of the recursion level.

	@param	aggregate	The aggregate Parameter to which to add new
		Parameters from the input source.
	@return	The original aggregate Parameter.
	@throws	Aggregate_Closure_Mismatch
		In {@link strict(bool) strict mode}, when an END Aggregate
		Parameter does not match the specific Parameter::Type containing
		the aggregate list. This is only added to the {@link warnings()
		warnings list} when {@link strict(bool) strict mode} is not
		enabled.
	@see	get_parameter()
*/
Aggregate& add_parameters (Aggregate& aggregate);

/**	Gets a Parameter from the source of PVL statements.

	First, #get_comment is used to to collect any and all comment
	sequences preceeding the parameter proper as the Parameter's
	comment string.

	The next sequence of non #WHITESPACE characters is taken to be the
	Parameter's name. If the sequence is quoted (i.e. starts with a
	#TEXT_DELIMITER or #SYMBOL_DELIMITER), then the name is all
	characters within the quotes. Otherwise it is all characters up to
	but not including the next #PARAMETER_NAME_DELIMITER,
	#STATEMENT_END_DELIMITER, #WHITESPACE character, or comment sequence.
	The name is checked for any reserved characters and a {@link
	warnings() warning} will be registered (exception thrown in {@link
	strict(bool) strict mode}) if one is found. If {@link
	verbatim_strings(bool) verbatim strings} is not enabled, then all
	character escape sequences in the name are replaced with their
	corresponding special characters.

	If the name is associated with a #special_type, then the Parameter
	is given that Parameter::Type; otherwise it is tentatively set as a
	TOKEN.

	If, after any #WHITESPACE or comments following the Parameter name, a
	#PARAMETER_NAME_DELIMITER is found, then the #get_value method is
	used to obtain the expected Parameter Value. If the Parameter had
	been given the TOKEN Parameter::Type, then the Type is upgraded to
	ASSIGNMENT. If, however, the Parameter had been given an AGGREGATE
	Type as a result of its special name, and the Value obtained is a
	STRING type, then the Parameter's name is changed to the Value
	String; if, in this case, the Value found is not a STRING type a
	{@link warnings() warning} will be registered (exception thrown in
	{@link strict(bool) strict mode}).

	Having assembled a valid Parameter, the next_location in the input
	stream is moved forward past any whitespace or
	#STATEMENT_END_DELIMITER.

	@return	The next Parameter assembled from the input stream, or NULL
		if no Parameter can be assembled because the input stream is
		empty.
	@throws	Invalid_Syntax
		Strict mode is enabled and the Parameter name was quoted.
	@throws Reserved_Character
		The Parameter name contained a bad character and {@link
		strict(bool) strict mode} is enabled.
	@throws Invalid_Aggregate_Value
		The Value obtained for an Aggregate Parameter is not a STRING
		type.
	@see	get_comment()
	@see	Parameter::comment()
	@see	Parameter::name()
	@see	reserved_character(const std::string&)
	@see	translate_from_escape_sequences(String)
	@see	special_type(String)
	@see	get_value()
*/
Parameter* get_parameter (bool Assignment_only = false);

/**	Gets the next sequence of comments from the source of PVL statements
	as a single comment string.

	After skipping over any whitespace, the next characters must start
	a comment sequence or nothing (NULL) is returned.

	A PVL comment uses C-style conventions: It starts after the
	#COMMENT_START_DELIMITERS and ends before the
	#COMMENT_END_DELIMITERS. A comment without the closing
	#COMMENT_END_DELIMITERS will result in a Missing_Comment_End
	exception in {@link strict(bool) strict mode}; otherwise a {@link
	warnings() warning} is registered and the next line break,
	#STATEMENT_END_DELIMITER, or the end of the input stream is taken as
	the end of the comment. <b>N.B.</b>: Though an effort is made to
	recover from encountering an unending comment, this will only be
	effective when no other normally closed comment occurs in the input
	stream (if a normally closed comment does occur after an unclosed
	comment, the latter will be taken as the end of the former), and in
	this case the input stream will have been read into memory until it
	is empty.

	Sequential comments, with nothing but white space intervening, are
	accumulated with a single new-line ('\n') chararacter separating them
	in the resulting string that is returned. In {@link strict(bool)
	strict mode} comments that wrap across line breaks cause an
	exception. When {@link verbatim_strings(bool) verbatim strings} are
	not enabled whitespace is trimmed from the end of each comment (but
	not the beginning), and escape sequences are translated into their
	corresponding special characters.

	If any comments are found the next_location of the input stream is
	moved to the position immediately following the last comment.

	@return	A string containing the comment found, or NULL if no
		comment occurs before the next PVL item or the end of input.
	@throws	Invalid_Syntax
		Strict mode is enabled and a comment continues on more than one
		line.
	@throws Missing_Comment_End
		A comment does not have #COMMENT_END_DELIMITERS and {@link
		strict(bool) strict mode} is enabled. If strict is not enabled
		and a line or statement delimiter can not be found, then the
		exception is thrown.
	@see	verbatim_strings(boolean)
*/
std::string get_comment ();

/*------------------------------------------------------------------------------
	Value
*/
/**	Adds to an Array the next Values found from the input source.

	While the source of PVL statements is not empty and #get_value can
	assemble a Value from the source, each new Value is added to the
	specified Array's element list.

	@param	array	The Array to which to add new Values.
	@return	The original Array.
*/
Value& add_values (Array& array);

/**	Gets a Value from the source of PVL statements.

	If the next character, after skipping any whitespace and comments,
	is a #SET_START_DELIMITER or #SEQUENCE_START_DELIMITER the Value
	being assembled is typed as a SET or SEQUENCE, respectively, and
	the next_location in the input stream is moved over the character;
	otherwise the Value is tentatively typed as a SET, and the
	next_location is not moved.

	Now a cycle is entered to obtain as many sequential datum values as
	are available. The first step is to skip any whitespace and
	comments and test the character that is found. If the character is
	a #SET_END_DELIMITER or #SEQUENCE_END_DELIMITER, then the
	next_location in the input stream is moved over the character and
	#get_units is used to set the Array Value's units description
	before ending the datum cycle. If the character is a
	#STATEMENT_END_DELIMITER the datum cycle ends. A reserved
	#PARAMETER_NAME_DELIMITER, #PARAMETER_VALUE_DELIMITER,
	#UNITS_START_DELIMITER, #UNITS_END_DELIMITER, or
	#NUMBER_BASE_DELIMITER character here is an Invalid_Syntax
	exception. For any other character the next_location is moved
	forward to its position as a possible datum. 

	When the character at this location is a #SET_START_DELIMITER or
	#SEQUENCE_START_DELIMITER this method is called recursively to get
	a subarray as the datum. Otherwise the #get_datum method is used to
	get a basic value followed by the #get_units method to get any
	units description for the new datum. If  no datum is obtained the
	datum cycle ends, otherwise the new datum is added to the Vector of
	Value's being accumulated for this new Value.

	After skipping any whitespace and comments the next character is
	checked. A reserved #PARAMETER_NAME_DELIMITER,
	#UNITS_START_DELIMITER, #UNITS_END_DELIMITER, or
	#NUMBER_BASE_DELIMITER character here is an Invalid_Syntax exception.
	A #SET_START_DELIMITER or #SEQUENCE_START_DELIMITER character will
	also generate an Invalid_Syntax exception, but if {@link strict(bool)
	strict} mode is not enabled this will just be registered as a {@link
	warnings() warning} and the next_location will be moved to the
	character's position before continuing the datum cycle from the
	beginning. If the character is a #PARAMETER_VALUE_DELIMITER the
	next_location is moved over the character and the datum cycle returns
	to the beginning. For a #SET_END_DELIMITER} or
	#SEQUENCE_END_DELIMITER}, if the character does not correspond to the
	delimiter that began the array an Array_Closure_Mismatch a {@link
	warnings() warning} is registered (this exception is thrown in {@link
	strict(bool) strict mode}). Then the next_location is moved over the
	character and the #get_units method is used to set this Value's units
	description before ending the datum cycle. Any other character also
	causes the datum cycle to end.

	After the datum cycle has collected as many Values as possible, if
	this new Value was not begun with a #SET_START_DELIMITER or
	#SEQUENCE_START_DELIMITER and the accumulated Values Vector
	containins less than two Values then the initial tentative SET type
	does not apply. In this case an empty accumlated Values Vector
	results in this new Value being an UNNOWN type (i.e. it is empty).
	When only one Value was collected it is the new Value that is
	returned. When two or more Values were collected the Vector
	containing them is set as the data of this Array Value.

	@return	The next Value assembled from the input stream, or NULL if
		no Value can be assembled because the input stream is empty.
	@throws	Reserved_Character
		A misplaced reserved character was found.
	@throws Array_Closure_Mismatch
		The delimiter character ending an array of Values does not
		correspond to the one that began the array.
	@see	Value::type(int)
	@see	Value::units(string)
	@see	get_datum(Value)
*/
Value* get_value ();

/**	Gets a datum from the source of PVL statements.

	After skipping any whitespace or comments, the next character is
	checked to determine the type of datum to parse. For a
	#STATEMENT_END_DELIMITER nothing happens. A reserved
	#PARAMETER_NAME_DELIMITER, #PARAMETER_VALUE_DELIMITER},
	#SET_START_DELIMITER, #SET_END_DELIMITER,
	#SEQUENCE_START_DELIMITER}, #SEQUENCE_END_DELIMITER,
	#UNITS_START_DELIMITER, #UNITS_END_DELIMITER, or
	#NUMBER_BASE_DELIMITER character here is an Invalid_Syntax
	exception. For a #TEXT_DELIMITER or #SYMBOL_DELIMITER the
	get_quoted_string() method is used to set the datum of the Value and
	its type is set to TEXT or SYMBOL respectively.

	For any ordinary character the substring up to, but not including,
	the next #WHITESPACE, #PARAMETER_VALUE_DELIMITER,
	#STATEMENT_END_DELIMITER, #COMMENT_START_DELIMITERS, any of the
	SET/SEQUENCE/UNITS START/END delimiters, or the end of the input
	stream is used for parsing a datum. If {@link verbatim_strings(bool)
	verbatim strings} is not enabled, then all escape sequences in the
	substring are converted to their special character equivalents.

	The datum substring is first assumed to represent a number. If the
	substring contains a #NUMBER_BASE_DELIMITER ('\#') the number is
	presumed to be in radix base notation:

	[<i>sign</i>]<i>base</i><b>#</b><i>value</i><b>#</b>

	In this case the initial base integer is obtained and becomes the
	Value's numeric base, and the value number is obtained using the base
	for parsing the representation and becomes the Value's datum. The
	sign is applied to the value number. The Value becomes type INTEGER.
	Without the #NUMBER_BASE_DELIMITER the datum substring is taken to be
	in decimal notation. In addition, unless DECIMAL_NOTATION_ONLY was
	defined at compile time, a datum substring that begins with "0x" or
	"0X" is taken to be in hexadecimal notation.

	If this number conversion fails, then floating point representation
	parsing is tried on the datum substring to produce a type REAL Value.
	If this succeeds the number of decimal precision digits of the Value
	is determined by finding the location of the decimal point character
	('.') in the datum string and the number of decimal digits that
	follow it. For scientific notation - in which an 'e' or 'E' exponent
	notation is present - the decimal point is expected to follow one
	non-zero digit. <b>N.B.</b>: For scientific notation the decimal
	point is assumed to be the second character in the datum string
	regardless of its actual location, which ensures that at least the
	full numeric precision of the value will be represented. The format
	flags are set to fixed point or scientific notation corresponding to
	the datum substring representation; the showpoint format flag is
	always set.

	Numeric conversions are checked for validity. For example, overflow
	conditions result in an invalid condition.

	If parsing the datum substring as a number fails, then the Value is a
	STRING and its datum is the substring. If the substring contains one
	of the #DATE_TIME_DELIMITERS the Value is given the DATE_TIME type.
	Otherwise it is the IDENTIFIER type. The datum substring is also
	checked for any reserved characters with a {@link warnings() warning}
	being registered (exception thrown in {@link strict(bool) strict
	mode}) if one is found.

	Once a datum has been given to the Value the next_location in the
	input stream is moved to the position immediately following the datum
	substring.

	@return	The_Value, or NULL if the input stream is empty.
	@throws	Reserved_Character
		A STRING Value contains a reserved character.
	@throws Invalid_Value
		A number conversion was found to be invalid.
	@see	reserved_character(const std::string&)
	@see	translate_from_escape_sequences(String)
	@see	Value::base(const int)
	@see	Value::digits(const int)
	@see	Real::format_flags(std::ios::fmtflags)
*/
Value* get_datum ();

/**	Gets a units description string from the source of PVL statements.

	After skipping over any #WHITESPACE, the next character must start
	a units description sequence or nothing (NULL) is returned. A units
	description sequence starts after a #UNITS_START_DELIMITER and ends
	before a #UNITS_END_DELIMITER. A units description sequence without
	the closing #UNITS_END_DELIMITER will result in a Missing_Units_End
	exception in {@link strict(bool) strict mode}; otherwise a {@link
	warnings() warning} is registered and the next
	#PARAMETER_VALUE_DELIMITER, any of the SET/SEQUENCE/UNITS START/END
	delimiters, #STATEMENT_END_DELIMITER, or the end of the input stream
	is taken as the end of the units description. <b>N.B.</b>: Though an
	effort is made to recover from encountering an unending units
	description, this will only be effective when no other normally
	closed units descripiton occurs in the input stream (if a normally
	closed units descripiton does occur after an unclosed units
	description, the latter will be taken as the end of the former), and
	in this case the input stream will have been read into memory until
	it is empty.

	Sequential comments, with nothing but white space intervening, are
	accumulated with a single new-line ('\\n') chararacter separating
	them in the resulting string that is returned. In {@link strict(bool)
	strict mode} comments that wrap across line breaks cause an
	exception. When {@link verbatim_strings(bool) verbatim strings} are
	not enabled whitespace is trimmed from the end of each comment (but
	not the beginning), and escape sequences are translated into their
	corresponding special characters.

	If a units descripiton is found the next_location of the input
	stream is moved to the position immediately following it. The units
	description is trimmed of leading and trailing whitespace. If
	{@link verbatim_strings(bool) verbatim strings} is not enabled, then
	all comment sequences are removed from the units description string,
	all whitespace sequences are collapsed to a single space (' ')
	character, and escape sequences are substituted for their
	corresponding special characters.

	@return	A units description string, or NULL if no units description
		occurs before the next PVL item or the end of input.
	@throws	Missing_Units_End
		A units description does not have a #UNITS_END_DELIMITER.
	@see	verbatim_strings(boolean)
	@see	translate_from_escape_sequences(String)
*/
std::string get_units ();

/*==============================================================================
	Utility functions:
*/
/**	Gets a quoted string from the source of PVL statements.

	The next non-whitespace character is taken to be the "quote"
	character. The characters following the first quote character up to
	but not including the next, non-escaped (not preceeded by a
	backslash, '\\') quote character are the quoted string. If the
	closing quote character can not be found, then a Missing_Quote_End
	{@link warnings() warning} will be registered (the exception will be
	thrown in {@link strict(bool) strict mode}) and the quoted string will
	end at the end of the input stream. <b>N.B.</b>: The lack of a
	closing quote character will cause the entire input stream to be read
	into memory until it is emtpy. The next_location is moved to the
	position immediately following the last quote character.

	If {@link verbatim_strings(bool) verbatim strings} is not enabled
	then line break sequences (one or more sequential line breaks) and
	any surrounding whitespace (whitespace ending the last line and
	beginning the next line) are replaced with a single space (' ')
	character. If, however, {@link string_continuation(bool) string
	continuation} is enabled and the last non-whitespace character before
	the line break squence is a #STRING_CONTINUATION_DELIMITER that is
	not preceded by a space character (and the delimiter is not the first
	character of the string) then no space remains (i.e. the string
	ending with the last non-whitespace character on the last line is
	continued with the first non-whitspace character on the next line).
	In addition, escape sequences are translated to their corresponding
	special characters. Sequences of characters bracketed by
	#VERBATIM_STRING_DELIMITERS are taken verbatim; they are subject to
	neither end of line treatment nor escape sequence translation.

	@return	A string, or NULL if no non-whitespace character occurs
		before the end of input.
	@throws	Missing_Quote_End
		A closing quote was not found in the input stream.
	@see	verbatim_strings(boolean)
	@see	String::escape_to_special()
*/
std::string get_quoted_string ();

/**	Gets the next Location in the PVL source stream following any
	sequence of whitespace and/or comments.

	A #STATEMENT_CONTINUATION_DELIMITER and any {@link
	commented_lines(bool) commented lines} (if enabled) are included in
	the whitespace category. <b>N.B.</b>: As with the #get_comment
	method, a comment without a closing sequence is taken to end and the
	next line break, #STATEMENT_END_DELIMITER}, or the end of the input
	stream; but this condition will cause the input stream to be read to
	its limit into memory.

	@param	location	The starting Location from which to skip over
		whitespace and comments.
	@return	The Location of the next character after any whitespace or
		comments.
	@throws	Missing_Comment_End
		A comment does not have #COMMENT_END_DELIMITERS and {@link
		strict(bool) strict mode} is enabled.
*/
Location skip_whitespace_and_comments (Location location);

/*==============================================================================
	Protected Functions.
*/
protected:

/**	Ingests from the input source all Parameters found, putting them in
	the aggregate.

	This method is invoked recursively on each aggregate Parameter
	ingested.

	@param	aggregate	The aggregate Parameter to receive new
		Parameters from the input source.
	@return	The Parameter::Type last ingested; this will be END when
		the last Parameter, or an error condition, was encountered.
	@throws	Aggregate_Closure_Mismatch
		In {@link strict(bool) strict mode}, when an END Aggregate
		Parameter does not match the specific Parameter::Type containing
		the aggregate list. This is only a {@link warnings() warning}
		when strict mode is not enabled.
	@see	get_parameter()
*/
Parameter::Type ingest_parameters (Aggregate& aggregate);

Location skip_commented_line (Location location);

void WARNING (Exception* exception);

/**	Generates a string describing the line position of the location.

	The string is formatted as:

	"<b>  at</b>|<b>before character </b><i>C</i> of line <i>L</i>\\n</b>"

	where the choice of "at" or "before" is conditional on the before
	argument, and <i>L</i> is the line number and <i>C</i> is the
	character offset in that line corresponding to the location in the
	input stream.

	If the Parser is not using a Line_Count_Filter, or the location is
	invalid, then an empty string is returned.

	@param	before	true if the description starts with "before";
		otherwise the string starts with "at".
	@param	location	The Location in the input stream for which
		to find a line position.
	@return	A string describing the line position, or the empty string.
*/
#define	BEFORE_POSITION	true
#define	AT_POSITION		false

std::string
line_position
	(
	bool		before,
	Location	location
	) const;

/*==============================================================================
	Private Data
*/
private:

//!	Modes:
bool
	Strict,					//	Enforce strict PVL syntax rules.
	Verbatim_Strings,		//	Verbatim strings.
	Commented_Lines,		//	Allow crosshatched-to-EOL comments.
	String_Continuation;	//	Recognize the string continuation delimiter.

//	Filters:
//	Removes VMS binary record structures.
idaeim::Strings::VMS_Records_Filter
	*VMS_records_filter;
//	Counts lines in the input stream.
idaeim::Strings::Line_Count_Filter
	*Line_count_filter;

//!	Warnings; when throwing an exception is inappropriate.
Warnings_List
	*Warnings;

};		//	End of Parser class.

/*==============================================================================
	Helper Functions:
*/
/**	Gets the Parameter::Type code corresponding to the specified name.

	The special Parameter names and their Type codes are:

	Begin_Object - OBJECT<br>
	BeginObject - OBJECT<br>
	Object - OBJECT<br>
	End_Object - END_OBJECT<br>
	EndObject - END_OBJECT<br>
	Begin_Group - GROUP<br>
	BeginGroup - GROUP<br>
	Group - GROUP<br>
	End_Group - END_GROUP<br>
	EndGroup - END_GROUP<br>
	End - END<br>

	The names are not case senstive.

	@param	name	The name string to check.
	@return	The Parameter::Type code associated with the name; or 0 if
		the name is not a special Parameter name.
*/
Parameter::Type special_type (const std::string& name);

/**	Translates escape sequences in a string to their corresponding
	special characters.

	The String::escape_to_special() method is used to translate escape
	sequences to special characters. However the occurance of
	Parser::VERBATIM_STRING_DELIMITERS starts a sequence of characters
	that are taken verbatim (they are not translated) up to the next
	Parser::VERBATIM_STRING_DELIMITERS or the end of the string. The
	Parser::VERBATIM_STRING_DELIMITERS are dropped.

	@param	a_string	The string to be translated.
	@return	The translated string.
	@see	String::escape_to_special(Index, Index)
*/
std::string& translate_from_escape_sequences (std::string& a_string);

/**	Translates special characters in a string to their corresponding
	escape sequences.

	In addition to the special characters translated by the
	String::special_to_escape() method, the Parser::TEXT_DELIMITER and
	Parser::SYMBOL_DELIMITER characters are also escaped. The occurance
	of Parser::VERBATIM_STRING_DELIMITERS starts a sequence of characters
	that are taken verbatim (they are not translated) up to and including
	the next Parser::VERBATIM_STRING_DELIMITERS or the end of the string.

	@param	a_string	The string to be translated in place.
	@return	The translated string reference.
	@see	String::special_to_escape()
*/
std::string& translate_to_escape_sequences (std::string& a_string);

/**	Checks a string for any character reserved by the PVL syntax.

	A reserved character is one of the Parser::RESERVED_CHARACTERS or a
	non-printable character.

	@param	a_string	The string to check.
	@return	The index of the first reserved character found in the string,
		or string::npos if no reserved characters were found.
*/
std::string::size_type reserved_character (const std::string& a_string);

}		//	namespace PVL
}		//	namespace idaeim
#endif	//	idaeim_PVL_Parser_hh
