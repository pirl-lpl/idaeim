/*	Fetch_URL

CVS ID: Fetch_URL.hh,v 1.1 2014/01/15 05:25:18 castalia Exp
*/
#ifndef FETCH_URL_HH
#define FETCH_URL_HH

#include	<QWidget>

namespace idaeim {
class URL_Fetcher;}
using idaeim::URL_Fetcher;

class QLineEdit;
class QCheckBox;
class QSpinBox;
class QElapsedTimer;
class QPushButton;
class QProgressBar;
class QLabel;


class Fetch_URL
:	public QWidget
{
Q_OBJECT

public:

Fetch_URL ();

public slots:

void fetch ();
void progress (qint64 received, qint64 total);
void completed (bool success);


private:

URL_Fetcher
	*Fetcher;

QElapsedTimer
	*Timer;

QLineEdit
	*From_URL,
	*To_File;
QCheckBox
	*Allow_Overwrite;
QSpinBox
	*Wait;
QPushButton
	*Fetch;
QProgressBar
	*Progress;
QLabel
	*Elapsed,
	*Result,
	*Status,
	*Fetched;
};

#endif
