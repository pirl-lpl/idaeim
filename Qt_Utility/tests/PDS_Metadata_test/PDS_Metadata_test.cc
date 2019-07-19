/*	PDS_Metadata_test

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

idaeim CVS ID: PDS_Metadata_test.cc,v 1.6 2014/01/17 07:16:31 castalia Exp


This file is a fork from the UA HiRISE Project source code produced by
Bradford Castalia at the HiRISE Operations Center:

HiROC CVS ID: PDS_Metadata_test.cc,v 1.1 2013/09/01 01:22:08 castalia Exp

Copyright (C) 2012  Arizona Board of Regents on behalf of the
Planetary Image Research Laboratory, Lunar and Planetary Laboratory at
the University of Arizona.

This library is free software; you can redistribute it and/or modify it
under the terms of the GNU Lesser General Public License, version 2.1,
as published by the Free Software Foundation.

This library is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with this library; if not, write to the Free Software Foundation,
Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA.

*******************************************************************************/

#include	"PDS_Metadata.hh"
using idaeim::PDS_Metadata;

#include	"PVL.hh"
using namespace idaeim::PVL;

#include	<QCoreApplication>
#include	<QString>
#include	<QStringList>
#include	<QUrl>

#include	<string>
#include	<iostream>
#include	<iomanip>
#include	<cstdlib>
using namespace std;


std::ostream& operator<< (std::ostream& stream, const QString& qstring)
	{return stream << qPrintable (qstring);}


#define DEFAULT_URL "file:PVL.test"

int
main
	(
	int		arguments_count,
	char**	arguments
	)
{
QCoreApplication
	application (arguments_count, arguments);

QString
	URL_name;
char
	*character;
long
	wait = -1;
QString
	pathname;
bool
	get_string_value (false),
	get_numeric_value (false);

for (int
		count = 1;
		count < arguments_count;
		count++)
	{
	if (arguments[count][0]== '-')
		{
		switch (toupper (arguments[count][1]))
			{
			case 'W':	//	Wait.
				if (++count == arguments_count)
					{
					cout << "Missing msecs to wait." << endl;
					goto Usage;
					}
				wait = strtol (arguments[count], &character, 0);
				if (*character)
					{
					cout << "Invalid wait argument: "
							<< arguments[count] << endl;
					goto Usage;
					}
				break;

			case 'F':	//	Find.
				if (++count == arguments_count)
					{
					cout << "Missing pathname to find." <<endl;
					goto Usage;
					}
				pathname = arguments[count];
				break;

			case 'S':	//	String value.
				get_string_value = true;
				break;

			case 'N':	//	Numeric value.
				get_numeric_value = true;
				break;

			default:
				cout << "Unknown option: " << arguments[count] << endl;

			case 'H':	//	Help.
				Usage:
				cout << "Usage: " << *arguments << " -Help |"
						<< " [-Wait <secs>] [-Find <pathname>]"
						<< " [-String_Value] [-Numeric_Value] [URL]"
						<< endl;
				exit (1);
			}
		}
	else
		URL_name = arguments[count];
	}
if (URL_name.isEmpty ())
	{
	URL_name = DEFAULT_URL;
	cout << "Using the default URL: " << DEFAULT_URL << endl;
	}

PDS_Metadata
	Metadata;
if (wait >= 0)
	Metadata.wait (wait * 1000);

QUrl
	URL (URL_name);
bool
	OK;
int
	result,
	status;

cout << "  Fetching URL: " << URL.toString () << endl;
OK = Metadata.fetch (URL);

URL = Metadata.redirected_URL ();
if (! URL.isEmpty ())
	cout << "Redirected URL: " << URL.toString () << endl;

result = Metadata.request_status ();
cout << "    Accessible: " << boolalpha << OK << endl
	 << "        result: " << result << " - "
		<< Metadata.status_description (result) << endl;
status = Metadata.HTTP_status ();
if (status != Metadata.NO_STATUS)
	cout << "        status: " << status << " - "
		<< Metadata.HTTP_status_description () << endl;

if (Metadata.metadata ())
	{
	cout << "Metadata.metadata -" << endl
		 << *Metadata.metadata ();

	if (! pathname.isEmpty ())
		{
		cout << endl
			 << "find_parameter " << pathname << endl;
		Parameter*
			parameter (Metadata.find_parameter (pathname));
		if (parameter)
			cout << *parameter;
		else
			cout << "Not found." << endl;

		if (get_string_value)
			{
			cout << endl
				 << "string_value: ";
			QString
				string_value;
			try
				{
				string_value = Metadata.string_value (pathname);
				cout << string_value << endl;
				}
			catch (idaeim::Exception except)
				{
				cout << "Exception -" << endl
					 << except.message () << endl;
				}
			}
			
		if (get_numeric_value)
			{
			cout << endl
				 << "numeric_value: ";
			double
				numeric_value;
			try
				{
				numeric_value = Metadata.numeric_value (pathname);
				cout << numeric_value << endl;
				}
			catch (idaeim::Exception except)
				{
				cout << "Exception -" << endl
					 << except.message () << endl;
				}
			}
		}
	}
else
	cout << "No metadata" << endl;

exit (0);
}
