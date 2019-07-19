/*	Synchronized_Event

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

idaeim CVS ID: Synchronized_Event.cc,v 1.3 2014/01/15 05:23:15 castalia Exp


This file is a fork from the UA HiRISE Project source code produced by
Bradford Castalia at the HiRISE Operations Center:

HiROC CVS ID: Synchronized_Event.cc,v 1.1 2012/10/04 02:11:11 castalia Exp

Copyright (C) 2010-2011  Arizona Board of Regents on behalf of the
Planetary Image Research Laboratory, Lunar and Planetary Laboratory at
the University of Arizona.

This library is free software; you can redistribute it and/or modify it
under the terms of the GNU Lesser General Public License, version 2.1,
as published by the Free Software Foundation.

This library is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

*******************************************************************************/

#include	"Synchronized_Event.hh"

#include	<QWaitCondition>
#include	<QMutex>

namespace idaeim
{
const char* const
	Synchronized_Event::ID =
		"idaeim::Synchronized_Event (1.3 2014/01/15 05:23:15)";


Synchronized_Event::Synchronized_Event
	(
	bool	auto_reset
	)
	:	Auto_Reset (auto_reset),
		Event_is_Set (false),
		Wait_Condition (new QWaitCondition)
{}


Synchronized_Event::~Synchronized_Event ()
{
Wait_Condition->wakeAll ();
delete Wait_Condition;
}


void
Synchronized_Event::set ()
{
if (! Event_is_Set)
	{
	Event_is_Set = true;
	if (Auto_Reset)
		Wait_Condition->wakeOne ();
	else
		Wait_Condition->wakeAll ();
	}
}


bool
Synchronized_Event::wait
	(
	QMutex*			mutex,
	unsigned long	timeout
	)
{
bool
	condition = Event_is_Set;
if (! condition)
	{
	if (timeout <= 0)
		timeout = ULONG_MAX;
	condition = Wait_Condition->wait (mutex, timeout);
	if (Auto_Reset)
		Event_is_Set = false;
	}
return condition;
}


}	//	namespace idaeim
