/*	URL_Fetch_data_buffer_test

CVS ID: URL_Fetch_data_buffer_test.cc,v 1.1 2014/01/19 12:24:57 castalia Exp
*/

#include	"URL_Fetcher.hh"
using idaeim::URL_Fetcher;

#include	<QCoreApplication>
#include	<QUrl>

#include	"Qlisters.hh"

#include	<string>
#include	<cctype>
#include	<iostream>
#include	<iomanip>
using namespace std;


int
main
	(
	int		arguments_count,
	char**	arguments
	)
{
cout << "Qt version - compiled " << QT_VERSION_STR
		<< ", runtime " << qVersion () << endl;
cout << boolalpha;

QString
	URL_string;
qint64
	amount (64),
	count;
char
	*character;

for (count = 1;
	 count < arguments_count;
	 count++)
	{
	if (arguments[count][0]== '-')
		{
		switch (toupper (arguments[count][1]))
			{
			case 'A':	//	Amount.
				if (++count == arguments_count)
					{
					cout << "Missing seconds." << endl;
					goto Usage;
					}
				amount = strtoll (arguments[count], &character, 0);
				if (*character)
					{
					cout << "Invalid data amouont value: "
							<< arguments[count] << endl;
					goto Usage;
					}
				break;
			default:
				cout << "Unknown option: " << arguments[count] << endl;
			case 'H':	//	Help.
				Usage:
				cout << "Usage: " << *arguments
						<< "[-Amount <amount>] <URL>" << endl;
				exit (1);
			}
		}
	else
		URL_string = arguments[count];
	}
		
if (URL_string.isEmpty ())
	{
	cout << "A URL was not specified." << endl;
	goto Usage;
	}

QCoreApplication
	application (arguments_count, arguments);

QUrl
	URL (URL_string);
char*
	data_buffer = new char[amount];
URL_Fetcher
	URL_fetcher;

cout << "Fetching " << amount << " bytes from " << URL_string << endl;
bool
	OK (URL_fetcher.fetch (URL, data_buffer, amount));
if (OK)
	{
	amount = URL_fetcher.amount_fetched ();
	cout << " Fetched " << amount << " bytes" << endl;
	character = data_buffer;
	for (count = 0;
		 count < amount;
		 count++,
		 character++)
		{
		if (isprint (*character) ||
			*character == '\n' ||
			*character == '\t')
			cout << *character;
		else
			cout << '?';
		}
	cout << endl;
	}

exit (0);
}
