/*	Vectal

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

CVS ID: Vectal.hh,v 1.16 2004/10/26 10:23:05 castalia Exp
*******************************************************************************/

#ifndef idaeim_PVL_Vectal_hh
#define idaeim_PVL_Vectal_hh

#include	<vector>
#include	<iterator>
#include	<string>
#include	<stdexcept>


namespace idaeim
{
/*******************************************************************************
	Vectal
*/
/**	A <i>Vectal_ID</i> provides a container for the static const ID
	value inheritied by every instantiation of a Vectal template.
*/
struct Vectal_ID
{static const char* const ID;};
//	The actual definition of the ID is in the compiled Vectal.cc file.
#define	VECTAL_ID "idaeim::Vectal (1.40 2014/04/16 02:56:48)"

/**	A <i>Vectal</i> is a <i><b>vect</b></i>or of virtu<i><b>al</b></i>
	objects of any type.

	From the user's point of view a Vectal is a vector of objects of
	the class specified by the template parameter. The interface is
	essentially the same. The Vectal provides efficient storage of, and
	access to, objects by only storing their pointers into the
	corresponding vector from which it is derived. This also allows the
	Vectal to contain objects of pure virtual polymorphic classes which
	can only be used with pointers or references (since they can not be
	constructed directly).

<h4>Cloning
</h4>
	More importantly, when an object is inserted into a Vectal it makes
	a copy of the object for internal storage. It does not copy the
	pointer. Since an abstract class can not be copied directly it must
	provide a "virtual constructor" (see Stroustrup, "The C++
	Programming Language", [special edition] sect. 15.6.2) as a method
	named <CODE>clone</CODE> which takes no arguments and returns a
	pointer to a copy of the object. <b>Note</b>: The presence of the
	<CODE>clone</CODE> method is the only special requirement of a
	class contained in a Vectal. This method is typically implemented
	as follows:
<code>
<pre>	class Abstract_Class {
	public:
	Abstract_Class (const Abstract_Class&);	// Copy constructor.
	virtual Abstract_Class* clone () const = 0;
	// ...
	};

	class Implementing_Class : public Abstract_Class {
	public:
	Implementing_Class (const Implementing_Class&);	// Copy constructor.
	Implementing_Class* clone () {return new Implementing_Class (*this);}
	// ...
	};
</pre></code>

<h4>Element pointers
</h4>
	Methods are provided to manipulate the object value pointers
	directly in the backing vector:
	<ul>
	<li>pointer peek(iterator& position)
	<li>const_pointer peek(const_iterator& position) const
	<li>pointer peek_back()
	<li>const_pointer peek_back() const
	<li>iterator poke(iterator position, const_pointer pointer_value)
	<li>iterator poke(iterator position, const_iterator start, const_iterator stop)
	<li>void poke_back(pointer pointer_value)
	<li>pointer replace(iterator, pointer pointer_value)
	<li>iterator pull(iterator position)
	<li>iterator pull(iterator start, iterator stop)
	<li>pointer pull_out(iterator position)
	<li>pointer pull_back()
	</ul>
	<b>Note</b>: These methods copy and delete pointers - not the
	objects they point to - into and out of the backing vector. The two
	<CODE>erase</CODE> methods delete the value object(s) in the Vectal
	<b>before</b> they erase the value pointer(s) directly from the
	Vectal; they do not use the <CODE>pull</CODE> methods because
	the intent is to destroy the erased objects, not just to remove
	their pointers from the Vectal.

	Classes that want to control what happens to an object when it is
	entered or removed should subclass Vectal to override these virtual
	methods:
<code>
<pre>	void entering (pointer value);
	void removing (pointer value);
</pre></code>

<h4>Iterator reference semantics
</h4>
	Iterators are provided that implement reference semantics: their
	dereference (*), pointer (->) and indexing ([]) operators
	automatically dereference the object pointers in the backing vector.
	Thus, from the user's perspective the iterators appear to reference
	object values directly just as vector iterators do. However, instead
	of copying out objects as do vectors, direct object value references
	to the contents of the Vectal are provided.

<h4>Caveats
</h4>
	The Vectal class is flawed. One could easily make the argument that
	there is no need for a Vectal; a vector of pointers to virtual types
	should be sufficient. However, the author wanted to provide the
	convenience of reference semantics, transparent reusability with
	different subclasses, and insert and removal hooks to the virtual
	class being managed to allow them to take special action for their
	own purposes. Thus the justification for the Vectal.

	However, the Vectal does not correctly satisfy all the requirements
	of an STL container (see Josuttis, "The C++ Standard Library", sect.
	5.10.1). In particular the elements of a Vectal are not copyable by a
	copy constructor. As pointed out above, the virtual elements of a
	Vectal require the use of a clone method for the elements of the
	class being managed. However the STL containers, which work with
	copies of their elements, use copy constructors, not a clone method.
	This results, at best, in object slicing at copy time. Pointers can,
	of course, be easily copied. But the STL containers do not, in the
	process, copy (i.e. clone) the object being pointed to. This can
	cause unacceptable problems when the user expects each element in a
	container to be unique.

	The problem of the copy constructor for virtual classes poses a major
	challenge. The effect of this is that Vectals do not always work well
	with STL algorithms. Sorting is an excellent example: It is obviously
	necessary to be able to compare the values of the elements being
	sorted; thus using a vector of pointers to the objects to be sorted
	will not do. This is solved by having the Vectal iterators implement
	reference semantics. However, reordering elements requires that
	temporary copies of elements must be made. It would be fine to copy
	and reorder the backing pointers, but the Vectal iterators reference
	semantics brings up the copy constructor problem. Many STL algorithms
	will still work just fine with a Vectal as long as they do not have
	to make copies of the Vectal elements.

	@author		Bradford Castalia
	@version	1.40
*/
template <typename T>
class Vectal
:	public std::vector<T*>,
	public Vectal_ID
{
/**	Defines a memory allocation margin.

	It is possible for two vectors to have continguous memory storage;
	i.e. one vector's storage immediately follows another's in memory
	address space. This can result in the end iterator for the first
	being equivalent to the begin iterator for the second. Typically
	this doesn't matter because the iterators for the two vectors are
	not considered to be comparable. However, when iterators for two
	vectors may be meaningfully compared (e.g. in the Depth_Iterator
	implementation for PVL:Aggregate and PVL::Array) then it is
	necessary to guarantee that the end interator can not correspond to
	any iterator value of another iterator. To accomplish this the
	memory allocated for a vector - its capacity - must be kept larger
	than the vector's contents - its size. Since this involves both a
	check and possible memory reallocation, the VECTAL_CAPACITY_MARGIN
	defines the margin to apply should a value insertion into the
	vector exhaust the available capacity; i.e. there will always be
	some extra capacity (and thus a margin of memory between
	potentially contiguously allocated vectors), and when reallocation
	is needed sufficient extra capacity for the specified number of
	values will be provided.

	If the value of the symbol is zero, no extra capacity will be
	provided and the capacity check will be eliminated. While this
	usually results in Vectals with no excess memory allocated, it is
	not necessarily the most efficient as each unit of extra capacity
	is only the size of a pointer and each addition of a Vectal element
	may require a memory rellocation operation which could be avoided
	if sufficient capacity is already available and done in modestly
	larger chunks.
*/
#ifndef VECTAL_CAPACITY_MARGIN
#define VECTAL_CAPACITY_MARGIN 8
#endif

public:

//	Forward references:
class Iterator;
class Const_Iterator;
class Reverse_Iterator;
class Const_Reverse_Iterator;

/*==============================================================================
	Types:
*/
//	Types from the base vector class:

//!	Base vector class.
typedef std::vector<T*>							Base;
//!	Base vector iterator.
typedef typename Base::iterator					Base_iterator;
//!	Base vector const interator.
typedef typename Base::const_iterator			Base_const_iterator;
//!	Base vector reverse iterator.
typedef typename Base::reverse_iterator			Base_reverse_iterator;
//!	Base vector const reverse iterator.
typedef typename Base::const_reverse_iterator	Base_const_reverse_iterator;

//	Types to conform to the STL container iterator traits:

//!	Element virtual class type.
typedef T										value_type;
//!	Element const type.
typedef const value_type						const_value_type;
//!	Element reference.
typedef value_type&								reference;
//!	Element const reference.
typedef const value_type&						const_reference;
//!	Element pointer type.
typedef value_type*								pointer;
//!	Element const pointer.
typedef const pointer							const_pointer;
//!	Unsigned integral type for size values.
typedef typename Base::size_type				size_type;
//!	Element pointer difference type.
typedef typename Base::difference_type			difference_type;
//!	Storage allocate type (pointer allocator).
typedef typename Base::allocator_type			allocator_type;

//!	Vectal iterator.
typedef Iterator								iterator;
//!	Vectal const iterator.
typedef Const_Iterator							const_iterator;
//!	Vectal reverse iterator.
typedef Reverse_Iterator						reverse_iterator;
//!	Vectal const reverse iterator.
typedef Const_Reverse_Iterator					const_reverse_iterator;

/*==============================================================================
	Constructors
*/
//!	Constructs an empty Vectal.
Vectal ()
{}

/**	Copy constructor.

	@param	vectal	The Vectal to be copied.
	@see	operator=(const Vectal&)
*/
explicit Vectal (const Vectal& vectal) : std::vector<T*> ()
{operator= (vectal);}

private:
/**	Construct with a number of default elements.

	Users of the vector interface expect this constructor to provide
	objects of the container's element type created with the class'
	default constructor to fill the size space. Since a virtual class
	can not be constructed, this is not possible. So this constructor
	should not be publicly available. Use the constructor that
	provides a fill value to accomplish this.

	Instead, the empty Vectal is constructed with the space reserved
	for the specified number of elements.

	@param	size	The number of element slots to reserve.
	@see	reserve(size_type)
*/
explicit Vectal (size_type size)
{reserve (size);}

public:

/**	Construct with with a number of value copies.

	@param	size	The number of value copies with which to initialize
		the contents.
	@param	value	The value to be copied.
*/
Vectal (size_type size, const_reference value)
{assign (size, value);}

/**	Construct from an Iterator range of values.

	The values in the range [start, stop) are used to intialize the
	contents.

	@param	start	An Iterator positioned where the range starts
		(inclusive). The position is not range checked.
	@param	stop	An Iterator positioned where the range stops
		(exclusive). The position is not range checked.
	@see	assign(Iterator, Iterator)
*/
template <typename Iterator>
Vectal (Iterator start, Iterator stop)
{assign (start, stop);}

/**	Destroys the Vectal and its contents.

	The value elements are destroyed before destroying the Vectal.

	@see	clear()
*/
virtual ~Vectal ()
{clear ();}

/*==============================================================================
	Accessors
*/
/**	Gets the indexed element.

	@param	index	The range checked index of an element.
	@return	A reference to the value at the indexed element.
	@throws	out_of_range	If the index is not in the range of existing
		index elements. <b>N.B.</b>: This exception will always be thrown
		if the Vectal is empty.
	@see	operator[](size_type)
*/
reference at (size_type index)
{return *Base::at (index);}

/**	Gets the indexed element.

	@param	index	The range checked index of an element.
	@return	A const reference to the value at the indexed element.
	@throws	out_of_range	If the index is not in the range of existing
		index elements. <b>N.B.</b>: This exception will always be thrown
		if the Vectal is empty.
	@see	operator[](size_type)const
*/
const_reference at (size_type index) const
{return *Base::at (index);}

/**	Gets the indexed element.

	Unchecked - faster, but not safe - access to an indexed element.

	@param	index	An element index of a value in the Vectal.
		<b>N.B.</b>: The validity of the index is not checked.
	@return	A reference to the value at the indexed element.
	@see	at(size_type)
*/
reference operator[] (size_type index)
{return *Base::operator[] (index);}

/**	Gets the indexed element.

	Unchecked - faster, but not safe - access to an indexed element.

	@param	index	An element index of a value in the Vectal.
		<b>N.B.</b>: The validity of the index is not checked.
	@return	A const reference to the value at the indexed element.
	@see	at(size_type)const
*/
const_reference operator[] (size_type index) const
{return *Base::operator[] (index);}

/**	Gets the first element.

	<b>N.B.</b>: The Vectal must not be empty.

	@return	A reference to the first value.
	@see	operator[](size_type)
*/
reference front ()
{return operator[] (0);}

/**	Gets the first element.

	<b>N.B.</b>: The Vectal must not be empty.

	@return	A const reference to the first value.
	@see	operator[](size_type)const
*/
const_reference front () const
{return operator[] (0);}

/**	Gets the last element.

	<b>N.B.</b>: The Vectal must not be empty.

	@return	A reference to the last value.
	@see	operator[](size_type)
*/
reference back ()
{return operator[] (Base::size () - 1);}

/**	Gets the last element.

	<b>N.B.</b>: The Vectal must not be empty.

	@return	A const reference to the last value.
	@see	operator[](size_type)const
*/
const_reference back () const
{return operator[] (Base::size () - 1);}

/**	Gets a mutaable value pointer at an iterator position.

	@param	position	The iterator position to peek at. The position is
		not range checked.
	@return	A pointer to the value at the position. This will be NULL if
		the Vectal is empty.
	@see	Iterator::operator->()
*/
pointer peek (const iterator& position)
{
if (Base::empty ())
	return NULL;
return position.operator->();
}

/**	Gets a const value pointer at an iterator position.

	@param	position	The iterator position to peek at. The position is
		not range checked.
	@return	A const pointer to the value at the position. This will be
		NULL if the Vectal is empty.
	@see	peek(const_iterator
*/
pointer peek (const const_iterator& position) const
{
if (Base::empty ())
	return NULL;
return position.operator->();
}

/**	Gets the last value pointer.

	@return	A pointer to the last value. This will be NULL if the
		Vectal is empty.
*/
pointer peek_back ()
{
if (Base::empty ())
	return NULL;
return Base::back ();}

/**	Gets the last value pointer.

	@return	A pointer to the last value. This will be NULL if the
*/
pointer peek_back () const
{
if (Base::empty ())
	return NULL;
return Base::back ();
}

/**	Provides access to the base vector.

	@return	A reference to the base vector of object pointers.
		This is equivalent to dynamic_cast<Base&>(*this).
*/
Base& vector_base ()
{return dynamic_cast<Base&>(*this);}

/*==============================================================================
	Add to contents
*/
protected:
/**	Catches values as they are being entered into the Vectal.

	This stub does nothing by default. It should be overridden by
	subclasses that want to apply special handling to the object before
	they are entered into the Vectal.

	@param	value_pointer	The value pointer that is being entered into
		the Vectal.
*/
virtual void entering (pointer
	#ifdef DOXYGEN_PROCESSING
	value_pointer
	#endif
	)
{}

/*------------------------------------------------------------------------------
	Assign
*/
public:

/**	Assigns a number of value copies.

	Any existing values, up to the number being assigned, are assigned
	from the value provided. The value's own assignment operator is
	used. Any excess values are removed. A shortfall is filled with
	copies of the value.

	@param	number	The number of value copies to assign.
	@param	value	The value to be assigned.
	@see	resize(size_type, const_reference)
*/
void assign (size_type number, const_reference value)
{
size_type
	index = 0;
while (index < number && index < Base::size ())
	//	Replace, by assignment, existing values.
	operator[] (index++) = value;
if (index != number || index != Base::size ())
	//	Resize the remainder.
	resize (number, value);
}

/**	Assigns values from an iterator range.

	Dereferencing (operator*) the Iterator must provide a reference to an
	object of the type contained by this Vectal. Existing values are
	replaced by assignment. Excess input values are {@link
	push_back(const_reference) appended}. Excess existing elements are
	erased.

	@param	start	An Iterator positioned where the range starts
		(inclusive). The position is not range checked.
	@param	stop	An Iterator positioned where the range stops
		(exclusive). The position is not range checked.
	@see	push_back(const_reference)
	@see	erase(iterator, iterator)
*/
template <typename Iterator>
void assign (Iterator start, Iterator stop)
{
iterator
	position;

//	Replace any existing values.
for (position = begin ();
	 position != end () &&
	 	start != stop;
	 ++position,
	 	++start)
	*position = *start;

if (start != stop)
	{
	//	Add additional values.
	while (start != stop)
		{
		push_back (*start);
		++start;
		}
	}
else
	//	Remove excess values.
	erase (position, end ());
}

/**	Assigns values from another Vectal.

	@param	vectal	A Vectal of the same type as this Vectal.
	@return	This Vectal.
	@see	assign(const_iterator, const_iterator)
*/
Vectal& operator= (const Vectal<value_type>& vectal)
{
if (this != &vectal)
	assign (vectal.begin (), vectal.end ());
return *this;
}

/*------------------------------------------------------------------------------
	Insert
*/
/**	Inserts a value at an iterator position.

	A clone pointer of the value is poked into the backing vector.

	@param	position	The iterator position where the value is to be
		inserted.
	@param	value	The value to be inserted.
	@return	The position of the inserted value.
	@see	poke(iterator, pointer)
*/
iterator insert (iterator position, const_reference value)
{return poke (position, value.clone ());}

/**	Inserts a value at an element index position.

	A clone pointer of the value is poked into the backing vector.

	If the index is greater than or equal to the size of the Vectal
	the value is appended.

	@param	index	The element index where the value is to be inserted.
	@param	value	The value to be inserted.
	@see	poke(size_type, pointer)
*/
void insert (size_type index, const_reference value)
{return poke (index, value.clone ());}

/**	Inserts a number of value copies starting at an iterator position.

	@param	position	The iterator position at which to start inserting
		the values.
	@param	number		The number of value copies to insert.
	@param	value		The value to be inserted.
	@see	insert(iterator, const_reference)
*/
void insert (iterator position, size_type number, const_reference value)
{
while (number--)
	{
	//	The previous iterator position may become invalidated.
	position = insert (position, value);
	++position;
	}
}

private:
/**	Inserts values from an iterator range starting at an iterator position.

	This template method is provide to ensure that only input iterators
	are used for insertion.

	@param	position	The iterator position at which to start inserting
		the values. The position is not range checked.
	@param	start	The InputIterator positioned where the range starts
		(inclusive). The position is not range checked.
	@param	stop	The InputIterator positioned where the range stops
		(exclusive). The position is not range checked.
	@param	std::input_iterator_tag ensures that an input iterator is
		used.
	@see	insert(iterator, InputIterator, InputIterator)
*/
template <typename InputIterator>
void insert_ (iterator position, InputIterator start, InputIterator stop,
	std::input_iterator_tag)
{
for (; start != stop; ++start, ++position)
	position = insert (position, *start);
}

public:

/*	Inserts values from an iterator range starting at an iterator position.

	Any input iterators from which a value reference may be obtained
	by dereferencing (*InputIterator) are acceptable.

	@param	position	The iterator position at which to start inserting the
		values. The position is not range checked.
	@param	start	The InputIterator positioned where the range starts
		(inclusive). The position is not range checked.
	@param	stop	The InputIterator positioned where the range stops
		(exclusive). The position is not range checked.
*/
template <typename InputIterator>
void insert (iterator position, InputIterator start, InputIterator stop)
{insert_ (position, start, stop,
	typename std::iterator_traits<InputIterator>::iterator_category ());}

/*------------------------------------------------------------------------------
	Push/Poke
*/
/**	Pushes a value on the end of the Vectal.

	A clone pointer of the value is poked into the end of the backing vector.

	@param	value	A value reference.
	@see	poke_back(pointer).
*/
void push_back (const_reference value)
{poke_back (value.clone ());}

/**	Inserts a value pointer (not a value copy) at an iterator position.

	<b>N.B.</b>: The value pointer is provided to the {@link
	entering(pointer) entering method} before being inserted into the
	backing vector.

	If the VECTAL_CAPACITY_MARGIN is not 0 and the allocated storage
	capacity has been reached, then storage reserve is increased to the
	next largest multiple of the margin amount.

	@param	position	The iterator position where the value pointer
		will be placed. The position is not range checked.
	@param	value_pointer	The value pointer to be stored.
	@return	The iterator positioned at the new value.
*/
iterator poke (iterator position, pointer value_pointer)
{
//	Give the user a chance to handle the value before it enters.
entering (value_pointer);
position = Base::insert (position, value_pointer);
#if (VECTAL_CAPACITY_MARGIN != 0)
size_type
	index = position - begin ();
if (adjust_capacity ())
	//	The iterator position may have moved.
	position = begin () + index;
#endif
return position;
}

/**	Inserts a value pointer (not a value copy) at an element index position.

	If the index is greater than or equal to the size of the Vectal
	the value is appended.

	@param	index	The element index where the value is to be inserted.
	@param	value_pointer	A pointer to the value to be inserted.
	@see	poke_back(pointer)
	@see	poke(iterator, pointer)
*/
void poke (size_type index, pointer value_pointer)
{
if (index >= Base::size ())
	poke_back (value_pointer);
else
	{
	iterator
		position (begin ());
	if (index > 0)
		position += index;
	poke (position, value_pointer);
	}
}

/*	Inserts value pointers (not value copies) from an iterator range
	starting at an iterator position.

	<b>Warning</b>: The values in the source iterator range are not
	removed from whatever container they are in. This method inserts
	pointers to the values in their present location.

	@param	position	The iterator position at which to start inserting
		the values.
	@param	start	A const_iterator positioned where the range starts
		(inclusive). The position is not range checked.
	@param	stop	A const_iterator positioned where the range stops
		(exclusive). The position is not range checked.
	@return	An iterator positioned at the stop location.
	@see	poke(iterator, pointer)
*/
iterator poke (iterator position, const_iterator start, const_iterator stop)
{
while (start != stop)
	{
	//	The previous iterator position may become invalidated.
	position = poke (position, start.operator->());
	++position;
	++start;
	}
return position;
}

/**	Pushes a value pointer (not a value copy) on the end of the Vectal.

	<b>N.B.</b>: The value pointer is provided to the {@link
	entering(pointer) entering method} before being pushed on the end of
	the backing vector.

	If the VECTAL_CAPACITY_MARGIN is not 0 and the allocated
	storage capacity has been reached, then storage reserve is
	increased to the next largest multiple of the margin amount.

	@param	value_pointer	The value pointer to be stored.
*/
void poke_back (pointer value_pointer)
{
//	Give the user a chance to handle the value before it enters.
entering (value_pointer);
Base::push_back (value_pointer);
adjust_capacity ();
}

/*==============================================================================
	Remove from contents
*/
protected:
/**	Catches values as they are being removed from the Vectal.

	This stub does nothing by default. It should be overridden by
	subclasses that want to apply special handling to the object before
	they are removed from the Vectal.

	@param	value_pointer	The value pointer that is being removed from
		the Vectal.
*/
virtual void removing (pointer
	#ifdef DOXYGEN_PROCESSING
	value_pointer
	#endif
	)
{}

/*------------------------------------------------------------------------------
	Erase
*/
public:

/**	Removes a value at an iterator position.

	The removed element is destroyed.

	@param	position	The iterator position of the value to be
		removed.
	@return	An iterator positioned at the next value (or the end
		position). If the Vectal was empty then nothing is done and the
		original position is returned.
	@see	peek(iterator)
	@see	pull(iterator)
*/
iterator erase (iterator position)
{
pointer
	value_pointer = peek (position);
if (value_pointer)
	{
	position = pull (position);
	delete value_pointer;
	}
return position;
}

/**	Removes a value at an element index position.

	The removed element is destroyed.

	@param	index	An element index position. <b>N.B.</b>: Nothing is done
		ff the index is greater than or equal to the size of the Vectal.
	@see	erase(iterator)
*/
void erase (size_type index)
{
if (index < Base::size ())
	erase (begin () += index);
}

/*	Removes a range of values.

	<b>N.B.</b>: Each value pointer is provided to the {@link
	removing(pointer) removing method} before the value it points to is
	destroyed. After the values have been deleted the value pointers are
	removed from the backing vector.

	@param	start	An iterator positioned where the range starts
		(inclusive). The position is not range checked.
	@param	stop	An iterator positioned where the range stops (exclusive). The
		position is not range checked.
	@return	An iterator positioned at the next value after the last one
		erased  (or the end position). If the Vectal was empty then
		nothing is done and the stop position is returned.
*/
iterator erase (iterator start, iterator stop)
{
if (Base::empty ())
	return stop;
for (iterator
		position = start;
		position != stop;
		++position)
	{
	removing (position.operator-> ());
	delete position.operator-> ();
	}
return Base::erase (start, stop);
}

/**	Removes, and destroys, all values.

	@see	erase(iterator, iterator)
*/
void clear ()
{erase (begin (), end ());}

/**	Removes all of the value pointers, but does not destroy the values.

	<b>Warning</b>: It is the user's responsibility to make sure that
	the value objects are properly managed to prevent a memory leak.

	This method might be used if the value pointers have been copied
	elsewhere, or the value objects have already been destroyed.
*/
void wipe ()
{Base::clear ();}

/*------------------------------------------------------------------------------
	Pop/Pull
*/
/**	Removes the last value.

	Does nothing if the Vectal is empty.

	The value that is removed is {@link erase(iterator) erased}.
*/
void pop_back ()
{
if (! Base::empty ())
	erase (end () - 1);
}

/*	Removes the value pointer at an iterator position.

	The value itself is not destroyed.

	<b>N.B.</b>: The value pointer is provided to the {@link
	removing(pointer) removing method} before the value pointer is
	removed from the backing vector.

	@param	position	The iterator position of the value. The
		position is not range checked.
	@return	The position of the next value. If the Vectal was empty
		then nothing is done and the original position is returned.
*/
iterator pull (iterator position)
{
if (Base::empty ())
	return position;
removing (position.operator-> ());
return Base::erase (position);
}

/*	Removes the value pointers from an iterator range.

	The values are not destroyed.

	<b>N.B.</b>: All pointers to be pulled are provided to the {@link
	removing(pointer) removing method} before the pointers are removed
	from the base vector.

	@param	start	An iterator positioned where the range starts
		(inclusive). The position is not range checked.
	@param	stop	An iterator positioned where the range stops
		(exclusive). The position is not range checked.
	@return	An iterator positioned at the next value after the last one
		pulled (or the end position). If the Vectal was empty then
		nothing is done and the stop position is returned.
*/
iterator pull (iterator start, iterator stop)
{
if (Base::empty ())
	return stop;
for (iterator
		position = start;
		position != stop;
		++position)
	removing (position.operator-> ());
return Base::erase (start, stop);
}

/**	Removes the value pointer at an iterator position.

	The value itself is not destroyed.

	@param	position	The iterator position of the value.
	@return	The value pointer removed. This will be NULL if the Vectal
		is empty.
	@see	peek (position)
	@see	pull (position)
*/
pointer pull_out (iterator position)
{
pointer
	value_pointer = peek (position);
pull (position);
return value_pointer;
}

/*	Removes the value pointer at an element index position.

	The value itself is not destroyed.

	<b>N.B.</b>: The value pointer is provided to the {@link
	removing(pointer) removing method} before the value pointer is
	removed from the backing vector.

	@param	index	An element index position.
	@return	The value pointer removed. This will be NULL if the 
		index is greater than or equal to the size of the Vectal.
*/
pointer pull_out (size_type index)
{
pointer
	value_pointer = NULL;
if (index < Base::size ())
	value_pointer = pull_out (begin () += index);
return value_pointer;
}

/**	Removes and returns the last value pointer.

	The value itself is not destroyed.

	@return	A pointer to the last value. This will be NULL if the
		Vectal is empty.
*/
pointer pull_back ()
{
if (Base::empty ())
	return NULL;
pointer
	value_pointer = Base::back ();
if (value_pointer)
	pull (end () - 1);
return value_pointer;
}

/*==============================================================================
	Resize and Replace
*/
private:
/**	Adjusts the capacity to be the next largest multiple of the
	#VECTAL_CAPACITY_MARGIN.
*/
bool adjust_capacity ()
{
#if (VECTAL_CAPACITY_MARGIN != 0)
size_type
	amount = Base::size ();
if (amount == Base::capacity ())
	{
	amount += VECTAL_CAPACITY_MARGIN - (amount % VECTAL_CAPACITY_MARGIN);
	Base::reserve (amount);
	return true;
	}
#endif
return false;
}

public:

/**	Resizes by removing excess elements, or adding new ones.

	To increase the size a value is needed to fill the space. Virtual
	classes have no default constructor so the last value is used to
	copy into the extra space. However, if the Vectal is empty
	there's nothing to copy so an out_of_range error will be thrown in
	this case.

	@param	new_size	The new size for the Vectal.
	@see	resize(size_type, const_reference)
*/
void resize (size_type new_size)
{
if (Base::empty ())
	throw std::out_of_range
		(
		std::string (ID) + '\n' +
		"To resize an empty Vectal "
		"a value is needed to fill the new space.\n"
		"Use resize (size_type new_size, const_reference value) instead."
		);
//	Replicate the last value to fill the space.
resize (new_size, back ());
}

void resize (size_type new_size, const_reference value)
{
size_type
	amount = Base::size ();
if (new_size > amount)
	{
	Base::reserve (new_size);
	new_size -= amount;
	//	Fill with copies of the value.
	while (new_size--)
		push_back (value);
	}
else
if (new_size < amount)
	{
	//	Destroy the excess elements.
	erase (end () - (amount - new_size), end ());
	Base::resize (new_size);
	adjust_capacity ();
	}
}

/**	Replaces an existing element.

	Nothing is done if the Vecatal is empty.

	<b>N.B.</b>: The previous value pointer at the iterator position is
	provided to the {@link removing(pointer) removing method}. The value
	is not destroyed. The new value pointer is provided to the {@link
	entering(pointer) entering method} before replacing the previous
	pointer in the backing vector.

	@param	position	The iterator position of the value. The
		position is not range checked.
	@param	value_pointer	The value pointer to be stored.
	@return	The value pointer replaced. This will be NULL if the Vectal
		is empty.
*/
pointer replace (iterator position, pointer value_pointer)
{
pointer
	previous = peek (position);
if (previous)
	{
	removing (previous);
	entering (value_pointer);
	(static_cast<Base_iterator>(position)).operator*() = value_pointer;
	}
return previous;
}

/*******************************************************************************
	Vectal Iterators
*/
/*============================================================================*/
/**	An <i>Iterator</i> provides a random access iterator for a Vectal.

	An Iterator operates the same as the Base_iterator - an interator over
	a vector of pointer objects - that provides the backing implementation,
	except that reference semantics are provided to the object pointed to
	rather that to the pointer.
*/
class Iterator
:	public std::iterator<std::random_access_iterator_tag, T>
{
public:

/**	Constructs an uninitialized Iterator.

	<b>N.B.</b>: The iterator is invalid until {@link
	operator=(const Iterator&) assigned}.
*/
Iterator ()
{}

/**	Constructs a copy of another Iterator.

	@param	other_iterator	An Iterator to be copied.
*/
Iterator (const Iterator& other_iterator)
	: Base_Iterator (other_iterator.Base_Iterator)
{}

/**	Constructs an Iterator from a Base_iterator.

	Implements implicit type conversion.

	@param	base_iterator	A Base_iterator from which to construct this
		Iterator.
*/
Iterator (const Base_iterator& base_iterator)
	: Base_Iterator (base_iterator)
{}

/**	Assignment operator.
	
	@param	other_iterator	An Iterator to be assigned to this Iterator.
	@return	This Iterator.
*/
Iterator& operator= (const Iterator& other_iterator)
{Base_Iterator = other_iterator.Base_Iterator; return *this;}

/**	Type conversion operator to Base_iterator.

	The backing Base_iterator for this Iterator is returned.
*/
operator Base_iterator () const
{return Base_Iterator;}

/**	Type conversion operator to Base_const_iterator.

	The backing Base_iterator for this Iterator is returned.
*/
operator Base_const_iterator ()
{return Base_Iterator;}

/**	Dereference operator.

	Reference semantics are implemented, even though the backing
	Base_iterator operates on pointer objects.

	@return	A reference to the value at this Iterator's current position.
*/
T& operator* () const
{return **Base_Iterator;}

/**	Member dereference (pointer) operator.

	The semantics for a container of objects of type T are implemented,
	even though the backing Base_iterator operates on a container of
	pointers to objects of type T.

	@return A pointer to the value at this Iterator's current position.
*/
T* operator-> () const
{return *Base_Iterator;}

/**	Array reference operator.

	@param	index	An element index of a value in the Vectal.
		<b>N.B.</b>: The validity of the index is not checked.
	@return	A reference to the value at the Vectal index.
*/
T& operator[] (size_type index) const
{return *Base_Iterator[index];}

/**	Pre-increment operator.

	@return	A reference to this Iterator after its position has been
		incremented.
*/
Iterator& operator++ ()
{++Base_Iterator; return *this;}

/**	Post-increment operator.

	This Iterator is incremented to the next position after a copy of
	this Iterator has been made.

	@return	A copy of this Iterator at its original position.
*/
Iterator operator++ (int)
{
Iterator
	copy (*this);
++*this;
return copy;
}

/**	Pre-decrement operator.

	@return	A reference to this Iterator after its position has been
		decremented.
*/
Iterator& operator-- ()
{--Base_Iterator; return *this;}

/**	Post-decrement operator.

	This Iterator is decremented to the previous position after a copy of
	this Iterator has been made.

	@return	A copy of this Iterator at its original position.
*/
Iterator operator-- (int)
{
Iterator
	copy (*this);
--*this;
return copy;
}

/**	Moves the Iterator position forward some distance from the position
	of this Iterator.

	@param	distance	The distance (number of values) to move.
	@return	This Iterator.
*/
Iterator& operator+= (int distance)
{Base_Iterator += distance; return *this;}

/**	Provides an Iterator moved forward some distance from the position
	of this Iterator.

	A copy of this Iterator is made and the copy is repositioned.

	@param	distance	The distance - number of values - to move
		the Iterator copy position.
	@return	A copy of this Iterator at the new position.
*/
Iterator operator+ (int distance) const
{
Iterator
	copy (*this);
copy += distance;
return copy;
}

/**	Moves the Iterator position backward some distance from the position
	of this Iterator.

	@param	distance	The distance - number of values - to move.
	@return	This Iterator.
*/
Iterator& operator-= (int distance)
{Base_Iterator -= distance; return *this;}

/**	Provides an Iterator moved backward some distance from the position
	of this Iterator.

	A copy of this Iterator is made and the copy is repositioned.

	@param	distance	The distance - number of values - to move.
	@return	A copy of this Iterator at the new position.
*/
Iterator operator- (int distance) const
{
Iterator
	copy (*this);
copy -= distance;
return copy;
}

/**	Distance operator.

	@param other_iterator	Another Iterator from which to obtain the
		distance from this Iterator.
	@return The distance, in values, between this and the other Iterator.
*/
typename Iterator::difference_type
operator- (const Iterator& other_iterator) const
{return Base_Iterator - other_iterator.Base_Iterator;}

/**	Equality operator.

	@param other_iterator	Another Iterator to compare with this
		Iterator.
	@return	true if the Iterators are equal; false otherwise.
*/
bool operator== (const Iterator& other_iterator) const
{return Base_Iterator == other_iterator.Base_Iterator;}

/**	Inequality operator.

	@param other_iterator	Another Iterator to compare with this
		Iterator.
	@return	true if the Iterators are not equal; false otherwise.
*/
bool operator!= (const Iterator& other_iterator) const
{return ! (*this == other_iterator);}

/**	Inequality operator with a Base_iterator.

	@param base_iterator	A Base_iterator to compare with another
		Iterator.
	@param other_iterator	Another Iterator to compare with the
		Base_iterator.
	@return	true if the iterators are not equal; false otherwise.
*/
friend bool operator!=
	(const Base_iterator& base_iterator, const Iterator& other_iterator)
{return ! (base_iterator == other_iterator.Base_Iterator);}

/**	Less than operator.

	@param other_iterator	Another Iterator to compare with this
		Iterator.
	@return	true if this Iterator is at a position before (less than) the
		position of the other Iterator; false otherwise.
*/
bool operator< (const Iterator& other_iterator) const
{return Base_Iterator < other_iterator.Base_Iterator;}

/**	Less than or equal to operator.

	@param other_iterator	Another Iterator to compare with this
		Iterator.
	@return	true if this Iterator is not at a position after the position of
		the other Iterator; false otherwise.
*/
bool operator<= (const Iterator& other_iterator) const
{return ! (other_iterator < *this);}

/**	Greater than operator.

	@param other_iterator	Another Iterator to compare with this
		Iterator.
	@return	true if this Iterator is at a position after (greater than)
		the position of the other Iterator; false otherwise.
*/
bool operator> (const Iterator& other_iterator) const
{return other_iterator < *this;}

/**	Greater than or equal to operator.

	@param other_iterator	Another Iterator to compare with this
		Iterator.
	@return	true if this Iterator is not at a position before the position of
		the other Iterator; false otherwise.
*/
bool operator>= (const Iterator& other_iterator) const
{return ! (*this < other_iterator);}


private:

//	Provide Const_Iterator access to Base_Iterator.
friend class Const_Iterator;
//	Provide Reverse_Iterator access to Base_Iterator.
friend class Reverse_Iterator;

Base_iterator
	Base_Iterator;

};		//	End of Iterator class.

/**	Gets an Iterator for this Vectal positioned at the first value.

	@return	An Iterator positioned at the first value.
*/
Iterator begin ()
{return Base::begin ();}

/**	Gets an Iterator for this Vectal positioned after the last value.

	@return	An Iterator positioned after the last value.
*/
Iterator end ()
{return Base::end ();}

/*============================================================================*/
/**	A <i>Const_Iterator</i> provides a random access iterator for a const
	Vectal.

	A Const_Iterator operates the same as the Base_iterator - an
	interator over a vector of pointer objects - that provides the
	backing implementation, except that reference semantics are provided
	to the object pointed to rather that to the pointer.
*/
class Const_Iterator
:	public std::iterator<std::random_access_iterator_tag, T>
{
public:

/**	Constructs an uninitialized Const_Iterator.

	<b>N.B.</b>: The iterator is invalid until {@link
	operator=(const Const_Iterator&) assigned}.
*/
Const_Iterator ()
{}

/**	Constructs a copy of another Const_Iterator.

	@param	other_iterator	A Const_Iterator to be copied.
*/
Const_Iterator (const Const_Iterator& other_iterator)
	:	Base_Iterator (other_iterator.Base_Iterator)
{}

/**	Constructs a Const_Iterator from an Iterator.

	@param	other_iterator	An Iterator to be copied.
*/
Const_Iterator (const Iterator& other_iterator)
	:	Base_Iterator (other_iterator.Base_Iterator)
{}

/**	Constructs a Const_Iterator from a Base_const_iterator.

	Implements implicit type conversion.

	@param	base_iterator	A Base_const_iterator from which to construct
		this Const_Iterator.
*/
Const_Iterator (const Base_const_iterator& base_iterator)
	:	Base_Iterator (base_iterator)
{}

/**	Assignment operator.
	
	@param	other_iterator	A Const_Iterator to be assigned to this
		Const_Iterator.
	@return	This Const_Iterator.
*/
Const_Iterator& operator= (const Const_Iterator& other_iterator)
{Base_Iterator = other_iterator.Base_Iterator; return *this;}

/**	Assignment operator from Iterator.
	
	@param	other_iterator	An Iterator to be assigned to this
		Const_Iterator.
	@return	This Const_Iterator.
*/
Const_Iterator& operator= (const Iterator& other_iterator)
{Base_Iterator = other_iterator.Base_Iterator; return *this;}

/**	Type conversion operator to Base_const_iterator.

	The backing Base_const_iterator for this Const_Iterator is returned.
*/
operator Base_const_iterator ()
{return Base_Iterator;}

/**	Dereference operator.

	Reference semantics are implemented, even though the backing
	Base_const_iterator operates on pointer objects.

	@return	A const reference to the value at this Const_Iterator's
		current position.
*/
const T& operator* () const
{return **Base_Iterator;}

/**	Member dereference (pointer) operator.

	The semantics for a container of objects of type T are implemented,
	even though the backing Base_const_iterator operates on a container
	of pointers to objects of type T.

	@return A pointer to the value at this Const_Iterator's current
		position.
*/
T* operator-> () const
{return *Base_Iterator;}

/**	Array reference operator.

	@param	index	An element index of a value in the Vectal.
		<b>N.B.</b>: The validity of the index is not checked.
	@return	A const reference to the value at the Vectal index.
*/
const T& operator[] (size_type index) const
{return *Base_Iterator[index];}

/**	Pre-increment operator.

	@return	A reference to this Const_Iterator after its position has
		been incremented.
*/
Const_Iterator& operator++ ()
{++Base_Iterator; return *this;}

/**	Post-increment operator.

	This Const_Iterator is incremented to the next position after a copy
	of this Const_Iterator has been made.

	@return	A copy of this Const_Iterator at its original position.
*/
Const_Iterator operator++ (int)
{
Const_Iterator
	copy = *this;
++*this;
return copy;
}

/**	Pre-decrement operator.

	@return	A reference to this Const_Iterator after its position has
		been decremented.
*/
Const_Iterator& operator-- ()
{--Base_Iterator; return *this;}

/**	Post-decrement operator.

	This Const_Iterator is decremented to the previous position after a
	copy of this Const_Iterator has been made.

	@return	A copy of this Const_Iterator at its original position.
*/
Const_Iterator operator-- (int)
{
Const_Iterator
	copy = *this;
--*this;
return copy;
}

/**	Moves the Const_Iterator position forward some distance from the
	position of this Const_Iterator.

	@param	distance	The distance (number of values) to move.
	@return	This Const_Iterator.
*/
Const_Iterator& operator+= (int distance)
{Base_Iterator += distance; return *this;}

/**	Provides an Const_Iterator moved backward some distance from the
	position of this Const_Iterator.

	A copy of this Const_Iterator is made and the copy is repositioned.

	@param	distance	The distance - number of values - to move.
	@return	A copy of this Const_Iterator at the new position.
*/
Const_Iterator operator+ (int distance)
{
Const_Iterator
	copy = *this;
copy += distance;
return copy;
}

/**	Moves the Const_Iterator position backward some distance from the
	position of this Const_Iterator.

	@param	distance	The distance - number of values - to move.
	@return	This Const_Iterator.
*/
Const_Iterator& operator-= (int distance)
{Base_Iterator -= distance; return *this;}

/**	Provides an Const_Iterator moved backward some distance from the
	position of this Const_Iterator.

	A copy of this Const_Iterator is made and the copy is repositioned.

	@param	distance	The distance - number of values - to move.
	@return	A copy of this Const_Iterator at the new position.
*/
Const_Iterator operator- (int distance)
{
Const_Iterator
	copy (*this);
copy -= distance;
return copy;
}

/**	Distance operator.

	@param other_iterator	Another Const_Iterator from which to obtain
		the distance from this Const_Iterator.
	@return The distance, in values, between this and the other
		Const_Iterator.
*/
typename Const_Iterator::difference_type
operator- (const Const_Iterator& other_iterator) const
{return Base_Iterator - other_iterator.Base_Iterator;}

/**	Equality operator.

	@param other_iterator	Another Const_Iterator to compare with this
		Const_Iterator.
	@return	true if the Const_Iterators are equal; false otherwise.
*/
bool operator== (const Const_Iterator& other_iterator) const
{return Base_Iterator == other_iterator.Base_Iterator;}

/**	Inequality operator.

	@param other_iterator	Another Const_Iterator to compare with this
		Const_Iterator.
	@return	true if the Const_Iterators are not equal; false otherwise.
*/
bool operator!= (const Const_Iterator& other_iterator) const
{return ! (*this == other_iterator);}

/**	Inequality operator with a Base_const_iterator.

	@param base_iterator	A Base_const_iterator to compare with another
		Const_Iterator.
	@param other_iterator	Another Const_Iterator to compare with the
		Base_const_iterator.
	@return	true if the iterators are not equal; false otherwise.
*/
friend bool operator!=
	(const Base_const_iterator& base_iterator,
	 const Const_Iterator& other_iterator)
{return ! (base_iterator == other_iterator.Base_Iterator);}

/**	Less than operator.

	@param other_iterator	Another Const_Iterator to compare with this
		Const_Iterator.
	@return	true if this Const_Iterator is at a position before (less
		than) the position of the other Const_Iterator; false otherwise.
*/
bool operator< (const Const_Iterator& other_iterator) const
{return Base_Iterator < other_iterator.Base_Iterator;}

/**	Less than or equal to operator.

	@param other_iterator	Another Const_Iterator to compare with this
		Const_Iterator.
	@return	true if this Const_Iterator is not at a position after the
		position of the other Const_Iterator; false otherwise.
*/
bool operator<= (const Const_Iterator& other_iterator) const
{return ! (other_iterator < *this);}

/**	Greater than operator.

	@param other_iterator	Another Const_Iterator to compare with this
		Const_Iterator.
	@return	true if this Const_Iterator is at a position after (greater
		than) the position of the other Const_Iterator; false otherwise.
*/
bool operator> (const Const_Iterator& other_iterator) const
{return other_iterator < *this;}

/**	Greater than or equal to operator.

	@param other_iterator	Another Const_Iterator to compare with this
		Const_Iterator.
	@return	true if this Const_Iterator is not at a position before the
		position of the other Const_Iterator; false otherwise.
*/
bool operator>= (const Const_Iterator& other_iterator) const
{return ! (*this < other_iterator);}


private:

//	Provide Const_Reverse_Iterator access to Base_Iterator.
friend class Const_Reverse_Iterator;

Base_const_iterator
	Base_Iterator;

};		//	End of Const_Iterator class.

/**	Gets a Const_Iterator for this Vectal positioned at the first value.

	@return	A Const_Iterator positioned at the first value.
*/
Const_Iterator begin () const
{return Base::begin ();}

/**	Gets a Const_Iterator for this Vectal positioned after the last value.

	@return	A Const_Iterator positioned after the last value.
*/
Const_Iterator end () const
{return Base::end ();}

/*============================================================================*/
/**	A <i>Reverse_Iterator</i> provides a reverse random access iterator
	for a Vectal

	A Reverse_Iterator operates the same as the Base_iterator - an
	interator over a vector of pointer objects - that provides the
	backing implementation, except that reference semantics are provided
	to the object pointed to rather that to the pointer.
*/
class Reverse_Iterator
:	public std::iterator<std::random_access_iterator_tag, T>
{
public:

/**	Constructs an uninitialized Iterator.

	<b>N.B.</b>: The iterator is invalid until {@link
	operator=(const Reverse_Iterator&) assigned}.
*/
Reverse_Iterator ()
{}

/**	Constructs a copy of another Reverse_Iterator.

	@param	other_iterator	A Reverse_Iterator to be copied.
*/
Reverse_Iterator (const Reverse_Iterator& other_iterator)
	: Base_Iterator (other_iterator.Base_Iterator)
{}

/**	Constructs a Reverse_Iterator from an Iterator.

	Implements explicit type conversion.

	<b>N.B.</b>: The Reverse_Iterator will be positioned immediately
	<i>before</i> the Iterator that was converted.

	@param	other_iterator	An Iterator to be converted.
*/
explicit Reverse_Iterator (const Iterator& other_iterator)
	: Base_Iterator (other_iterator.Base_Iterator)
{}

/**	Constructs a Reverse_Iterator from a Base_reverse_iterator.

	Implements explicit type conversion.

	@param	base_iterator	A Base_reverse_iterator from which to
		construct this Reverse_Iterator.
*/
Reverse_Iterator (const Base_reverse_iterator& base_iterator)
	:	Base_Iterator (base_iterator)
{}

/**	Assignment operator.
	
	@param	other_iterator	A Reverse_Iterator to be assigned to this
		Reverse_Iterator.
	@return	This Reverse_Iterator.
*/
Reverse_Iterator& operator= (const Reverse_Iterator& other_iterator)
{Base_Iterator = other_iterator.Base_Iterator; return *this;}

/**	Convert this Reverse_Iterator to an Iterator.
*/
Iterator base () const
{return Iterator (Base_Iterator.base ());}

/**	Type conversion operator to Base_reverse_iterator.

	The backing Base_reverse_iterator for this Reverse_Iterator is returned.
*/
operator Base_reverse_iterator () const
{return Base_Iterator;}

/**	Dereference operator.

	Reference semantics are implemented, even though the backing
	Base_reverse_iterator operates on pointer objects.

	@return	A reference to the value at this Reverse_Iterator's
		current position.
*/
T& operator* () const
{return **Base_Iterator;}

/**	Member dereference (pointer) operator.

	The semantics for a container of objects of type T are implemented,
	even though the backing Base_reverse_iterator operates on a container
	of pointers to objects of type T.

	@return A pointer to the value at this Reverse_Iterator's
		current position.
*/
T* operator-> () const
{return *Base_Iterator;}

/**	Array reference operator.

	@param	index	An element index of a value in the Vectal.
		<b>N.B.</b>: The validity of the index is not checked.
	@return	A reference to the value at the Vectal index.
*/
T& operator[] (size_type index) const
{return *Base_Iterator[index];}

/**	Pre-increment operator.

	@return	A reference to this Reverse_Iterator after its position has
		been incremented.
*/
Reverse_Iterator& operator++ ()
{++Base_Iterator; return *this;}

/**	Post-increment operator.

	This Reverse_Iterator is incremented to the next position after a
	copy of this Reverse_Iterator has been made.

	@return	A copy of this Reverse_Iterator at its original position.
*/
Reverse_Iterator operator++ (int)
{
Reverse_Iterator
	position = *this;
++*this;
return position;
}

/**	Pre-decrement operator.

	@return	A reference to this Reverse_Iterator after its position has
		been decremented.
*/
Reverse_Iterator& operator-- ()
{--Base_Iterator; return *this;}

/**	Post-decrement operator.

	This Reverse_Iterator is decremented to the previous position after a
	copy of this Reverse_Iterator has been made.

	@return	A copy of this Reverse_Iterator at its original position.
*/
Reverse_Iterator& operator-- (int)
{
Reverse_Iterator
	position = *this;
--*this;
return position;
}

/**	Moves the Reverse_Iterator position forward some distance from the
	position of this Reverse_Iterator.

	@param	distance	The distance (number of values) to move.
	@return	This Reverse_Iterator.
*/
Reverse_Iterator& operator+= (int distance)
{Base_Iterator += distance; return *this;}

/**	Provides an Reverse_Iterator moved backward some distance from the
	position of this Reverse_Iterator.

	A copy of this Reverse_Iterator is made and the copy is repositioned.

	@param	distance	The distance - number of values - to move.
	@return	A copy of this Reverse_Iterator at the new position.
*/
Reverse_Iterator operator+ (int distance)
{
Reverse_Iterator
	copy (*this);
copy += distance;
return copy;
}

/**	Moves the Reverse_Iterator position backward some distance from the
	position of this Reverse_Iterator.

	@param	distance	The distance - number of values - to move.
	@return	This Reverse_Iterator.
*/
Reverse_Iterator& operator-= (int distance)
{Base_Iterator -= distance; return *this;}

/**	Provides an Reverse_Iterator moved backward some distance from the
	position of this Reverse_Iterator.

	A copy of this Reverse_Iterator is made and the copy is repositioned.

	@param	distance	The distance - number of values - to move.
	@return	A copy of this Reverse_Iterator at the new position.
*/
Reverse_Iterator operator- (int distance)
{
Reverse_Iterator
	copy (*this);
copy -= distance;
return copy;
}

/**	Distance operator.

	@param other_iterator	Another Reverse_Iterator from which to obtain
		the distance from this Const_Iterator.
	@return The distance, in values, between this and the other
		Reverse_Iterator.
*/
typename Reverse_Iterator::difference_type
operator- (const Reverse_Iterator& other_iterator) const
{return Base_Iterator - other_iterator.Base_Iterator;}

/**	Equality operator.

	@param other_iterator	Another Reverse_Iterator to compare with this
		Reverse_Iterator.
	@return	true if the Reverse_Iterator are equal; false otherwise.
*/
bool operator== (const Reverse_Iterator& other_iterator) const
{return Base_Iterator == other_iterator.Base_Iterator;}

/**	Inequality operator.

	@param other_iterator	Another Reverse_Iterator to compare with this
		Reverse_Iterator.
	@return	true if the Reverse_Iterator are not equal; false otherwise.
*/
bool operator!= (const Reverse_Iterator& other_iterator) const
{return ! (*this == other_iterator);}

/**	Inequality operator with a Base_reverse_iterator.

	@param base_iterator	A Base_reverse_iterator to compare with another
		Reverse_Iterator.
	@param other_iterator	Another Reverse_Iterator to compare with the
		Base_reverse_iterator.
	@return	true if the iterators are not equal; false otherwise.
*/
friend bool operator!=
	(const Base_reverse_iterator& base_iterator,
	 const Reverse_Iterator& other_iterator)
{return ! (base_iterator == other_iterator.Base_Iterator);}

/**	Less than operator.

	@param other_iterator	Another Reverse_Iterator to compare with this
		Reverse_Iterator.
	@return	true if this Reverse_Iterator is at a position before (less
		than) the position of the other Reverse_Iterator; false otherwise.
*/
bool operator< (const Reverse_Iterator& other_iterator) const
{return Base_Iterator < other_iterator.Base_Iterator;}

/**	Less than or equal to operator.

	@param other_iterator	Another Reverse_Iterator to compare with this
		Reverse_Iterator.
	@return	true if this Reverse_Iterator is not at a position after the
		position of the other Reverse_Iterator; false otherwise.
*/
bool operator<= (const Reverse_Iterator& other_iterator) const
{return ! (other_iterator < *this);}

/**	Greater than operator.

	@param other_iterator	Another Reverse_Iterator to compare with this
		Reverse_Iterator.
	@return	true if this Reverse_Iterator is at a position after (greater
		than) the position of the other Reverse_Iterator; false otherwise.
*/
bool operator> (const Reverse_Iterator& other_iterator) const
{return other_iterator < *this;}

/**	Greater than or equal to operator.

	@param other_iterator	Another Reverse_Iterator to compare with this
		Reverse_Iterator.
	@return	true if this Reverse_Iterator is not at a position before the
		position of the other Reverse_Iterator; false otherwise.
*/
bool operator>= (const Reverse_Iterator& other_iterator) const
{return ! (*this < other_iterator);}


private:

//	Provide Const_Reverse_Iterator access to Base_Iterator.
friend class Const_Reverse_Iterator;

Base_reverse_iterator
	Base_Iterator;

};		//	End of Reverse_Iterator class.

/**	Gets a Reverse_Iterator for this Vectal positioned at the last value.

	@return	A Reverse_Iterator positioned at the last value.
*/
Reverse_Iterator rbegin ()
{return Base::rbegin ();}

/**	Gets a Reverse_Iterator for this Vectal positioned before the first value.

	@return	A Reverse_Iterator positioned before the first value.
*/
Reverse_Iterator rend ()
{return Base::rend ();}

/*============================================================================*/
/**	A <i>Const_Reverse_Iterator</i> provides a reverse random access
	iterator for a const Vectal.

	A Const_Reverse_Iterator operates the same as the Base_iterator - an
	interator over a vector of pointer objects - that provides the
	backing implementation, except that reference semantics are provided
	to the object pointed to rather that to the pointer.
*/
class Const_Reverse_Iterator
:	public std::iterator<std::random_access_iterator_tag, T>
{
public:

/**	Constructs an uninitialized Const_Reverse_Iterator.

	<b>N.B.</b>: The iterator is invalid until {@link
	operator=(const Const_Reverse_Iterator&) assigned}.
*/
Const_Reverse_Iterator ()
{}

/**	Constructs a copy of another Const_Reverse_Iterator.

	@param	other_iterator	A Const_Reverse_Iterator to be copied.
*/
Const_Reverse_Iterator (const Const_Reverse_Iterator& other_iterator)
	: Base_Iterator (other_iterator.Base_Iterator)
{}

/**	Constructs a Const_Reverse_Iterator from a Reverse_Iterator.

	@param	other_iterator	A Reverse_Iterator to be copied.
*/
Const_Reverse_Iterator (const Reverse_Iterator& other_iterator)
	: Base_Iterator (other_iterator.Base_Iterator)
{}


/**	Constructs a Const_Reverse_Iterator from a Const_Iterator.

	Implements explicit type conversion.

	@param	other_iterator	A Const_Iterator to be converted.
*/
explicit Const_Reverse_Iterator (const Const_Iterator& other_iterator)
	:	Base_Iterator (other_iterator.Base_Iterator)
{}

/**	Constructs an Const_Reverse_Iterator from a Base_const_reverse_iterator.

	Implements implicit type conversion.

	@param	base_iterator	A Base_const_reverse_iterator from which to construct
		this Const_Reverse_Iterator.
*/
Const_Reverse_Iterator (const Base_const_reverse_iterator& base_iterator)
	:	Base_Iterator (base_iterator)
{}

/**	Assignment operator.
	
	@param	other_iterator	A Const_Reverse_Iterator to be assigned to
		this Const_Reverse_Iterator.
	@return	This Const_Reverse_Iterator.
*/
Const_Reverse_Iterator& operator= (const Const_Reverse_Iterator& other_iterator)
{Base_Iterator = other_iterator.Base_Iterator; return *this;}

/**	Convert this Const_Reverse_Iterator to a Const_Iterator.
*/
Const_Iterator base () const
{return Const_Iterator (Base_Iterator.base ());}

/**	Assignment operator from a Reverse_Iterator.
	
	@param	other_iterator	A Reverse_Iterator to be assigned to this
		Const_Reverse_Iterator.
	@return	This Const_Reverse_Iterator.
*/
Const_Reverse_Iterator& operator= (const Reverse_Iterator& other_iterator)
{Base_Iterator = other_iterator.Base_Iterator; return *this;}

/**	Type conversion operator to Base_const_reverse_iterator.

	The backing Base_const_reverse_iterator for this Const_Reverse_Iterator is returned.
*/
operator Base_const_reverse_iterator ()
{return Base_Iterator;}

/**	Dereference operator.

	Reference semantics are implemented, even though the backing
	Base_const_reverse_iterator operates on pointer objects.

	@return	A const reference to the value at this Const_Reverse_Iterator's
		current position.
*/
const T& operator* () const
{return **Base_Iterator;}

/**	Member dereference (pointer) operator.

	The semantics for a container of objects of type T are implemented,
	even though the backing Base_const_reverse_iterator operates on a container
	of pointers to objects of type T.

	@return A pointer to the value at this Const_Reverse_Iterator's current
		position.
*/
T* operator-> () const
{return *Base_Iterator;}

/**	Array reference operator.

	@param	index	An element index of a value in the Vectal.
		<b>N.B.</b>: The validity of the index is not checked.
	@return	A const reference to the value at the Vectal index.
*/
const T& operator[] (size_type index) const
{return *Base_Iterator[index];}

/**	Pre-increment operator.

	@return	A reference to this Const_Reverse_Iterator after its position has
		been incremented.
*/
Const_Reverse_Iterator& operator++ ()
{++Base_Iterator; return *this;}

/**	Post-increment operator.

	This Const_Reverse_Iterator is incremented to the next position after a copy
	of this Const_Reverse_Iterator has been made.

	@return	A copy of this Const_Reverse_Iterator at its original position.
*/
Const_Reverse_Iterator operator++ (int)
{
Const_Reverse_Iterator
	copy = *this;
++*this;
return copy;
}

/**	Pre-decrement operator.

	@return	A reference to this Const_Reverse_Iterator after its position has
		been decremented.
*/
Const_Reverse_Iterator& operator-- ()
{Base_const_reverse_iterator::operator-- (); return *this;}

/**	Post-decrement operator.

	This Const_Reverse_Iterator is decremented to the previous position after a
	copy of this Const_Reverse_Iterator has been made.

	@return	A copy of this Const_Reverse_Iterator at its original position.
*/
Const_Reverse_Iterator& operator-- (int)
{
Const_Reverse_Iterator
	copy = *this;
--*this;
return copy;
}

/**	Moves the Const_Reverse_Iterator position forward some distance from the
	position of this Const_Reverse_Iterator.

	@param	distance	The distance (number of values) to move.
	@return	This Const_Reverse_Iterator.
*/
Const_Reverse_Iterator& operator+= (int distance)
{Base_Iterator += distance; return *this;}

/**	Provides an Const_Reverse_Iterator moved backward some distance from the
	position of this Const_Reverse_Iterator.

	A copy of this Const_Reverse_Iterator is made and the copy is repositioned.

	@param	distance	The distance - number of values - to move.
	@return	A copy of this Const_Reverse_Iterator at the new position.
*/
Const_Reverse_Iterator operator+ (int distance)
{
Const_Reverse_Iterator copy (*this);
copy += distance;
return copy;
}

/**	Moves the Const_Reverse_Iterator position backward some distance from the
	position of this Const_Reverse_Iterator.

	@param	distance	The distance - number of values - to move.
	@return	This Const_Reverse_Iterator.
*/
Const_Reverse_Iterator& operator-= (int distance)
{Base_Iterator -= distance; return *this;}

/**	Provides an Const_Reverse_Iterator moved backward some distance from the
	position of this Const_Reverse_Iterator.

	A copy of this Const_Reverse_Iterator is made and the copy is repositioned.

	@param	distance	The distance - number of values - to move.
	@return	A copy of this Const_Reverse_Iterator at the new position.
*/
Const_Reverse_Iterator operator- (int distance)
{
Const_Reverse_Iterator copy (*this);
copy -= distance;
return copy;
}

/**	Distance operator.

	@param other_iterator	Another Const_Reverse_Iterator from which to obtain
		the distance from this Const_Reverse_Iterator.
	@return The distance, in values, between this and the other
		Const_Reverse_Iterator.
*/
typename Const_Reverse_Iterator::difference_type
operator- (const Const_Reverse_Iterator& other_iterator) const
{return Base_Iterator - other_iterator.Base_Iterator;}

/**	Equality operator.

	@param other_iterator	Another Const_Reverse_Iterator to compare with this
		Const_Reverse_Iterator.
	@return	true if the Const_Reverse_Iterators are equal; false otherwise.
*/
bool operator== (const Const_Reverse_Iterator& other_iterator) const
{return Base_Iterator == other_iterator.Base_Iterator;}

/**	Inequality operator.

	@param other_iterator	Another Const_Reverse_Iterator to compare with this
		Const_Reverse_Iterator.
	@return	true if the Const_Reverse_Iterators are not equal; false otherwise.
*/
bool operator!= (const Const_Reverse_Iterator& other_iterator) const
{return ! (*this == other_iterator);}

/**	Inequality operator with a Base_const_reverse_iterator.

	@param base_iterator	A Base_const_reverse_iterator to compare with another
		Const_Reverse_Iterator.
	@param other_iterator	Another Const_Reverse_Iterator to compare with the
		Base_const_reverse_iterator.
	@return	true if the iterators are not equal; false otherwise.
*/
friend bool operator!=
	(const Base_const_reverse_iterator& base_iterator,
	 const Const_Reverse_Iterator &other_iterator)
{return ! (base_iterator == other_iterator.Base_Iterator);}

/**	Less than operator.

	@param other_iterator	Another Const_Reverse_Iterator to compare with this
		Const_Reverse_Iterator.
	@return	true if this Const_Reverse_Iterator is at a position before (less
		than) the position of the other Const_Reverse_Iterator; false otherwise.
*/
bool operator< (const Const_Reverse_Iterator& other_iterator) const
{return Base_Iterator < other_iterator.Base_Iterator;}

/**	Less than or equal to operator.

	@param other_iterator	Another Const_Reverse_Iterator to compare with this
		Const_Reverse_Iterator.
	@return	true if this Const_Reverse_Iterator is not at a position after the
		position of the other Const_Reverse_Iterator; false otherwise.
*/
bool operator<= (const Const_Reverse_Iterator& other_iterator) const
{return ! (other_iterator < *this);}

/**	Greater than operator.

	@param other_iterator	Another Const_Reverse_Iterator to compare with this
		Const_Reverse_Iterator.
	@return	true if this Const_Reverse_Iterator is at a position after (greater
		than) the position of the other Const_Reverse_Iterator; false otherwise.
*/
bool operator> (const Const_Reverse_Iterator& other_iterator) const
{return other_iterator < *this;}

/**	Greater than or equal to operator.

	@param other_iterator	Another Const_Reverse_Iterator to compare with this
		Const_Reverse_Iterator.
	@return	true if this Const_Reverse_Iterator is not at a position before the
		position of the other Const_Reverse_Iterator; false otherwise.
*/
bool operator>= (const Const_Reverse_Iterator& other_iterator) const
{return ! (*this < other_iterator);}


private:

Base_const_reverse_iterator
	Base_Iterator;

};		//	End of Const_Reverse_Iterator class.

/**	Gets a Const_Reverse_Iterator for this Vectal positioned at the last
	value.

	@return	An Const_Reverse_Iterator positioned at the last value.
*/
Const_Reverse_Iterator rbegin () const
{return Base::rbegin ();}

/**	Gets a Const_Reverse_Iterator for this Vectal positioned before the
	first value.

	@return	An Const_Reverse_Iterator positioned before the first value.
*/
Const_Reverse_Iterator rend () const
{return Base::rend ();}

};		//	End of Vectal class.
}		//	namespace idaeim
#endif	//	idaeim_Vectal_hh
