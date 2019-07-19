/*	Log_Lock

Copyright (C) 2013-2014 Bradford Castalia, idaeim studio.

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

CVS ID: Log_Lock.cc,v 1.3 2014/04/06 23:59:56 castalia Exp
*******************************************************************************/

#include	<QMutex>


namespace idaeim
{
QMutex
	Log_Lock (QMutex::Recursive);

}	//	namespace idaeim
