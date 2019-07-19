/*	QHostInfo_test

idaeim CVS ID: QHostInfo_test.cc,v 1.3 2013/12/07 04:56:04 castalia Exp

Copyright (C) 2012 - 2013 Bradford Castalia, idaeim studio.

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

*******************************************************************************/

#include	<QCoreApplication>
#include	<QString>
#include	<QList>
#include	<QHostInfo>
#include	<QHostAddress>

#include	<string>
#include	<iostream>
#include	<iomanip>
using namespace std;


std::ostream& operator<< (std::ostream& stream, const QString& qstring)
	{return stream << qPrintable (qstring);}


int
main
	(
	int		count,
	char**	arguments
	)
{
QCoreApplication
	application (count, arguments);
cout << "Qt version - compiled " << QT_VERSION_STR
		<< ", runtime " << qVersion () << endl << endl;

if (count < 2)
	{
	cout << "Usage: " << arguments[0] << " <hostname> [...]" << endl;
	exit (1);
	}

cout << "local host name: " << QHostInfo::localHostName () << endl
	 << "local domain name: " << QHostInfo::localDomainName () << endl
	 << endl;

QHostInfo
	host_info;
QList<QHostAddress>
	addresses;
for (int
		arg = 1;
		arg < count;
		arg++)
	{
	cout << arguments[arg] << " -" << endl;
	host_info = QHostInfo::fromName (arguments[arg]);
	if (host_info.error () == QHostInfo::NoError)
		{
		if (host_info.hostName () != arguments[arg])
			cout << "  hostname: " << host_info.hostName () << endl;
		addresses = host_info.addresses ();
		for (int
				index = 0;
				index < addresses.size ();
				index++)
			cout << "   address: " << addresses[index].toString () << endl;
		}
	else
		cout << "  lookup failed." << endl;
	}

exit (0);
}
