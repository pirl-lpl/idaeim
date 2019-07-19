/*	Qstrings_test

Copyright (C) 2013 Bradford Castalia, idaeim studio.

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

CVS ID: Qstrings_test.cc,v 1.13 2014/02/24 02:24:41 castalia Exp
*******************************************************************************/

#include	"Qstrings.hh"

#include	"Qlisters.hh"

#include	"Utility/Checker.hh"
using namespace idaeim;

#include	<QCoreApplication>
#include	<QUrl>

#include	<string>
#include	<iostream>
#include	<iomanip>
#include	<cstdlib>
using namespace std;


int
main
	(
	int		count,
	char**	arguments
	)
{
QCoreApplication
	application (count, arguments);

Checker
	checker;
ostringstream
	message;

if (count > 1)
	{
	if (*arguments[1] == '-')
		 arguments[1]++;
	if (*arguments[1] == 'v' ||
		*arguments[1] == 'V')
		checker.Verbose = true;
	}

cout << "--- image_reader_formats -" << endl;
QString
	format;
foreach (format, image_reader_formats ())
	cout << ' ' << format;
cout << endl;
cout << "--- image_reader_formats_file_filters -" << endl
	 << ' ' << image_reader_formats_file_filters () << endl;
cout << "--- image_writer_formats -" << endl;
foreach (format, image_writer_formats ())
	cout << ' ' << format;
cout << endl;
cout << "--- image_writer_formats_file_filters -" << endl
	 << ' ' << image_writer_formats_file_filters () << endl;

cout << endl;

checker.check ("magnitude_of (1024)",
	"1K", magnitude_of (1024).toStdString ());
checker.check ("magnitude_of (2000000)",
	"2M", magnitude_of (2000000).toStdString ());
checker.check ("magnitude_of (1048064)",
	"1M", magnitude_of (1048064).toStdString ());
checker.check ("magnitude_of (3758096384)",
	"4G", magnitude_of (3758096384).toStdString ());

string
	test_string ("abcX123X");
if (checker.Verbose)
	cout << "--- test_string = \"" << test_string << '"' << endl;
checker.check ("uppercase (test_string)",
	"ABCX123X", uppercase (test_string));
checker.check ("remove (test_string, 'X')",
	"abc123", remove (test_string, 'X'));
checker.check ("replace (test_string, 'X', '0')",
	"abc01230", replace (test_string, 'X', '0'));

cout << endl;

checker.check ("is_URL (\"pathname\")",
	false, is_URL ("pathname"));
checker.check ("is_URL (QUrl (\"pathname\"))",
	true, is_URL (QUrl ("pathname")));
checker.check ("is_URL (\"foobar://host/path\")",
	false, is_URL ("foobar://host/path"));

checker.check ("is_URL (\"http://host/path\")",
	true, is_URL ("http://host/path"));
checker.check ("is_URL (\"https://host/path\")",
	true, is_URL ("https://host/path"));
checker.check ("is_HTTP_URL (\"http://host/path\")",
	true, is_HTTP_URL ("http://host/path"));

checker.check ("is_URL (\"ftp://host/path\")",
	true, is_URL ("ftp://host/path"));
checker.check ("is_URL (\"ftps://host/path\")",
	true, is_URL ("ftps://host/path"));
checker.check ("is_HTTP_URL (\"ftp://host/path\")",
	false, is_HTTP_URL ("ftp://host/path"));
checker.check ("is_FTP_URL (\"ftp://host/path\")",
	true, is_FTP_URL ("ftp://host/path"));
checker.check ("is_FTP_URL (\"ftps://host/path\")",
	true, is_FTP_URL ("ftps://host/path"));
checker.check ("is_FTP_URL (\"http://host/path\")",
	false, is_FTP_URL ("http://host/path"));

checker.check ("is_URL (\"jpip://host/path\")",
	true, is_URL ("jpip://host/path"));
checker.check ("is_JPIP_URL (\"jpip://host/path\")",
	true, is_JPIP_URL ("jpip://host/path"));
checker.check ("is_JPIP_URL (\"http://host/path\")",
	false, is_JPIP_URL ("http://host/path"));

checker.check ("is_URL (\"file:///pathname\")",
	true, is_URL ("file:///pathname"));
checker.check ("is_file_URL (\"file:///pathname\")",
	true, is_file_URL ("file:///pathname"));
checker.check ("is_file_URL (\"file://host/pathname\")",
	true, is_file_URL ("file://host/pathname"));
checker.check ("is_file_URL (\"http://host/pathname\")",
	false, is_file_URL ("http://host/pathname"));
checker.check ("is_file_URL (\"file://host\")",
	false, is_file_URL ("file://host"));
checker.check ("is_file_URL (\"pathname\")",
	false, is_file_URL ("pathname"));
checker.check ("is_file_URL (QUrl (\"pathname\"))",
	true, is_file_URL (QUrl ("pathname")));

string
	expected,
	obtained;
expected = "pathname.suffix";
obtained = pathname_suffix ("pathname.ext", "suffix").toStdString ();
checker.check ("pathname_suffix (\"pathname.ext\", \"suffix\")",
	expected, obtained);
expected = "pathname.suffix";
obtained = pathname_suffix ("pathname.ext", "name.suffix").toStdString ();
checker.check ("pathname_suffix (\"pathname.ext\", \"name.suffix\")",
	expected, obtained);
expected = "pathname.";
obtained = pathname_suffix ("pathname.ext", "").toStdString ();
checker.check ("pathname_suffix (\"pathname.ext\", \"\")",
	expected, obtained);
expected = "pathname.and.suffix";
obtained = pathname_suffix ("pathname.and.ext", "suffix").toStdString ();
checker.check ("pathname_suffix (\"pathname.and.ext\", \"suffix\")",
	expected, obtained);
expected = "/path/name.suffix";
obtained = pathname_suffix ("/path/name.ext", "suffix").toStdString ();
checker.check ("pathname_suffix (\"/path/name.ext\", \"suffix\")",
	expected, obtained);
expected = "/path/name.suffix";
obtained = pathname_suffix ("/path/name", "suffix").toStdString ();
checker.check ("pathname_suffix (\"/path/name\", \"suffix\")",
	expected, obtained);
expected = "/path/name";
obtained = pathname_suffix ("/path/name", "").toStdString ();
checker.check ("pathname_suffix (\"/path/name\", \"\")",
	expected, obtained);
expected = "/path/name.suffix";
obtained = pathname_suffix ("/path/name.", "suffix").toStdString ();
checker.check ("pathname_suffix (\"/path/name.\", \"suffix\")",
	expected, obtained);
expected = "/path.dir/name.suffix";
obtained = pathname_suffix ("/path.dir/name", "suffix").toStdString ();
checker.check ("pathname_suffix (\"/path.dir/name\", \"suffix\")",
	expected, obtained);
expected = "http://host.com:80/directory/pathname.suffix";
obtained = pathname_suffix
	("http://host.com:80/directory/pathname.ext", "suffix").toStdString ();
checker.check
	("pathname_suffix (\"http://host.com:80/directory/pathname.ext\", \"suffix\")",
	expected, obtained);
expected = "file://directory/pathname.suffix";
obtained = pathname_suffix
	("file://directory/pathname.ext", "suffix").toStdString ();
checker.check
	("pathname_suffix (\"file://directory/pathname.ext\", \"suffix\")",
	expected, obtained);

expected = "http://host/absolute/location";
obtained = resolved_reference
	("http://host/path/to/file", "/absolute/location").toStdString ();
checker.check ("resolved_reference ("
	"\"http://host/path/to/file\", \"/absolute/location\")",
	expected, obtained);
expected = "http://host/path/to/relative/location";
obtained = resolved_reference
	("http://host/path/to/file", "relative/location").toStdString ();
checker.check ("resolved_reference ("
	"\"http://host/path/to/file\", \"relative/location\")",
	expected, obtained);
expected = "http://host/a/path/resolved";
obtained = resolved_reference
	("http://host/a/path/to/some/file", "../../resolved").toStdString ();
checker.check ("resolved_reference ("
	"\"http://host/path/to/some/file\", \"../../resolved\")",
	expected, obtained);
expected = "../relative/source/path/resolved";
obtained = resolved_reference
	("../relative/source/path/file", "resolved").toStdString ();
checker.check ("resolved_reference ("
	"\"../relative/source/path/file\", \"resolved\")",
	expected, obtained);
expected = "../../relative/source/path/resolved";
obtained = resolved_reference
	("../../relative/source/path/file", "resolved").toStdString ();
checker.check ("resolved_reference ("
	"\"../../relative/source/path/file\", \"resolved\")",
	expected, obtained);

QString
	pathname;
checker.check ("is_pathname (\"/path/to/file\")",
	true, is_pathname ("/path/to/file", &pathname));
expected = "/path/to/file";
obtained = pathname.toStdString ();
checker.check ("is_pathname (\"/path/to/file\") pathname",
	expected, obtained);
checker.check ("is_pathname (\"file:///path/to/file\")",
	true, is_pathname ("file:///path/to/file", &pathname));
expected = "/path/to/file";
obtained = pathname.toStdString ();
checker.check ("is_pathname (\"file:///path/to/file\") pathname",
	expected, obtained);
checker.check ("is_pathname (\"D:\\path\\to\\file\")",
	true, is_pathname ("D:\\path\\to\\file", &pathname));
expected = "D:\\path\\to\\file";
obtained = pathname.toStdString ();
checker.check ("is_pathname (\"D:\\path\\to\\file\") pathname",
	expected, obtained);
pathname = "not a pathname";
expected = pathname.toStdString ();
checker.check ("is_pathname (\"http://host/path/to/file\")",
	false, is_pathname ("http://host/path/to/file", &pathname));
obtained = pathname.toStdString ();
checker.check ("is_pathname (\"http://host/path/to/file\") pathname",
	expected, obtained);

cout << endl
	 << "Checks: " << checker.Checks_Total << endl
	 << "Passed: " << checker.Checks_Passed << endl;

exit (0);
}
