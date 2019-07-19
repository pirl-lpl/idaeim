/*	Exception

Copyright (C) 2003 - 2013  Bradford Castalia

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

CVS ID: Exception.hh,v 1.18 2014/01/26 00:47:53 castalia Exp
*******************************************************************************/

#ifndef idaeim_Utility_Exception_hh
#define idaeim_Utility_Exception_hh

#include	<exception>
#include	<stdexcept>
#include	<string>
#include	<ios>


namespace idaeim
{
/**	An <i>Exception</i> thrown by classes in the idaeim namespace.

This class is the root of of the idaeim hierarchy of classes used to
signal types of Exceptions:

<dl>
<dt>Exception
	<dd>The base class of all other idaeim exceptions.
	<dl>
	<dt>Error
		<dd>Associated with an error condition.
		<dl>
		<dt>Invalid_Argument [std::invalid_argument]
			<dd>An invalid or otherwise unrecognized argument value from
				which no local recovery is available.
		<dt>IO_Failure [std::ios_base::failure]
			<dd>A failure condition occured during an I/O operation.
		<dt>Out_of_Range [std::out_of_range]
			<dd>A value is outside of an acceptable range.
		</dl>
	</dl>
</dl>

Each idaeim module is expected to provide a set of one or more Exception
subclasses that more specifically identify exception occurances within
the module.

For all Exceptions a message string should be provided that describes the
reason for the Exception. The message method will return this message,
while the what method will prepend the Exception class ID to the message.

<b>N.B.</b>: An Exception is a subclass of std::exception and the
Exception sublasses are subclasses of std::exception that accept a string
argument for their constructors that initializes the value returned by
their what methods. The Exception class overrides the what method of
std::exception to provide a user specified message and/or class ID.
Nevertheless the subclasses of Exception initialize their base
std::exception subclasses with a string composed of the Exception ID and
the specified caller ID (if specified) which be returned by the
std::exception what method if the Exception class is sliced to its base
class.

@author		Bradford Castalia
@version	1.18 
*/
class Exception
:	public std::exception
{
public:
/*==============================================================================
	Constants
*/
//!	Class identification name with source code version and date.
static const char* const
	ID;

/*==============================================================================
	Constructor
*/
/**	Constructs an Exception containing a message.

	If the message ends with a new-line character ('\\n'), it is removed
	(but only one is removed). If a caller ID is provided (non-NULL and
	not empty), it preceeds the message string separated by a new-line
	character.

	@param	message	The message string. May be empty.
	@param	caller_ID	An optional C-string (NULL terminated char*)
		identifying the source of the Exception. If NULL or empty it
		is not included in the {@link message()const message}.
	@see formatted_message(const std::string&, const char*)
*/
explicit Exception
	(
	const std::string&	message = "",
	const char*			caller_ID = NULL
	);

//!	Exception destructor.
virtual ~Exception () throw ()
	{}

/*==============================================================================
	Accessors
*/
/**	Gets a C-string describing what produced the Exception.

	@return	The C-string (NULL terminated char*) that includes the
		Exception ID as the first line followed by the
		{@link message()const message string}.
*/
virtual const char* what () const throw ();

/**	Gets the user-provided caller ID(s) and message(s).

	The message string begins with the caller ID folowed by a
	new-line character and the message string.

	@return	The message string.
	@see add_message(const std::string&, const char*)
*/
inline std::string message () const throw ()
	{return Message;}

/**	Sets a new message string.

	The message and/or caller ID is {@link
	formatted_message(const std::string&, const char*, std::string::size_type*) formatted
	in the standard Exception format} which is then used to replace the
	existing message.

	@param	message	A string to replace the previous message.
	@param	caller_ID	An optional C-string (NULL terminated char*)
		identifying the source of the Exception. If NULL or empty, it is
		not included.
	@see message()const
*/
Exception& message (const std::string& message, const char* caller_ID = NULL);

/**	Adds a message string to the existing message.

	The specified message and/or caller ID is {@link
	formatted_message(const std::string&, const char*, std::string::size_type*) formatted
	in the standard Exception format} which is then inserted at the
	boundary between any existing caller ID(s) and message(s).

	This method is used to add messages in a chain of Exception handling.
	The resulting {@link message()const message} will be of the form:

	caller ID-0<br>
	caller ID-1<br>
	...<br>
	caller ID-M<br>
	message-N<br>
	...<br>
	message-1<br>
	message-0<br>

	where each caller ID and message is added in sequence, though any
	addition may not include a caller ID and/or message. Thus the caller
	IDs are presented in the order in which they are added, starting with
	the caller ID (if any) used to construct the Exception; the messages
	are presented with the "highest level" message first (last added)
	with other messages expected to have been added at "lower level" code
	locations where greater detail is available. This is typical when
	chained exception handling occurs starting with a specific event
	where the Exception is generated and proceeding through exception
	handlers that catch the Exception and add a message describing the
	activity there and the ID of that location. The intent is to enable
	an Exception to describe both what was happening at each level of
	procesing as well as identifying the location of each of the
	processing levels.

	Note that the number of caller IDs may be different from the number
	of messages.

	Because exception chaining often occurs within a module having the
	same caller ID, when a message is added with a caller ID that
	duplicates the previous caller ID the duplicate is excluded from the
	new message. It is possible, of course, that duplicate caller ID(s)
	may be present from other previous messages; this feature only avoids
	sequential duplicates.

	@param	message	A string to be added to the previous message. If
		the message is empty it is not added.
	@param	caller_ID	An optional C-string (NULL terminated char*)
		identifying the source of the Exception. If NULL or empty, it is
		not added.
	@see message(const std::string&, const char*)
*/
Exception& add_message (const std::string& message, const char* caller_ID = NULL);

/*==============================================================================
	Utility
*/
/**	A formatted Exception message is produced from a message and caller ID.

	The formatted message string has the form:

	[<i>caller ID</i>[\\n]]<br>
	[<i>message</i>]<br>

	If the <i>caller ID</i> is NULL or empty it and the trailing new-line
	character is not included.  If the <i>message</i> has a trailing
	new-line character it is removed (but only one is removed). If the
	effective message string is empty it is not included. When both the
	caller ID and effective message strings are not empty a new-line
	character is provided to separate them.

	@param	message	A message string. If empty it is not included in
		the formatted message string.
	@param	caller_ID	A caller ID C-string. If NULL or empty it is
		not included in the formatted message string.
	@param	message_index	A pointer to a std::string::size_type
		variable which, if the pointer is non-NULL, will be incremented
		by the index within the formatted message of the boundary between
		the caller ID and message.
	@return A formatted Exception message string.
*/
static std::string formatted_message
	(const std::string& message, const char* caller_ID,
		std::string::size_type*	message_index = NULL);

/*==============================================================================
	Data
*/
private:
//!	The Exception message including the caller ID(s) and user message(s).
std::string
	Message;

/**	The index within the Message where the caller ID(s) end and the user
	message(s) begin.
*/
std::string::size_type
	Message_Index;
};

/*=*****************************************************************************
	Error
*/
/**	An Error is for an unrecoverable error condition.

	An Error is a subclass of Exception.
*/
struct Error
:	public Exception
{
/**	Constructs an Error object.

	@param	message	The message string. May be empty. The message is
		prefixed with "Error: " before being used to initialize the
		base Exception.
	@param	caller_ID	An optional C-string (NULL terminated char*)
		identifying the source of the Exception. If NULL or empty it
		is not included in the {@link message()const message}.
*/
explicit Error
	(
	const std::string&	message = "",
	const char*			caller_ID = NULL
	)
	:	Exception (std::string ("Error: ") + message, caller_ID)
	{}
};

/*------------------------------------------------------------------------------
	Invalid_Argument
*/
/**	An Invalid_Argument is for an unrecoverable invalid argument condition.

	<b>N.B.</b>: In addition to being a subclass of Error, Invalid_Argument
	is a subclass of the std::invalid_argument exception class.
*/
struct Invalid_Argument
:	public Error,
	public std::invalid_argument
{
/**	Constructs an Invalid_Argument object.

	The std::invalid_argument base class is initialized with the
	Exception::ID followed by the caller ID (if not empty) on a new line.

	@param	message	The message string. May be empty. The message is
		prefixed with "Invalid_Argument" and a new-line character before
		being used to initialize the base Error.
	@param	caller_ID	An optional C-string (NULL terminated char*)
		identifying the source of the Exception. If NULL or empty it
		is not included in the {@link message()const message}.
*/
explicit Invalid_Argument
	(
	const std::string&	message = "",
	const char*			caller_ID = NULL
	)
	:	Error (std::string ("Invalid_Argument\n") + message, caller_ID),
		std::invalid_argument
			(
			std::string (Exception::ID)
			+ (caller_ID ? (std::string ("\n") + caller_ID) : "")
			)
	{}
};

/*------------------------------------------------------------------------------
	IO_Failure
*/
/**	An IO_Failure is for an unrecoverable I/O failure condtion.

	<b>N.B.</b>: In addition to being a subclass of Error, IO_Failure
	is a subclass of the std::ios_base::failure exception class.
*/
struct IO_Failure
:	public Error,
	public std::ios_base::failure
{
/**	Constructs an IO_Failure object.

	The std::ios_base::failure base class is initialized with the
	Exception::ID followed by the caller ID (if not empty) on a new line.

	@param	message	The message string. May be empty. The message is
		prefixed with "IO_Failure" and a new-line character before
		being used to initialize the base Error.
	@param	caller_ID	An optional C-string (NULL terminated char*)
		identifying the source of the Exception. If NULL or empty it
		is not included in the {@link message()const message}.
*/
explicit IO_Failure
	(
	const std::string&	message = "",
	const char*			caller_ID = NULL
	)
	:	Error (std::string ("IO_Failure\n") + message, caller_ID),
		std::ios_base::failure
			(
			std::string (Exception::ID)
			+ (caller_ID ? (std::string ("\n") + caller_ID) : "")
			)
	{}
};

/*------------------------------------------------------------------------------
	Out_of_Range
*/
/**	An Out_of_Range is for a value found to be out of its allowable range
	and for which no recovery is possible.

	<b>N.B.</b>: In addition to being a subclass of Error, Out_of_Range
	is a subclass of the std::out_of_range exception class.
*/
struct Out_of_Range
:	public Error,
	public std::out_of_range
{
/**	Constructs an Out_of_Range object.

	The std::out_of_range base class is initialized with the
	Exception::ID followed by the caller ID (if not empty) on a new line.

	@param	message	The message string. May be empty. The message is
		prefixed with "Out_of_Range" and a new-line character before
		being used to initialize the base Error.
	@param	caller_ID	An optional C-string (NULL terminated char*)
		identifying the source of the Exception. If NULL or empty it
		is not included in the {@link message()const message}.
*/
explicit Out_of_Range
	(
	const std::string&	message = "",
	const char*			caller_ID = NULL
	)
	:	Error (std::string ("Out_of_Range\n") + message, caller_ID),
		std::out_of_range
			(
			std::string (Exception::ID)
			+ (caller_ID ? (std::string ("\n") + caller_ID) : "")
			)
	{}
};


}		//	namespace idaeim
#endif	//	idaeim_Exception_hh
