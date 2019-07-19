/*	Check_URL

CVS ID: Check_URL.cc,v 1.4 2014/01/21 02:03:04 castalia Exp
*/

#include	"URL_Checker.hh"
using idaeim::URL_Checker;

#include	<QApplication>
#include	<QFormLayout>
#include	<QHBoxLayout>
#include	<QLineEdit>
#include	<QLabel>
#include	<QSpinBox>
#include	<QUrl>
#include	<QElapsedTimer>

#include	"Qlisters.hh"

#include	<string>
#include	<iostream>
#include	<iomanip>
#include	<cassert>
using namespace std;


class Check_URL
: public QWidget
{
Q_OBJECT

public:

Check_URL ()
	:	Checker (new URL_Checker)
{
QFormLayout
	*form_layout = new QFormLayout (this);
form_layout->setFormAlignment (Qt::AlignLeft | Qt::AlignTop);
form_layout->setLabelAlignment (Qt::AlignRight);
form_layout->setFieldGrowthPolicy (QFormLayout::ExpandingFieldsGrow);
form_layout->setRowWrapPolicy (QFormLayout::DontWrapRows);

Selected_URL = new QLineEdit;
connect (Selected_URL,
	SIGNAL (returnPressed ()),
	SLOT (selected ()));
form_layout->addRow ("URL:", Selected_URL);
form_layout->addRow ("Redirect:", Redirect = new QLabel);

QHBoxLayout
	*wait_layout = new QHBoxLayout;
Wait = new QSpinBox;
Wait->setRange (0, 100);
Wait->setSuffix (" seconds");
Wait->setValue ((int)Checker->default_wait_time () / 1000);
wait_layout->addWidget (Wait);
wait_layout->addWidget (new QLabel ("Elapsed:"));
wait_layout->addWidget (Elapsed = new QLabel);
form_layout->addRow ("Wait:", wait_layout);

form_layout->addRow ("Request_Status:", Request_Status = new QLabel);
form_layout->addRow ("HTTP_Status:", HTTP_Status = new QLabel);

form_layout->addRow ("Size:", Size = new QLabel);
form_layout->addRow ("", Check = new QLabel);

setMinimumWidth (400);
}


public slots:

void
selected ()
{
Redirect->clear ();
Elapsed->clear ();
Request_Status->clear ();
HTTP_Status->clear ();
Size->clear ();
Check->clear ();

Checker->wait (Wait->value () * 1000);

QUrl
	URL (Selected_URL->text ());
QElapsedTimer
	timer;
timer.start ();
bool
	OK = Checker->check (URL);

URL = Checker->redirected_URL ();
while (! URL.isEmpty ())
	{
	Redirect->setText (URL.toString ());
	OK = Checker->check (URL);
	URL = Checker->redirected_URL ();
	}
Elapsed->setNum ((double)timer.elapsed () / 1000.0);

int
	result = Checker->request_status ();
Request_Status->setText
	(QString::number (result)
		.append (" - ")
		.append (Checker->status_description (result)));
int
	status = Checker->HTTP_status ();
if (status != Checker->NO_STATUS)
	HTTP_Status->setText
		(QString::number (status)
			.append (" - ")
			.append (Checker->HTTP_status_description ()));

qint64
	content_size (Checker->content_size ());
if (content_size >= 0)
	Size->setText (QString::number (content_size));

Check->setText (OK ?
	"<font color=\"green\">Succeeded" : "<font color=\"red\">Failed");
}


public:

URL_Checker
	*Checker;

QLineEdit
	*Selected_URL;
QSpinBox
	*Wait;
QLabel
	*Redirect,
	*Elapsed,
	*Request_Status,
	*HTTP_Status,
	*Size,
	*Check;
};
#include "Check_URL.moc"



int
main
	(
	int		arguments_count,
	char**	arguments
	)
{
if (arguments_count == 1)
	{
	QApplication
		application (arguments_count, arguments);
	Check_URL
		*panel = new Check_URL;
	panel->show ();
	return application.exec ();
	}


QCoreApplication
	application (arguments_count, arguments);

QStringList
	URL_list;
char
	*character;
long
	wait_time = URL_Checker::default_wait_time ();
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
				wait_time *= 1000;	//	Convert to msecs.
				break;
			case 'H':	//	Help.
				Usage:
				cout << "Usage: " << arguments[0]
						<< " [-Wait <secs>] [<URL> [...]]" << endl;
				exit (1);
			default:
				cout << "Unknown option: " << arguments[count] << endl;
				goto Usage;
			}
		}
	else
		URL_list << arguments[count];
	}
if (URL_list.isEmpty ())
	{
	cout << "A URL was not specified." << endl;
	goto Usage;
	}

URL_Checker
	Checker;
Checker.wait_time (wait_time);

QUrl
	URL;
bool
	OK;
int
	result,
	status;
while (! URL_list.isEmpty ())
	{
	cout << endl;

	URL.setUrl (URL_list.takeFirst (), QUrl::TolerantMode);
	cout << "    Checking URL: " << URL.toString () << endl;
	QElapsedTimer
		timer;
	timer.start ();
	OK = Checker.check (URL);

	URL = Checker.redirected_URL ();
	while (! URL.isEmpty ())
		{
		cout << "      Redirected: " << URL.toString () << endl;
		OK = Checker.check (URL);
		URL = Checker.redirected_URL ();
		}
	cout << "        Max Wait: "
			<< ((double)Checker.wait_time () / 1000.0) << " seconds" << endl
		 << "         Elapsed: "
		 	<< ((double)timer.elapsed () / 1000.0) << " seconds" << endl;

	result = Checker.request_status ();
	cout << "  Request Status: " << result << " - "
		 	<< Checker.status_description (result) << endl;
	status = Checker.HTTP_status ();
	if (status != Checker.NO_STATUS)
		cout << "     HTTP Status: " << status << " - "
			<< Checker.HTTP_status_description () << endl;

	qint64
		size (Checker.content_size ());
	if (size >= 0)
		cout << "            Size: " << size << " bytes" << endl;

	cout << (OK ? "Succeeded" : "Failed") << endl;
	}

exit (0);
}
