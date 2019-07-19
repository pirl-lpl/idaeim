/*	Vectal_test

Copyright (C) 2002 - 2013  Bradford Castalia

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

CVS ID:	Vectal_test.cc,v 1.15 2013/02/24 22:27:12 castalia Exp
*******************************************************************************/

#include	"Vectal.hh"
#include	"Utility/Checker.hh"
using namespace idaeim;

#include	<iostream>
#include	<sstream>
#include	<string>
#include	<algorithm>
#include	<exception>
#include	<cstdlib>
using namespace std;

Checker
	checker;


//	A virtual base class with pure virtual methods.
struct Virtual
{
static unsigned int Total;	//	total number of Virtuals.
string name;				//	name of this Virtual.

Virtual () : name ("Virtual")
{
++Total;
if (checker.Verbose)
	cout << ">>> New Virtual: " << Total << endl;
}

//	The clone method is required by the Vectal.
virtual Virtual* clone () const = 0;
virtual string identify () const = 0;
bool operator== (const Virtual& a_virtual) const
	{return identify () == a_virtual.identify ();}
virtual ~Virtual () {}	//	To avoid compiler warning.
};
unsigned int Virtual::Total = 0;

//	Virtual output operator.
ostream& operator<< (ostream& out, const Virtual& a_virtual)
{return out << a_virtual.identify ();}


//	Makes a new object name.
string
new_name (const char* const of, int id, int count)
{
ostringstream name;
name << of << ':' << id << '/' << count;
return name.str ();
}


//	An implementation of the Virtual base class.
struct Actual_A
:	public Virtual
{
unsigned int id;			// ID number of this Actual_A.

Actual_A () : id (Total)
{
name = new_name ("Actual_A", id, Total);
if (checker.Verbose)
	cout << ">>> New Actual_A" << identify () << endl;
}

//	Copy from Virtual.
Actual_A (const Virtual& a_virtual) : id (Total)
{
name = new_name ("Actual_A", id, Total) + "<-" + a_virtual.name;
if (checker.Verbose)
	cout << ">>> Copy Actual_A (Virtual " << a_virtual.name << ')'
		 << identify () << endl;
}

//	Copy from Actual_A.
Actual_A (const Actual_A& actual_A) : Virtual (), id (Total)
{
name = new_name ("Actual_A", id, Total) + "<-" + actual_A.name;
if (checker.Verbose)
	cout << ">>> Copy Actual_A (Actual_A " << actual_A.name << ')'
		 << identify () << endl;
}

//	Clone itself.
Virtual* clone () const
{
if (checker.Verbose)
	cout << ">>> Clone Actual_A from " << identify () << endl;
Actual_A* new_actual_A = new Actual_A (*this);
if (checker.Verbose)
	cout << "<<< Clone Actual_A to " << new_actual_A->identify () << endl;
return new_actual_A;
}

string identify () const
{
ostringstream
	identity;
identity
	<< name << " (Actual_A " << id
	<< " of " << Total << " Virtual" << ((Total > 1) ? "s)" : ")");
return identity.str ();
}

virtual ~Actual_A () {}	//	To avoid compiler warning.
};


//	Another implementation of the Virtual base class.
struct Actual_B
: public Virtual
{
unsigned int id;			// ID number of this Actual_B.

Actual_B () : id (Total)
{
name = new_name ("Actual_B", id, Total);
if (checker.Verbose)
	cout << ">>> New Actual_B" << identify () << endl;
}

//	Copy from Virtual.
Actual_B (const Virtual& a_virtual) : id (Total)
{
name = new_name ("Actual_B", id, Total) + "<-" + a_virtual.name;
if (checker.Verbose)
	cout << ">>> Copy Actual_B (Virtual " << a_virtual.name << ')'
		 << identify () << endl;
}

//	Copy from Actual_B.
Actual_B (const Actual_B& actual_B) : Virtual (), id (Total)
{
name = new_name ("Actual_B", id, Total) + "<-" + actual_B.name;
if (checker.Verbose)
	cout << ">>> Copy Actual_B (Actual_B " << actual_B.name << ')'
		 << identify () << endl;
}

//	Clone itself.
Virtual* clone () const
{
if (checker.Verbose)
	cout << ">>> Clone Actual_B from " << identify () << endl;
Actual_B* new_actual_B = new Actual_B (*this);
if (checker.Verbose)
	cout << "<<< Clone Actual_B to " <<  new_actual_B->identify () << endl;
return new_actual_B;
}

string identify () const
{
ostringstream
	identity;
identity
	<< name << " (Actual_B " << id
	<< " of " << Total << " Virtual" << ((Total > 1) ? "s)" : ")");
return identity.str ();
}

virtual ~Actual_B () {}	//	To avoid compiler warning.
};


