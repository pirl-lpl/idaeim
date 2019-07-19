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

CVS ID: PVL_Exceptions.cc,v 1.16 2014/04/17 03:36:00 castalia Exp
*******************************************************************************/

#include	"PVL_Exceptions.hh"
using idaeim::Exception;

#include	<string>
using std::string;
#include	<sstream>
using std::ostringstream;




namespace idaeim
{
namespace PVL
{
/*==============================================================================
	Constants:
*/
const char*
	idaeim::PVL::Invalid_Syntax::ID =
		"idaeim::PVL::Invalid_Syntax (1.16 2014/04/17 03:36:00)";

/*==============================================================================
	Invalid_Syntax
*/
Invalid_Syntax::Invalid_Syntax
	(
	const std::string&	message,
	const int&			location,
	const char*			caller_ID
	)
	:	Exception (),
		Syntax_Location (location)
{
ostringstream
	report;
report << "Invalid PVL Syntax";
if (static_cast<long>(Syntax_Location) >= 0)
	report << " at Location "
	       << static_cast<long>(Syntax_Location);
if (! message.empty ())
	//	The message starts on the next line.
	report << std::endl << message;
Exception::message (report.str (), caller_ID);
}

}		//	namespace PVL
}		//	namespace idaeim
