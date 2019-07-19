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

CVS ID:	Parameter.cc,v 1.52 2013/11/07 03:34:31 castalia Exp
*******************************************************************************/

#include	"Parameter.hh"
#include	"Parser.hh"
#include	"Lister.hh"
#include	"PVL_Exceptions.hh"

#include	<string>
using std::string;
#include	<cstring>
#include	<algorithm>
#include	<cctype>
#include	<istream>
using std::istream;
#include	<ostream>
using std::ostream;
#include	<sstream>
using std::ostringstream;
#include	<iomanip>
using std::endl;




namespace idaeim
{
namespace PVL
{
/*******************************************************************************
	Parameter
*/
/*==============================================================================
	Constants:
*/
const char* const
	Parameter::ID =
		"idaeim::PVL::Parameter (1.52 2013/11/07 03:34:31)";

#ifndef PATHNAME_DELIMITER
#define PATHNAME_DELIMITER '/'
#endif
//!	The default delimiter for Parameter pathnames
const char
	Parameter::DEFAULT_PATHNAME_DELIMITER			= PATHNAME_DELIMITER;

#ifndef CASE_SENSITIVITY
#define CASE_SENSITIVITY	true
#endif
const bool
	Parameter::DEFAULT_CASE_SENSITIVITY				= CASE_SENSITIVITY;

/*==============================================================================
	Constructors
*/
Parameter::Parameter
	(
	const std::string&	name
	)
	:	Parent (NULL),
		Name (name),
		Comment ("")
{
//	Reset the local syntax modes.
mode_state (NULL);
}

/**	Parameter copy constructor.
<P>
	The parent of the copied Parameter will not be inherited; i.e.
	the new Paramter will have no parent.
*/
Parameter::Parameter
	(
	const Parameter&	parameter
	)
	:	Parent (NULL),	// Never inherit the parent.
		Name (parameter.Name),
		Comment (parameter.Comment)
{
//	Copy the local syntax modes.
mode_state (parameter.mode_state ());
}

/*==============================================================================
	Accessors
*/
Parameter&
Parameter::operator[]
	(
	unsigned int
	)
{
throw Invalid_Argument
	(
	string ("Can't index ([]) Assignment Parameter - ") + pathname (),
	ID
	);
}

const Parameter&
Parameter::operator[]
	(
	unsigned int
	)
	const
{
throw Invalid_Argument
	(
	string ("Can't index ([]) Assignment Parameter - ") + pathname (),
	ID
	);
}

/*------------------------------------------------------------------------------
	Name and Parent
*/
string
Parameter::pathname
	(
	const Parameter		*root
	)
	const
{
if (Parent == root ||
	//	Special case: CONTAINER_NAME qualifies as default root.
	(! root && Parent->name () == Parser::CONTAINER_NAME))
	{
	return string (1, Path_Delimiter) + Name;
	}
else if (! Parent)
	//	The root_parent is not a parent of the Parameter.
	throw Invalid_Argument
		(
		string ("The pathname for Parameter ") + pathname ()
		+ "\n  doesn't include the root Parameter "
		+ root->pathname () + '.',
		ID
		);
else
	{
	return Parent->pathname (root) + Path_Delimiter + Name;
	}
}

string
Parameter::pathname_to
	(
	const Parameter		*root
	)
	const
{
string
	path = pathname (root);
return path.substr (0, path.rfind (Path_Delimiter) + 1);
}

#ifndef DOXYGEN_PROCESSING
namespace
{
string&
to_uppercase
	(
	string&				a_string
	)
{
//	Uppercase the string in-place.
for (char* character = const_cast<char*>(a_string.c_str ());
	 *character;
	 character++)
	*character = (char)toupper (*character);
return a_string;
}
}	//	Local namespace
#endif	//	DOXYGEN_PROCESSING

bool
Parameter::at_pathname
	(
	std::string			pathname,
	bool				case_sensitive,
	const Parameter		*root
	)
	const
{
string
	this_pathname;
if (! case_sensitive)
	to_uppercase (pathname);
if (pathname.empty () || pathname[0] != Path_Delimiter)
	{
	//	Relative path.
	this_pathname = this->pathname ();
	if (! case_sensitive)
		to_uppercase (this_pathname);
	return
		this_pathname.length () > pathname.length () &&
		this_pathname[this_pathname.length () - pathname.length () - 1]
			== Path_Delimiter &&
		this_pathname.substr (this_pathname.length () - pathname.length ())\
			== pathname;
	}
//	Absolute path.
try {this_pathname = this->pathname (root);}
catch (Invalid_Argument exception)
	{
	return false;
	}
if (! case_sensitive)
	to_uppercase (this_pathname);
return this_pathname == pathname;
}


char
	Parameter::Path_Delimiter = DEFAULT_PATHNAME_DELIMITER;

char
Parameter::path_delimiter
	(
	const char	delimiter
	)
{
char
	previous_delimiter = Path_Delimiter;
Path_Delimiter = delimiter;
return previous_delimiter;
}


bool
	Parameter::Case_Sensitive = DEFAULT_CASE_SENSITIVITY;

bool
Parameter::case_sensitive
	(
	bool	enable
	)
{
bool
	previous_state = Case_Sensitive;
Case_Sensitive = enable;
return previous_state;
}


string
basename
	(
	const std::string&	pathname
	)
{
string::size_type
	last = pathname.find_last_not_of (Parameter::path_delimiter ());
if (last == string::npos)
	//	Empty pathname (possibly only delimiter(s)).
	return "";
string::size_type
	first = pathname.find_last_of (Parameter::path_delimiter (), last);
if (first == string::npos)
	first = 0;
else
	first++;
return pathname.substr (first, ++last - first);
}

string
pathname_to
	(
	const std::string&	pathname
	)
{
string::size_type
	index = pathname.rfind (Parameter::path_delimiter ());
if (index == string::npos)
	//	No path.
	return "";
if (index)
	{
	string::size_type
		last = pathname.find_last_not_of (Parameter::path_delimiter (), index - 1);
	if (last != string::npos)
		index = last + 1;
	}
return pathname.substr (0, ++index);
}

Aggregate*
Parameter::root () const
{
Aggregate
	*parent	= Parent,
	*root_aggregate = NULL;
while (parent)
	{
	root_aggregate = parent;
	parent = root_aggregate->parent ();
	}
return root_aggregate;
}

bool
Parameter::has_children () const
{return is_Aggregate () && dynamic_cast<const Aggregate*>(this)->size ();}

/*------------------------------------------------------------------------------
	Subtype
*/
string
Parameter::type_name
	(
	const Type		type_code
	)
{
switch (type_code)
	{
	case ASSIGNMENT:	return "Assignment";
		case TOKEN:			return "Token";

	case AGGREGATE:		return "Aggregate";
		case GROUP:			return "Group";
		case OBJECT:		return "Object";

	case END:			return "End";
		case END_GROUP:		return "End_Group";
		case END_OBJECT:	return "End_Object";

	default:			return "Invalid";
	};
}

/*==============================================================================
	I/O
*/
ostream&
operator<<
	(
	std::ostream&		out,
	const Parameter&	parameter
	)
{return parameter.print (out);}

unsigned int
Parameter::write
	(
	std::ostream&		out,
	int					depth
	)
	const
{
Lister
	lister (out);
lister.write (*this, depth);
return lister.total ();
}

istream&
operator>>
	(
	std::istream&		in,
	Parameter&			parameter
	)
{
try {parameter.read (in);}
catch (Exception exception)
	{in.setstate (std::ios::failbit);}
return in;
}

/*==============================================================================
	Syntax modes
*/
Parameter::MODE_STATE
	Parameter::DEFAULT_MODES[TOTAL_LOCAL_MODES] =
		{
		VERBATIM_STRINGS,
		INDENTING
		};


//	Gets a MODE_STATE.
Parameter::MODE_STATE
Parameter::mode_state
	(
	const MODE			mode,
	const WHICH_MODE	which
	)
	const
{
if (which != DEFAULT_MODE_SET)
	{
	MODE_STATE
		state = Modes[mode];
	if (which == LOCAL_MODE_SET || state != MODE_INHERITED)
		return state;
	Aggregate
		*parent	= Parent;
	while (parent)
		{
		if ((state = parent->Modes[mode]) != MODE_INHERITED)
			return state;
		parent = parent->parent ();
		}
	}
return DEFAULT_MODES[mode] ? MODE_TRUE : MODE_FALSE;
}


//	Sets a MODE_STATE.
Parameter&
Parameter::mode_state
	(
	const MODE			mode,
	const MODE_STATE	state,
	const WHICH_MODE	which
	)
{
switch (which)
	{
	case DEFAULT_MODE_SET:
		DEFAULT_MODES[mode] = state;
		break;
	case RECURSIVE_MODE_SET:
		if (is_Aggregate ())
			{
			Aggregate::iterator
				location = (static_cast<Aggregate*>(this))->begin (),
				finish   = (static_cast<Aggregate*>(this))->end ();
			while (location != finish)
				(location++)->mode_state (mode, state, which);
			}
		//	Fall through to set the local mode.
	case LOCAL_MODE_SET:
		Modes[mode] = state;
	}
return *this;
}


//	Sets a set of MODE_STATEs.
Parameter&
Parameter::mode_state
	(
	const MODE_STATE* const	modes,
	const WHICH_MODE		which
	)
{
static MODE_STATE
	Built_In_Modes[TOTAL_LOCAL_MODES] =
		{
		VERBATIM_STRINGS,
		INDENTING
		};

switch (which)
	{
	case DEFAULT_MODE_SET:
		for (int mode = 0;
				 mode < TOTAL_LOCAL_MODES;
				 mode++)
			DEFAULT_MODES[mode] = modes ? modes[mode] : Built_In_Modes[mode];
		break;
	case RECURSIVE_MODE_SET:
		if (is_Aggregate ())
			{
			Aggregate::iterator
				location = (static_cast<Aggregate*>(this))->begin (),
				finish   = (static_cast<Aggregate*>(this))->end ();
			while (location != finish)
				(location++)->mode_state (modes, which);
			}
		//	Fall through to set the local modes.
	case LOCAL_MODE_SET:
		for (int mode = 0;
				 mode < TOTAL_LOCAL_MODES;
				 mode++)
			Modes[mode] = modes ? modes[mode] : MODE_INHERITED;
	}
return *this;
}

/*******************************************************************************
	Assignment
*/
//	Constructors

Assignment::Assignment
	(
	const std::string	name
	)
	:	Parameter (name),
		The_Value (new Parameter::Default_Value ()),
		Subtype (ASSIGNMENT)
{}

//		from Parser
Assignment::Assignment
	(
	Parser&				parser,
	const std::string	name
	)
{
Parameter
	*parameter = parser.get_parameter (true /* Assignment Only */);
if (parameter)
	{
	*this = *parameter;
	delete parameter;
	}
else
	{
	The_Value = new Parameter::Default_Value ();
	Subtype = TOKEN;
	}
if (! name.empty ())
	Name = name;
}

//		Copy
Assignment::Assignment
	(
	const Assignment&	assignment
	)
	:	Parameter (assignment),
		The_Value (assignment.The_Value->clone ()),
		Subtype (assignment.Subtype)
{}

//		Copy virtual
Assignment::Assignment
	(
	const Parameter&	parameter
	)
	:	Parameter (parameter)
{
if (parameter.is_Assignment ())
	{
	The_Value = static_cast<const Assignment&>(parameter).The_Value->clone ();
	Subtype = parameter.type ();
	}
else
	{
	The_Value = NULL;
	Subtype = ASSIGNMENT;
	throw Invalid_Argument
		(
		parameter.type_name () + " Parameter " + parameter.name ()
		+ "\n  can't be used to construct an Assignment Parameter.",
		ID
		);
	}
}

/*------------------------------------------------------------------------------
	Subtype
*/
Assignment&
Assignment::type
	(
	const int		type_code
	)
{
if (type_code == static_cast<int>(Parameter::ASSIGNMENT) ||
	type_code == static_cast<int>(Parameter::TOKEN))
	//	It's an Assignment subtype.
	Subtype = static_cast<Parameter::Type>(type_code);
else
	{
	//	Try passing it on to the Value.
	try {The_Value->type (static_cast<const Value::Type>(type_code));}
	catch (Invalid_Argument& exception)
		{
		exception.add_message
			(string ("For the Assignment Parameter ") + name () + '.',
			Parameter::ID);
		throw;
		}
	}
return *this;
}

/*==============================================================================
	Operators
*/
/*------------------------------------------------------------------------------
	Assignment operators
*/
Assignment&
Assignment::operator=
	(
	const Parameter&	parameter
	)
{
if (this != &parameter)
	{
	if (parameter.is_Assignment ())
		{
		//	DO NOT assign the Parent!
		Name = parameter.name ();
		Comment = parameter.comment ();
		//	Copy the local syntax modes.
		mode_state (parameter.mode_state ());
		delete The_Value;
		The_Value = static_cast<const Assignment&>(parameter).The_Value->clone ();
		Subtype = parameter.type ();
		}
	else
		throw Invalid_Argument
			(
			type_name () + " Parameter " + name ()
			+ "\n  can't be assigned the " + parameter.type_name ()
			+ " Parameter " + parameter.name () + '.',
			ID
			);
	}
return *this;
}

Assignment&
Assignment::operator=
	(
	const Value&		value
	)
{
if (The_Value != &value)
	{
	delete The_Value;
	The_Value = value.clone ();
	Subtype = ASSIGNMENT;
	}
return *this;
}

/*------------------------------------------------------------------------------
	Compare
*/
int
Assignment::compare
	(
	const Parameter&	parameter
	)
	const
{
if (this == &parameter)
	return 0;
int
	comparison = Name.compare (parameter.name ());
if (comparison == 0)
	{
	if (parameter.is_Aggregate ())
		//	Aggregates are greater than Assignments of the same name.
		return -1;
	if (Subtype == TOKEN)
		{
		if (static_cast<const Assignment&>(parameter).Subtype == TOKEN)
			return 0;
		else
			//	Tokens are less than Assignments of the same name.
			return -1;
		}

	//	Compare Values.
	return The_Value->compare
		(*static_cast<const Assignment&>(parameter).The_Value);
	}
return comparison;
}

/*------------------------------------------------------------------------------
	Utility functions
*/
int
write_comment
	(
	const std::string&	comment,
	std::ostream&		out,
	int					depth,
	bool				single_line
	)
{
Lister
	lister (out);
lister.single_line_comments (single_line)
	  .write_comment (comment, depth);
unsigned int
	total_written = lister.total () + lister.flush ();
return total_written;
}

/*------------------------------------------------------------------------------
	Read
*/
void
Assignment::read
	(
	std::istream&	in
	)
{
Parser
	parser (in);
read (parser);
}

void
Assignment::read
	(
	Parser&		parser
	)
{
Parameter
	*parameter = parser.get_parameter (true /* Assignment Only */);
if (parameter)
	{
	*this = *parameter;
	delete parameter;
	}
}

/*******************************************************************************
	Aggregate
*/
//	Constructors

Aggregate::Aggregate
 	(
	const std::string	name,
	const Type			type_code
	)
	:	Parameter (name)
{
type (type_code);
}

//		Copy
Aggregate::Aggregate
	(
	const Aggregate&	aggregate
	)
	:	Parameter (aggregate),
		Vectal<Parameter> (),
		Subtype (aggregate.Subtype)
{
/*	Copy in the new Parameter List.

	This is done, rather than using the Parameter_List copy method,
	because the Parent of each cloned Parameter must be set as it is
	placed into the Parameter_List. Note that the cloning method will,
	itself, copy any children of aggregates being cloned.
*/
Parameter_List::const_iterator
	element  = aggregate.begin (),
	stop     = aggregate.end ();
while (element != stop)
	{
	push_back (*element);
	++element;
	}
}

//		Copy virtual
Aggregate::Aggregate
	(
	const Parameter&	parameter
	)
{
if (parameter.is_Aggregate ())
	*this = parameter;
else
	{
	//	Make the parameter the first element of a default Aggregate.
	Subtype = Default_Subtype;
	push_back (parameter);
	}
}

//		Parser
Aggregate::Aggregate
	(
	Parser&				parser,
	const std::string	name
	)
	:	Parameter (name),
		Subtype (Default_Subtype)
{parser.add_parameters (*this);}

//	Destructor
Aggregate::~Aggregate ()
{
clear ();
}

/*------------------------------------------------------------------------------
	Subtype
*/
#ifndef DEFAULT_AGGREGATE_SUBTYPE
#define DEFAULT_AGGREGATE_SUBTYPE	GROUP
#endif
Parameter::Type
Aggregate::Default_Subtype = Parameter::DEFAULT_AGGREGATE_SUBTYPE;

Parameter::Type
Aggregate::default_type
	(
	const int		type_code
	)
{
Type
	default_subtype = Default_Subtype;
if (type_code == static_cast<int>(Parameter::GROUP) ||
	type_code == static_cast<int>(Parameter::OBJECT))
	Default_Subtype = static_cast<Parameter::Type>(type_code);
else
	{
	ostringstream
		message;
	message << "The " << Parameter::type_name
				(static_cast<Parameter::Type>(type_code))
			<< " Type (" << type_code
			<< ") is an inappropriate default for an Aggregate Parameter.";
	throw Invalid_Argument (message.str (), Parameter::ID);
	}
return default_subtype;
}

Aggregate&
Aggregate::type
	(
	const int		type_code
	)
{
if (type_code == static_cast<int>(Parameter::GROUP) ||
	type_code == static_cast<int>(Parameter::OBJECT))
	Subtype = static_cast<Parameter::Type>(type_code);
else
	{
	ostringstream
		message;
	message << "The " << Parameter::type_name
				(static_cast<Parameter::Type>(type_code))
			<< " Type (" << type_code
			<< ") is inappropriate for an Aggregate Parameter.";
	throw Invalid_Argument (message.str (), Parameter::ID);
	}
return *this;
}

Value&
Aggregate::value () const
{
throw Invalid_Argument
	(
	type_name () + " Parameter " + name () + " does not have a Value.",
	Parameter::ID
	);
}

/*==============================================================================
	Operators
*/
/*------------------------------------------------------------------------------
	Assignment operators
*/
Aggregate&
Aggregate::operator=
	(
	const Parameter&	parameter
	)
{
if (this != &parameter)
	{
	if (parameter.is_Aggregate ())
		{
		//	DO NOT assign the Parent!
		Comment = parameter.Comment;
		Name = parameter.Name;
		//	Copy the local syntax modes.
		mode_state (parameter.mode_state ());
		Subtype = parameter.type ();
		/*
			Copy the old Parameter List, but not the Parameters.
			Remove the old Parameter List.
			Copy in the Parameters from the new List.

			This is done, rather than using the Parameter_List copy
			method, because the Parent of each cloned Parameter must be
			set as it is placed into the Parameter_List.  The old list
			is set-aside without deleting it, as a copy of the
			Parameter pointers, in case the parameter being assigned is
			enclosed within this Aggregate. Note that the cloning
			method will, itself, copy any children of aggregates being
			cloned.
		*/
		//	Copy the old Parameter List (just the pointers).
		std::vector<Parameter*>
			list (vector_base ());
		//	Remove the old Parameter pointers but don't delete the Parameters.
		wipe ();
		//	Copy the source Parameters into this Aggregate.
		copy_in (static_cast<const Aggregate&>(parameter));
		//	Delete the old Parameters.
		for (std::vector<Parameter*>::iterator
				first = list.begin (),
				last  = list.end ();
			 first != last;
			 ++first)
			delete *first;
		}
	else
		//	Append a copy of the Assignment to the Parameter_List.
		push_back (parameter);
	}
return *this;
}

Aggregate&
Aggregate::operator=
	(
	const Value&
	)
{
throw Invalid_Argument
	(
	type_name () + " Parameter " + name () + "\n  can't be assigned a Value.",
	Parameter::ID
	);
}

/*------------------------------------------------------------------------------
	Compare
*/
int
Aggregate::compare
	(
	const Parameter&	parameter
	)
	const
{
if (this == &parameter)
	{
	return 0;
	}
int
	comparison = Name.compare (parameter.name ());
if (comparison != 0)
	{
	return comparison;
	}
if (parameter.is_Assignment ())
	{
	//	Aggregates are greater than Assignments of the same name.
	return 1;
	}

//	Compare the lists.
const Aggregate&
	aggregate = static_cast<const Aggregate&>(parameter);
comparison = size () - aggregate.size ();
if (comparison == 0)
	{
	//	Compare all elements.
	const_iterator
		stop (end ());
	for (const_iterator
			this_element (begin ()),
			that_element (aggregate.begin ());
			this_element != stop;
			++this_element,
			++that_element)
		{
		comparison = this_element->compare (*that_element);
		if (comparison != 0)
			break;
		}
	}
return comparison;
}

/*------------------------------------------------------------------------------
	Find
*/
Parameter*
Aggregate::find
	(
	const std::string&				pathname,
	bool							case_sensitive,
	int								skip,
	idaeim::PVL::Parameter::Type	type_code
	)
	const
{

if (skip < 0)
	skip = 0;
Aggregate::depth_iterator
	parameter (*this),
	aggregate_end = parameter.end_depth ();
for (;
	 parameter != aggregate_end;
	 ++parameter)
	{
	if (parameter->at_pathname (pathname, case_sensitive, this) &&
		parameter->is (type_code) &&
		! skip--)
		break;
	}
if (parameter == aggregate_end)
	{
	return NULL;
	}
return &(*parameter);
}

/*------------------------------------------------------------------------------
	Select
*/
void
Aggregate::select
	(
	Selection*	selections
	)
	const
{
if (! selections)
	{
	return;
	}

Parameter
	*parameter;
Value
	*value;
Array
	*array;
int
	count,
	array_index,
	amount;

while (selections->Type)
	{
	if (! selections->Variable)
		continue;

	if (selections->Type & Value::ARRAY)
		{
		ostringstream
			message;
		message
			<< "The selection of pathname "
				<< selections->Pathname << endl
			<< "specifies the invalid "
				<< Value::type_name (selections->Type)
				<< " Value Type (" << selections->Type << ')';
		throw Invalid_Argument (message.str (), Parameter::ID);
		}

	if ((parameter = find
			(
			selections->Pathname,
			false,	//	Not case sensitive.
			0,		//	No parameter skipping.
			ASSIGNMENT
			)))
		{
		if (parameter->value ().is_Array ())
			{
			array = dynamic_cast<Array*>(&(parameter->value ()));
			amount = array->size ();
			}
		else
			{
			array = NULL;
			amount = 1;
			}

		if (selections->Count <= 0)
			selections->Count  = 1;

		for (array_index = 0,
			 count = 0;
			 count < selections->Count &&
			 array_index < amount;
			 array_index++)
			{
			if (array)
				value = &(array->at (array_index));
			else
				value = &(parameter->value ());

			if (value->is_Integer () &&
				selections->Type == Value::INTEGER)
				{
				((Value::Integer_type*)selections->Variable)[count++] =
					(Value::Integer_type)(*value);
				}
			else
			if (value->is_Real () &&
				selections->Type == Value::REAL)
				{
				((Value::Real_type*)selections->Variable)[count++] =
					(Value::Real_type)(*value);
				}
			else
			if (value->is_String () &&
				(selections->Type & Value::STRING))
				{
				((Value::String_type*)selections->Variable)[count++] =
					(Value::String_type)(*value);
				}
			}
		}
	++selections;
	}
}

std::ostream&
operator<<
	(
	std::ostream&				stream,
	const Aggregate::Selection&	selection
	)
{
stream
	<< "Selection:" << endl
	<< "  Pathname - " << selection.Pathname << endl
	<< "  Variable @ " << selection.Variable << endl
	<< "      Type = " << Value::type_name (selection.Type) << endl
	<< "     Count = " << selection.Count << endl;
return stream;
}

/*------------------------------------------------------------------------------
	Read
*/
void
Aggregate::read
	(
	std::istream&	in
	)
{
Parser
	parser (in);
read (parser);
}

void
Aggregate::read
	(
	Parser&		parser
	)
{
Parameter
	*parameter = parser.get_parameters ();
if (parameter)
	{
	if (empty () && parameter->is_Aggregate ())
		{
		//	Move the Parameters into this Aggregate.
		move_in (*(static_cast<Aggregate*>(parameter)));
		delete parameter;
		}
	else
		//	Add the Parameter into this Aggregate.
		poke_back (parameter);
	}
}

/*------------------------------------------------------------------------------
	Convenient
*/
Aggregate&
Aggregate::copy_in
	(
	const Aggregate&	aggregate
	)
{
Parameter_List::const_iterator
	element  = aggregate.begin (),
	stop     = aggregate.end ();
while (element != stop)
	{
	push_back (*element);
	++element;
	}
return *this;
}

Aggregate&
Aggregate::move_in
	(
	Aggregate&	aggregate
	)
{
/*	Implementation note:

	The code would be more compact if the parameter pointers were moved
	in a peek, pull, poke_back loop. However, a pull instead of a
	pull_back would be used to maintain the order of the original
	parameters which would entail an unecessary and potentially
	expensive shift-up of all the remaining pointers in the list after
	each pull. To avoid this all the pointers are simply copied, wiped
	from the source aggregate (which also avoids unecessary parent
	pointer NULL-ing) and then poke_back is used to move the copied
	pointers into this aggregate list.
*/
//	Copy the new Parameter List (just the pointers).
std::vector<Parameter*>
	list (aggregate.vector_base ());
//	Remove the pointers but don't delete the Parameters.
aggregate.wipe ();
//	Move the parameter pointers into this Parameter List.
for (std::vector<Parameter*>::const_iterator
		parameter = list.begin (),
		last      = list.end ();
	 parameter != last;
	 ++parameter)
	{
	poke_back (*parameter);	//	Poke it into the destination (Parent reset).
	}
return *this;
}


Parameter*
Aggregate::remove
	(
	iterator	position
	)
{return pull_out (position);}


Parameter*
Aggregate::remove
	(
	size_type	index
	)
{return pull_out (index);}


Parameter*
Aggregate::remove
	(
	Parameter	*parameter
	)
{
if (parameter)
	{
	for (iterator
			position  = begin (),
			last      = end ();
			position != last;
		  ++position)
		{
		if (peek (position) == parameter)
			{
			pull (position);
			return parameter;
			}
		}
	}
return NULL;
}


Parameter*
Aggregate::remove_depth
	(
	Parameter	*parameter
	)
{
if (parameter)
	{
	for (Depth_Iterator
			position  = begin_depth (),
			last      = end_depth ();
			position != last;
		  ++position)
		{
		if (peek (position) == parameter)
			{
			position.aggregate ()->pull (position);
			return parameter;
			}
		}
	}
return NULL;
}

/*------------------------------------------------------------------------------
	Vectal related methods.
*/
void
Aggregate::entering
	(
	Parameter*	parameter
	)
{
Parameter
	*parent = Parent;
while (parent)
	{
	if (parent == parameter)
		throw Invalid_Argument
			(
			string ("Can't enter Parameter \"") + parameter->pathname () + "\"\n"
			+ "  into the list of Aggregate \"" + pathname () + "\"\n"
			+ "  because it is already in the Aggregate's path.",
			Parameter::ID
			);
	parent = parent->Parent;
	}
parameter->Parent = this;
}

void
Aggregate::removing
	(
	Parameter*	parameter
	)
{
parameter->Parent = NULL;
}

Aggregate::size_type
Aggregate::total_size ()
{
int
	total = 0;
if (! empty ())
	{
	for (iterator
			first = begin (),
			last  = end ();
		first != last;
		++first,
		++total)
		if (first->is_Aggregate ())
			total += (static_cast<Aggregate&>(*first)).total_size ();
	}
return total;
}

/*******************************************************************************
	Depth_Iterator
*/
//	Aggregate begin/end methods.

Aggregate::Depth_Iterator
Aggregate::begin_depth ()
{
return Depth_Iterator (*this);
}

Aggregate::Depth_Iterator
Aggregate::end_depth ()
{
Depth_Iterator
	depth_iterator = begin_depth ().end_depth ();
return depth_iterator;
}

/*------------------------------------------------------------------------------
	Constructors/Assignment
*/
Aggregate::Depth_Iterator::Depth_Iterator
	(
	const Aggregate&	aggregate
	)
	:	Current_Position ((const_cast<Aggregate&>(aggregate)).begin ()),
		Current_Aggregate (const_cast<Aggregate*>(&aggregate))
{
}

Aggregate::Depth_Iterator::Depth_Iterator
	(
	const Aggregate::iterator&	list_iterator
	)
	:	Current_Position (list_iterator),
		Current_Aggregate ((*list_iterator).parent ())
{
}

//	End.
Aggregate::Depth_Iterator
Aggregate::Depth_Iterator::end_depth () const
{
Aggregate
	*top = root ();
if (! top)
	throw Error
		("Can't get the end of an invalid Depth_Iterator", Parameter::ID);
Depth_Iterator
	depth_iterator (*top);
depth_iterator.Current_Position = depth_iterator.Current_Aggregate->end ();
return depth_iterator;
}

//	Assignment from iterator.
Aggregate::Depth_Iterator&
Aggregate::Depth_Iterator::operator=
	(
	const Aggregate::iterator&	list_iterator
	)
{
if (Current_Position != list_iterator)
	{
	if (Current_Aggregate->end () == list_iterator)
		{
		Current_Position = list_iterator;
		if (! Previous_Position.empty ())
			{
			if (Current_Aggregate->empty ())
				{
				//	Increment to a valid position.
				do
					{
					/*	Ascend to the Previous_Position in the parent Aggregate.
						The pre-increment will then move to the next Parameter
						in the parent Aggregate's Parameter_List.
					*/
					Current_Position = Previous_Position.back ();
					Previous_Position.pop_back ();
					if (! (Current_Aggregate = Current_Position->parent ()))
						throw Error
							(string ("Depth_Iterator increment found no parent for \"")
							+ Current_Position->name () + "\".",
							Parameter::ID);
					}
					while (++Current_Position == Current_Aggregate->end () &&
							! Previous_Position.empty ());
				}
			else
				//	Back off from the end.
				--Current_Position;
			}
		}
	else if (Current_Aggregate == list_iterator->parent ())
		{
		Current_Position = list_iterator;
		}
	else
		{
		//	Try to find the list_iterator in the scope of this Depth_Iterator.
		Depth_Iterator
			position (*this);
		try
			{
			while (--*this != list_iterator) ;
			}
		catch (Out_of_Range out_of_range)
			{
			try
				{
				*this = position;
				while (++*this != list_iterator) ;
				}
			catch (Out_of_Range exception)
				{
				*this = position;
				throw Invalid_Argument
					(
					string ("Can't assign the Parameter iterator at ")
					+ list_iterator->pathname () + '\n'
					+ "  to the Depth_Iterator at "
					+ Current_Position->pathname ()
					+ " from " + root ()->pathname () + ".\n"
					+ "  The former is not in the scope of the later.",
					Parameter::ID
					);
				}
			}
		}
	}
return *this;
}

/*------------------------------------------------------------------------------
	Accessors
*/
//	Root Aggregate.
Aggregate*
Aggregate::Depth_Iterator::root () const
{
if (Previous_Position.empty ())
	return Current_Aggregate;
else
	return Previous_Position[0]->parent ();
}

/*------------------------------------------------------------------------------
	Incrementors
*/
//	Prefix increment.
Aggregate::Depth_Iterator&
Aggregate::Depth_Iterator::operator++ ()
{
if (! Current_Aggregate)
	throw Error
		("Can't increment an invalid Depth_Iterator", Parameter::ID);
if (! Previous_Position.empty () ||
	Current_Position != Current_Aggregate->end ())
	{
	if (Current_Position->is_Aggregate () &&
		! (static_cast<Aggregate&>(*Current_Position)).empty ())
		{
		/*	The current Parameter is a non-empty Aggregate.
			Save the current position, then
			descend to the first Parameter in this Aggregate.
		*/
		Previous_Position.push_back (Current_Position);
		Current_Aggregate =
			static_cast<Aggregate*>(Current_Position.operator-> ());
		Current_Position = Current_Aggregate->begin ();
		}
	else
		{
		//	Pre-increment to the next position.
		while (++Current_Position == Current_Aggregate->end () &&
				! Previous_Position.empty ())
			{
			/*	End of the current Parameter_List in a child Aggregate.
				Ascend to the Previous_Position in the parent Aggregate.
				The pre-increment will then move to the next Parameter
				in the parent Aggregate's Parameter_List.
			*/
			Current_Position = Previous_Position.back ();
			Previous_Position.pop_back ();
			if (! (Current_Aggregate = Current_Position->parent ()))
				throw Error
					(string ("Depth_Iterator increment found no parent for \"")
					+ Current_Position->name () + "\".",
					Parameter::ID);
			}
		}
	return *this;
	}
throw Out_of_Range
	(
	string ("Can't increment a Depth_Iterator beyond the end\n")
	+ "  of its root Aggregate: " + Current_Aggregate->pathname (),
	Parameter::ID
	);
}


Aggregate::Depth_Iterator&
Aggregate::Depth_Iterator::next_sibling ()
{
if (! Current_Aggregate)
	throw Error
		("No next siblilng for an invalid Depth_Iterator", Parameter::ID);
if (! Previous_Position.empty () ||
	Current_Position != Current_Aggregate->end ())
	{
	//	Pre-increment to the next position.
	while (++Current_Position == Current_Aggregate->end () &&
			! Previous_Position.empty ())
		{
		/*	End of the current Parameter_List in a child Aggregate.
			Ascend to the Previous_Position in the parent Aggregate.
			The pre-increment will then move to the next Parameter
			in the parent Aggregate's Parameter_List.
		*/
		Current_Position = Previous_Position.back ();
		Previous_Position.pop_back ();
		if (! (Current_Aggregate = Current_Position->parent ()))
			throw Error
				(string ("Depth_Iterator next sibling found no parent for \"")
				+ Current_Position->name () + "\".",
				Parameter::ID);
		}
	return *this;
	}
throw Out_of_Range
	(
	string ("A Depth_Iterator has no next sibling beyond the end\n")
	+ "  of its root Aggregate: " + Current_Aggregate->pathname (),
	Parameter::ID
	);
}


//	Postfix increment.
Aggregate::Depth_Iterator
Aggregate::Depth_Iterator::operator++ (int)
{
Depth_Iterator
	depth_iterator = *this;
++*this;
return depth_iterator;
}

//	Prefix decrement.
Aggregate::Depth_Iterator&
Aggregate::Depth_Iterator::operator-- ()
{
if (! Current_Aggregate)
	throw Error
		("Can't decrement an invalid Depth_Iterator", Parameter::ID);
if (! Previous_Position.empty () ||
	Current_Position != Current_Aggregate->begin ())
	{
	if (Current_Position == Current_Aggregate->begin ())
		{
		/*	The current position is the beginning of a child aggregate.
			Ascend to the previous position in the parent Aggregate.
		*/
		Current_Position = Previous_Position.back ();
		Previous_Position.pop_back ();
		if (! (Current_Aggregate = Current_Position->parent ()))
			throw Error
				(string ("Depth_Iterator decrement found no parent for \"")
				+ Current_Position->name () + "\".",
				Parameter::ID);
		}
	else
		{
		//	Pre-decrement to the previous position.
		while ((*--Current_Position).is_Aggregate () &&
				! (static_cast<Aggregate&>(*Current_Position)).empty ())
			{
			/*	Backed onto a non-empty child Aggregate.
				Save the current position, then
				descend to the end of the Parameter_List in this Aggregate.
				The pre-decrement will back to the last Parameter in the
				Aggregate's Parameter_List.
			*/
			Previous_Position.push_back (Current_Position);
			Current_Aggregate =
				static_cast<Aggregate*>(Current_Position.operator-> ());
			Current_Position = Current_Aggregate->end ();
			}
		}
	return *this;
	}
throw Out_of_Range
	(
	string ("Can't decrement a Depth_Iterator beyond the beginning\n")
	+ "  of its root Aggregate: " + Current_Aggregate->pathname (),
	Parameter::ID
	);
}


Aggregate::Depth_Iterator&
Aggregate::Depth_Iterator::previous_sibling ()
{
if (! Current_Aggregate)
	throw Error
		("No previous sibling for an invalid Depth_Iterator", Parameter::ID);
if (! Previous_Position.empty () ||
	Current_Position != Current_Aggregate->begin ())
	{
	//	Pre-decrement to the previous position.
	while (Current_Position == Current_Aggregate->begin () &&
			! Previous_Position.empty ())
		{
		Current_Position = Previous_Position.back ();
		Previous_Position.pop_back ();
		if (! (Current_Aggregate = Current_Position->parent ()))
			throw Error (string
				("Depth_Iterator previous sibling found no parent for \"")
				+ Current_Position->name () + "\".",
				Parameter::ID);
		}
	return *this;
	}
throw Out_of_Range
	(
	string ("A Depth_Iterator has no previous sibling beyond the beginning\n")
	+ "  of its root Aggregate: " + Current_Aggregate->pathname (),
	Parameter::ID
	);
}

//	Postfix decrement.
Aggregate::Depth_Iterator
Aggregate::Depth_Iterator::operator-- (int)
{
Depth_Iterator
	depth_iterator = *this;
--*this;
return depth_iterator;
}

//		Array-style indexing.
Parameter&
Aggregate::Depth_Iterator::operator[]
	(
	int		offset
	) const
{return *(*this + offset);}

//	Offset.
Aggregate::Depth_Iterator&
Aggregate::Depth_Iterator::operator+=
	(
	int		distance
	)
{
if (distance > 0)
	while (distance--)
		++*this;
else
	while (distance++)
		--*this;
return *this;
}

Aggregate::Depth_Iterator
Aggregate::Depth_Iterator::operator+
	(
	int		distance
	) const
{
Depth_Iterator
	depth_iterator (*this);
return depth_iterator += distance;
}

Aggregate::Depth_Iterator&
Aggregate::Depth_Iterator::operator-=
	(
	int		distance
	)
{return operator+= (-distance);}

Aggregate::Depth_Iterator
Aggregate::Depth_Iterator::operator-
	(
	int		distance
	) const
{return operator+ (-distance);}

//	Distance.

int
Aggregate::Depth_Iterator::operator-
	(
	const Aggregate::iterator&	list_iterator
	) const
{
int
	distance;
Depth_Iterator
	position;
try
	{
	for (distance = 0,
		 position = *this;
		 position != list_iterator;
		 --position,
		 ++distance) ;
	}
catch (Out_of_Range exception)
	{
	for (distance = 0,
		 position = *this + 1;
		 position != list_iterator;
		 ++position,
		 --distance) ;
	}
return distance;
}

int
Aggregate::Depth_Iterator::operator-
	(
	const Depth_Iterator&	depth_iterator
	) const
{return operator- (depth_iterator.Current_Position);}

/*------------------------------------------------------------------------------
	Logical operators
*/
bool
Aggregate::Depth_Iterator::operator<
	(
	const Depth_Iterator& 	depth_iterator
	) const
{return *this < depth_iterator.Current_Position;}

bool
Aggregate::Depth_Iterator::operator<
	(
	const Aggregate::iterator&	list_iterator
	) const
{
if (*this == list_iterator)
	return false;
Depth_Iterator
	position (*this);
try
	{
	while (++position != list_iterator) ;
	return true;
	}
catch (Out_of_Range exception) {return false;}
}

/*******************************************************************************
	Utility functions
*/
void
sort
	(
	Aggregate::iterator	first,
	Aggregate::iterator	last
	)
{
if (first == last)
	{
	return;
	}
Aggregate::Base_iterator
	bubble,
	this_parameter;
Parameter
	*parameter;
for (--last;
	 first != last;
	 --last)
	{
	bubble = first;
	do
		{
		this_parameter = bubble;
		 ++bubble;
		if (**bubble < **this_parameter)
			{
			//	Swap Value pointers.
			parameter = *bubble;
			*bubble = *this_parameter;
			*this_parameter = parameter;
			}
		}
		while (bubble != last);
	}
}

void
sort
	(
	Aggregate::depth_iterator	first,
	Aggregate::depth_iterator	last
	)
{
if (first == last)
	{
	return;
	}
if (first->parent () !=
	((static_cast<Aggregate::iterator>(last)) - 1)->parent ())
	throw Invalid_Argument
		("Can't sort a range for depth_iterators in different Aggregates",
		Parameter::ID);
sort (static_cast<Aggregate::iterator>(first),
	  static_cast<Aggregate::iterator>(last));
while (first != last)
	{
	if (first->is_Aggregate ())
		sort (
			(static_cast<Aggregate&>(*first)).begin (),
			(static_cast<Aggregate&>(*first)).end ());
	++first;
	}
}

}		//	namespace PVL
}		//	namespace idaeim