//	Report the identity of a Virtual class.
void Identify (const Virtual& a_virtual)
{cout << "    " << a_virtual.identify () << endl;}


void Done (int status = 1)
{
cout << endl
	 << "Checks: " << checker.Checks_Total << endl
	 << "Passed: " << checker.Checks_Passed << endl;

exit (status);
}

/*==============================================================================
	Test:
*/
int
main
	(
	int		count,
	char	**arguments
	)
{
cout << "*** Vectal class template test" << endl
	 << Vectal<Virtual>::ID << endl
	 << endl;

ostringstream
	description,
	expected,
	obtained;

if (count > 1)
	{
	if (*arguments[1] == '-')
		 arguments[1]++;
	if (*arguments[1] == 'v' ||
		*arguments[1] == 'V')
		checker.Verbose = true;
	}


if (checker.Verbose)
	cout << "--- Vectal<Virtual> list" << endl;
Vectal<Virtual>
	list;
Virtual
	*the_virtual;

//	Exception
if (checker.Verbose)
	cout << "--- list.resize (1)" << endl;
try
	{
	list.resize (1);
	expected << "An out_of_range exception should have been thrown.";
	}
catch (std::out_of_range except)
	{
	expected
		<< "An out_of_range exception was thrown." << endl
		<< except.what () << endl;
	obtained << expected.str ();
	}
checker.check ("list.resize exception",
	expected, obtained);

if (checker.Verbose)
	cout << "--- Virtual *the_virtual = new Actual_A" << endl;
the_virtual = new Actual_A;

//	Element 0: poke_back, at, []
if (checker.Verbose)
	cout << "--- list.poke_back (the_virtual); list.at (0)" << endl;
list.poke_back (the_virtual);
if (! checker.check ("list.poke_back (the_virtual); list.at (0)",
		(void*)the_virtual, (void*)(&(list.at (0)))) ||
	! checker.check ("list[0]",
		(void*)the_virtual, (void*)(&list[0])))
	Done ();

//	Element 1: push_back
if (checker.Verbose)
	cout << "--- list.push_back (*the_virtual)" << endl;
list.push_back (*the_virtual);
checker.Compare_Equals = false;
if (! checker.check ("list.push_back (*the_virtual)",
		(void*)the_virtual, (void*)(&list[1])))
	Done ();
checker.Compare_Equals = true;

//	front
if (checker.Verbose)
	cout << "--- list.front ()" << endl;
checker.check ("list.front ()",
	(void*)the_virtual, (void*)(&list.front ()));

//	Element 2: clone, poke_back, back
if (checker.Verbose)
	cout << "--- the_virtual = new Actual_B (list[0])" << endl;
the_virtual = new Actual_B (list[0]);
if (checker.Verbose)
	cout << "--- list.poke_back (the_virtual); list.back ()" << endl;
list.poke_back (the_virtual);
if (! checker.check ("list.poke_back (clone of list[0]); list.back ()",
		(void*)the_virtual, (void*)(&list.back ())))
	Done ();

//	Element 3: push_back, back
if (checker.Verbose)
	cout << "--- list.push_back (list.back ())" << endl;
list.push_back (list.back ());
checker.Compare_Equals = false;
if (! checker.check ("list.push_back (list.back ())",
		(void*)the_virtual, (void*)(&list.back ())))
	Done ();
checker.Compare_Equals = true;

//	size
if (checker.Verbose)
	cout << "--- list.size ()" << endl;
if (! checker.check ("list.size () == 4",
		4, (int)list.size ()))
	{
	for (unsigned int count = 0; count < list.size (); count++)
		cout << "--- list[" << count << "].identify (): "
				<< list[count].identify () << endl;
	Done ();
	}

//	iterator
Vectal<Virtual>::iterator
	position;
if (checker.Verbose)
	cout << endl
		 << "--- Vectal<Virtual>::iterator position" << endl
		 << "    for (position = list.begin ();" << endl
		 << "         position != list.end ();" << endl
		 << "         ++position)" << endl;
int
	index = 0;
for (position = list.begin ();
	 position != list.end ();
	 ++position,
	 	++index)
	{
	description.str ("");
	description << "list[" << index << "].identify == position->identify ()";
	checker.check (description.str (),
		list[index].identify (), position->identify ());
	description.str ("");
	description << "list[" << index << "] == *position";
	checker.check (description.str (), list[index], *position);
	}

//	copy list
if (checker.Verbose)
	cout << "--- Vectal<Virtual> *copy = new Vectal<Virtual> (list)" << endl;
Vectal<Virtual>
	*copy = new Vectal<Virtual> (list);
if (checker.Verbose)
	{
	cout << "--- for_each (copy->begin (), copy->end (), Identify)" << endl;
	for_each (copy->begin (), copy->end (), Identify);
	}
if (! checker.check ("copy of list.size () == copy->size ()",
		list.size (), copy->size ()))
	Done ();
if (! checker.check ("copy->at (0) value",
		"Actual_A:5/5<-Actual_A:1/1 (Actual_A 5 of 8 Virtuals)",
		copy->at (0).identify ()))
	Done ();
if (! checker.check ("copy->at (1) value",
		"Actual_A:6/6<-Actual_A:2/2<-Actual_A:1/1 (Actual_A 6 of 8 Virtuals)",
		copy->at (1).identify ()))
	Done ();
if (! checker.check ("copy->at (2) value",
		"Actual_B:7/7<-Actual_B:3/3<-Actual_A:1/1 (Actual_B 7 of 8 Virtuals)",
		copy->at (2).identify ()))
	Done ();
if (! checker.check ("copy->at (3) value",
		"Actual_B:8/8<-Actual_B:4/4<-Actual_B:3/3<-Actual_A:1/1 (Actual_B 8 of 8 Virtuals)",
		copy->at (3).identify ()))
	Done ();
delete copy;

//	repeat element copy
if (checker.Verbose)
	cout << "--- *copy = new Vectal<Virtual> (3, list[0])" << endl;
copy = new Vectal<Virtual> (3, list[0]);
if (! checker.check ("three copies of list[0] copy.size () == 3",
	3, (int)copy->size ()))
	Done ();
if (! checker.check ("copy->at (0) value",
		"Actual_A:9/9<-Actual_A:1/1 (Actual_A 9 of 11 Virtuals)",
		copy->at (0).identify ()))
	Done ();
if (! checker.check ("copy->at (1) value",
		"Actual_A:10/10<-Actual_A:1/1 (Actual_A 10 of 11 Virtuals)",
		copy->at (1).identify ()))
	Done ();
if (! checker.check ("copy->at (2) value",
		"Actual_A:11/11<-Actual_A:1/1 (Actual_A 11 of 11 Virtuals)",
		copy->at (2).identify ()))
	Done ();

//	insert element
if (checker.Verbose)
	cout << "--- copy->insert (copy->begin (), list[0])" << endl;
position = copy->insert (copy->begin (), list[0]);
if (! checker.check ("copy->insert (copy->begin (), list[0]",
	"Actual_A:12/12<-Actual_A:1/1 (Actual_A 12 of 12 Virtuals)",
	copy->at (0).identify ()))
	Done ();
if (! checker.check ("element insert returned iterator",
		copy->at (0), *position))
	Done ();

//	erase
if (checker.Verbose)
	cout << "--- copy->erase (copy->begin () + 1, copy->end () - 1)" << endl;
copy->erase (copy->begin () + 1, copy->end () - 1);
if (! checker.check ("erase all but the first and last elements",
		2, (int)copy->size ()))
	{
	for_each (copy->begin (), copy->end (), Identify);
	Done ();
	}

//	insert range
if (checker.Verbose)
	cout << "--- list.insert (list.begin (), copy->begin (), copy->end ())" << endl;
list.insert (list.begin (), copy->begin (), copy->end ());
if (! checker.check ("list.insert range",
		true,
		(
		list[0].identify () ==
			"Actual_A:13/13<-Actual_A:12/12<-Actual_A:1/1 (Actual_A 13 of 14 Virtuals)" &&
		list[1].identify () ==
			"Actual_A:14/14<-Actual_A:11/11<-Actual_A:1/1 (Actual_A 14 of 14 Virtuals)"
		)))
	{
	for_each (list.begin (), list.begin () + 2, Identify);
	Done ();
	}

//	assign
if (checker.Verbose)
	cout << "--- list.assign (copy->begin (), copy->end ())" << endl;
list.assign (copy->begin (), copy->end ());
if (! checker.check ("list.assign range",
		copy->size (), list.size ()))
	{
	for_each (list.begin (), list.end (), Identify);
	cout << "from -" << endl;
	for_each (copy->begin (), copy->end (), Identify);
	Done ();
	}

//	clear
if (checker.Verbose)
	cout << "--- list.clear ()" << endl;
list.clear ();
if (! checker.check ("list.clear ()",
		0, (int)list.size ()))
	Done ();

//	peek
if (checker.Verbose)
	cout << "--- copy->peek (copy->begin ())" << endl;
if (! checker.check ("copy->peek (copy->begin ())",
		&(copy->at (0)), copy->peek (copy->begin ())))
	Done ();
if (checker.Verbose)
	cout << "--- list.peek (list.begin ())" << endl;
if (! checker.check ("list.peek (list.begin ()) on empty list",
		NULL, list.peek (list.begin ())))
	Done ();

//	poke range
if (checker.Verbose)
	cout << "--- list.poke (list.begin (), copy->begin (), copy->end ())" << endl;
list.poke (list.begin (), copy->begin (), copy->end ());
if (! checker.check ("list.poke range",
		copy->size (), list.size ()))
	Done ();
for (index = 0;
	 index < (int)list.size ();
	 index++)
	{
	description.str ("");
	description << "copy->at (" << index << ") == list[" << index << ']';
	if (! checker.check (description.str (), copy->at (index), list[index]))
		Done ();
	}

//	pull range
if (checker.Verbose)
	cout << "--- copy->pull (copy->begin (), copy->end ())" << endl;
copy->pull (copy->begin (), copy->end ());
if (! checker.check ("pull entire range leaves empty",
	true, copy->empty ()))
	Done ();

//	const list
if (checker.Verbose)
	cout << "--- const Vectal<Virtual> const_list (list)" << endl;
const Vectal<Virtual>
	const_list (list);
if (! checker.check ("list.size () == const_list.size ()",
		list.size (), const_list.size ()))
	Done ();

//	copy pointers
copy->poke (copy->begin (), const_list.begin (), const_list.end ());
if (! checker.check ("copy pointers of const_list",
		list.size (), copy->size ()))
	Done ();

//	const_iterator
Vectal<Virtual>::const_iterator
	const_list_position;
bool
	identical = true;
if (checker.Verbose)
	cout << endl
		 << "--- Vectal<Virtual>::const_iterator const_list_position" << endl
		 << "    for (const_list_position = const_list.begin ()," << endl
		 << "            position = copy->begin ();" << endl
		 << "         const_list_position != const_list.end ();" << endl
		 << "         ++const_list_position)" << endl
		 << "             ++position)" << endl
		 << "        *const_list_position == *position" << endl;
for (const_list_position = const_list.begin (),
		position = copy->begin ();
	 const_list_position != const_list.end ();
	 ++const_list_position,
	 	++position)
	{
	if (checker.Verbose)
		cout << "        *const_list_position = " << *const_list_position << endl
			 << "                   *position = " << *position << endl;
	if (! (*const_list_position == *position))
		{
		identical = false;
		break;
		}
	}
if (! checker.check ("*const_iterator == *iterator over entire lists",
		true, identical))
	Done ();

//	reverse_iterator
if (checker.Verbose)
	cout << "--- Vectal<Virtual>::reverse_iterator reverse_position (position)" << endl;
Vectal<Virtual>::reverse_iterator
	reverse_position (position);
--position;
checker.check ("reverse_iterator constructed from iterator at end",
	*position, *reverse_position);

if (checker.Verbose)
	cout << endl
		 << "--- reverse_position = copy->rbegin ()" << endl
		 << "    position = copy->end ();" << endl
		 << "    while (reverse_position != copy->rend ())" << endl
		 << "        *reverse_position++; *--position;" << endl;
reverse_position = copy->rbegin ();
position = copy->end ();
while (reverse_position != copy->rend ())
	{
	--position;
	if (checker.Verbose)
		cout << "        *reverse_position: " << *reverse_position << endl
			 << "                *position: " << *position << endl;
	if (! (*reverse_position == *position))
		{
		identical = false;
		break;
		}
	++reverse_position;
	}
if (! checker.check ("*reverse_iterator == *iterator over entire lists",
		true, identical))
	Done ();

//	const_reverse_iterator
Vectal<Virtual>::const_reverse_iterator
	const_list_reverse_position;
identical = true;
if (checker.Verbose)
	cout << endl
		 << "--- Vectal<Virtual>::const_reverse_iterator const_list_reverse_position" << endl
		 << "    for (const_list_reverse_position = const_list.rbegin ()," << endl
		 << "            reverse_position = copy->rbegin ();" << endl
		 << "         const_list_reverse_position != const_list.rend ();" << endl
		 << "         ++const_list_reverse_position)" << endl
		 << "             ++reverse_position)" << endl
		 << "        *const_list_reverse_position == *reverse_position" << endl;
for (const_list_reverse_position = const_list.rbegin (),
		reverse_position = copy->rbegin ();
	 const_list_reverse_position != const_list.rend ();
	 ++const_list_reverse_position,
	 	++reverse_position)
	{
	if (checker.Verbose)
		cout << "        *const_list_reverse_position = " << *const_list_reverse_position << endl
			 << "                   *reverse_position = " << *reverse_position << endl;
	if (! (*const_list_reverse_position == *reverse_position))
		{
		identical = false;
		break;
		}
	}
if (! checker.check ("*const_reverse_iterator == *reverse_iterator over entire lists",
		true, identical))
	Done ();


Done (0);
}
