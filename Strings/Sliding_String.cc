/*	Sliding_String

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

CVS ID: Sliding_String.cc,v 1.22 2013/02/24 00:50:13 castalia Exp
*******************************************************************************/

#include	"Sliding_String.hh"
using idaeim::Strings::Sliding_String;
using idaeim::Strings::String;

#include	<string>
using std::string;
#include	<istream>
using std::istream;
#include	<sstream>
using std::ostringstream;
#include	<algorithm>
using std::min;
using std::max;
#include	<stdexcept>
#include	<cstring>




namespace idaeim
{
namespace Strings
{
//	For notational convenience:
typedef	String::Index				Index;
typedef Sliding_String::Location	Location;

/*==============================================================================
	Constants
*/
const char* const
	Sliding_String::ID =
		"idaeim::Strings::Sliding_String (1.22 2013/02/24 00:50:13)";

const Index
	Sliding_String::DEFAULT_SIZE_INCREMENT	= 8096;

const Location
	Sliding_String::DEFAULT_READ_LIMIT		= 16 * DEFAULT_SIZE_INCREMENT;

const Index
	Sliding_String::DEFAULT_NON_TEXT_LIMIT	= 1;

const Location
	Sliding_String::NO_LIMIT				= (Location)-1;

/*==============================================================================
	Constructors
*/
//	Default constructor
Sliding_String::Sliding_String ()
	:	Reader				(NULL),
		Data_Buffer			(new char[DEFAULT_NON_TEXT_LIMIT + 3]),
		Data_Buffer_Size	(DEFAULT_NON_TEXT_LIMIT + 3),
		Data_Amount			(0),
		Size_Increment		(DEFAULT_SIZE_INCREMENT),
		String_Location		(0),
		Next_Location		(0),
		Read_Limit			(NO_LIMIT),
		Total_Read			(0),
		Non_Text_Limit		(DEFAULT_NON_TEXT_LIMIT),
		Non_Text_Count		(0)
{
}

//	Construct from input stream
Sliding_String::Sliding_String
	(
	std::istream&			source,
	Location				limit
	)
	:	Reader				(&source),
		Data_Buffer			(new char[DEFAULT_NON_TEXT_LIMIT + 3]),
		Data_Buffer_Size	(DEFAULT_NON_TEXT_LIMIT + 3),
		Data_Amount			(0),
		Size_Increment		(DEFAULT_SIZE_INCREMENT),
		String_Location		(0),
		Next_Location		(0),
		Read_Limit			(limit),
		Total_Read			(0),
		Non_Text_Limit		(DEFAULT_NON_TEXT_LIMIT),
		Non_Text_Count		(0)
{
read_limit (limit); /* Just in case limit=0 */
}

//	Construct from string
Sliding_String::Sliding_String
	(
	const std::string&		string_source
	)
	:	String				(string_source),
		Reader				(NULL),
		Data_Buffer			(NULL),
		Data_Buffer_Size	(0),
		Data_Amount			(0),
		Size_Increment		(DEFAULT_SIZE_INCREMENT),
		String_Location		(0),
		Next_Location		(0),
		Read_Limit			(NO_LIMIT),
		Total_Read			(string_source.length ()),
		Non_Text_Limit		(NO_LIMIT),
		Non_Text_Count		(0)
{
}

//	Copy constructor
Sliding_String::Sliding_String
	(
	const Sliding_String&	sliding_string
	)
	:	String				(sliding_string),
		Reader				(sliding_string.Reader),
		Data_Buffer			(new char[sliding_string.Data_Buffer_Size]),
		Data_Buffer_Size	(sliding_string.Data_Buffer_Size),
		Data_Amount			(sliding_string.Data_Amount),
		Size_Increment		(sliding_string.Size_Increment),
		String_Location		(sliding_string.String_Location),
		Next_Location		(sliding_string.Next_Location),
		Read_Limit			(sliding_string.Read_Limit),
		Total_Read			(sliding_string.Total_Read),
		Non_Text_Limit		(sliding_string.Non_Text_Limit),
		Non_Text_Count		(sliding_string.Non_Text_Count)
{
memcpy (Data_Buffer, sliding_string.Data_Buffer, Data_Amount);
}

//	Destructor
Sliding_String::~Sliding_String ()
{
if (Data_Buffer)
	delete[] Data_Buffer;
}

//	Assignment
Sliding_String&
Sliding_String::operator=
	(
	const Sliding_String&	sliding_string
	)
{
if (this != &sliding_string)
	{
	String::operator= (sliding_string);
	if (Data_Buffer)
		delete[] Data_Buffer;
	Reader			= sliding_string.Reader;
	Data_Buffer		= new char[sliding_string.Data_Buffer_Size];
	memcpy (Data_Buffer, sliding_string.Data_Buffer, Data_Amount);
	Data_Buffer_Size= sliding_string.Data_Buffer_Size;
	Data_Amount		= sliding_string.Data_Amount;
	Size_Increment	= sliding_string.Size_Increment;
	String_Location	= sliding_string.String_Location;
	Next_Location	= sliding_string.Next_Location;
	Read_Limit		= sliding_string.Read_Limit;
	Total_Read		= sliding_string.Total_Read;
	Non_Text_Limit	= sliding_string.Non_Text_Limit;
	Non_Text_Count	= sliding_string.Non_Text_Count;
	}
return *this;
}

/*==============================================================================
	Accessors
*/
namespace
{
//	Exception message
string
EXCEPTION
	(
	const string&	report,
	Location		location
	)
{
ostringstream
	message;
message << Sliding_String::ID << '\n'
		<< report;
if (location != Sliding_String::NO_LIMIT)
	message << '\n'
			<< "At data input location " << location << '.';
return message.str ();
}
}	//	Local namespace

Sliding_String&
Sliding_String::reader
	(
	std::istream&	source
	)
{
if (Reader != &source)
	{
	Reader = &source;
	Data_Amount = 0;
	}
return *this;
}

Location
Sliding_String::next_location
	(
	Location		location
	)
{
//	Use get_location to slide the String to the location.
return Next_Location = get_location (location);
}

Location
Sliding_String::read_limit
	(
	Location		limit
	)
{
Location
	previous_limit = Read_Limit;
if (Reader &&
	Non_Text_Count < Non_Text_Limit)
	{
	if (limit)
		Read_Limit = limit;
	else
		Read_Limit = DEFAULT_READ_LIMIT;
	}
return previous_limit;
}

Index
Sliding_String::size_increment
	(
	Index			amount
	)
{
Index
	previous_amount = Size_Increment;
if (amount)
	Size_Increment = amount;
else
	Size_Increment = DEFAULT_SIZE_INCREMENT;
return previous_amount;
}

Index
Sliding_String::non_text_limit
	(
	Index			limit
	)
{
Index
	previous_limit = Non_Text_Limit;
if (! (Non_Text_Limit = limit))
	Non_Text_Limit = 1;
data_buffer_size (Non_Text_Limit);
return previous_limit;
}

/*==============================================================================
	Utility Methods
*/
/*	String-like methods using character stream Locations.

	The Location arguments are not limited to the current contents of
	the String; the String will be extended as needed to bring the
	specified Location into the String, until the end of input data is
	reached.

	>>> NOTE <<< Methods that search will ingest all available data if
	the search is not satisfied. There is thus a big memory consumption
	penalty for failed searches.
*/

//	A character that should not occur in any valid string.
const char
	Sliding_String::INVALID_CHARACTER = 0;

char
Sliding_String::at
	(
	Location		location
	)
{
Index
	index;
if (location < String_Location ||
	(index = get_index (location)) == length ())
	return INVALID_CHARACTER;
return string::at (index);
}

String
Sliding_String::substring
	(
	Location		start,
	Location		end
	)
{
if (start > end)
	{
	Location location = start;
	start = end;
	end = location;
	}
Index
	first,
	last;
/*
	Make sure the char before the end Location is in the String first,
	in case this causes the start index to move in the String as a
	result of sliding forward.
*/
last = get_index (end - 1) + 1;
if (last > length ())
	last = length ();
first = get_index (start);	//	index_of doesn't catch bad locations.
return string::substr (first, last - first);
}

String
Sliding_String::substr
	(
	Location		start,
	Location		length
	)
{
if ((length += start) < start)
	length = NO_LIMIT;
return substring (start, length);
}

Location
Sliding_String::skip_over
	(
	const std::string&	skip,
	Location			location
	)
{
while (is_end (location = location_of
			(String::skip_over (skip, get_index (location)))) &&
		! ended ()) ;
return location;
}

Location
Sliding_String::skip_until
	(
	const std::string&	find,
	Location			location
	)
{
while (is_end (location = location_of
			(String::skip_until (find, get_index (location)))) &&
		! ended ()) ;
return location;
}

Location
Sliding_String::location_of
	(
	const std::string&	pattern,
	Location			location
	)
{
Location
	new_location;
while (is_end (new_location = location_of
			(String::index_of (pattern, get_index (location)))) &&
		! ended ())
	{
	//	Move back to allow for a pattern match across the String end.
	if (pattern.length () > end_location ())
		location =
			max (static_cast<Location>(end_location () - pattern.length () + 1), location);
	slide ();
	}
return new_location;
}

Location
Sliding_String::location_of
	(
	char			character,
	Location		location
	)
{
while (is_end (location = location_of
			(String::index_of (character, get_index (location)))) &&
		! ended ()) ;
return location;
}

bool
Sliding_String::begins_with
	(
	const std::string&	pattern,
	Location			location,
	bool				case_sensitive
	)
{
//	Make sure the pattern doesn't overlap the end of the buffer.
Location
	end_of_pattern = get_location (location + pattern.length () - 1);
if (end_of_pattern == end_location ())
	{
	return false;
	}
return
	String::begins_with (pattern, get_index (location), case_sensitive);
}

Location
Sliding_String::get_location
	(
	Location		location
	)
{
/*
	Make sure the location has been read into the buffer. A location
	at or beyond the end of data input is set to the end location.

	Note: As a special case a location of -1 is taken to mean the
	location of the next input character (i.e. end_location ()). This
	accommodates sequential searches on the buffer where the return
	value is -1 when the search fails.
*/
if (location < String_Location)
	{
	ostringstream
		message;
	message << "Can't get to location " << location
			<< "  with the Sliding_String located at "
			<< String_Location << '.';
	throw std::out_of_range (EXCEPTION (message.str (), location));
	}
if (location == NO_LIMIT)
	{
	location = end_location ();
	}
while (location >= end_location () && slide ()) ;
if (location >= end_location ())
	{
	return end_location ();
	}
return location;
}

Index
Sliding_String::get_index
	(
	Location		location
	)
{return index_of (get_location (location));}

/*==============================================================================
	Data Buffer Management
*/
bool
Sliding_String::slide ()
{

Index
	index = min (next_index (), length ());
if (index)
	{
	//	Free the consumed data.
	erase (0, index);
	//	Update the Location of the first character in the String.
	String_Location += index;
	}

if (ended () ||
	Non_Text_Count >= Non_Text_Limit)
	{
	/*	The end of input has been reached.

		When the non-text data input threshold is first encountered
		after reading input (below) the read_limit is set to the input
		Location before the non-text data. So this condition suggests
		that the read_limit must have been changed (user set the max
		up, the limit up, then the max down?). In any case, this is an
		end of input condition.
		
		N.B.: A Non_Text_Limit of 0 prevents any input.
	*/
	return false;
	}

/*	The curent end index before extending the String
	where the new characters will start.
*/
index = length ();

/*	Read more data.

	Determine how far to extend the String:
    the lesser of the Size_Increment
	or the amount from the Total_Read up to the Read_Limit.
*/
std::streamsize
	read_amount = static_cast<std::streamsize>(Size_Increment);
if (Read_Limit != NO_LIMIT)
	read_amount = min
		(static_cast<Location>(read_amount), Read_Limit - Total_Read);
if ((index + read_amount) < capacity ())
	reserve (index + read_amount);	//	Increase the String capacity.
while (read_amount--)
	{
	//	Append to the end of any remaining data in the Data_Buffer.
	Reader->get (Data_Buffer[Data_Amount]);
	if (Reader->eof ())
		{
		//	End of input encounterd.
		Read_Limit = Total_Read;
		break;
		}
	else if (Reader->fail ())
		//	Input failure.
		throw std::ios_base::failure (EXCEPTION
			(Reader->bad () ?
				"slide: The istream has lost integrity!" :
				"slide: Reading the istream failed.",
			Total_Read + 1));
	Data_Amount++;
	Total_Read++;
	if (! data_ingest ())
		break;
	}

if (index < length ())
	//	Apply any post-slide filtering to the new data.
	filter (index);

return (! ended ());
}

/*	Data Ingest.

	Because this class is managing a <CODE>String</CODE>, it is assumed
	that non-text data from the <CODE>Reader</CODE> should not be
	transferred into the <CODE>String</CODE>; such data are not valid
	<CODE>String</CODE> characters. However, to allow for possible
	input filter requirements (such as binary record size values), a
	completed sequence of non-text data (a non-text data sequence is
	completed when a text datum is encountered from the input stream)
	smaller than <CODE>Non_Text_Limit</CODE> is allowed into the
	<CODE>String</CODE>. A non-text data sequence is held in the
	<CODE>Data_Buffer</CODE> until 1) it is completed, or 2) <CODE>the
	Non_Text_Limit</CODE> is reached.

	1 - When a non-text data sequence is completed the entire contents
	of the <CODE>Data_Buffer</CODE> are appended to the
	<CODE>String</CODE>. Since data read from the <CODE>Reader</CODE>
	during a slide may end on an incomplete non-text data sequence,
	non-text data is retained in the <CODE>Data_Buffer</CODE> between
	<CODE>slide</CODE> operations. <CODE>Non_Text_Count</CODE> is the
	number of non-text data in the <CODE>Data_Buffer</CODE>.
	<CODE>Non_Text_Count</CODE> will be reset to 0 when a data byte
	read <CODE>{@link #is_text(char) is_text}</CODE>.

	2 - When <CODE>Non_Text_Count == Non_Text_Limit</CODE> In the
	second case, an attempt is made to reposition the
	<CODE>Reader</CODE> to the first non-text datum and the
	<CODE>Read_Limit</CODE> is set to correspond to the last text datum
	which effectively halts further input from the <CODE>Reader</CODE>.
	If the limitations of the <CODE>Reader</CODE> prevent repositioning
	it back to the first non-text datum, this data remains in the
	<CODE>Data_Buffer</CODE> for retrieval by the application using the
	<CODE>{@link #remains(Index) remains}</CODE> method.

	<B>Note</B>: The <CODE>Data_Buffer</CODE> will usually only contain
	<CODE>Non_Text_Count</CODE> data bytes.
*/
bool
Sliding_String::data_ingest ()
{
if (Non_Text_Limit == NO_LIMIT)
	{
	//	Append everything.
	append (Data_Buffer, Data_Amount);
	Data_Amount = 0;
	Non_Text_Count = 0;
	}
else
	{
	for (Index
			index = Non_Text_Count;	//	Data in the head of the Data_Buffer.
			index < Data_Amount;	//	Total data in the Data_Buffer.
			index++)
		{
		if (is_text (Data_Buffer[index]))
			{
			//	Move forward to the next non-text datum, if present.
			while (++index < Data_Amount &&
					is_text (Data_Buffer[index])) ;
			//	Move the character(s) into the String.
			append (Data_Buffer, index);
			Data_Amount -= index;	//	Amount remaining.
			if (Data_Amount)
				{
				//	Move the remains to the front of the Data_Buffer.
				memmove (Data_Buffer, &Data_Buffer[index], Data_Amount);
				//	Continue with the first byte which is non-text.
				index = 0;
				}
			else
				{
				Non_Text_Count = 0;
				return true;		//	Nothing left in the Data_Buffer.
				}
			}
		if (++Non_Text_Count == Non_Text_Limit)
			{
			/*	Max non-text data.
				Try to reposition the Reader at the beginning of the sequence.
			*/
			Reader->seekg (-((std::streamoff)Non_Text_Count), std::ios::cur);
			if (Reader->fail ())
				{
				//	Fall back to pushing the character(s) back.
				Reader->clear ();
				while (Non_Text_Count)
					{
					if (! Reader->unget ())
						{
						//	Couldn't push them all back. Sorry.
						Reader->clear ();
						break;
						}
					else
						Non_Text_Count--;
					}
				}
			else
				Non_Text_Count = 0;
			Data_Amount -= Non_Text_Limit - Non_Text_Count;
			Total_Read  -= Non_Text_Limit - Non_Text_Count;
			//	The Read_Limit is set to the amount of valid data read.
			Read_Limit = Total_Read - Non_Text_Count;
			return false;
			}
		}
	}
return true;
}

void
Sliding_String::data_buffer_size
	(
	Index			size
	)
{
if (size == 0)
	size = Non_Text_Limit;
if (size == NO_LIMIT)
	size = 1;
if (size > Data_Buffer_Size)
	{
	char
		*new_buffer = new char[size];
	if (Data_Amount)
		{
		memmove (new_buffer, Data_Buffer, Data_Amount);
		}
	delete[] Data_Buffer;
	Data_Buffer = new_buffer;
	Data_Buffer_Size = size;
	}
}

Index
Sliding_String::remains
	(
	char	*buffer
	)
{
if (Data_Amount && buffer)
	memcpy (buffer, Data_Buffer, Data_Amount);
return Data_Amount;
}

/*==============================================================================
	String Filtering
*/
void
Sliding_String::insert_filter
	(
	String_Filter*	filter,
	Index			index
	)
{
if (index >= String_Filters.size ())
	String_Filters.push_back (filter);
else
	String_Filters.insert (String_Filters.begin () + index, filter);
}

bool
Sliding_String::remove_filter
	(
	String_Filter*	filter
	)
{
bool
	removed = false;
if (! filter)
	{
	removed = ! String_Filters.empty ();
	String_Filters.clear ();
	}
else
	{
	Filter_List::iterator position = String_Filters.begin ();
	while (position != String_Filters.end ())
		{
		if (*position == filter)
			{
			position = String_Filters.erase (position);
			removed = true;
			}
		else
			++position;
		}
	}
return removed;
}

void
Sliding_String::filter
	(
	Index			start,
	Index			end
	)
{
if (! String_Filters.empty ())
	for (Filter_List::iterator position = String_Filters.begin ();
		 position != String_Filters.end ();
		 ++position)
		(*position)->filter (*this, start, end);
}


}		//	namespace Strings
}		//	namespace idaeim
