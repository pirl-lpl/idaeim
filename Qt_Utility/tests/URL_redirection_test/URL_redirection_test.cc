/*	URL_redirection_test

CVS ID: URL_redirection_test.cc,v 1.2 2014/02/22 02:33:12 castalia Exp
*/

#include	"URL_Checker.hh"
using idaeim::URL_Checker;

#include	<QApplication>
#include	<QUrl>
#include	<QStringList>

#include	"Qlisters.hh"

#include	<string>
#include	<iostream>
#include	<iomanip>
using namespace std;


int
main
	(
	int		count,
	char**	arguments
	)
{
cout << "Qt version - compiled " << QT_VERSION_STR
		<< ", runtime " << qVersion () << endl;
cout << boolalpha;

if (count != 2)
	{
	cout << "Usage: " << *arguments << " <URL>" << endl;
	exit (1);
	}

QCoreApplication
	application (count, arguments);

QUrl
	URL (arguments[1]);
int
	status;
qint64
	content_size;

cout << "URL_Checker::URL_redirection -" << endl;
QStringList
	URL_list (URL_Checker::URL_redirection (URL, &status, &content_size));
foreach (const QString& url, URL_list)
	cout << url << endl;
cout << "  status = " << URL_Checker::status_description (status)
		<< " (" << status << ')' << endl
	 << "    size = " << content_size << endl;

QString
	source;
cout << "URL_Checker::check -" << endl;
status = URL_Checker::check(arguments[1], &source, &content_size);
cout << "  source = " << source << endl
	 << "  status = " << URL_Checker::status_description (status)
		<< " (" << status << ')' << endl
	 << "    size = " << content_size << endl;

exit (0);
}
