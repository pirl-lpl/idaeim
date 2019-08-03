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

CVS ID: Sliding_String.hh,v 1.20 2013/07/18 05:48:55 castalia Exp
*******************************************************************************/

#ifndef	idaeim_Strings_Sliding_String_hh
#define	idaeim_Strings_Sliding_String_hh

#include	"String.hh"
#include	"String_Filter.hh"

#include	<iosfwd>
#include	<vector>


namespace idaeim
{
namespace Strings
{
/**	A <i>Sliding_String</i> provides methods to manipulate a character
	stream as if it were a <i>String</i> by backing it with an istream
	to provide the characters for the String.

	The intended use is to consume the character stream, in a
	semi-sequential manner, processing <i>statements</i> as they are
	encountered by moving along the stream from statement to statement.
	A statement is composed of a contiguous sequence of characters of
	variable length.

	The character stream may be sourced from an istream or a string. A
	string source is a convenience that allows a string to be used
	transparently in the same context where an istream might also be
	used, but instead a "pre-read" source of characters is provided
	directly (a istringstream could also be used).

	For an istream source the character stream is read into a String
	that may slide along the stream: the end of the String may be
	extended by reading increments of more characters from the stream,
	and the beginning of the String may have characters removed
	("consumed"). The stream's character contents are referenced by
	virtual <i>Location</i> values that act as indexes into the entire
	character stream; the first character of the stream is at Location
	0. The String is automatically extended to contain characters from
	any Location available in the input stream.

	To avoid a large number of read operations on the input stream, the
	String is always extended by a specified <i>size_increment</i>. In
	addition, a <i>read_limit</i> may be specified to force input
	termination at any Location in the character stream. Input may also
	be terminated when a threshold of non-text bytes (a specified
	sequential limit) occurs in the stream, as would typically be found
	after the initial label area of an image file.

	When statements from the character stream have been processed and
	are no longer needed the #next_location is updated. That part of
	the character stream in the String before the next_location is
	considered to be disposable so that the next time the String needs
	to be extended to a new stream Location the portion of the String
	containing characters from before the next_location are deleted,
	which slides the String forward. Thus instead of simply extending
	indefinitely, the String is reused by moving the contents to cover
	the section of the character stream being processed and removing
	contents no longer needed. In this way character streams of
	indefinite length may be processed using a relatively small String
	size.

	Modifications may be made to the contents of the String using any
	of the String/string methods. When the String needs to be extended,
	because a reference is made to a Location beyond the end of the
	current String contents, input characters are always appended to
	the end of the existing String contents regardless of changes that
	have been made that might have altered the number of characters it
	contains. However, the #next_location will not change unless
	specifically updated by the user, and all characters before the
	#next_location will be deleted whenever the String contents are
	slid forward, so characters before the #next_location should be
	considered consumed. Thus the Sliding_String effectively offers
	String access to all characters obtained from the istream at the
	#next_location and extending on. If the #next_location is never
	moved forward (it can be moved backwards, but not before the
	current #string_location) then all characters from the istream are
	always available (this is always the case for a string source of
	characters), but sliding the String forward past consumed
	statements is what provides efficient use of String storage for
	indefinately long input streams.

	<b>Note</b>: Those Sliding_String methods which search forward for
	character patterns will extend the String to the end of input if
	the pattern is not found. To avoid unnecessarily large String
	extensions the #read_limit can be set to some location downstream
	that the application considers to be beyond where the pattern could
	reasonably be found.

	When the character source is a string, nothing is ever read.
	Nevertheless, the contents of the String may still be appended with
	additional characters and the #next_location may still be moved
	forward. When buffer manipulation methods attempt to extend the
	String, it will still be slid forward by deleting characters before
	the #next_location. In this case virtual Location values may be
	thought of as the number of charaters consumed (slid over) plus the
	number of characters in the String.

	A special feature of the Sliding_String is its ability to apply
	filters to the data used to extend the string. Filtering objects
	that implement the String_Filter interface may be inserted in any
	order and selectively removed. All currently registered filters
	will be applied automatically to the new data after each slide.
	<b>N.B.</b>: Filtering will only be applied when the source of
	characters is an istream.

@see		String
@see		String_Filter

@author		Bradford Castalia
@version	1.20 
*/
class Sliding_String
:	public String
{
public:
/*==============================================================================
	Types
*/
//!	A virtual location in the Sliding_String.
typedef unsigned long					Location;

//!	A vector of filters.
typedef std::vector<String_Filter*>		Filter_List;

/*==============================================================================
	Constants
*/
//!	Class identification.
static const char* const
	ID;

//! The value when no limit is to be applied, or no location was found.
static const Location
	NO_LIMIT;

//!	The default limit on the number of characters to read.
static const Location
	DEFAULT_READ_LIMIT;

//!	The default String extension increment.
static const String::Index
	DEFAULT_SIZE_INCREMENT;

//!	The default limit on non-text data.
static const String::Index
	DEFAULT_NON_TEXT_LIMIT;

//!	A character that should not occur in any valid string.
static const char
	INVALID_CHARACTER;

/*==============================================================================
	Constructors
*/
/**	Creates a Sliding_String with no character source.

	@see	reader(std::istream&)
*/
Sliding_String ();

/*------------------------------------------------------------------------------
	File input
*/
/**	Creates a Sliding_String with the reader as the
    source of characters up to the specified limit.

	@param	reader	The istream to use as the source of input.
	@param	limit	The maximum number of character bytes to read.
	@see	read_limit(Location)
*/
Sliding_String (std::istream& reader, Location limit = NO_LIMIT);

/*------------------------------------------------------------------------------
	String input
*/
/**	Creates a Sliding_String with a string source of characters.

	@param	string_source	The string to use as the source of input.
*/
explicit Sliding_String (const std::string& string_source);

/*------------------------------------------------------------------------------
	Copy
*/
/**	Creates a copy of a Sliding_String.

	State is preserved, including a copy of the current String and
	the contents of the internal Data_Buffer, and the Reader is shared.
*/
Sliding_String (const Sliding_String& sliding_string);

/*------------------------------------------------------------------------------
	Destructor
*/
/**	Deletes the current Data_Buffer before destroying the object.
*/
virtual ~Sliding_String ();

/*------------------------------------------------------------------------------
	Assignment
*/
Sliding_String& operator= (const Sliding_String& sliding_string);

/*==============================================================================
	Accessors
*/
/**	Sets the istream to use as the source of characters.

	Changing the istream in mid stream may have unexpected side
	effects. Any data still being held in the internal buffer pending
	further processing before transfer to the String will be dropped.
	The #total_read will not be reset by a change of istream; i.e. a
	single virtual character input stream is seen.

	@param	reader	The reader to associate with this object. If it is
		the same as the current reader then no change is made (the
		current Data_Buffer is not emptied).
	@return	This Sliding_String.
*/
Sliding_String& reader (std::istream& reader);

/**	Gets the istream used as the source of characters.

	@return	The istream associated with this object; will be NULL
		if the object currently has no istream (e.g. it was created
		with a string character source).
	@see	reader(std::istream&)
*/
std::istream& reader () const
	{return *Reader;}

/**	Tests if the source of characters is an istream.

	@return	true if the source of characters is an istream.
*/
bool reader_source () const
	{return static_cast<bool>(Reader);}

/**	Tests if the source of characters is a string; i.e. there is
	no istream.

	@return	true if the source of characters is a string.
*/
bool string_source () const
	{return ! static_cast<bool>(Reader);}

/**	Gets the Location in the character stream of the Index in the String.

	Location values must be used when accessing the contents of the
	virtual String using Sliding_String methods.

	Location values are virtual with respect to the entire character
	input stream. Location 0 corresponds to the first character that
	was placed in the virtual String. If the number of characters in
	the String is only changed when it is slid forward, then Location
	values are relative to all characters in the input stream. However,
	since the number of characters in the String may be changed by
	String/string methods, Location values are actually relative to all
	characters that the buffer has slid over (been consumed) plus the
	current contents of the buffer; i.e. the virtual character stream.
	Changing the contents of the String may change the Location of
	characters in the String, and the effective Location of characters
	yet to be read into the String: Location values are relative to the
	current Location of the first character in the String.

	@param	index	A String Index value.
	@return	The corresponding character stream Location. The location of
		the special index value #EOS is #NO_LIMIT.
*/
Location location_of (Index index) const
	{return (index == String::EOS) ?
		NO_LIMIT : (String_Location + index);}

/**	Gets the Index in the String for the Location in the input stream.

	Index values are relative to the current contents of the String;
	Index 0 corresponds to the character currently at the beginning of
	the String. Index values must be used when using String methods.

	<b>Note</b>: Location values are long; Index values are int.

	@param	location	A Location in the input stream.
	@return	The corresponding buffer Index relative to the current
		Location of the beginning of the buffer in the input stream. The
		location of the special location value #NO_LIMIT is #EOS.
*/
String::Index index_of (Location location) const
	{return (location == NO_LIMIT) ?
		String::EOS : (Index)(location - String_Location);}

/**	Gets the current Location of the beginning of the String in the
	character stream.

	@return	The Location of the first character in the String.
*/
Location string_location () const
	{return String_Location;}

/**	Sets the next Location to position the String when sliding it
	forward.

	If the Location is beyond the end of the current String contents
	the String is slid forward to include the Location, if possible.

	@param	location	The next Location to use for the beginning of
		the String when the String is slid forward.
	@return	The new value of the next Location. The only case where
		this will be different from the specified Location is when the
		end of input has been reached before the specified Location
		could be reached; in which case the return value is the final
		end Location.
	@throws	out_of_range	If the Location is before the beginning of
		the String.
	@see	slide()
	@see	end_location()
*/
Location next_location (Location location);

/**	Gets the current value of the next Location.

	@return	The current value of the next Location.
	@see	next_location(Location)
*/
Location next_location () const
	{return Next_Location;}

/**	Sets the next Location using a String Index value.

	@param	index	The String Index value for the next Location.
	@return	The Index value of the new next Location.
	@see	next_location(Location)
*/
String::Index next_index (Index index)
	{return index_of (next_location (location_of (index)));}

/**	Gets the Index in the String of the next Location.

	@return	The Index in the String of the next Location.
	@see	next_location()
*/
String::Index next_index () const
	{return (Index)(Next_Location - String_Location);}

/**	Gets the Location immediately following the last character in the
	String.

	@return	The Location immediately following the last character in
		the String. 
*/
Location end_location () const
	{return String_Location + length ();}

/**	Sets the Location where character input is to stop.

	This is the maximum number of characters to read from the #reader.

	If the limit is 0, then input stops immediately. Use the value
	#NO_LIMIT to allow for unlimited input. Initially the read limit is
	set to the #DEFAULT_READ_LIMIT.

	<b>Note</b>: The read limit is reset to the total number of
	characters read if an istream encounters the end of the input
	stream.

	<b>Note</b>: The read limit will have no effect when the character
	source is a string. In this case nothing is ever read so the read
	limit is identical to the string size.

	<b>Note</b>: When the threshold for sequential non-text data has
	been reached while sliding the buffer, then the read limit is
	automatically set to the number of characters read before the
	non-text sequence. Once this condition has been encountered, the
	read limit can not be changed without first resetting the
	#non_text_limit threshold up to a higher value.

	This class was designed for use with the PVL Parser (which is a
	subclass) for processing PVL syntax statements. For example, the
	read limit sets the maximum initial amount of a file to read when
	parsing an image file label. Without a recognizable end-of-label
	marker it is quite possible for non-label file data to be
	interpreted as PVL statements. Thus it is advisable to set a
	reasonable limit on the amount of file data to read. It is typical
	for a PVL label to contain as one of its parameters the size of the
	label. This suggests a strategy of using a reasonably small limit
	(enough to ensure that the label size parameter, usually near the
	start of the PVL statements, will be read), finding this parameter,
	rewinding the file, and ingesting the PVL statements again using
	the parameter value as the limiting value. A less "intelligent"
	(but likely to be easier) approach is to use the default limit and
	simply check the validity of parameters, since applications are
	likely to know what parameters are valid for them.

	@param	limit	The Location where character input is to stop.
	@return	The previous limit.
	@see	non_text_limit(Index)
*/
Location read_limit (Location limit);

/**	Gets the current limit where reading characters is to stop;
	the maximum number of characters to input from the istream.

	This will be #NO_LIMIT if there is no limit to the number of
	characters to read from the character stream.

	@return	The current read limit.
	@see	read_limit(Location)
*/
Location read_limit () const
	{return Read_Limit;}

/**	Gets the total number of characters read so far.

	This value is the actual number of characters input from the
	istream regardless of any subsequent use of the data.

	@return	The total number of characters read.
*/
Location total_read () const
	{return Total_Read;}

/**	Tests if the end of input has been reached.

	The end of input occurs when the read limit has been reached. When
	the input source is a string, then it has always ended (i.e. there
	is nothing to read).

	@return	true if the character source has ended.
	@see	read_limit(Location)
*/
bool ended () const
	{return
		(Read_Limit != NO_LIMIT && Total_Read >= Read_Limit) ||
		string_source ();}

/**	Tests if there are no more characters at the next Location; the
	source of characters is effectively empty.

	There will be no more characters at the next Location if the input
	has ended and the next Location is at, or beyond, the end Location.

	@return	true if there are no more characters available.
	@see	next_location()
	@see	ended()
	@see	end_location()
*/
bool is_empty () const
	{return (ended () && Next_Location >= end_location ());}

/**	Tests if the specified Location is at or beyond the end Location.

	<b>N.B.</b>: Unlike #ended, is_end only indicates that the Location
	is beyond the current String contents. There may still be more
	characters available.

	@param	location	The Location to test.
	@return	true if the Location is at or beyond the #end_location.
	@see	end_location()
*/
bool is_end (Location location) const
	{return location >= end_location ();}

/**	Sets the size increment by which the String will be extended when
	it is slid forward in the character stream.

	If the amount is less than or equal to 0, then the
	#DEFAULT_SIZE_INCREMENT will be applied. If the amount is less than
	or equal to the #non_text_limit threshold, then it will be
	increased by that amount. If the current internal buffer array
	contains more pending input data that the size increment, then it
	will be set to the amount of data being held. If the new size
	increment is different from the length of the current internal
	buffer, then a new buffer will be allocated and any pending input
	data in the old buffer will be copied into it.

	@param	amount	The new size increment.
	@return	The previous size_increment.
	@see	non_text_limit(Index)
*/
String::Index size_increment (Index amount);

/**	Gets the size increment by which the input buffer will be
	extended when it is slid forward in the istream's input stream.

	@return	The current size increment.
	@see	size_increment(Index)
*/
String::Index size_increment () const
	{return Size_Increment;}

/**	Sets the length of a non-text data sequence that will cause input
	from the character stream to the the String to stop.

	Text characters are recognized by the #is_text method; characters
	that fail this test are non-text.

	Since this class is managing a String, it is assumed that non-text
	data from the input should not be transferred into the buffer; i.e.
	the data are not valid String characters. However, to allow for
	possible input filter requirements (e.g. binary record size
	values), a sequence of non-text data less than the limit is allowed
	into the String. When a sequence of non-text data in the input
	stream reaches the limit the #read_limit is set to the amount read
	up to, but not including, the sequence of non-text data. A limit of
	0 has the same effect as a limit of 1; i.e. no non-text data are
	acceptable. Checking for non-text data is disabled by specifing
	#NO_LIMIT; i.e. any non-text data is acceptable.

	<b>Note</b>: No sequence of non-text data as long as the limit
	amount is allowed into the String; shorter sequences are allowed to
	pass. And since the #read_limit is set to the Location in the input
	stream immediately preceeding a limiting non-text sequence, the
	String will not be slid forward beyond this Location. However, the
	non_text_limit may subsequently be increased and the #read_limit
	lifted (in that order; the #read_limit can not be changed while a
	non_text_limit block is in effect) to allow further processing of
	the input stream (data read from the istream but not transferred to
	the String is never lost). Nevertheless, any non-text data sequence
	outstanding will remain in effect and will be included in counting
	the length of the next non-text sequence; the non-text sequence
	length is only reset to 0 when a text character is seen in the
	input stream.

	<b>Warning</b>: The non_text_limit may affect the operation of an
	input filter. The input filter provided with this class requires a
	non_text_limit of a least 4.

	@param	limit	The maximum allowed sequence of non-text input data.
	@return	The previous non-text limit.
	@see	is_text(char)
	@see	read_limit(Location)
*/
String::Index non_text_limit (Index limit);

/**	Get the current limit for non-text data input.

	@return	The non-text data limit.
	@see	non_text_limit(Index)
*/
String::Index non_text_limit () const
	{return Non_Text_Limit;}

/**	Tests if a character is considered to be text.

	Text is evaluated against the ASCII code set and includes all
	printable characters - values from 0x20 (' ') to 0x7E ('~')
	inclusive - plus the usual format control characters - HT, LF, VT,
	FF, and CR (0x9 to 0xD inclusive).

	@param	character	The character to be tested.
	@return	true if the character is text; false otherwise.
*/
static bool is_text (char character)
	{
	return ((character >= 0x20 && character <  0x7F) ||	//	Printable text
			(character >= 0x9  && character <= 0xD));	//	HT,LF,VT,FF,CR
	}

/*==============================================================================
	Utility Methods
*/
/*	String-like methods using character stream locations.

	The location arguments are not limited to the current contents of
	the character buffer; the buffer will be extended as needed to
	bring the specified location into the buffer, until the end of
	input data is reached.

	<b>N.B.</b> Methods that search will ingest all available data if
	the search is not satisfied. There is thus a big memory consumption
	penalty for failed searches.
*/
/**	Gets the character at the specified Location in the virual stream.

	The current contents of the character buffer will be automatically
	slid forward to the specified Location.

	@param	location	The Location from which to get a character.
	@return	The char found at the location. If the location is invalid
		for any reason then the #INVALID_CHARACTER is
		returned.
*/
char at (Location location);

/**	Gets the substring including the characters from the start Location
	up to, but not including, the end Location in the character stream.

	The current contents of the String will be automatically slid
	forward to include the specified Locations. If the substring
	extends beyond the end of input Location, then that portion up to
	the end of input will be returned. If both start and end Locations
	are beyond the end of input, then an empty String will be returned.

	@param	start	The Location of the first character of the substring.
	@param	end		The Location of end of the substring (the Location
		immediately following the last character in the substring).
	@return	The String from the start Location, inclusive, to the end
		Location, exclusive.
*/
String substring (Location start, Location end);

/**	Gets the substring including the length number of characters from
	the start Location.

	The current contents of the String will be automatically slid
	forward to include the specified substring. If the substring
	extends beyond the end of input Location, then that portion up to
	the end of input will be returned. If both start and end Locations
	are beyond the end of input, then an empty String will be returned.

	@param	start	The Location of the first character of the substring.
	@param	length	The length of the substring.
	@return	The String from the start Location for length characters.
*/
String substr (Location start, Location length);

/**	Skips over a character set in the character stream.

	Starting at the specified Location find the Location of the next
	character that is not in the skip string. The skip string is not a
	pattern; i.e. a character in the character stream that matches any
	character in the skip string is skipped during the search. The
	return Location will be for a character that does not occur in the
	skip string.

	The String will be slid forward until a non-skip character is
	found. <b>N.B.</b>: The String will be extended to include all
	available input if all characters from the beginning Location on
	are in the skip String.

	@param	skip		The string containing characters to be skipped.
	@param	location	The Location from which to start the search.
	@return	The Location of the next character not in the skip string.
		This will be #NO_LIMIT if the end of input data is reached (i.e.
		all characters from the start Location on are in the skip
		string).
*/
Location skip_over (const std::string& skip, Location location);

/**	Skips until a member of the character set is found in the character
	stream.

	Starting at the specified Location find the Location of the next
	character that is in the find string. The find string is not a
	pattern; i.e. a character in the string that matches any character
	in the find string satisfies the search. The return Location will
	be for a character that occurs in the find string.

	The character buffer will be slid forward until a find character is
	found. <b>N.B.</b>: The character buffer will be extended to
	include all available input if all characters from the beginning
	Location on are not in the find string.

	@param	find		The string containing characters to be found.
	@param	location	The Location from which to start the search.
	@return	The Location of the next character also in the find string.
		If the end of input data is reached (i.e. all characters from the
		start Location on are not in the find string), then #NO_LIMIT is
		returned.
*/
Location skip_until (const std::string& find, Location location);

/**	Finds the next Location of the pattern string.

	Starting at the specified Location find the Location of the next
	occurance of the substring that matches the pattern string.

	The character buffer will be slid forward until the pattern
	substring is found. <b>N.B.</b>: The character buffer will be
	extended to include all available input if the pattern can not be
	found.

	@param	pattern		The string to be found.
	@param	location	The Location from which to start the search.
	@return	The Location of the beginning of the pattern substring. If
		the end of input data is reached without finding the pattern
		string, then #NO_LIMIT is returned.
*/
Location location_of (const std::string& pattern, Location location);

/**	Finds the next Location of the character.

	Starting at the specified Location find the next Location of the
	specified character.

	The String will be slid forward until the specified character is
	found. <b>N.B.</b>: The String will be extended to include all
	available input if the character can not be found.

	@param	character	The character to be found.
	@param	location	The Location from which to start the search.
	@return	The Location of the next occurance of the character. If the
		end of input data is reached without finding the character, then
		#NO_LIMIT is returned.
*/
Location location_of (char character, Location location);

/**	Tests if the pattern matches the substring starting at the Location.

	The String will be slid forward as needed to include a substring
	starting at the specified Location that is as long as the pattern
	String.

	@param	pattern		The string to compare against the substring.
	@param	location	The location of the substring to compare.
	@param	case_sensitive	Case sensitive match?
	@return	true if the substring equals the pattern string; false
		otherwise.
*/
bool begins_with
	(const std::string& pattern, Location location, bool case_sensitive = true);

private:
/**	Gets a Location into the String.

	If the Location is not current in the String, the String is slid
	forward to include the Location. A Location at or beyond the end of
	data input is set to the end Location.

	<b>N.B.</b>: As a special case a Location of #NO_LIMIT is taken to
	mean the Location of the next input character (i\.e. #end_location).
	This accommodates sequential searches on the String where the return
	value is #NO_LIMIT when the search fails.

	@param	location	The Location to get into the String.
	@return	The specified location, or the #end_location if the location
		could not be reached.
	@throws	out_of_range	If the Location is before the beginning of
		the String.
*/
Location get_location (Location location);

/**	Gets a Location into the String and returns its Index.

	@param	location	The Location to get into the String.
	@return	The Index of the specified location, or the Index of the
		#end_location if the
		location could not be reached.
	@throws	out_of_range	If the Location is before the beginning of
		the String.
	@see	get_location(Location)
*/
Index get_index (Location location);

/*==============================================================================
	Data Buffer Management
*/
public:
/**	Slide the string forward by extending it with additional input characters.

	The procedure to slide the buffer has several steps:

<dl>
	<dt><b>Slide the buffer forward.</b>
		<dd>If the #next_location is beyond the #string_location then
		the consumed contents of the buffer - i.e. from the beginning
		up to, but not including, the #next_index - are deleted.
	<dt><b>Check for end of input.</b>
		<dd>The end of input occurs when the end of the istream's data
		stream has been reached, or the amount of input has reached the
		#read_limit. When this object was constructed from a string the
		end of input has been reached by definition. When the end of
		input has been reached the buffer can not be slid so the method
		returns false.
	<dt><b>Determine how much to slide the buffer forward.</b>
		<dd>The buffer will be extended by the lesser of the amount of
		free space in the internal character array or the amount from
		the current #total_read up to the #read_limit. Of course, if
		there is no read limit then the former is always used. The
		internal character array, where characters read from the
		istream are stored for checking before being transferred to the
		object's buffer, has a length of #size_increment. However,
		during each read cycle less than the entire character array
		contents may be transferred to the buffer; the remainder is
		carried over in the array where new input data is added in the
		next read cycle. Thus the buffer will be extended by no more
		than #size_increment characters.
	<dt><b>Read characters.</b>
		<dd>Characters are read from the istream into an internal
		character array. Here they may be scanned for non-text data
		before being appended to the object's character buffer.
	<dt><b>Check for non-text data.</b>
		<dd>Characters read into the internal storage array may be
		scanned for a sequence of non-text data of the current maximum
		length. This check is only done if the #non_text_limit length
		is non-negative. Counting of sequential non-text data is
		continuous across buffer extensions and is only reset to zero
		when a text character is found. If a non-text sequence of
		#non_text_limit length is found, then only the data preceeding
		the sequence is appended to the object's character buffer and
		the #read_limit is reset to the corresponding position in the
		data stream. Shorter sequences of non-text data at the end of
		scanned input are also omitted from the transfer to the buffer,
		against the possibility of more non-text data from the next
		read, but the #read_limit is not changed. Data not transferred
		to the buffer is retained in the internal array where it is
		added to in the next read cycle.
	<dt><b>Transfer characters to the buffer.</b>
		<dd>The data in the character array, from the beginning up to
		but not including any trailing non-text data (if non-text data
		checking is enabled), is appended to the object's character
		buffer.
	<dt><b>Filter the new characters.</b>
		<dd>If input filtering is enabled, the #filter method is
		invoked on the new characters.
</dl>

	<b>N.B.</b>: Altering the istream's input position in the character
	stream may have have unpredictable consequences. The impact on the
	operation of the filter_string method in particular could be
	fatal. Without input filtering, however, it is quite possible to
	move the istream's input position (e.g. by read, ignore or seekg
	operations) to suit application needs. It is important to keep in
	mind that any such alterations will go undetected, and the value of
	the #total_read and source stream Locations will continue to
	indicate the amount of data actually read and consumed from a
	virtually sequential input stream.

	@return true if more input is available; false if the end of
		input was reached.
	@throws	std::ios_base::failure	If the read method of the istream
		failed.
	@see	next_location(Location)
	@see	string_location()
	@see	next_index()
	@see	ended()
	@see	read_limit(Location)
	@see	total_read()
	@see	size_increment(Index)
	@see	is_text(char)
	@see	non_text_limit(Index)
	@see	filter(Index, Index)
*/
virtual bool slide ();

/**	Get the remains of any data read but not appended to the String.

	This data will be the currently outstanding non-text data sequence
	read from the input stream. It will never be longer than
	Non-Text_Limit bytes.

	When a Non_Text_Limit sequence of non-text data is encountered in
	the input stream, thus ending input, an attempt is made to
	reposition the input stream back to the first non-text datum.
	However, limitations of the input stream may prevent all or part of
	this push back. This method may be used by the application to
	retieve the remaining non-text data.

	The data may be transferred to a specified char buffer, but it it
	is not removed from the internal buffer.

	@param	buffer	A pointer to char storage. This storage must be
		large enough to hold all the remaining data, which will never
		be more than Non_Text_Limit bytes. If this argument is not
		present, no data is transferred.
	@return	The number of remaining data bytes. This will be the number
		of bytes transferred if a buffer is specified.
*/
String::Index remains (char *buffer = NULL);

/*------------------------------------------------------------------------------
	Data_Buffer management variables
*/
protected:
/**	An istream source of characters.

	This will remain NULL if the source is a string.
*/
std::istream*
	Reader;

/**	Input Data Buffer.

	The Data_Buffer holds the raw data input from the Reader. The data
	bytes are scanned for non-text data before being appended to the
	String base class.
*/
char*
	Data_Buffer;

//	Data_Buffer limits:
//!	The size of the Data_Buffer (bytes).
String::Index
	Data_Buffer_Size;
//!	The number of valid data bytes in the Data_Buffer.
String::Index
	Data_Amount;
//!	The amount to read when sliding the String forward.
String::Index
	Size_Increment;

/**	Reallocate the Data_Buffer with a new size.

	@param	size	The new buffer size.
*/
void data_buffer_size (Index size);

//!	Ingest the contents of the Data_Buffer into the String.
bool data_ingest ();

//	String contents locations:
//!	Location of the first character in the buffer.
Location
	String_Location;
/**	Location of the next statement.

	Characters before this location are disposable.
*/
Location
	Next_Location;

//	Read limits:
//!	The maximum amount (bytes) to read from the input stream.
Location
	Read_Limit;
//!	The total amount read.
Location
	Total_Read;

//	Non-text limits:
//!	The number of non-text chars that will end data input.
String::Index
	Non_Text_Limit;
//!	The number of sequential non-text characters last read.
String::Index
	Non_Text_Count;

/*==============================================================================
	String Filtering
*/
public:
/**	Inserts a String_Filter pointer into the vector of
	filters that will be applied to the Sliding_String.

	A String_Filter is an abstract class that provides an
	interface used to filter the String contents during each
	#slide forward.

	@param	filter	A String_Filter pointer.
	@param	index	The vector element location for the filter. Index 0
		is the first filter, index -1 is the last filter. If the index
		is beyond the end of the list, the filter will be added to the
		end of the list. The default is to add the filter at the end of
		the list.
	@see	String_Filter
*/
void insert_filter
	(String_Filter* filter, Index index = EOS);

/**	Removes String_Filter pointers from the vector of
	filters.

	@param	filter	A pointer to the filter to be removed. If the
		pointer is NULL (the default) all filters are removed.
	@return	true if one or more filters were removed; false otherwise.
*/
bool remove_filter (String_Filter* filter = NULL);

/**	Applies all of the filters to the current String.
*/
void filter (Index start = 0, Index end = String::EOS);

/*------------------------------------------------------------------------------
*/
private:

//!	The vector of filters to be applied to the String contents.
Filter_List
	String_Filters;

};		//	End of Sliding_String class

}		//	namespace Strings
}		//	namespace idaeim
#endif	//	Sliding_String_hh
