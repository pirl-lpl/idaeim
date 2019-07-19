/*	Internet

Copyright (C) 2014 Bradford Castalia, idaeim studio.

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

CVS ID: Network_Access_Manager.cc,v 1.2 2014/01/18 04:46:56 castalia Exp
*******************************************************************************/

#include	"Network_Access_Manager.hh"

#include	<QNetworkAccessManager>


namespace idaeim
{
QNetworkAccessManager*
	Network_Access_Manager (NULL);

QNetworkAccessManager*
network_access_manager ()
{
if (! Network_Access_Manager)
	Network_Access_Manager = new QNetworkAccessManager;
return Network_Access_Manager;
}

}	//	namespace idaeim
