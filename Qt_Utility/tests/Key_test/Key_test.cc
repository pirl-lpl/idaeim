/*	Key_test

CVS ID : Key_test.cc,v 1.1 2012/10/04 02:25:23 castalia Exp
*/
#include	<QApplication>
#include	<QLabel>
#include	<QKeyEvent>

#include	<QDebug>

#include	<string>
#include	<sstream>
#include	<iostream>
#include	<iomanip>

using namespace std;


class Key_Monitor
:	public QLabel
{
public:

Key_Monitor ()
	:	QLabel ("Press a key")
{setAlignment (Qt::AlignCenter);}

void
keyPressEvent
	(
	QKeyEvent* event
	)
{
cout << ">>>   keyPressEvent:" << setfill ('0') << hex
		<< " key 0x" << setw (8) << event->key ()
		<< " mod 0x" << setw (8) << event->modifiers ()
		<< (event->isAutoRepeat () ? " repeat" : "") << endl;
}

void
keyReleaseEvent
	(
	QKeyEvent*	event
	)
{
cout << "<<< keyReleaseEvent:" << setfill ('0') << hex
		<< " key 0x" << setw (8) << event->key ()
		<< " mod 0x" << setw (8) << event->modifiers ()
		<< (event->isAutoRepeat () ? " repeat" : "") << endl;
}
};


int
main
	(
	int		count,
	char**	arguments
	)
{
QApplication
	application (count, arguments);

Key_Monitor
	*key_monitor = new Key_Monitor ();
key_monitor->resize (100, 50);
key_monitor->show ();

return application.exec ();
}
