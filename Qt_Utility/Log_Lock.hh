/*	Log_Lock

Copyright (C) 2013-2014 Bradford Castalia, idaeim studio.

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

CVS ID: Log_Lock.hh,v 1.8 2014/04/07 00:00:30 castalia Exp
*******************************************************************************/

#ifndef LOG_LOCK_HH
#define LOG_LOCK_HH

#include <QMutex>
namespace idaeim {
extern QMutex Log_Lock;}

/**	Acquire the log lock.

	The Log_Lock is a global QMutex for use during process logging.

	The mutex operates in recursive mode. This will allow a thread
	to access a section locked by the mutex while the thread holds a
	lock on the mutex from some other section; e.g. a section of
	code covered by a lock on the mutex may call a method that also
	has sections of code covered by a lock on the mutex. The mutex
	will only become unlocked after the same number of unlock
	as lock calls have been called on the mutex; i.e. when the top
	level locked section is unlocked. However, other threads different
	from the thread that holds the mutex lock, will still block when
	they attempt to lock the mutex until the initial thread unlocks
	the mutex.

	The Log_Lock is intended for DEBUG sequences that generate log
	output that might occur while log output is also being generated
	in some other thread. It is not intended to serve as an application
	process lock, which should be provided by a lock specifically for
	the purpose of the application.

	<b>WARNING</b>: Each use of LOCK_LOG must be paired with the use
	of the UNLOCK_LOG macro at the end of the locked sequence.

	An example of its use:

	@code



	void A_Class::a_method (const QString& name, const QStringList* list)
	{
	...

	@endcode

	Alternatively the LOCKED_LOGGING macro can be used for a slightly
	simpler syntax.
*/
#define LOCK_LOG	idaeim::Log_Lock.lock ()

/**	Release the log lock.

	Releases the Log_Lock which must have been set by the use of the
	LOCK_LOG macro.
*/
#define UNLOCK_LOG	idaeim::Log_Lock.unlock ()

/**	Thread safe expression locking.

	The specified expression is preceeded by LOCK_LOG and followed by
	UNLOCK_LOG. This offers a simple syntax for providing a log lock
	for a simple expression.

	For example:

	@code


	@endcode

	It may be necessary to use double enclosing parentheses, instead of
	the single enclose paretheses of the example, if the enclosed
	expression contains a comma that is mistaken for a multiple macro
	arguments separator. Always using double parentheses is safe.
*/
#define	LOCKED_LOGGING(expression)	\
	LOCK_LOG; \
	expression; \
	UNLOCK_LOG

#endif
