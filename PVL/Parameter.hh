/*	Parameter

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

CVS ID: Parameter.hh,v 1.56 2014/04/16 02:49:06 castalia Exp
*******************************************************************************/

#ifndef idaeim_PVL_Parameter_hh
#define idaeim_PVL_Parameter_hh

#include	"Value.hh"
#include	"Vectal.hh"

#include	<string>
#include	<iostream>	//	Needed for default cin and cout arguments.
#include	<iterator>


namespace idaeim
{
/**	%Parameter %Value Language.
*/
namespace PVL
{
//	Forward references.
class Assignment;
class Aggregate;
class Value;
class Integer;

/*=*****************************************************************************
	Parameter
*/
/**	A <i>Parameter</i> is a general-purpose, abstract, named parameter
	entity.

	Every Parameter has a name, which is a string of zero or more
	characters. A Parameter may also have a comment string associated
	with it that contains any descriptive text.

	There are two types of Parameter implementations: an
	<i>Assignment</i> has a <i>Value</i>; an <i>Aggregate</i> is a list
	of Parameters. Because Parameters are typically grouped into
	Aggregate hierarchical structures, each Parameter contains a pointer
	to its parent Aggregate; Parameters that are not contained within an
	Aggregate have a NULL parent. Parameters in an Aggregate hierarchy
	may be referred to by their pathname - as in a filename directory
	hierachy - in either an absolute or relative manner. <b>N.B.</b>: The
	name of a Parameter need not be unique within an Aggregate list; as a
	result pathnames need not be unique either.

	A <i>Parser</i> is used for interpreting the %Parameter %Value
	Language syntax text into Parameter objects. A <i>Lister</i> is
	used for generating %Parameter %Value Language syntax from
	Parameter objects. There are also syntax modes that may be set for
	each Parameter to control the operation of the Lister. These modes
	may be specific to the Parameter object or inherited from a parent
	Parameter or a static default setting.

	@see		Value
	@see		Parser
	@see		Lister

	@author		Bradford Castalia
	@version	1.56 
*/
class Parameter
{
friend class Assignment;
friend class Aggregate;

public:
/*==============================================================================
	Types:
*/
//!	Default Value type for an Assignment.
typedef Integer							Default_Value;

/*==============================================================================
	Constants:
*/
//!	Class identification name with source code version and date.
static const char* const
	ID;

/**	Type specifications.

	There are two specific implementations of Parameter: Assignment and
	Aggregate. However, a Parameter can also be distinguished in the
	way it is represented with the PVL syntax. Since this distinction is
	not functional - in fact is a characteristic that can be readily
	changed - additional subclasses are not used. Instead, a Type code
	is provided and used as a "subtype" characteristic, in addition to
	offering a convenient means of identifying the general type (class)
	of a Parameter:

<dl>
<dt>ASSIGNMENT
	<dd>The fundamental Parameter containing a Value object. If an
	Assignment does not have an effective Value, it is a TOKEN. A token
	has a #Default_Value which is usually ignored unless the Parameter
	is elevated to full Assignment status.
<dt>AGGREGATE
	<dd>A Parameter that contains a list of zero or more Parameters. An
	Aggregate may have a GROUP or OBJECT Type specification; a
	distinction without a functional difference (but which may be
	useful to the application).
<dt>END
	<dd>A special ephemeral token parameter that marks the end of an
	Aggregate Parameter list. The Type specifications END_GROUP and
	END_OBJECT are expected to be paired with a preceeding GROUP or
	OBJECT Aggregate, with the general END token marking the last
	Parameter in an input container (Parser::CONTAINER_NAME) Aggregate.
	These Type specifications are used by the Parser class during parsing
	of PVL syntax into Aggregate objects; they should not be needed
	otherwise.
</dl>

	Type specification codes are bit flags. These are organized such
	that the general Type bit is present for all specific members: A
	TOKEN contains the ASSIGNMENT bit, both GROUP and OBJECT contain
	the AGGREGATE bit, and both END_GROUP and END_OBJECT contain the
	END bit.

	<b>Note</b>: Parameter Type codes are guaranteed to be distinct
	from Value Type codes: they occupy completely separate bit fields
	and can not be accidently confused.
*/
enum Type
	{
	ASSIGNMENT		= (Value::ARRAY << 1),	// Distinct from Value::Type
		TOKEN,

	AGGREGATE		= (ASSIGNMENT << 1),
		GROUP,
		OBJECT,

