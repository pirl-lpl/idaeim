/*	PVL_Exceptions

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

CVS ID: PVL_Exceptions.hh,v 1.18 2014/04/17 03:36:00 castalia Exp
*******************************************************************************/

#ifndef idaeim_PVL_Exceptions_hh
#define idaeim_PVL_Exceptions_hh

#include	"Utility/Exception.hh"

#include	<string>


namespace idaeim
{
namespace PVL
{
/**	<I>Invalid_Syntax</I> is the base class for PVL exceptions.

	Invalid_Syntax is a subclass of idaeim::Exception which provides for
	a message string describing the reason for the Exception and a caller
	ID the indicates what produced the Exception. For Invalid_Syntax and
	its subclasses an optional location may be specified to indicate
	where the syntax violation occured in the PVL syntax. If specified it
	will be included on the message line that an Invalid_Syntax Exception
	adds to the user's descriptive message.

	An Invalid_Syntax exception usually happens when the Parser
	encounters invalid syntax in a source stream. It can also happen when
	an attempt is made to convert a string to a number but it is not a
	valid representation of the numerical type. Subclasses are used to
	signal the different types of exception conditions:

<dl>
<dt>Invalid_Syntax
	<dd>A PVL syntax specification violation was encountered. These
	exceptions may be recoverable if strict mode is not enabled. This
	Exception will be thrown when an attempt to convert a string to a
	integer or real value fails due to inappropriate characters in the
	string.
	<dl>
	<dt>Aggregate_Closure_Mismatch
		<dd>The End parameter does not match the specific type of the
		Begin parameter for an Aggregate of Parameters.
	<dt>Reserved_Character
		<dd>An item contains a character reserved for special meaning
		in the PVL syntax.
	<dt>Invalid_Aggregate_Value
		<dd>The value assigned to the PVL parameter that begins an
		Aggregate of Parameters is not a text name.
	<dt>Missing_Comment_End
		<dd>A comment end sequence can not be found for a comment begin
		sequence.
	<dt>Multiline_Comment
		<dd>A comment extends across more than one line.
	<dt>Missing_Units_End
		<dd>A units specification end sequence can not be found for a
		units specification begin sequence.
	<dt>Missing_Quote_End
		<dd>A second quoted string character can not be found that
		matches an initial quoted string character.
	<dt>Array_Closure_Mismatch
		<dd>The special character that begins an Array of Values does
		not match the special character that ends the Array. This
		usually due to imbalanced SET or SEQENCE enclosures in the PVL
		syntax.
	<dt>Invalid_Value
		<dd>A valid numeric value could not be formed.
	</dl>
</dl>

	@author		Bradford Castalia
	@version	1.18
	@see		idaeim::Exception
*/
class Invalid_Syntax
:	public Exception
{
public:

//!	Class identification name with source code version and date.
static const char*
	ID;

/**	Creates an Invalid_Syntax Exception optionally containing a
	descriptive message string, the location of the exception, and the
	ID of the caller.

	The Exception message will have the caller ID, if present, on the
	first line. Next will be the "Invalid PVL Syntax" notation followed
	on the same line with the stream location of the exception, if
	present. If a non-empty descriptive message is provided, that is
	appended starting on the next line.

	@param	message	A string desribing the Exception [default: no
		message].
	@param	location	The location where the invalid syntax occured
		[default = -1; no location].
	@param	caller_ID	A C-string (NULL-terminated char*) providing
		the identification of the caller.
*/
explicit Invalid_Syntax
	(
	const std::string&	message = "",
	const int&			location = -1,
	const char*			caller_ID = NULL
	);

/**	Gets the location where the invalid syntax occured.

	@return	A character location in the PVL source.
*/
int location () const throw ()
	{return Syntax_Location;}

private:
//!	The location where the invalid syntax occured.
int
	Syntax_Location;
};

/**	Aggregate closure mismatch.

	The End parameter does not match the specific type of the Begin
	parameter for an Aggregate of Parameters.
*/
struct Aggregate_Closure_Mismatch
:	public Invalid_Syntax
{
Aggregate_Closure_Mismatch
	(
	const std::string&	message = "",
	const int&			location = -1,
	const char*			caller_ID = NULL
	)
	:	Invalid_Syntax
			(std::string ("PVL Aggregate Closure Mismatch\n") + message,
				location, caller_ID)
	{}
};

/**	Reserved character found.

	An item contains a character reserved for special meaning in the
	PVL syntax.
*/
struct Reserved_Character
:	public Invalid_Syntax
{
Reserved_Character
	(
	const std::string&	message = "",
	const int&			location = -1,
	const char*			caller_ID = NULL
	)
	:	Invalid_Syntax
			(std::string ("Reserved PVL Character\n") + message,
				location, caller_ID)
	{}
};

/**	Invalid Aggregate Value

	The value assigned to the PVL parameter that begins an Aggregate of
	Parameters is not a text name.
*/
struct Invalid_Aggregate_Value
:	public Invalid_Syntax
{
Invalid_Aggregate_Value
	(
	const std::string&	message = "",
	const int&			location = -1,
	const char*			caller_ID = NULL
	)
	:	Invalid_Syntax
			(std::string ("Invalid PVL Aggregate Value\n") + message,
				location, caller_ID)
	{}
};

/**	Missing comment end.

	A comment end sequence can not be found for a comment begin
	sequence.
*/
struct Missing_Comment_End
:	public Invalid_Syntax
{
Missing_Comment_End
	(
	const std::string&	message = "",
	const int&			location = -1,
	const char*			caller_ID = NULL
	)
	:	Invalid_Syntax
			(std::string ("Missing PVL Comment End\n") + message,
				location, caller_ID)
	{}
};

/**	Multiline comment.

	A comment extends across more than one line.
*/
struct Multiline_Comment
:	public Invalid_Syntax
{
Multiline_Comment
	(
	const std::string&	message = "",
	const int&			location = -1,
	const char*			caller_ID = NULL
	)
	:	Invalid_Syntax
			(std::string ("Multi-line PVL Comment\n") + message,
				location, caller_ID)
	{}
};

/**	Missing units end.

	A units specification end sequence can not be found for a units
	specification begin sequence.
*/
struct Missing_Units_End
:	public Invalid_Syntax
{
Missing_Units_End
	(
	const std::string&	message = "",
	const int&			location = -1,
	const char*			caller_ID = NULL
	)
	:	Invalid_Syntax
			(std::string ("Missing PVL Units End\n") + message,
				location, caller_ID)
	{}
};

/**	Missing quote end.

	A second quoted string character can not be found that
	matches an initial quoted string character.
*/
struct Missing_Quote_End
:	public Invalid_Syntax
{
Missing_Quote_End
	(
	const std::string&	message = "",
	const int&			location = -1,
	const char*			caller_ID = NULL
	)
	:	Invalid_Syntax
			(std::string ("Missing PVL Quote End\n") + message,
				location, caller_ID)
	{}
};

/**	Array closure mismatch.

	The special character that begins an Array of Values does not match
	the special character that ends the Array. This usually due to
	imbalanced SET or SEQENCE enclosures in the PVL syntax.
*/
struct Array_Closure_Mismatch
:	public Invalid_Syntax
{
Array_Closure_Mismatch
	(
	const std::string&	message = "",
	const int&			location = -1,
	const char*			caller_ID = NULL
	)
	:	Invalid_Syntax
			(std::string ("PVL Array Closure Mismatch\n") + message,
				location, caller_ID)
	{}
};

/**	Invalid Value.

	A valid numeric value could not be formed.
*/
struct Invalid_Value
:	public Invalid_Syntax
{
Invalid_Value
	(
	const std::string&	message = "",
	const int&			location = -1,
	const char*			caller_ID = NULL
	)
	:	Invalid_Syntax
			(std::string ("Invalid PVL Value\n") + message,
				location, caller_ID)
	{}
};


}		//	namespace PVL
}		//	namespace idaeim
#endif	//	idaeim_PVL_Exceptions_hh
