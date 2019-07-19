/*	QIODevice_for_test

CVS ID: QIODevice_for_test.cc,v 1.1 2014/01/19 05:12:28 castalia Exp
*/

#include	"URL_Checker.hh"
using idaeim::QIODevice_for;

#include	<QCoreApplication>
#include	<QFile>
#include	<QNetworkReply>
#include	<QString>

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

QString
	source_name (arguments[1]),
	failure_message;
QIODevice
	*device = QIODevice_for (source_name, &failure_message);
if (device)
	cout << "Got "
			<< (dynamic_cast<QFile*>(device) ? "QFile" : "QNetworkReply")
			<< endl;
else
	cout << failure_message << endl;
exit (0);
}
