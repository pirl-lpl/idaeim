/*	Qistream_test

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

CVS ID: Qistream_test.cc,v 1.4 2013/02/24 22:36:07 castalia Exp
*******************************************************************************/

#include	"Qistream.hh"
using idaeim::Qistream;
using idaeim::Qistreambuf;

#include	"Utility/Checker.hh"
using idaeim::Checker;

#include	<QApplication>
#include	<QWidget>
#include	<QVBoxLayout>
#include	<QLineEdit>
#include	<QTextBrowser>
#include	<QNetworkAccessManager>
#include	<QNetworkReply>
#include	<QFile>
#include	<QByteArray>
#include	<QBuffer>

#include	<string>
#include	<sstream>
#include	<iostream>
#include	<iomanip>
using namespace std;


std::ostream& operator<< (std::ostream& stream, const QString& qstring)
	{return stream << qPrintable (qstring);}


class Qistream_test
:	public QWidget
{
Q_OBJECT

public:

Qistream_test
	(
	const char*	source
	)
	:	QWidget (),
		Source_Field (new QLineEdit (source)),
		Text (new QTextBrowser)
{
QVBoxLayout
	*layout = new QVBoxLayout (this);

layout->addWidget (Source_Field);
Source_Field->setToolTip ("Source pathname or URL");
connect (Source_Field,
	SIGNAL (returnPressed ()),
	SLOT (fetch_source ()));

Text->setTabStopWidth (40);
layout->addWidget (Text);
}


protected slots:

void
fetch_source ()
{
Text->clear ();
Source_Field->setEnabled (false);
QString
	source (Source_Field->text ());
clog << ">>> fetch_source: " << source << endl;
if (! source.isEmpty ())
	{
	Qistream
		*qistream;
	QNetworkReply
		*network_reply (NULL);
	QFile
		*file (NULL);

	if (source.startsWith ("HTTP:", Qt::CaseInsensitive) ||
		source.startsWith ("FILE:", Qt::CaseInsensitive))
		{
		clog << "Getting Network source." << endl;
		network_reply =
			Network_Access_Manager.get (QNetworkRequest (QUrl (source)));
		clog << "network_reply -" << endl
			 << "          isOpen = " << network_reply->isOpen () << endl
			 << "      isReadable = " << network_reply->isReadable () << endl
			 << "    isRunning () = " << network_reply->isRunning () << endl;
		qistream = new Qistream (network_reply);
		}
	else
		{
		clog << "Getting file source." << endl;
		file = new QFile (source);
		if (! file->exists ())
			{
			clog << "No such file: " << source << endl;
			return;
			}
		if (! file->open (QIODevice::ReadOnly))
			{
			clog << "Couldn't open file: " << source << endl;
			return;
			}
		qistream = new Qistream (file);
		}

	source.clear ();
	char
		data[1024];
	int
		amount = -1;
	clog << "--> read -" << endl;
	while (amount)
		{
		qistream->read (data, 1023);
		amount = qistream->gcount ();
		if (amount)
			{
			data[amount] = 0;
			clog << data;
			source.append (data);
			}
		}
	Text->setText (source);

	if (network_reply)
		delete network_reply;
	if (file)
		delete file;
	delete qistream;
	}
Source_Field->setEnabled (true);
clog << endl
	 << "<<< fetch_source" << endl;
}


private:

QNetworkAccessManager
	Network_Access_Manager;
QLineEdit*
	Source_Field;
QTextBrowser*
	Text;

};
#include "Qistream_test.moc"


int
main
	(
	int		count,
	char**	arguments
	)
{
QApplication
	application (count, arguments);

if (count > 1 &&
	*arguments[1] != '-')
	{
	//	Interactive testing.
	Qistream_test
		qstream_test (arguments[1]);
	qstream_test.show ();
	return application.exec ();
	}


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

if (checker.Verbose)
	clog << "Qistreambuf::DEFAULT_BUFFER_SIZE = "
			<< Qistreambuf::DEFAULT_BUFFER_SIZE << endl;
char
	data[Qistreambuf::DEFAULT_BUFFER_SIZE << 1];
int
	datum (-1);
while (datum++ < (int)(sizeof (data)))
	data[datum] = datum % 128;
data[datum] = 0;
QBuffer
	*source (new QBuffer);
source->open (QBuffer::ReadWrite);
source->write (data, sizeof data);
source->seek (0);

Qistream
	*qistream (new Qistream (source));

datum = qistream->tellg ();
checker.check ("tellg position of first character.",
	0, datum);

datum = qistream->peek ();
checker.check ("peek at first character.",
	(int)data[0], datum);

datum = qistream->get ();
checker.check ("get first character.",
	(int)data[0], datum);

datum = qistream->tellg ();
checker.check ("tellg position of second character.",
	1, datum);

datum = qistream->peek ();
checker.check ("peek at second character.",
	(int)data[1], datum);

qistream->unget ();
datum = qistream->peek ();
checker.check ("unget and peek.",
	(int)data[0], datum);

qistream->seekg (100, std::ios::beg);
datum = qistream->tellg ();
if (! checker.check ("seekg (100, std::ios::beg) position.",
	100, datum))
	exit (2);

datum = qistream->get ();
checker.check ("get character at position 100.",
	(int)data[100], datum);

qistream->seekg (64);
datum = qistream->tellg ();
if (! checker.check ("seekg (64) position.",
	64, datum))
	exit (2);

datum = qistream->get ();
checker.check ("get character at position 64.",
	(int)data[64], datum);

count = Qistreambuf::DEFAULT_BUFFER_SIZE + 10;
qistream->seekg (count);
datum = qistream->tellg ();
message << "seekg (" << count << ") position.";
if (! checker.check (message.str (),
	count, datum))
	exit (2);

datum = qistream->get ();
message.str ("");
message << "get character at position " << count << '.';
checker.check (message.str (),
	(int)data[count], datum);

count = sizeof (data) - 10;
qistream->seekg (-10, std::ios::end);
datum = qistream->tellg ();
if (! checker.check ("seekg (-10, std::ios::end) position.",
	count, datum))
	exit (2);

qistream->seekg (64);
datum = qistream->tellg ();
if (! checker.check ("seekg (64) position.",
	64, datum))
	exit (2);

datum = qistream->get ();
checker.check ("get character at position 64.",
	(int)data[64], datum);

cout << endl
	 << "Checks: " << checker.Checks_Total << endl
	 << "Passed: " << checker.Checks_Passed << endl;

exit (0);
}