	//	These type codes are only used by the Parser.
	END				= (AGGREGATE << 1),
		END_GROUP,
		END_OBJECT
	};

//!	Default pathname delimiter.
static const char
	DEFAULT_PATHNAME_DELIMITER;

//!	Default case sensitivity.
static const bool
	DEFAULT_CASE_SENSITIVITY;

/*==============================================================================
	Constructors
*/
/**	Constructs a Parameter base class.

	The Parameter will have no parent and no comment.

	@param	name	The name of the Parameter.
*/
explicit Parameter (const std::string& name = "");

/**	Parameter copy constructor.

	The parent of the copied Parameter will not be inherited; i.e.
	the new Paramter will have no parent.

	@param	parameter	The Parameter to be copied.
*/
Parameter (const Parameter& parameter);

/**	Creates a Parameter clone of the Parameter.

	This method is required by the Aggregate Vectal.
*/
virtual Parameter* clone () const = 0;

//!	Virtual destructor.
virtual ~Parameter ()
	{}

/*==============================================================================
	Accessors
*/
/*------------------------------------------------------------------------------
	Name and Parent
*/
protected:

//!	The Parameter parent Aggregate (NULL if none).
Aggregate			*Parent;

//!	The Parameter name.
std::string			Name;

//!	Delimter character for pathname components.
static char			Path_Delimiter;

//!	Default case sensitivity.
static bool			Case_Sensitive;

public:

/**	Gets the name of the Parameter.

	@return	The name of the Parameter.
*/
std::string name () const
	{return Name;}

/**	Sets the Parameter's name.

	@param	name	The name string.
	@return	This Parameter.
*/
Parameter& name (const std::string& name)
	{Name = name; return *this;}

/**	Gets the Parameter's parent.

	<b>Note</b>: The parent of a Parameter should only be set by using
	a containing Aggregate's Vectal interface methods (for convenience,
	see the Aggregate's {@link Aggregate::add(Parameter*) add}
	and {@link Aggregate::remove(Parameter*) remove} methods).

	@return	A pointer to the parent Aggregate. This will be NULL if this
		Parameter has no parent.
*/
Aggregate* parent () const
	{return Parent;}

/**	Tests if the Parameter has children Parameters.

	@return	true if the Parameter {@link is_Aggregate() is an Aggregate}
		containing one or more Parameters; false otherwise.
*/
bool has_children () const;

/**	Gets the root Aggregate enclosing this Parameter.

	@return	A pointer to the root parent Aggregate; i.e. the base of
		the parent hierarchy for this Parameter. This will be NULL if
		this Parameter has no parent.
*/
Aggregate* root () const;

/**	Gets the fully qualified (absolute) pathname of the Parameter,
	optionally from some root Parameter.

	A Parameter pathname is the name of the Parameter preceeded by the
	current #path_delimiter character. If this Parameter has a #parent
	that is not the root Parameter then the delimiter is preceeded by the
	parent's pathname. This recursion produces a pathname that starts
	with the #path_delimiter character and includes the names of each
	Parameter from, but not including, the root up to, and including,
	this Parameter, with each name separated by the #path_delimiter
	character.

	If no root Parameter is specified (or the root is NULL), the first
	segment of the pathname will be the name of the Parameter with no
	parent; this is an "absolute" pathname. <b>N.B.</b>: As a special
	case when no root is specified the Aggregate named
	Parser::CONTAINER_NAME (see Parser::get_parameters()) will qualify as
	the root. If a (non-NULL) root is found the pathname is relative to
	the specified root. If the root Parameter does not enclose this
	Parameter - i.e. the root is not in the path to this Parameter - an
	Invalid_Argument exception will be thrown.

	@param	root	A pointer to a pathname root Parameter.
	@return	The pathname string for the Parameter.
	@throws Invalid_Argument If the root Parameter is not in the path of
		this Parameter.
*/
std::string pathname (const Parameter *root = NULL) const;

/**	Gets the fully qualified (absolute) pathname of the Parameter
	from some root Parameter.

	@param	root	A parent of this Parameter.
	@return	The pathname string for the Parameter.
	@throws PVL::Invalid_Argument If the root Parameter is not in the path
		of this Parameter.
	@see	pathname(Parameter*)
*/
std::string pathname (const Parameter& root) const
	{return pathname (&root);}

/**	Gets the pathname to the Parameter without the Parameter's name.

	The trailing #path_delimiter character is included.

	@param	root	A pointer to a parent of this Parameter.
		[Default: NULL; the top Parameter of the hierarchy]
	@return	The pathname string up to, but not including, this
		Parameter's Name.
	@see	pathname(const Parameter*) const
*/
std::string pathname_to (const Parameter *root = NULL) const;

/**	Gets the pathname to the Parameter without the Parameter's name.

	@param	root	A parent of this Parameter.
	@return	The pathname string for the Parameter.
	@throws Invalid_Argument If the root Parameter is not in the path
		of this Parameter.
	@see	pathname_to(Parameter*)
*/
std::string pathname_to (const Parameter& root) const
	{return pathname_to (&root);}

/**	Test if the Parameter is at an optionally case-insensitive pathname,
	possibly from a specific root Parameter.

	The pathname string argument may be a full (absolute) or partial
	(relative) name of the form:

	[<b>/</b>]<i>Name</i>[<b>/</b><i>Name</i>[...]]

	A pathname string beginning with a #path_delimiter character is
	"absolute". In this case a Parameter will test true when the
	#pathname from the specified root Parameter exists and matches the
	specified pathname using the case_sensitive criteria. <b>Note</b>:
	This is the only case in which the root argument is used; it provides
	the absolute anchor for the pathname argument.

	A pathname string that does not begin with a #path_delimiter
	character is "relative". In this case a Parameter will test true when
	the pathname string matches (again, using the case_sensitive
	criteria) an ending substring of the Parameter #pathname that is also
	preceded by a #path_delimiter character. The basic form of a relative
	pathname is a simple name that contains no #path_delimiter character.

	@param	pathname	The pathname string to be matched.
	@param	case_sensitive	true if case sensitive string matching is
		to be applied; false otherwise.
	@param	root	A Parameter pointer for the root Parameter of of an
		absolute pathname. [Default: NULL; the top Parameter of the
		hierarchy]
	@return	true if this Parameter is located at the pathname; false
		otherwise.
*/
bool at_pathname
	(
	std::string			pathname,
	bool				case_sensitive,
	const Parameter		*root = NULL
	) const;

/**	Test if the Parameter is at an optionally case-insensitive pathname
	from from a root Parameter.

	@param	pathname	The pathname string to be matched.
	@param	case_sensitive	true if case sensitive string matching is
		to be applied; false otherwise.
	@param	root	A root Parameter of of an absolute pathname.
	@return	true if this Parameter is located at the pathname; false
		otherwise.
	@see	at_pathname(string, bool, Parameter*)
*/
bool at_pathname
	(
	std::string			pathname,
	bool				case_sensitive,
	const Parameter&	root
	) const
	{return at_pathname (pathname, case_sensitive, &root);}

/**	Test if the Parameter is at a pathname from the top of the Parameter
	heirarchy.

	The case sensitivity is determined by the current {@link
	case_sensitive() case_sensitive} state.

	@param	pathname	The pathname string to be matched.
	@return	true if this Parameter is located at the pathname; false
		otherwise.
	@see	at_pathname(string, bool, Parameter*)
*/
bool at_pathname (std::string pathname) const
	{return at_pathname (pathname, Case_Sensitive);}

/**	Gets the current Path_Name delimiter.

	@return	The delimiter char used for constructing pathnames.
*/
static char path_delimiter ()
	{return Path_Delimiter;}

/**	Sets the pathname delimiter.

	<b>Note</b>: The delimiter is a static class variable that applies
	to all Parameters.

	@param	delimiter	The delimiter char.
	@return	The previous delimiter character.
*/
static char path_delimiter (const char delimiter);

/**	Gets the default case sensitivity state.

	@return	true, if the default state is case sensitive; false otherwise.
	@see	case_sensitive(bool)
*/
static bool case_sensitive ()
	{return Case_Sensitive;}

/**	Sets the default case sensitivity state.

	@param	enable	true, if the default state is case sensitive; false
		otherwise.
	@return	The previous case sensitive state.
*/
static bool case_sensitive (bool enable);

/*------------------------------------------------------------------------------
	Comment
*/
protected:

//!	Comment string associated with the Parameter.
std::string			Comment;

public:

/**	Gets the Parameter comment.

	@return A string, possibly multi-line, containing an arbitrary
		comment associated with the Parameter.
*/
std::string comment () const
	{return Comment;}

/**	Sets the Parameter comment.

	@param	comment	A string used to provide any commentary
		about the Parameter.
	@return	This Parameter.
*/
Parameter& comment (const std::string& comment)
	{Comment = comment; return *this;}

/*------------------------------------------------------------------------------
	Subtype
*/
/**	Test if the Parameter is an Assignment.

	@return true if the implementing class is an Assignment;
		false otherwise.
*/
virtual bool is_Assignment () const
	{return false;}

/**	Test if the Parameter is a Token Assignment.

	@return true if the implementing class is an Assignment having a
		TOKEN Type specification; false otherwise.
*/
virtual bool is_Token () const
	{return false;}

/**	Test if the Parameter is an Aggregate.

	@return true if the implementing class is an Aggregate; false
		otherwise.
*/
virtual bool is_Aggregate () const
	{return false;}

/**	Test if the Parameter is a Group Aggregate.

	@return true if the implementing class is an Aggregate having a
		GROUP Type specification; false otherwise.
*/
virtual bool is_Group () const
	{return false;}

/**	Test if the Parameter is an Object Aggregate.

	@return true if the implementing class is an Aggregate having an
		OBJECT Type specification; false otherwise.
*/
virtual bool is_Object () const
	{return false;}

/**	Test if the Parameter is of some Type.

	The test is the logical AND of the Parameter Subtype and the
	specified Type value. Thus, conceptually, the test is if the
	Parameter is any one of a possible combination of Types. For
	example, <code>parameter.is (GROUP | OBJECT)</code> will be
	true for any AGGREGATE parameter, which
	could just as easily be tested by <code>parameter.is (AGGREGATE)</code>;
	and <code>parameter.is (ASSIGNMENT | AGGREGATE)</code> can be
	expected to always be true.

	@param	type	The Type specification code against which to test
		the Parameter.
	@return true if the implementing class is of the specified Type.
		an OBJECT Type specification;
		false otherwise.
*/
virtual bool is (const Type type) const = 0;

/**	Gets the Parameter's Type specification.

	@return	A Type specification code.
*/
virtual Type type () const = 0;

/**	Sets the Parameter's Type specification.

	@param	type	A Type specification code.
	@return	This Parameter.
	@throws	Invalid_Argument If the Parameter's implementing class can
		not accept the specified Type.
*/
virtual Parameter& type (const int type) = 0;

/**	Gets the Parameter's Type specification name.

	@return	A one word name string corresponding to the Parameter's
		Type specification code.
*/
virtual std::string type_name () const = 0;

/**	Gets name of a Type specification.

	A Type specification name is the same as its token name (except
	only the first letter is capitalized). If the specified value
	is not a valid Parameter Type specification code, then "Invalid"
	is returned.

	@param	type	A Type specification code.
	@return	A one word name string corresponding to the Type
		specification code.
*/
static std::string type_name (const Type type);

/*------------------------------------------------------------------------------
	Value
*/
/**	Gets the Value of an Assignment.

	@return	A reference to an Assignment Parameter's Value.
	@throws	Invalid_Argument if the Parameter is not an Assignment.
*/
virtual Value& value () const = 0;

/*==============================================================================
	Operators
*/
/**	Array notation accessor.

	@param	index	The index in the Aggregate of the Parameter to reference.
	@return	A reference to the index-th Aggregate Parameter.
	@throws	Invalid_Argument	If the Parameter is not an Aggregate.
	@see	Aggregate::operator[]
*/
virtual Parameter& operator[] (unsigned int index);

/**	Array notation accessor.

	@param	index	The index in the Aggregate of the Parameter to reference.
	@return	A const reference to the index-th Aggregate Parameter.
	@throws	Invalid_Argument	If the Parameter is not an Aggregate.
	@see	Aggregate::operator[]
*/
virtual const Parameter& operator[] (unsigned int index) const;

//	Assignments

/**	Assigns the defining characteristics of another Parameter to this Parameter.

	The name, comment and Type specification are copied, but not the
	parent. Assignment Parameters clone the assigned Value. Aggregate
	Parameters clone the assigned Aggregate's Parameter list.

	@param	parameter	The Parameter to be assigned.
	@return	This Parameter.
	@throws Invalid_Argument	If the Parameter to be assigned is not
		of the same implementing class as this Parameter.
*/
virtual Parameter& operator= (const Parameter& parameter) = 0;

/**	Assigns a Value to this Parameter.

	The current Value is deleted and the assigned Value is cloned in
	its place. A token Parameter becomes an Assignment.

	@param	value	The Value to be assigned.
	@return	This Parameter.
	@throws Invalid_Argument	If this Parameter is not an Assignment
		(or token).
*/
virtual Parameter& operator= (const Value& value) = 0;

//	Logical

/**	Compares this Parameter with another Parameter.

	@param	parameter	The Parameter to compare against.
	@return	An int that is less than, equal to, or greater than 0 if
		this Parameter is logically less than, equal to, or greater
		than, respectively, the other Parameter.
	@see	Assignment::compare(const Parameter&)
	@see	Aggregate::compare(const Parameter&)
*/
virtual int compare (const Parameter& parameter) const = 0;

/**	Test if this Parameter is logically equivalent to another Parameter.

	@param	parameter	The Parameter to compare against.
	@return	true or false based on how the parameters compare.
	@see	compare(const Parameter&) const
*/
bool operator== (const Parameter& parameter) const
	{return compare (parameter) == 0;}

/**	Test if this Parameter is logically less than another Parameter.

	@param	parameter	The Parameter to compare against.
	@return	true or false based on how the parameters compare.
	@see	compare(const Parameter&) const
*/
bool operator< (const Parameter& parameter) const
	{return compare (parameter) < 0;}

/**	Test if a Parameter is logically not equivalent to another Parameter.

	@param	parameter	The Parameter to compare against.
	@return	true or false based on how the parameters compare.
	@see	compare(const Parameter&) const
*/
bool operator!= (const Parameter& parameter) const
	{return compare (parameter) != 0;}

/**	Test if a Parameter is logically greater than another Parameter.

	@param	parameter	The Parameter to compare against.
	@return	true or false based on how the parameters compare.
	@see	compare(const Parameter&) const
*/
bool operator> (const Parameter& parameter) const
	{return compare (parameter) > 0;}

/**	Test if a Parameter is logically less than or equal to another Parameter.

	@param	parameter	The Parameter to compare against.
	@return	true or false based on how the parameters compare.
	@see	compare(const Parameter&) const
*/
bool operator<= (const Parameter& parameter) const
	{return compare (parameter) <= 0;}

/**	Test if a Parameter is logically greater than or equal to another Parameter.

	@param	parameter	The Parameter to compare against.
	@return	true or false based on how the parameters compare.
	@see	compare(const Parameter&) const
*/
bool operator>= (const Parameter& parameter) const
	{return compare (parameter) >= 0;}

/*==============================================================================
	I/O
*/
//	Write

/**	Writes the Parameter on an output stream using PVL syntax.

	@param	out	The ostream to receive the PVL syntax.
	@param	depth	The indenting depth.
	@return	The number of characters written.
	@see	Lister::write(const Parameter&, int)
*/
unsigned int write (std::ostream& out = std::cout, int depth = 0) const;

/**	Prints the Parameter on an output stream using PVL syntax.

	@param	out	The ostream to receive the PVL syntax.
	@return The out ostream.
	@see	write(ostream&, int)
*/
std::ostream& print (std::ostream& out = std::cout) const
	{write (out); return out;}


//	Read

/**	Reads an input stream parsed as PVL syntax into this Parameter.

	@param	in	The istream to be parsed for PVL syntax.
	@see	Assignment::read(istream&)
	@see	Aggregate::read(istream&)
*/
virtual void read (std::istream& in = std::cin) = 0;

/**	Assigns the next Parameter from a Parser to this Parameter.

	@param	parser	The Parser from which to obtain a Parameter.
	@see	Assignment::read(istream&)
	@see	Aggregate::read(istream&)
*/
virtual void read (Parser& parser) = 0;

/*==============================================================================
	Syntax modes
*/
//!	Mode selectors.
enum MODE
	{
	Verbatim_Strings,
	Indenting,
	TOTAL_LOCAL_MODES
	};

//!	States for local/inherited modes.
enum MODE_STATE
	{
	MODE_FALSE				= 0,
	MODE_TRUE				= 1,
	MODE_INHERITED			= 2
	};

protected:

//!	Default verbatim strings use.
#ifndef	VERBATIM_STRINGS
#define	VERBATIM_STRINGS		MODE_FALSE
#endif

//!	Default indenting.
#ifndef	INDENTING
#define	INDENTING				MODE_TRUE
#endif

//!	Default mode states.
static MODE_STATE
	DEFAULT_MODES[TOTAL_LOCAL_MODES];

//!	Local mode states.
MODE_STATE
	Modes[TOTAL_LOCAL_MODES];

public:

//!	Selects which mode set is applicable.
enum WHICH_MODE
	{
	LOCAL_MODE_SET			= 0,
	DEFAULT_MODE_SET		= 1,
	RECURSIVE_MODE_SET		= 2
	};
	

/**	Sets the state of a syntax mode.

	@param	mode	The selected #MODE.
	@param	state	The #MODE_STATE to be set. For the DEFAULT_MODE_SET
		MODE_INHERITED is equivalent to MODE_TRUE.
	@param	which	DEFAULT_MODE_SET if the mode applies to the
		#DEFAULT_MODES; LOCAL_MODE_SET if the mode only applies to this
		Parameter; RECURSIVE_MODE_SET if the mode also applies to all
		children of an Aggregate (if the Parameter is not an Aggregate
		only the LOCAL_MODE_SET applies).
	@return	This Parameter.
*/
Parameter& mode_state
	(const MODE mode, const MODE_STATE state,
	const WHICH_MODE which = LOCAL_MODE_SET);

/**	Gets the state of a syntax mode.

	@param	mode	The selected #MODE.
	@param	which	DEFAULT_MODE_SET if the mode applies to the
		#DEFAULT_MODES; LOCAL_MODE_SET if the mode only applies to this
		Parameter; RECURSIVE_MODE_SET if the mode also applies to any
		inherited mode when the local mode state is MODE_INHERITED.
	@return	The #MODE_STATE found.
*/
MODE_STATE mode_state
	(const MODE mode, const WHICH_MODE which = RECURSIVE_MODE_SET) const;

/**	Sets the state of all syntax modes.

	@param	modes	A pointer to an array of syntax modes. If NULL
		the local modes will all be reset to MODE_INHERITED;
		the #DEFAULT_MODES will be set to built-in defaults;
	@param	which	DEFAULT_MODE_SET if the mode applies to the
		#DEFAULT_MODES; LOCAL_MODE_SET if the mode only applies to this
		Parameter; RECURSIVE_MODE_SET if the mode also applies to all
		children of an Aggregate (if the Parameter is not an Aggregate
		only the LOCAL_MODE_SET applies).
	@return	This Parameter.
*/
Parameter& mode_state
	(const MODE_STATE* const modes, const WHICH_MODE which = LOCAL_MODE_SET);

/**	Gets the array of syntax mode states.

	@param	which	DEFAULT_MODE_SET if the #DEFAULT_MODES are to be
		obtained; otherwise the local modes for this Parameter are to
		be obtained.
	@return	A pointer to the array of mode states.
*/
const MODE_STATE* mode_state (const WHICH_MODE which = LOCAL_MODE_SET) const
	{return (which == DEFAULT_MODE_SET) ? DEFAULT_MODES : Modes;}


/**	Enable or disable verbatim quoted strings in PVL syntax.

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

	@param	state	The #MODE_STATE to be set. For the DEFAULT_MODE_SET
		MODE_INHERITED is equivalent to MODE_TRUE.
	@param	which	DEFAULT_MODE_SET if the mode applies to the
		#DEFAULT_MODES; LOCAL_MODE_SET if the mode only applies to this
		Parameter; RECURSIVE_MODE_SET if the mode also applies to all
		children of an Aggregate (if the Parameter is not an Aggregate
		only the LOCAL_MODE_SET applies).
	@return	This Parameter.
*/
Parameter& verbatim_strings
	(const MODE_STATE state, const WHICH_MODE which = LOCAL_MODE_SET)
	{return mode_state (Verbatim_Strings, state, which);}

/**	Enable or disable verbatim quoted strings for this Parameter.

	This is equivalent to using the corresponding method with a
	MODE_STATE of either MODE_TRUE or MODE_FALSE if enable is true or
	false, respectively and WHICH_MODE of LOCAL_MODE_SET;

	@param	enable	true if the mode is to be enabled; false otherwise.
	@return	This Parameter.
	@see	verbatim_strings(const MODE_STATE, const WHICH_MODE)
*/
Parameter& verbatim_strings (const bool enable)
	{return mode_state
		(Verbatim_Strings, enable ? MODE_TRUE : MODE_FALSE, LOCAL_MODE_SET);}

/**	Test if quoted strings will be written verbatim.

	If the mode is not set locally, then the state of the inherited
	mode, whether from a parent or the default, will be used.

	@param	which	DEFAULT_MODE_SET if the mode applies to the
		#DEFAULT_MODES; LOCAL_MODE_SET if the mode only applies to this
		Parameter; RECURSIVE_MODE_SET if the mode also applies to any
		inherited mode when the local mode state is MODE_INHERITED.
	@return	The #MODE_STATE found.
*/
MODE_STATE verbatim_strings
	(const WHICH_MODE which = RECURSIVE_MODE_SET) const
	{return mode_state (Verbatim_Strings, which);}


/**	Enable or disable indenting.

	@param	state	The #MODE_STATE to be set. For the DEFAULT_MODE_SET
		MODE_INHERITED is equivalent to MODE_TRUE.
	@param	which	DEFAULT_MODE_SET if the mode applies to the
		#DEFAULT_MODES; LOCAL_MODE_SET if the mode only applies to this
		Parameter; RECURSIVE_MODE_SET if the mode also applies to all
		children of an Aggregate (if the Parameter is not an Aggregate
		only the LOCAL_MODE_SET applies).
	@return	This Parameter.
*/
Parameter& indenting
	(const MODE_STATE state, const WHICH_MODE which = LOCAL_MODE_SET)
	{return mode_state (Indenting, state, which);}

/**	Enable or disable indenting for this Parameter.

	This is equivalent to using the corresponding method with a
	MODE_STATE of either MODE_TRUE or MODE_FALSE if enable is true or
	false, respectively and WHICH_MODE of LOCAL_MODE_SET;

	@param	enable	true if the mode is to be enabled; false otherwise.
	@return	This Parameter.
	@see	indenting(const MODE_STATE, const WHICH_MODE)
*/
Parameter& indenting (const bool enable)
	{return mode_state
		(Indenting, enable ? MODE_TRUE : MODE_FALSE, LOCAL_MODE_SET);}

/**	Test if indenting will be used.

	If the mode is not set locally, then the state of the inherited
	mode, whether from a parent or the default, will be used.

	@param	which	DEFAULT_MODE_SET if the mode applies to the
		#DEFAULT_MODES; LOCAL_MODE_SET if the mode only applies to this
		Parameter; RECURSIVE_MODE_SET if the mode also applies to any
		inherited mode when the local mode state is MODE_INHERITED.
	@return	The #MODE_STATE found.
*/
MODE_STATE indenting
	(const WHICH_MODE which = RECURSIVE_MODE_SET) const
	{return mode_state (Indenting, which);}

};	//	End of Parameter class.

/*==============================================================================
	I/O operators
*/
/**	Writes a Parameter on an output stream using PVL syntax.

	@param	out	The ostream to receive the PVL syntax.
	@param	parameter	The Parameter to be described.
	@return The ostream.
	@see	Lister::write(const Parameter&, int)
*/
std::ostream& operator<< (std::ostream& out, const Parameter& parameter);

/**	Reads an input stream parsed as PVL syntax into a Parameter.

	<b>N.B.</b>: If the Parser operating behind the scene encounters a
	PVL::Exception the failbit of the istream is set, but no exception is
	thrown. To obtain details about the cause of an input failure
	construct a Parser on the istream and use the read method directly.

	@param	in	The istream to be parsed for PVL syntax.
	@param	parameter	The Parameter to be assigned the Parameter definition
		that is read.
	@return	The istream.
	@see	Parameter::read(Parser&)
*/
std::istream& operator>> (std::istream& in, Parameter& parameter);

/*=*****************************************************************************
	Assignment
*/
/**	An <i>Assignment</i> implements a Parameter than contains a Value.

	The Assignment class is very lightweight: Objects of this class
	only add the Type specification code and a pointer to the assigned
	Value obect to the base Parameter class characteristics. No new
	methods beyond those declared for the base Parameter class for
	implmentation or override are defined here.
*/
class Assignment
:	public Parameter
{
public:

//	Constructors

/**	Constructs an Assignment with a Default_Value.

	@param	name The Parameter name string.
*/
explicit Assignment (const std::string name = "");

/**	Copy constructor.

	Copies the Type specification and clones the Value.

	@param	assignment	The Assignment Parameter to be copied.
*/
Assignment (const Assignment& assignment);

/**	Copy constructor from a Parameter.

	@param	parameter	The Parameter to be copied.
	@throws	Invalid_Argument	If the Parameter to be copied is not
		an Assignment. The resulting Assignment is valid, but it will
		have a NULL Value.
*/
Assignment (const Parameter& parameter);

/**	Construct an Assignment from a Parser.

	A single Assignment is read using the Paser's get_parameter
	method. If a valid Assignment Parameter is obtained from the Parser
	it is assigned to this newly constructed Assignment Parameter.
	Otherwise this Assignement is given the TOKEN Type specification
	and a #Default_Value.

	If a non-empty name is specified, the new Parameter is given that
	name.

	@param	parser	The Parser from which to read an Assignment definition.
	@param	name	A name for the new Parameter. [Default: the name of
		the parsed Parameter, or ""]
	@see	Parser::get_parameter(bool)
*/
Assignment (Parser& parser, const std::string name = "");

/**	Clones the Assignment by making a copy of itself.

	@return	A pointer to a copy of this Assignment.
	@see	Assignment::Assignment(const Assignment&)
*/
Assignment* clone () const
	{return new Assignment (*this);}

//!	Destructor: deletes the Assignment's Value.
~Assignment ()
	{delete The_Value;}

//	Subtype

/**	Test if this is an Assignment.

	@return	Always returns true.
*/
bool is_Assignment () const
	{return true;}

/**	Test if this is a token Parameter.

	A token Parameter is an Assignment without an effective Value.

	@return true if the Type
		specification is TOKEN.
*/
bool is_Token () const
	{return Subtype == Parameter::TOKEN;}

/**	Test if the Parameter is of some Type.

	The test is the logical AND of the Parameter Subtype and the
	specified Type value. Thus, conceptually, the test is if the
	Parameter is any one of a possible combination of Types. For
	example, <code>parameter.is (GROUP| OBJECT)</code> will be
	false for any ASSIGNMENT parameter, which
	could just as easily be tested by <code>parameter.is (AGGREGATE)</code>;
	and <code>parameter.is (ASSIGNMENT | AGGREGATE)</code> can be expected
	to always be true.

	@param	type	The Type specification code against which to test
		the Parameter.
	@return true if the implementing class is of the specified Type. an
		OBJECT Type specification; false otherwise.
*/
bool is (const Type type) const
	{return Subtype & type;}

/**	Gets the Type specification.

	@return	This should be either the ASSIGNMENT or
		TOKEN code.
*/
Type type () const
	{return Subtype;}

/**	Sets the Type specification.

	If the Type specification is ASSIGNMENT or
	TOKEN, then it applies to this Assignment. Otherwise the
	value is passed to the Assignment's Value.

	@param	type	The Type specification for the
		Assignment or its Value.
	@return	This Assignment.
	@throws	Invalid_Argument	If the type value is not valid for an
		an Assignment nor a Value.
*/
Assignment& type (const int type);

/**	Gets the name for this Assignment's Type
	specification.

	@return	The name string associated with the Assignment's
		Type specification.
	@see	Parameter::type_name(Type)
*/
std::string type_name () const
	{return Parameter::type_name (Subtype);}

/**	Gets the Value of the Assignment.

	@return	A reference to the Assignment's Value.
*/
Value& value () const
	{return *The_Value;}

//	Operators

//		Assignments

/**	Assigns a Parameter to this Assignment.

	If the Parameter is an Assignment its name, comment and
	Type specification are copied, but not its parent.
	It's Value is cloned and replaces this Assignment's Value, which is
	deleted.

	Nothing is done when assigning a Parameter to itself.

	@param	parameter	The Parameter to be assigned.
	@return	This Assignment.
	@throws	Invalid_Argument if the Parameter is not an Assignment.
*/
Assignment& operator= (const Parameter& parameter);

/**	Assigns a Value to this Assignment.

	The current Value is deleted and the new value is cloned in
	its place. The Type specification is set to ASSIGNMENT.

	@param	value	The Value to be assigned.
	@return	This Assignment.
*/
Assignment& operator= (const Value& value);

//		Compare

/**	Compares this Assignment with another Parameter.

	A Parameter compared with itself is, of course, identical.

	The Parameter names are compared and, if they are
	different, the result of this comparison is returned.

	When the Parameter names are the same, an Assignment is
	less than an Aggregate, and a TOKEN is less
	than an ASSIGNMENT.

	When all else is the same (regardless of any comment)
	the result of comparing the Values is returned.

	@param	parameter	The Parameter to compare against.
	@return	An int that is less than, equal to, or greater than 0 if
		this Assignment is logically less than, equal to, or greater
		than, respectively, the other Parameter.
*/
int compare (const Parameter& parameter) const;

//	I/O

/**	Reads an input stream parsed as PVL syntax into this Assignment.

	A Parser is constructed from the input stream and passed to
	the #read(Parser&) method.
	A single Assignment Parameter is parsed from the input stream

	@param	in	The istream to be parsed for PVL syntax.
*/
void read (std::istream& in = std::cin);

/**	Assigns the next Parameter from a Parser to this Parameter.

	A single Assignment Parameter is obtained from the Parser and
	assigned to this Parameter. If no Assignment Parameter is
	obtained from the Parser, this Assignment Parameter remains
	unchanged.

	<b>N.B</b>: Only an Assignment Parameter will be parsed even if the
	PVL syntax contains a name for the next parameter that is a
	#special_type keyword. Thus, for example, where the PVL syntax
	defines a GROUP parameter which begins an Aggregate Parameter an
	Assignment Parameter will be obtained instead which will be named
	"GROUP" having a value that would otherwise be the name of the
	Aggregate Parameter. Similarly, where the end of PVL syntax is marked
	a TOKEN Assignment named "END" will be obtained instead. As a result
	it is possible for subsequent operations of the Parser to become
	confused about the structural context of the PVL syntax having lost
	track of a structural marker parameter. This method should only be
	used when direct, individual processing of each parameter defined in
	the PVL syntax is intended. In most cases use of the
	Parser::get_parameter method is preferable.

	@param	parser	The Parser from which to obtain a Parameter.
	@see	Parser::get_parameter(bool)
	@see	#special_type
*/
void read (Parser& parser);

//..............................................................................
private:

Value*		The_Value;
Type		Subtype;

};		//	End of Assignment class.

/*=*****************************************************************************
	Aggregate
*/
/**	An <i>Aggregate</i> implements a Parameter that is a list of
	Parameters.

	An Aggregate provides for the hierarchical structuring of Parameters.

	The list is provided by subclassing a <i>Vectal</i> of Parameters.
	A Vectal is a <b>vect</b>or of virtu<b>al</b> objects that are
	stored as pointers but for which reference semantics are offered in
	the access interface. However, the normal value functionality is
	also provided by copying (actually cloning) objects into the list
	and deleting them on removal. An Aggregate ensures that, normally,
	whenever a Parameter is entered into its list the parent of the
	Parameter is set to the Aggregate. Thus the hierarchical
	relationships are automatically maintained. Provision is also made
	for pulling a Parameter from an Aggregate's list without deleting
	it, but also automatically resetting its parent to NULL to
	correctly reflect that it is no longer a member of the Aggregate
	hierarchy.

	In addition to the normal vector interators for traversing an
	Aggregate's Parameter list, specialized Depth_Iterators are
	provided that descend into Aggregates encountered in a Parameter
	list. They continue iterating the parent list where they left off
	after completing the interation of a child Aggregate. They also
	remember the root Aggregate where they started, which is where they
	will end. These iterators are bidirectional with some random access
	capabilities.

	@see	Vectal
*/
class Aggregate
:	public Parameter,
	public Vectal<Parameter>
{
public:
//	Types

//!	Aggregate as a Vectal (vector) of Parameters.
typedef Vectal<Parameter>				Parameter_List;

//!	Identify the iterator for compilers that need help.
typedef Parameter_List::iterator		iterator;
//!	Identify the iterator for compilers that need help.
typedef Parameter_List::const_iterator	const_iterator;

class Depth_Iterator;
//!	A specialized iterator that descends into Aggregates in the Parameter list.
typedef Depth_Iterator					depth_iterator;

//	Constructors

/**	Constructs an empty Aggregate.

	@param	name	The Parameter name.
	@param	type_code	The Type specification code for the Aggregate.
	@see	type(Type)
*/
explicit Aggregate (const std::string name = "",
	const idaeim::PVL::Parameter::Type type_code = Default_Subtype);

/**	Copy constructor.

	The entire Parameter list is replicated (cloned).

	@param	aggregate	The Aggregate to copy.
	@see	Parameter::Parameter(Parameter)
*/
Aggregate (const Aggregate& aggregate);

/**	Copy constructor from a Parameter.

	If the Parameter being copied is an Aggregate, it is assigned to
	the new Aggregate. If, however, the Parameter is an Assignment, it
	is copied as the first element in the new Aggregate's Parameter
	list.

	@param	parameter	The Parameter to copy.
*/
Aggregate (const Parameter& parameter);

/**	Constructs an Aggregate using a Parser to provide its contents.

	This is the same as constructing an empty Aggregate and then using
	the Parser::add_parameters(Aggregate&) method on it. Note that the
	Parameters obtained from the Parser are all added as elements of the
	new Aggregate. Thus even if the Parser only finds one Parameter -
	e.g. from a string defining a single Group - the result will be an
	Aggregate containing a single Parameter element.

	<b>Note</b>: If the new Aggregate is given the name
	Parser::CONTAINER_NAME then when the Aggregate is written to an
	ostream, only the content Parameters will be written.

	@param	parser	The Parser from which to get content Parameters.
	@param	name	The name for the Aggregate.
	@see	Parser::add_parameters(Aggregate&)
*/
Aggregate (Parser& parser, const std::string name = "");

/**	Destroys an Aggregate by clearing the Parameter list.

	Each Parameter in the list will be deleted.
*/
~Aggregate ();

/**	Clones the Aggregate by making a copy of itself.

	@return	A pointer to a copy of this Aggregate.
	@see	Aggregate(Aggregate&)
*/
Aggregate* clone () const
	{return new Aggregate (*this);}

//	Subtype

/**	Test if this is an Aggregate.

	@return	Always returns true.
*/
bool is_Aggregate () const
	{return true;}

/**	Test if this is a Group Aggregate.

	@return true if the Type
		specification is GROUP.
*/
bool is_Group () const
	{return Subtype == Parameter::GROUP;}

/**	Test if this is a Group Aggregate.

	@return true if the Type
		specification is OBJECT.
*/
bool is_Object () const
	{return Subtype == Parameter::OBJECT;}

/**	Test if the Parameter is of some Type.

	The test is the logical AND of the Parameter Subtype and the
	specified Type value. Thus, conceptually, the test is if the
	Parameter is any one of a possible combination of Types. For
	example, <code>parameter.is (GROUP| OBJECT)</code> will be
	true for any AGGREGATE parameter, which could just as easily be
	tested by <code>parameter.is (AGGREGATE)</code>; and
	<code>parameter.is (ASSIGNMENT | AGGREGATE)</code> can be expected
	to always be true.

	@param	type	The Type specification code
		against which to test the Parameter.
	@return true if the implementing class is of the specified Type.
		an OBJECT Type specification;
		false otherwise.
*/
bool is (const Type type) const
	{return Subtype & type;}

/**	Gets the Type specification.

	@return	This should be either the GROUP or
		OBJECT code.
*/
Type type () const
	{return Subtype;}

/**	Sets the Type specification.

	@param	type	The Type specification for the
		Aggregate.
	@return	This Aggregate.
	@throws	Invalid_Argument	If the type value is not
		GROUP or OBJECT.
*/
Aggregate& type (const int type);

/**	Gets the name for this Aggregate's Type specification.

	@return	The name string associated with the Aggregate's
		Type specification.
	@see	Parameter::type_name(Type)
*/
std::string type_name () const
	{return Parameter::type_name (Subtype);}

/**	Gets the default Type specification to be used when constructing a
	new Aggregate without a Type specified.

	@return	The current default Type specification.
*/
static Type default_type ()
	{return Default_Subtype;}

/**	Sets the default Type specification to be used when constructing a
	new Aggregate without a Type specified.

	@return	The previous default Type specification.
	@throws	Invalid_Argument	If the type value is not GROUP or
		OBJECT.
*/
static Type default_type (const int type);

/**	An Aggregate doesn't have a Value, so this always
	throws an Invalid_Argument exception.

	@return	Never returns ...
	@throws	Invalid_Argument always.
*/
Value& value () const;

//	Operators

//	Array notation accessor.

/**	Gets the Parameter at an index of the Aggregate list.

	This form of the operator is used with an assignable Parameter (lvalue)
	in an expression.

	@param	index	The index of a Parameter in the Aggregate list.
	@return	The Parameter reference at the index position.
*/
Parameter& operator[] (unsigned int index)
	{return Parameter_List::operator[] (index);}

/**	Gets the Parameter at an index of the Aggregate list.

	This form of the operator produces an unassignable (read only)
	Parameter.

	@param	index	The index of a Parameter in the Aggregate list.
	@return	The Parameter reference at the index position.
*/
const Parameter& operator[] (unsigned int index) const
	{return Parameter_List::operator[] (index);}

//		Assignments

/**	Assigns a Parameter to this Aggregate.

	If the Parameter is an Aggregate its name, comment and Type
	specification are copied, but not its parent. The Parameter list
	from the other Aggregate is replicated (cloned) and replaces the
	current list. The replacement is overlap safe: an Aggregate from
	within this Aggregate can be assigned. The current Parameter list is
	set aside but not deleted until the list replacement has completed.

	If the Parameter is an Assignment its clone is appended to the
	Parameter list.

	Nothing is done when assigning a Parameter to itself.

	@param	parameter	The Parameter to be assigned.
	@return	This Aggregate.
*/
Aggregate& operator= (const Parameter& parameter);

/**	Assigns an Aggregate to this Aggregate.

	@param	parameter	The Aggregate to be assigned.
	@return	This Aggregate.
	@see	operator=(const Parameter&)
*/
Aggregate& operator= (const Aggregate& parameter)
	{return operator= (static_cast<const Parameter&>(parameter));}

/**	Assigns an Assignment to this Aggregate.

	@param	parameter	The Assignment to be assigned.
	@return	This Aggregate.
	@see	operator=(const Parameter&)
*/
Aggregate& operator= (const Assignment& parameter)
	{return operator= (static_cast<const Parameter&>(parameter));}

/**	An Aggregate doesn't have a Value, so this always throws an
	Invalid_Argument exception.

	@return	Never returns ...
	@throws	Invalid_Argument always.
*/
Aggregate& operator= (const Value& value);

//	Compare

/**	Compares this Aggregate with another Parameter.

	A Parameter compared with itself is, of course, identical.

	The Parameter names are compared and, if they are
	different, the result of this comparison is returned.

	When the Parameter names are the same, an Aggregate is
	greater than an Assignment.

	When comparing Aggregates with the same name, the greater is the
	one with the larger Parameter list (not including the size of
	Parameter lists in any Aggregates in the list). When the size of
	both lists is the same, then each Parameter in the list is compared
	in the order they occur in the list and the result of the first
	non-equal comparison is returned; thus for the best results #sort
	both Aggregates first.

	@param	parameter	The Parameter to compare against.
	@return	An int that is less than, equal to, or greater than 0 if
		this Aggregate is logically less than, equal to, or greater
		than, respectively, the Parameter.
	@see	sort(depth_iterator, depth_iterator)
*/
int compare (const Parameter& parameter) const;

//	Find

//!	Flags for find case sensitivity argument.
enum
	{
	CASE_INSENSITIVE,
	CASE_SENSITIVE
	};

/**	Find a named Parameter.

	The Parameter to be found may be specified by a simple name, a
	relative pathname or an absolute pathname. A simple name is the name
	of a Parameter itself. A relative pathname is a simple name preceeded
	by the pathname of an Aggregate Parameter that must contain the
	Parameter with the simple name. An absolute pathname is a pathname
	that begins with a pathname delimiter ('/'); i.e. it is a fully
	qualified pathname beginning at the root of the Aggregate hierarchy,
	compared to a relative pathname that may begin at any Aggregate in
	the hierarchy.

	Since it is not necessary that the pathname to each Parameter be
	unique, the number of matching pathnames to skip before selecting a
	Parameter may need to be specified to get the desired Parameter.
	Also, a specific type of Parameter - an Aggregate of Parameters or an
	Assignment of Values - may be specified. When skipping parameters,
	only the specified type, if any, are counted.

	@param	pathname	The pathname of the parameter to find.
	@param	case_sensitive	If true, the name search is case
		sensitive; case insensitive otherwise.
	@param	skip		The number of parameters that match the
		criteria to skip before selecting a matching parameter. A
		value less than or equal to zero selects the first match.
	@param	type		A Type code that specifies the type of Parameter
		that is to be selected. By default any Type (ASSIGNMENT or
		AGGREGATE) will be acceptable.
	@return	A pointer to the matching parameter, or NULL if a match could not
		be found.
*/
Parameter*
find
	(
	const std::string&				pathname,
	bool							case_sensitive = false,
	int								skip = 0,
	idaeim::PVL::Parameter::Type	type = (Type)(ASSIGNMENT | AGGREGATE)
	)
	const;

//	Select

/**	A <i>Selection</i> is used to map the value of a named parameter to
	an internal variable.

	A Selection provides a generic way to specify what to look for in a
	Parameter Aggregate and the variable to store the value of the
	Parameter that is found. This is a mapping from PVL information to
	application data variables.

	The data members describing a Selection are all public so they can be
	accessed directly. The data members, in initialization order, are:

<dl>
<dt>Pathname (std::string)
<dd>A Parameter pathname. The pathname may be absolute or relative, and
	is not case sensitive.

<dt>Variable (void*)
<dd>The address of an appropriate variable (or array entry) to receive
	the value(s) obtained from the Parameter Value that is selected. The
	variable pointed to must, of course, have a data type corresponding
	to the associated Type.

	The values of Parameter Values that are selected are stored in
	sequential variable address locations starting at the Variable
	address; i.e. the Variable is treated as the address of an array of
	values. The array must be capable of holding up to the Count of
	values having the data type corresponding to the Selection Type.

<dt>Type (Value::Type)
<dd>The {@link Value::Type} of the Parameter's Value to be selected. It
	also specifies the data type of the Variable that is to receive the
	value:

	<ul>
	<li>Value::INTEGER - Value::Integer_type
	<li>Value::REAL - Value::Real_type
	<li>Value::STRING or any of its subtypes - Value::String_type
	</ul>

	<b>N.B.</b>: The Value::ARRAY Type, and its subtypes, is invalid. To
	select one or more Values from an ARRAY specify a Count greater than
	1. For a Parameter with an Array assignment the Values contained in
	the Array are used.

<dt>Count
<dd>The number of values to obtain. If the count is less than or equal to
	1 a single value will be obtained, which will be from the single
	Value of a the Parameter Assignment or the first Value of an ARRAY.
	Otherwise the Parameter Assignment must be for an ARRAY and the first
	Count Values will provide the values obtained for the Variable.

	<b>N.B.</b>: If the Count of values to obtain is greater than 1 then
	the Variable address must be for the first entry of an array of the
	corresponding data type.
</dl>

	Examples:

	\code
	Value::Integer_type
		integer,
		integers[3];
	Value::Real_type
		reals[4];
	Value::String_type
		strings[3];
	Selection
		selections[] =
		{
		Selection ("Integer", integer),
		Selection ("Integer_Array", *integers, 3),
		Selection ("Real", *reals),
		Selection ("Real_Array", reals[1], 3),
		Selection ("String", *strings),
		Selection ("String_Array", *strings, 3),
		Selection ()
		};
	\endcode

	In this example the strings array is used as the Variable for both
	the Selection with "String" Pathname and "String_Array" Pathname. In
	this case if the Selection with the "Real_Array" Pathname will put
	any values it obtains in the same reals array referenced by the
	Selection with the "Real" Pathname, but any "Real_Array" values that
	are obtained are assigned starting with the second entry of the reals
	array variable. By contrast if any values are obtained from a
	"String_Array" Parameter, the first value obtained will replace a
	value that might have been obtained from a "String" Parameter.

	Note that the selections array is terminated with an empty Selection.

	\code
	Value::Integer_type
		image_dimensions[3] = {-1, -1, -1};
	#define COLUMNS  0
	#define LINES    1
	#define BANDS    2
	Selection
		selections[] =
		{
		Selection ("LINE_SAMPLES",               image_dimensions[COLUMNS]),
		Selection ("NS",                         image_dimensions[COLUMNS]),
		Selection ("IMAGE/LINE_SAMPLES",         image_dimensions[COLUMNS]),
		Selection ("NL",                         image_dimensions[LINES]),
		Selection ("IMAGE_LINES",                image_dimensions[LINES]),
		Selection ("LINES",                      image_dimensions[LINES]),
		Selection ("IMAGE/LINES",                image_dimensions[LINES]),
		Selection ("IMAGE/BANDS",                image_dimensions[BANDS]),
		Selection ("/QUBE/CORE_ITEMS",          *image_dimensions, 3),
		Selection ("/SPECTRAL_QUBE/CORE_ITEMS", *image_dimensions, 3),
		Selection ()
		};
	\endcode

	In this example the three dimensions of an image - width (COLUMNS),
	height (LINES), and depth (BANDS) - may be obtained from Parameters
	at various pathnames, both relative and absolute, depending on the
	source image product image label PVL parameters. In this way a single
	Selection array will obtain the needed image_dimensions values
	regardless of the image product with which it is used, and the
	application can avoid complex code conditional on different image
	product sources.

	Note that the image_dimensions array was initialized with -1 values
	that the application can use as flags that a particular value was not
	obtained.
*/
struct Selection
{
//	The pathname - absolute or relative - of the parameter to select.
std::string
	Pathname;

//	The address of a variable to receive the parameter value(s).
void*
	Variable;

//	The Value::Type code of the expected parameter value.
Value::Type
	Type;

//	The number of values to obtain from the parameter value.
int
	Count;

//	Constructors

/**	Constructs an empty Selection

	The Pathname will be the empty string, the Variable will be NULL,
	the Type will be zero, and the Count will be zero.
*/
Selection ()
	:
	Pathname (),
	Variable (NULL),
	Type ((Value::Type)0),
	Count (0)
{}

/**	Constructs a fully specified Selection.

	@param	pathname	A Parameter pathname string. This may be an
		absolute or relative pathname.
	@param	variable	An address of a variable, or the first entry of
		an array, to receive the value(s) to be obtained.
	@param	value_type	A Value::Type code that specifies the Type of
		Value to be obtained, and indicates the data type of the variable
		to receive the value(s). <b>N.B.</b>: The Value::ARRAY Type and
		its subtypes are not valid.
	@param	count	The number of values to be obtained. If the count is
		less than 1 it will coerced to 1. If the count is greater than
		1 the variable argument must be the address of the first entry
		of an array capable of holding at least count values of the
		data type indicated by the value_type argument.
*/
Selection (const std::string& pathname, void* variable,
	Value::Type value_type, int count = 1)
	:
	Pathname (pathname),
	Variable (variable),
	Type (value_type),
	Count ((count > 1) ? count : 1)
{}

/**	Constructs a Selection for an integer value.

	@param	pathname	A Parameter pathname string. This may be an
		absolute or relative pathname.
	@param	variable	A reference to a Value::Integer_type variable
		that will receive the value obtained. This must be the first
		entry of an array capable of of holding at least count values.
	@param	count	The number of values to be obtained.
*/
Selection (const std::string& pathname, Value::Integer_type& variable,
	int count = 1)
	:
	Pathname (pathname),
	Variable (&variable),
	Type (Value::INTEGER),
	Count ((count > 1) ? count : 1)
{}

/**	Constructs a Selection for a real, floating point, value.

	@param	pathname	A Parameter pathname string. This may be an
		absolute or relative pathname.
	@param	variable	A reference to a Value::Real_type variable
		that will receive the value obtained. This must be the first
		entry of an array capable of of holding at least count values.
	@param	count	The number of values to be obtained.
*/
Selection (const std::string& pathname, Value::Real_type& variable,
	int count = 1)
	:
	Pathname (pathname),
	Variable (&variable),
	Type (Value::REAL),
	Count ((count > 1) ? count : 1)
{}

/**	Constructs a Selection for a string value.

	@param	pathname	A Parameter pathname string. This may be an
		absolute or relative pathname.
	@param	variable	A reference to a Value::String_type variable
		that will receive the value obtained. This must be the first
		entry of an array capable of of holding at least count values.
	@param	count	The number of values to be obtained.
*/
Selection (const std::string& pathname, Value::String_type& variable,
	int count = 1)
	:
	Pathname (pathname),
	Variable (&variable),
	Type (Value::STRING),
	Count ((count > 1) ? count : 1)
{}
};	//	End of Selection.

/**	Select parameter values to be assigned to variables.

	For each Selection with a non-NUL Variable an attempt is made to
	{@link find(const std::string&, bool, int, idaeim::PVL::Parameter::Type)const 
	find} an Assignment Parameter at the Selection Pathname. The first
	Parameter found is used; no parameter skipping is done. If no
	matching Parameter is found the Selection is not used.

	The Selection Count determines the maximum number of Parameter Values
	from which to obtain values that are assigned to the Selection
	Variable. Only Values having the Selection Type are used; a Value
	with any other Type is ignored (not counted). For Parameters with an
	Array, Values are always examined sequentially starting with the
	first Value of the Array; subarrays are not examined. Each Value that
	is used results in a value being assigned sequential entries of the
	Selection Variable address treated as an array. There may be
	insufficient matching Values to provide the entire Selection Count.

	Note that a Selection Variable may be left unassigned, so it may be
	appropriate to initialize the variable with a default value. However,
	the same variable may be referred to by more than one Selection,
	probably with a different Pathname, in which case the last successful
	Selection, if any, will determine the value assigned to the variable.
	It may be desirable to initialize a variable with a special value
	that can be used to indicate that no Selection was successful for
	that variable.

	Since a Selection that matches a Pathname but does not match a Value
	Type does not have a Variable assigned it may be appropriate to use
	the same Pathname in different Selections, probably with different
	Variables, to obtain values that may have different data types
	associated with the same Parameter pathname.

	@param	selections	An array of Selection mappings.
		<b>N.B.</b>: The Selection array must be terminated with a
		Selection having a Type of zero. This can be done by appending a
		default Selection to the array.
	@throws	Invalid_Argument if a Selection has an invalid Type. An invalid
		Type is Value::ARRAY or any of its subtypes.
*/
void select (Selection* selections) const;

//	I/O

/**	Reads an input stream parsed as PVL syntax into this Aggregate.

	A Parser is constructed from the input stream and passed to
	the #read(Parser&) method.

	@param	in	The istream to be parsed for PVL syntax.
*/
void read (std::istream& in = std::cin);

/**	Parameters read by a Parser are added to this Aggregate.

	All available Parameters are obtained from the Parser will be added
	to the end of this Aggregate's Parameter list. If the Parser produces
	an Aggregate then each Parameter in its list is moved into this
	Aggregate list if the list is currently empty; if the list is not
	empty the entire Aggregate that was obtained, not just its list of
	Parameters, is appended to the list. If the Parameter the Parser
	produces in not an Aggregate Parameter then it is appended to this
	Aggregate's list. If the Parser does not produce a Parameter, nothing
	is done.

	@param	parser	The Parser from which to obtain Parameters.
	@see	Parser::get_parameters()
*/
void read (Parser& parser);

//	Conceptually convenient.

/**	Adds a clone of the Parameter to the end of the Aggregate's
	Parameter list.

	@param	parameter	The Parameter to add.
	@return	This Aggregate.
	@see	Vectal::push_back(reference)
	@see	add(Parameter*)
*/
Aggregate& add (const Parameter& parameter)
	{push_back (parameter); return *this;}

/**	Adds the Parameter to the end of the Aggregate's Parameter list.

	<b>Note</b>: The Parameter is not cloned; The Parameter pointer is
	placed directly in the Parameter list backing store. The parent of
	the Parameter is changed to this Aggregate.

	@param	parameter	The Parameter to add.
	@return	This Aggregate.
	@see	Vectal::poke_back(pointer)
	@see	add(const Parameter&)
*/
Aggregate& add (Parameter* parameter)
	{poke_back (parameter); return *this;}

/**	Copies the Parameter list in an Aggregate into this Aggregate's list.

	Each Parameter in the source Aggregate Parameter list is added to
	the end of this Aggregate's Parameter list.

	@param	aggregate	The source Aggregate.
	@return	This Aggregate.
	@see	add(const Parameter&)
*/
Aggregate& copy_in (const Aggregate& aggregate);

/**	Moves the Parameter list in an Aggregate into this Aggregate's list.

	Each Parameter in the source Aggregate Parameter list is moved to
	the end of this Aggregate's Parameter list. As a result the source
	Aggregate will be emptied.

	@param	aggregate	The source Aggregate.
	@return	This Aggregate.
	@see	add(const Parameter*)
*/
Aggregate& move_in (Aggregate& aggregate);

/**	Removes	a Parameter at an iterator position from the Aggregate's
	Parameter list.

	@param	position	A valid iterator for the Aggregate's Parameter
		list.
	@return	A pointer to the Parameter that was removed.
	@see	pull_out(iterator)
*/
Parameter* remove (iterator position);

/**	Removes	a Parameter at an element index of the Aggregate's
	Parameter list.

	@param	index	An index of the Aggregate's Parameter list.
	@return	A pointer to the Parameter that was removed. This
		will be NULL if the index is not within the range of
		existing elements of the Aggregate's Parameter list.
	@see	pull_out(size_type)
*/
Parameter* remove (size_type index);

/**	Removes a Parameter from the Aggregate's Parameter list.

	@param	parameter	A pointer to the Parameter to be removed. If
		NULL nothing is done and NULL is returned.
	@return	A pointer to the Parameter that was removed, or NULL if
		the Parameter was not found in the Aggregate's Parameter list.
*/
Parameter* remove (Parameter *parameter);

/**	Removes a Parameter from the Aggregate's Parameter list or any
	of its sub-lists.

	@param	parameter	A pointer to the Parameter to be removed. If
		NULL nothing is done and NULL is returned.
	@return	A pointer to the Parameter that was removed, or NULL if
		the Parameter was not found.
*/
Parameter* remove_depth (Parameter *parameter);

/*------------------------------------------------------------------------------
	Vectal related methods.
*/
//	Overloads for setting the Parent.

/**	Sets the parent of a Parameter to this Aggregate.

	This method overrides the Vectal's method that is
	invoked whenever a Parameter is entered into the Parameter list. It
	is not intended for normal use since it affects the logical
	relationship of Parameters in an Aggregate hierarchy.

	The uniquness of the Parameter in the path hierachy is ensured;
	loops are prevented.

	@param	parameter	The Parameter whose parent is to be changed.
	@throws	Invalid_Argument	If the parameter is in the Aggregate's
		path; i.e. the Parameter is already in the parent hierachy of
		the Aggregate, or is the Aggregate itself.
*/
void entering (Parameter* parameter);

/**	Resets the parent of a Parameter to NULL.

	This method overrides the Vectal's method that is
	invoked whenever a Parameter is removed from the Parameter list. It
	is not intended for normal use since it affects the logical
	relationship of Parameters in an Aggregate hierarchy.

	@param	parameter	The Parameter whose parent is to be changed.
*/
void removing (Parameter* parameter);

/**	Counts the number of Parameters in the Aggregate's Parameter list
	and, recursively, the number of Parameters in the Parameter list
	of each Aggregate in the list.

	@return	The accumulated total number of Parameters enclosed by
		this Aggregate.
*/
size_type total_size ();

/*==============================================================================
	Depth_Iterator
*/
/**	A <i>Depth_Iterator</i> is a bidirectional iterator over an
	Aggregate's Parameter list and, depth-first, over the Parameter list
	of each Aggregate encountered.

	The Depth_Iterator operates like a normal breadth-only interator
	for a Parameter list until it encounters an Aggregate in the list.
	When it is incremented from an Aggregate position it decends into
	the Aggregate, and only returns to the next Parameter in the
	current list when the end of the Parameter list of the Aggregate
	has been reached. This process is recursive resulting in a
	depth-first traversal of the entire Aggregate Parameter hierarchy.
	The mechanism operates similarly when decrementing by descending
	into each Aggregate and reverse iterating its Parameter list, then
	returning to the current Parameter list when the beginning of the
	Aggregate's list has been reached.

	A Depth_Iterator is associated with a specific Aggregate which it
	knows as its root Aggregate. It will not ascend into the Parameter
	list of the root Aggregate's parent. Like other container
	iterators, all positions are expected to be for valid Parameters
	except the final end position of the root Aggregate; reverse
	iteration will not go beyond the initial begin position of the
	root Aggregate.

	Random-access operators are also provided. But because they are not
	truely random access, and do not operate in constant time, the
	Depth_Iterator is tagged as only bidirectional.
*/
class Depth_Iterator
:	public std::iterator<std::bidirectional_iterator_tag, Parameter>
{
public:

//	Constructors

/**	Constructs a Depth_Iterator without a root Aggregate.

	This Depth_Iterator is invalid until it is assigned a valid
	iterator.
*/
Depth_Iterator ()
	:	Current_Aggregate (NULL)
	{}

/**	Constructs a Depth_Iterator for an Aggregate.

	The initial position of this Depth_Iterator is the beginning
	of the Aggregate's Parameter list.

	@param	aggregate	The root Aggregate.
	@see	Vectal::begin()
*/
explicit Depth_Iterator (const Aggregate& aggregate);

/**	Constructs a Depth_Iterator from a normal Parameter_List iterator.

	<b>Warning</b>: The iterator must be valid; i.e. it must refer to a
	Parameter. An end iterator is not valid; use #end_depth to get the
	iterator for the end of a Depth_Iterator.

	@param	list_iterator	A Parameter_List iterator.
*/
Depth_Iterator (const Aggregate::iterator& list_iterator);

/**	Converts the Depth_Iterator to a Parameter_List iterator.

	@return An iterator at the current position of the Depth_Iterator.
*/
operator Aggregate::iterator ()
	{return Current_Position;}

/**	Gets the root Aggregate.

	@return	A pointer to the root Aggregate for this Depth_Iterator.
*/
Aggregate* root () const;

/**	Gets the current Aggregate context.

	@return	A pointer to the Aggregate containing the Parameter list
		for the current iterator position.
*/
Aggregate* aggregate () const
	{return Current_Aggregate;}

/**	Gets a Depth_Iterator positioned at the end of the traversal
	for this Depth_Iterator.

	This is the same as the end iterator for the root Aggregate.

	@return	A Depth_Iterator positioned at the end of this Depth_Iterator.
	@throws	Error	If this Depth_Iterator is invalid (it doesn't
		have a root Aggregate).
	@see	Vectal::end()
*/
Depth_Iterator end_depth () const;

/**	Assigns a Parameter list iterator to this Depth_Iterator.

	If the Depth_Iterator is currently positioned at the same
	Parameter as the list iterator nothing happens.

	If the Depth_Iterator and list iterator are positioned in the
	Parameter list of the same Aggregate, then the current position of
	the Depth_Iterator is simply moved to the list iterator position.
	However, if the list iterator position is the end of the current
	Parameter list, and the list is not in the root Aggregate, then the
	current position is moved to the position preceeding the end;
	unless the list has become empty (due to some other user action on
	the list) in which case the Depth_Iterator is incremented forward
	to the next valid position.

	When the Depth_Iterator and list iterator are not positioned in
	the same Aggregate, then a search is made for the list iterator
	position within the scope of the Depth_Iterator. First a search
	is made from the current Depth_Iterator position towards the
	beginning of its scope. If this fails a search is made
	from the current Depth_Iterator position towards the end of its
	scope. The current position of the Depth_Iterator becomes list
	iterator position if it is found.

	@param	list_iterator	The Parameter_List iterator position to be
		assigned to this Depth_Iterator.
	@return	This Depth_Iterator.
	@throws	Invalid_Argument	If the list iterator is not in the
		scope of this Depth_Iterator.
	@throws	Error				If on attempting to ascend from an
		Aggregate hierarchy, a non-root Aggregate is encountered without
		a parent (this should not happen unless the Aggregate hierachy
		was inappropriately modified during traversal).
*/
Depth_Iterator& operator= (const Aggregate::iterator& list_iterator);

/**	Gets a reference to the Parameter at the current position.

	@return	The reference to the Parameter at the current position.
*/
Parameter& operator* () const
	{return *Current_Position;}

/**	Gets a pointer to the Parameter at the current position.

	@return	The pointer to the Parameter at the current position.
*/
Parameter* operator-> () const
	{return Current_Position.operator-> ();}

//	Repositioning.

/**	Pre-increments forward to the next Parameter position.

	If the Parameter at the current position is a non-empty Aggregate,
	the current position is pushed onto a LIFO (last-in-first-out)
	stack and then the current position moves to the first entry in the
	child Aggregate's list.

	If the Parameter at the current position is an Assignment or an
	empty Aggregate, then the current position is moved forward to the
	next position in the Parameter list. If this position is the end of
	the Parameter list, and the list is not for the root Aggregate,
	then the previous position in the parent's Parameter list is popped
	from the stack to the current position and it is moved forward
	again. Ascending up the Aggregate hierarchy continues in this way
	until a non-end list position is found, or the end of the root
	Aggregate is encountered.

	<b>Caution</b>: Like any iterator, care must be taken not to
	inappropriately modify the container structure being traversed or the
	integrity of the Depth_Iterator will be lost. The Aggregate hierachy
	of the Depth_Iterator's current position is the primary issue. Each
	iterator, saved on the stack, to a previous position in the Aggregate
	heirarchy must remain consistent; the location of its Aggregate in
	the Parameter list must remain unchanged and the parent of the
	Aggregate must remain unchanged. Changes to the current Parameter
	list may be made as long as the current position of the
	Depth_Iterator remains valid; e.g. assigning back to the
	Depth_Iterator the iterator returned from an insert (or poke) or
	erase (or pull) using the current Depth_Iterator. Modfications
	anywhere in the Depth_Iterator scope beyond any current hierachy path
	position is safe; e.g. push_back or poke_back is always OK. But
	modifications at or before any current hierachy path position is
	almost always going to disrupt a Depth_Iterator.

	@return	This Depth_Iterator.
	@throws	Out_of_Range	If the current position is the end of the
		root Aggregate.
	@throws	Error			If the Depth_Iterator is invalid (doesn't
		have a root Aggregate). An Error will also be thrown if, on
		attempting to ascend the Aggregate hierarchy, a non-root
		Aggregate is encountered without a parent (this should not happen
		unless the Aggregate hierachy was inappropriately modified during
		traversal).
*/
Depth_Iterator& operator++ ();

/*	Moves to the next sibling Parameter position.

	If the Parameter at the current position is a non-empty Aggregate the
	Depth_Iterator does not descend into its Parameter list. This is the
	same as the pre-increment operator of a normal interator.

	If the current position is the end of a Parameter list then the
	Depth_Iterator will ascend to the next available Parameter or the
	end of the root Aggregate.

	@return	This Depth_Iterator.
	@throws	Out_of_Range	If the current position is the end of the
		root Aggregate.
	@throws	Error			If the Depth_Iterator is invalid (doesn't
		have a root Aggregate). An Error will also be thrown if, on
		attempting to ascend the Aggregate hierarchy, a non-root
		Aggregate is encountered without a parent (this should not happen
		unless the Aggregate hierachy was inappropriately modified during
		traversal).
	@see	operator++();
*/
Depth_Iterator& next_sibling ();

/**	Post-increments forward to the next Parameter position.

	@return A copy of the Depth_Iterator before it was incremented.
	@see	operator++();
*/
Depth_Iterator  operator++ (int);

/**	Pre-decrements backward to the previous Parameter position.

	If the current position is not the beginning of a Parameter list, the
	current position moves to the previous Parameter in the list. If this
	Parameter is a non-empty Aggregate, the current position is pushed
	onto a LIFO (last-in-first-out) stack and then the current position
	moves to the last entry (the Parameter immediately preceeding the end
	position) in the child Aggregate's list. If this Parameter is a
	non-empty Aggregate then the currrent positionis pushed onto the
	stack again and the current position becomes the last entry in this
	Aggregate. This back-down cycle continues until an Assignment, or
	empty Aggregate, Parameter is encountered.

	If the current position is the beginning of a Parameter list, and it
	is not the root Aggregate's Parameter list,	then the previous
	position in the parent's Parameter list is popped from the stack to
	the current position.

	@return	This Depth_Iterator.
	@throws Out_of_Range	If the current position is the beginning of the
		root Aggregate.
	@throws	Error			If the Depth_Iterator is invalid (doesn't
		have a root Aggregate). An Error will also be thrown if, on
		attempting to ascend the Aggregate hierarchy, a non-root
		Aggregate is encountered without a parent (this should not happen
		unless the Aggregate hierachy was inappropriately modified during
		traversal).
	@see	operator++()
*/
Depth_Iterator& operator-- ();

/*	Moves to the previous sibling Parameter position.

	If the Parameter at the previous position is a non-empty Aggregate
	the Depth_Iterator does not descend into its Parameter list. This is
	the same as the pre-decrement operator of a normal interator.

	If the current position is the beginning of a Parameter list then the
	Depth_Iterator will ascend to the previously available Parameter or
	the beginning of the root Aggregate.

	@return	This Depth_Iterator.
	@throws	Out_of_Range	If the current position is the beginning of the
		root Aggregate.
	@throws	Error			If the Depth_Iterator is invalid (doesn't
		have a root Aggregate). An Error will also be thrown if, on
		attempting to ascend the Aggregate hierarchy, a non-root
		Aggregate is encountered without a parent (this should not happen
		unless the Aggregate hierachy was inappropriately modified during
		traversal).
	@see	operator--();
*/
Depth_Iterator& previous_sibling ();

/**	Post-decrements backward to the previous Parameter position.

	@return A copy of the Depth_Iterator before it was incremented.
	@see	operator--();
*/
Depth_Iterator  operator-- (int);

/**	Array-style access to the Parameter at an offset from
	the current position.

	The Depth_Iterator is copied and offset to the new position,
	and then dereferenced.

	@param	offset	The position of the Parameter relative (positive
		or negative) to the current position.
	@return	The Parameter reference at the offset position.
	@see	operator+(int)
*/
Parameter&      operator[] (int offset) const;

/**	Relocates the current position by the specified distance.

	The Depth_Iterator is incremented for a positive distance,
	or decremented for a negative distance, repeatedly the
	absolute amount of the distance.

	<b>Note</b>: This is a random-access iterator operator
	that does not occur in constant time.

	@param	distance	The number of increment (positive) or
		decrement (negative) operations to apply.
	@return	This Depth_Iterator.
	@see	operator++()
	@see	operator--()
*/
Depth_Iterator& operator+= (int distance);

/**	Gets a Depth_Iterator for a position at some distance from
	the current position.

	A copy of this Depth_Iterator is offset the distance.

	@param	distance	The offset of the new Depth_Iterator.
	@return	A Depth_Iterator at the new position.
	@see	operator+=(int)
*/
Depth_Iterator  operator+  (int distance) const;

/**	Relocates the current position by the specified distance.

	@param	distance	The number of increment (negative) or
		decrement (positive) operations to apply.
	@return	This Depth_Iterator.
	@see	operator+=()
*/
Depth_Iterator& operator-= (int distance);

/**	Gets a Depth_Iterator for a position at some distance from
	the current position.

	@param	distance	The offset of the new Depth_Iterator.
	@return	A Depth_Iterator at the new position.
	@see	operator+(int)
*/
Depth_Iterator  operator-  (int distance) const;

/**	Gets the distance between this and another Depth_Iterator.

	@param	depth_iterator	A Depth_Iterator position to find.
	@return	The distance, in terms of decrement (positive) or
		increment (negative) count, to the depth_iterator position.
	@throw	Out_of_Range	If the depth_iterator is not within
		the scope of this Depth_Iterator.
	@see	operator-(const Aggregate::iterator&)
*/
int operator- (const Depth_Iterator& depth_iterator) const;

/**	Get the distance between this Depth_Iterator and an
	Aggregate list iterator.

	A search is made for the list iterator position in the
	scope of this Depth_Iterator; first backwards (positive
	distance), and then forwards (negative distance).

	<b>Note</b>: This is a random-access iterator operator
	that does not occur in constant time.

	@param	list_iterator	An Aggregate::iterator position
		to find.
	@return	The distance, in terms of decrement (positive) or
		increment (negative) count, to the list_iterator position.
	@throw	Out_of_Range	If the list_iterator is not within
		the scope of this Depth_Iterator.
*/
int operator- (const Aggregate::iterator& list_iterator) const;

//		Logical operators.

/**	Tests if the current positions of this Depth_Iterator and
	another Depth_Iterator are equivalent.

	@param	depth_iterator	The Depth_Iterator to compare against.
	@return	true if the current positions are identical; false otherwise.
*/
bool operator== (const Depth_Iterator& depth_iterator) const
	{
	return
		/*	Safety check:

			The comparison of two iterators on separate containers
			can be unsafe for the MS/Windows MSVC implementation.
		*/
		Current_Aggregate == depth_iterator.Current_Aggregate &&
		Current_Position  == depth_iterator.Current_Position;
	}

/**	Tests if the current positions of this Depth_Iterator and
	another Depth_Iterator are not equivalent.

	@param	depth_iterator	The Depth_Iterator to compare against.
	@return	false if the current positions are identical; true otherwise.
*/
bool operator!= (const Depth_Iterator& depth_iterator) const
	{return ! operator== (depth_iterator);}

/**	Tests if the current position of this Depth_Iterator and
	an Aggregate list iterator are equivalent.

	<b>WARNING</b>: This method may result in undefined behaviour if the
	list_iterator argument is not bound to the same Aggregate vector that
	the Depth_Iterator {@link Aggregate::iterator() current position
	iterator} is bound to. If this is possible a Depth_Iterator should
	first be constructed from the list_iterator before the equality test
	is made.

	@param	list_iterator	The Aggregate::iterator to compare against.
	@return	true if the positions are identical; false otherwise.
*/
bool operator== (const Aggregate::iterator& list_iterator) const
	{return Current_Position == list_iterator;}

/**	Tests if the current position of this Depth_Iterator and
	an Aggregate list iterator are not equivalent.

	@param	list_iterator	The Aggregate::iterator to compare against.
	@return	false if the positions are identical; true otherwise.
*/
bool operator!= (const Aggregate::iterator& list_iterator) const
	{return ! operator== (list_iterator);}

/**	Tests if the position of this Depth_Iterator is less than the
	position of another Depth_Iterator.

	@param	depth_iterator	The Depth_Iterator to compare against.
	@return	true if this Depth_Iterator preceeds the position of the
		other Depth_Iterator; flase otherwise.
	@see	operator<(const Aggregate::iterator&)
*/
bool operator<  (const Depth_Iterator& depth_iterator) const;

/**	Tests if the current position of this Depth_Iterator is
	less than the position of an Aggregate list iterator.

	A copy of this Depth_Iterator is incremented forward searching
	for the list iterator position.

	<b>Note</b>: This is a random-access iterator operator that does
	not occur in constant time. This is always more, often much more,
	expensive that the == (or !=) test. The latter should be used
	whenever possible (e.g. in loop conditions).

	@param	list_iterator	The Aggregate::iterator to compare against.
	@return	true if the list_iterator position is found; false otherwise.
*/
bool operator<  (const Aggregate::iterator& list_iterator) const;

/**	Tests if the position of this Depth_Iterator is greater than the
	position of another Depth_Iterator.

	@param	depth_iterator	The Depth_Iterator to compare against.
	@return	true if the other Depth_Iterator is less than 
		this Depth_Iterator; false otherwise.
	@see	operator<(const Aggregate::iterator&)
*/
bool operator>  (const Depth_Iterator& depth_iterator) const
	{return depth_iterator < *this;}

/**	Tests if the position of this Depth_Iterator is less than or
	equal to the position of another Depth_Iterator.

	@param	depth_iterator	The Depth_Iterator to compare against.
	@return	true if the other Depth_Iterator is not less than this
		Depth_Iterator; false otherwise.
	@see	operator<(const Aggregate::iterator&)
*/
bool operator<= (const Depth_Iterator& depth_iterator) const
	{return ! (depth_iterator < *this);}

/**	Tests if the position of this Depth_Iterator is greater than or
	equal to the position of another Depth_Iterator.

	@param	depth_iterator	The Depth_Iterator to compare against.
	@return	true if this Depth_Iterator is less than the other
		Depth_Iterator; false otherwise.
	@see	operator<(const Aggregate::iterator&)
*/
bool operator>= (const Depth_Iterator& depth_iterator) const
	{return ! (*this < depth_iterator);}

/**	Tests if the position of this Depth_Iterator is greater than the
	position of an Aggregate list iterator.

	@param	list_iterator	The Aggregate::iterator to compare against.
	@return	true if this Depth_Iterator is not less than or equal to
		the list_terator; false otherwise.
	@see	operator<(const Aggregate::iterator&)
*/
bool operator>  (const Aggregate::iterator& list_iterator) const
	{return ! (*this <= list_iterator);}

/**	Tests if the position of this Depth_Iterator is less than or
	equal to the position of an Aggregate list iterator.

	@param	list_iterator	The Aggregate::iterator to compare against.
	@return	true if this Depth_Iterator is eqivalent to, or less than, the
		the list_iterator; false otherwise.
	@see	operator<(const Aggregate::iterator&)
*/
bool operator<= (const Aggregate::iterator& list_iterator) const
	{return *this == list_iterator || *this < list_iterator;}

/**	Tests if the position of this Depth_Iterator is greater than or
	equal to the position of an Aggregate list iterator.

	@param	list_iterator	The Aggregate::iterator to compare against.
	@return	true if this Depth_Iterator is not less than the
		list iterator; false otherwise.
	@see	operator<(const Aggregate::iterator&)
*/
bool operator>= (const Aggregate::iterator& list_iterator) const
	{return ! (*this < list_iterator);}

//..............................................................................
private:

Aggregate::iterator
	Current_Position;
Aggregate
	*Current_Aggregate;
std::vector<Aggregate::iterator>
	Previous_Position;
};		//	End of Depth_Iterator class.

/**	Gets a Depth_Iterator positioned at the beginning of this Aggregate.

	@return	A Depth_Iterator.
*/
Depth_Iterator begin_depth ();

/**	Gets a Depth_Iterator positioned at the end of this Aggregate.

	@return	A Depth_Iterator.
*/
Depth_Iterator end_depth ();

//..............................................................................
private:

Type		Subtype;
static Type	Default_Subtype;

};		//	End of Aggregate class.

/**	Tests if the position of an Aggregate list iterator is less than
	the positon of a Depth_Iterator.

	@param	list_iterator	The Aggregate::iterator to compare against.
	@param	depth_iterator	The Depth_Iterator to compare against.
	@return	true if the Depth_Iterator is not greater than or equal to
		the list iterator; false otherwise.
	@see	operator<(const Aggregate::iterator&)
*/
inline bool operator<
	(
	const Aggregate::iterator&			list_iterator,
	const Aggregate::Depth_Iterator&	depth_iterator
	)
{return ! (depth_iterator >= list_iterator);}

/**	Writes an Aggregate::Selection description.

	@param	stream	The ostream where the Selection description will be
		written.
	@param	selection	The Selection to be described.
	@return The ostream.
*/
std::ostream& operator<< (std::ostream& stream,
	const Aggregate::Selection& selection);

/*=*****************************************************************************
	Utility functions
*/
/**	Writes a PVL comment sequence.

	A default Lister, with the single line comments mode set to the
	single_line value, is used to write the comment string. Unless the
	depth is negative, the output will be completed with an end-of-line
	sequence; but the output is always flushed in any case.

	<b>Note</b>: If the comment string is empty, no comment sequence
	is written.

	@param	comment	The string to be written in a comment sequence.
	@param	out		The ostream where the output will be written.
	@param	depth	The indent depth to apply.
	@param	single_line	Whether to use single line (true) or multi-line
		commenting mode.
	@return	The total number of bytes written.
	@see	Lister::write_comment(const std::string, int)
	@see	Lister::single_line_comments(const bool, const Lister::Which_Mode&)
	@see	Lister::flush(int)
*/
int write_comment
	(const std::string& comment, std::ostream& out = std::cout,
		int depth = 0, bool single_line = false);

/**	Gets the basename portion of a pathname.

	The basename is the portion of the string following the last path
	delimiter character. If the pathname ends with a sequence of one or
	more path delimiter characters, these characters are ignored and
	dropped from the result.

	@param	pathname	A Parameter pathname string.
	@return	The basename substring from the pathname.
	@see	Parameter::path_delimiter()
	@see	Parameter::pathname(const Parameter*) const
*/
std::string basename (const std::string& pathname);

/**	Gets the leading path name portion of a pathname.

	The leading path name is the portion of the string up to and
	including the last path delimiter character (or sequence of these
	characters, but only one will be included in the result). If the
	pathname does not contain a path delimiter character, then the
	empty string is returned.

	@param	pathname	A Parameter pathname string.
	@return	The leading path name substring from the pathname.
	@see	pathname(const Parameter*)
	@see	Parameter::path_delimiter()
*/
std::string pathname_to (const std::string& pathname);

/**	Sorts an Aggregate's list iterator range.

	A bubble sort is applied, in place, using the less than (<)
	comparison for the Parameters in the interator range [begin, end).

	@param	begin	The Aggregate::iterator for the first Parameter
		where sorting begins.
	@param	end		The Aggregate::iterator for the position after the
		last Parameter where sorting ends.
	@see	Parameter::operator<(const Parameter&) const
*/
void sort (Aggregate::iterator begin, Aggregate::iterator end);

/**	Sorts the Parameters in the iterator range, and all Parameters
	in all Aggregates contained within the range.

	This is a "deep" sort in that not only are the Parameters in the
	Aggregate of the iterator range sorted, but each Aggregate within
	the range has all of its Parameters sorted. Each Aggregate has its
	Parameters sorted independently (Parameters never leave their
	containing Aggregate). Though the iterator range is defined by a
	pair of Depth_Iterators, which are used to traverse the entire
	Aggregate hierarchy, they must both be positioned in the Parameter
	list of the same Aggregate which thus defines the range of
	Parameters to be sorted in the starting Aggregate.

	@param	begin	The Aggregate::Depth_Iterator for the first Parameter
		where sorting begins.
	@param	end		The Aggregate::Depth_Iterator for the position after the
		last Parameter where sorting ends.
	@throws	Invalid_Argument	If the begin and end iterators are
		not positioned in the same Parameter list.
	@see	sort(Aggregate::iterator, Aggregate::iterator)
*/
void sort (Aggregate::depth_iterator begin, Aggregate::depth_iterator end);

}		//	namespace PVL
}		//	namespace idaeim
#endif	//	idaeim_PVL_Parameter_hh
