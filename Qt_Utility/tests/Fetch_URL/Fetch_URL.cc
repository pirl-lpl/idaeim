/*	Fetch_URL

CVS ID: Fetch_URL.cc,v 1.3 2014/01/27 06:44:39 castalia Exp
*/

#include	"Fetch_URL.hh"

#include	"URL_Fetcher.hh"
using idaeim::URL_Fetcher;

#include	<QApplication>
#include	<QGridLayout>
#include	<QHBoxLayout>
#include	<QLabel>
#include	<QLineEdit>
#include	<QCheckBox>
#include	<QSpinBox>
#include	<QPushButton>
#include	<QProgressBar>
#include	<QUrl>
#include	<QElapsedTimer>

#include	"Qlisters.hh"
#include	"Qstrings.hh"

#include	<string>
#include	<iostream>
#include	<iomanip>
#include	<cassert>
using namespace std;




Fetch_URL::Fetch_URL ()
	:	Fetcher (new URL_Fetcher),
		Timer (new QElapsedTimer)
{
connect (Fetcher,
	SIGNAL (fetch_progress (qint64, qint64)),
	SLOT (progress (qint64, qint64)));
connect (Fetcher,
	SIGNAL (fetched (bool)),
	SLOT (completed (bool)));

QGridLayout
	*layout = new QGridLayout (this);
layout->setColumnStretch (1, 100);

int
	row = 0;
layout->addWidget (new QLabel ("From URL:"), row, 0, Qt::AlignRight);
layout->addWidget (From_URL = new QLineEdit, row, 1);

++row;
layout->addWidget (new QLabel ("To File:"), row, 0, Qt::AlignRight);
layout->addWidget (To_File = new QLineEdit, row, 1);

++row;
layout->addWidget (new QLabel ("Overwrite:"), row, 0, Qt::AlignRight);
layout->addWidget (Allow_Overwrite = new QCheckBox, row, 1, Qt::AlignLeft);
Allow_Overwrite->setChecked (Fetcher->default_allow_overwrite ());

++row;
layout->addWidget (new QLabel ("Wait:"), row, 0, Qt::AlignRight);
Wait = new QSpinBox;
Wait->setRange (0, 100);
Wait->setSuffix (" seconds");
Wait->setValue ((int)Fetcher->default_wait_time () / 1000);
QHBoxLayout
	*wait_layout = new QHBoxLayout;
wait_layout->addWidget (Wait, Qt::AlignLeft);
wait_layout->addWidget (new QLabel ("Elapsed:"), Qt::AlignLeft);
wait_layout->addWidget (Elapsed = new QLabel, Qt::AlignLeft);
wait_layout->addStretch (100);
layout->addLayout (wait_layout, row, 1, Qt::AlignLeft);

++row;
layout->addWidget (Fetch = new QPushButton ("Fetch"), row, 0);
connect (Fetch,
	SIGNAL (pressed ()),
	SLOT (fetch ()));
layout->addWidget (Progress = new QProgressBar, row, 1);
Progress->setHidden (true);
Progress->setRange (0, 0);

++row;
layout->addWidget (new QLabel ("Result:"), row, 0, Qt::AlignRight);
layout->addWidget (Result = new QLabel, row, 1);

++row;
layout->addWidget (new QLabel ("Status:"), row, 0, Qt::AlignRight);
layout->addWidget (Status = new QLabel, row, 1);

++row;
layout->addWidget (Fetched = new QLabel, row, 0, 1, 2);

setMinimumWidth (400);
}


void
Fetch_URL::fetch ()
{

Fetch->setEnabled (false);
Elapsed->clear ();
Result->clear ();
Status->clear ();
Fetched->clear ();

Fetcher->wait_time (Wait->value () * 1000);

QUrl
	URL (From_URL->text ());
Timer->start ();
Fetcher->fetch (URL, To_File->text (),
	Allow_Overwrite->isChecked (), URL_Fetcher::ASYNCHRONOUS);
}


void
Fetch_URL::progress
	(
	qint64	received_bytes,
	qint64	total_bytes
	)
{
int
	received = (int)received_bytes,
	total = (int)total_bytes;
if (total < 0)
	total = 0;
if (Progress->maximum () != total)
	Progress->setMaximum (total);
if (Progress->maximum () != 0)
	Progress->setValue (received);
if (Progress->isHidden ())
	Progress->setVisible (true);
}


void
Fetch_URL::completed
	(
	bool	success
	)
{
Elapsed->setNum ((double)Timer->elapsed () / 1000.0);

int
	result = Fetcher->request_status ();
Result->setText
	(QString::number (result)
		.append (" - ")
		.append (Fetcher->status_description (result)));
int
	status = Fetcher->HTTP_status ();
Status->setText
	(QString::number (status)
		.append (" - ")
		.append (Fetcher->HTTP_status_description ()));

Fetched->setText (success ?
	"<font color=\"green\">Succeeded" : "<font color=\"red\">Failed");

Progress->setHidden (true);
Fetch->setEnabled (true);
}



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

