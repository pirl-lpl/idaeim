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

CVS ID: Exception.cc,v 1.16 2014/01/26 00:50:52 castalia Exp
*******************************************************************************/

#include	"Exception.hh"

#include	<exception>
#include	<string>
using std::string;
#include	<cstring>




namespace idaeim
{
/*==============================================================================
	Constants
*/
const char* const
	Exception::ID =
		"idaeim::Exception (1.16 2014/01/26 00:50:52)";

/*==============================================================================
	Constructor
*/
Exception::Exception
	(
	const std::string&	message,
	const char*			caller_ID
	)
	:	Message (),
		Message_Index (0)
{
Message = formatted_message (message, caller_ID, &Message_Index);
}

/*==============================================================================
	Accessors
*/
const char*
Exception::what ()
	const throw ()
{
string
	message_string (ID);
if (! Message.empty ())
	{
	message_string += '\n';
	message_string += Message;
	}
return message_string.c_str ();
}


Exception&
Exception::message
	(
	const std::string&	new_message,
	const char*			caller_ID
	)
{
Message = formatted_message (new_message, caller_ID, &Message_Index);
return *this;
}


Exception&
Exception::add_message
	(
	const std::string&	new_message,
	const char*			caller_ID
	)
{
string::size_type
	index,
	length;

if (caller_ID &&
	(length = strlen (caller_ID)) != 0 &&
	 length <= Message_Index)
	{
	index = Message_Index - length;
	if (Message[Message_Index - 1] == '\n')
		--index;	//	Allow for the NL delimiter.
	if (Message.compare (index, length, caller_ID) == 0)
		{
		//	The last caller ID is duplicated; exclude it.
		caller_ID = NULL;
		}
	}

index = Message_Index;
string
	formatted (formatted_message (new_message, caller_ID, &Message_Index));
if (formatted.length ())
	{
	if (index < Message.length ())
		//	Insertion before EOS.
		formatted += '\n';
	Message.insert
		(index, formatted);
	}
return *this;
}

/*==============================================================================
	Utility
*/
std::string
Exception::formatted_message
	(
	const std::string&		message,
	const char*				caller_ID,
	std::string::size_type*	message_index
	)
{
string
	formatted (message);
string::size_type
	index (message.length ());
if (index &&
	message[--index] == '\n')
	//	Remove trailing new-line character.
	formatted.erase (index);
index = 0;
if (caller_ID &&
	(index = strlen (caller_ID)) != 0)
	{
	if (! formatted.empty ())
		{
		//	Prepend new-line separator.
		formatted.insert (0, 1, '\n');
		++index;
		}
	//	Prepend caller_ID.
	formatted.insert (0, caller_ID);
	}

if (message_index)
	*message_index += index;

return formatted;
}


}		//	namespace idaeim
