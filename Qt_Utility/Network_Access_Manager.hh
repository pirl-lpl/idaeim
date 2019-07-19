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

CVS ID: Network_Access_Manager.hh,v 1.2 2014/01/18 04:46:17 castalia Exp
*******************************************************************************/

#ifndef NETWORK_ACCESS_MANAGER_HH
#define NETWORK_ACCESS_MANAGER_HH

class QNetworkAccessManager;

namespace idaeim
{
/**	The global QNetworkAccessManager pointer.

	This will be NULL until the first time the {@link
	network_access_manager() network access manager} is requested, when a
	new QNetworkAccessManager is constructed.
*/
extern QNetworkAccessManager*
	Network_Access_Manager;

/**	Get the global network access manager.

	<b>N.B.</b>: A QNetworkAccessManager provides asynchronous network
	access operations. These operations require an event loop, which is
	typically provided by the the thread on which the
	QNetworkAccessManager is constructed. Thus if this function is called
	from a GUI application's main thread of execution the GUI event loop
	will be used for managing network access operations. Depending on the
	conditions for a network operation (available bandwidth, size of
	request, etc.) blocking calls (waitForXxx) on the QNetworkReply
	obtained from a network operation may adversely affect the user
	responsiveness of the GUI. Providing slot connections for the
	QNetworkAccessManager and/or QNetworkReply/QIODevice signals is the
	preferred to the use of blocking calls. Alternatively, the global
	QNetworkAccessManager may be constructed on a different thread from
	the thread on which the main GUI event loop is running by all this
	function from the other thread.

	@return	A pointer to a global QNetworkAccessManager.
*/
QNetworkAccessManager* network_access_manager ();

}	//	namespace idaeim

#endif