if (arguments_count == 1)
	{
	QApplication
		application (arguments_count, arguments);
	Fetch_URL
		*panel = new Fetch_URL;
	panel->show ();
	return application.exec ();
	}


QCoreApplication
	application (arguments_count, arguments);

QString
	from_URL,
	to_pathname;
char
	*character;
long
	wait_time = URL_Fetcher::default_wait_time () / 1000;;
bool
	allow_overwrite = URL_Fetcher::default_allow_overwrite ();
int
	count;

for (count = 1;
	 count < arguments_count;
	 count++)
	{
	if (arguments[count][0]== '-')
		{
		switch (toupper (arguments[count][1]))
			{
			case 'F':	//	From URL.
				if (++count == arguments_count ||
					*arguments[count] == '-')
					{
					cout << "Missing URL." << endl;
					goto Usage;
					}
				from_URL = arguments[count];
				break;
			case 'T':	//	To pathname.
				if (++count == arguments_count ||
					*arguments[count] == '-')
					{
					cout << "Missing pathname." << endl;
					goto Usage;
					}
				to_pathname = arguments[count];
				break;
			case 'W':	//	Wait.
				if (++count == arguments_count)
					{
					cout << "Missing seconds." << endl;
					goto Usage;
					}
				wait_time = strtol (arguments[count], &character, 0);
				if (*character)
					{
					cout << "Invalid wait time value: "
							<< arguments[count] << endl;
					goto Usage;
					}
				break;
			case 'O':	//	Overwrite.
				allow_overwrite = true;
				break;
			case 'N':	//	No_Overwrite.
				allow_overwrite = false;
				break;
			default:
				cout << "Unknown option: " << arguments[count] << endl;
			case 'H':	//	Help.
				Usage:
				cout << "Usage: " << arguments[0] << endl
					 << "  Options -" << endl
					 << "    -From <URL>" << endl
					 << "      The URL for the source file." << endl
					 << "      This may be a remote system URL"
					 			" or a local filesystem pathname." << endl
					 << "    -To <pathname>" << endl
					 << "      The pathname for the destination file." << endl
					 << "    -Wait <secs>" << endl
					 << "      The maximum time to wait for data to arrive." << endl
					 << "      This is not the total delivery time;"
					 			" the arrival of any data will reset the timer." << endl
					 << "      Default = " << wait_time << " seconds" << endl
					 << "    -[No_]Overwrite" << endl
					 << "      Whether to overwrite the destination file"
					 			" if it already exists." << endl
					 << "      Default = "
						<< (allow_overwrite ? "" : "No_") << "Overwrite" << endl;
				exit (1);
			}
		}
	else
		{
		cout << "Unrecognized argument: " << arguments[count] << endl;
		goto Usage;
		}
	}
if (from_URL.isEmpty ())
	{
	cout << "A URL was not specified." << endl;
	goto Usage;
	}
if (to_pathname.isEmpty ())
	{
	cout << "A pathname was not specified." << endl;
	goto Usage;
	}

URL_Fetcher
	Fetcher;
Fetcher.wait_time (wait_time * 1000);

QUrl
	URL (from_URL);
bool
	OK;
int
	result,
	status;

cout << "      Fetching: " << URL.toString () << endl;
QElapsedTimer
	timer;
timer.start ();
OK = Fetcher.fetch (URL, to_pathname, allow_overwrite);

if (OK)
	{
	qint64
		amount (Fetcher.amount_fetched ());
	cout << "       Fetched: "
			<< idaeim::magnitude_of (amount);
	if (amount > 1023)
		cout << " (" << amount << ')';
	cout << " bytes" << endl;
	}

URL = Fetcher.redirected_URL ();
if (! URL.isEmpty ())
	cout << "    Redirected: " << URL.toString () << endl;

cout << "      Max Wait: " << (Fetcher.wait_time () / 1000) << " seconds" << endl
	 << "       Elapsed: " << (timer.elapsed () / 1000.0) << " seconds" << endl;

result = Fetcher.request_status ();
cout << "Request Status: " << result << " - "
		<< Fetcher.status_description (result) << endl;

status = Fetcher.HTTP_status ();
cout << "   HTTP Status: " << status;
QString
	HTTP_status (Fetcher.HTTP_status_description ());
if (! HTTP_status.isEmpty ())
	cout << " - " << HTTP_status;
cout << endl;

cout << (OK ? "Succeeded" : "Failed") << endl;

exit (0);
}
