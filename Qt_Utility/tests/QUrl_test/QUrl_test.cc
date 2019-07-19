/*	QUrl_test
*/

#include	<QCoreApplication>

#include	<QUrl>
#include	<QList>
#include	<QPair>
#include	<QString>

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
if (count == 1)
	{
	cout << "Usage: " << *arguments << " <URL>" << endl;
	exit (1);
	}

QCoreApplication
	application (count, arguments);
cout << "Qt version - compiled " << QT_VERSION_STR
		<< ", runtime " << qVersion () << endl
	 << endl;

QUrl
	URL (arguments [1]);

cout << "URL is " << (URL.isValid () ? "" : "not ") << "valid" << endl;
if (! URL.isValid ())
	cout << "    " << URL.errorString () << endl;
cout << "    scheme: " << URL.scheme () << endl
	 << "      host: " << URL.host () << endl;
if (URL.port () >= 0)
	cout << "      port: " << URL.port () << endl;
cout << "      path: " << URL.path () << endl;

QList<QPair<QString, QString> >
	queries (URL.queryItems ());
QPair<QString, QString>
	query;
if (! queries.isEmpty ())
	{
	cout << "    queries -" << endl;
	foreach (query, queries)
		cout << "      " << query.first << " = " << query.second << endl;
	}
}
