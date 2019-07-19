/*	Qstrings

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

CVS ID: Qstrings.hh,v 1.27 2014/02/24 02:32:31 castalia Exp


This file contains code forked from the UA HiRISE Project source code
produced by Bradford Castalia at the HiRISE Operations Center:

HiROC CVS ID: HiView_Utilities.hh,v 1.2 2012/10/23 07:45:23 castalia Exp

Copyright (C) 2009-2011  Arizona Board of Regents on behalf of the
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

#ifndef QSTRINGS_HH
#define QSTRINGS_HH

class QString;
class QUrl;
class QStringList;
class QFont;
class QObject;

#include	<string>


namespace idaeim
{
/**	Add new-lines to a pathname as needed so it will fit within
	a specified display length.

	The pathname is split at pathname separators where line wrapping
	is applied.

	@param	pathname	A QString containing a pathname.
	@param	wrap_length	The display length within which to wrap the
		pathname. The minimum wrap length is 50. The default is 300.
	@param	font	The QFont that will be used to display the
		pathname.
	@return	A QString with the possibly wrapped pathname that will
		display within the wrap length.
*/
QString wrapped_pathname (const QString& pathname, int wrap_length,
	const QFont& font);

/**	Set the suffix (extension) of a pathname.

	If the source name is a URL representation the pathname is the path
	field. The returned string will be the URL representation with the
	modified path field.

	The suffix of the pathname is all characters after, but not
	including, the last '.' character that follows the last pathname
	segment delimiter character ('/' or '\\').

	The effective new suffix is the characters following the last '.'
	delimiter of the specified suffix string, or all the specified suffix
	characters if there is no such delimiter. The effective new suffix
	replaces the existing suffix. If the pathname does not have a suffix
	(there is no '.' in the pathname) and the effective suffix is empty
	then the pathname is unchanged; if the effective suffix is not empty,
	then a '.' delimiter is added and the speciffied suffix is appended.
	In all other cases an empty effective suffix will replace any
	existing suffix.

	@param	source_name	A filesystem pathname or URL representation.
	@param	suffix	The new pathname suffix. <b>N.B.</b>: The characters
		following the last '.' delimiter - or all characters if there is
		no such delimiter - will be used.
	@return	A QString containing the modified source name.
*/
QString pathname_suffix (const QString& source_name, const QString& suffix);

/**	Generate the representation of a value with magnitude annotation.

	The representation has the form:

	vS

	where v is the effective scaled value and S is the magnitude scaling
	annotation character.

	The value is scaled down by binary magnitude (1024) increments until
	it is less than 1024 after rounding up to the nearest integer. This
	is the scaled value. The scaled value is rounded up to the nearest
	integer to produce the effective scaled value.

	The magnitude scaling annotation character is determined by the
	number of scaling increments applied to the value selected from the
	"KMGTPEZ" character set, where one scaling increment selects 'K', two
	selects 'M', etc. If no scaling increments are applied (the value is
	less than 1024) then no magnitude scaling annotation will be appened
	to the value representation.

	@param	value	An unsigned long long integer value to be represented.
	@return	A QString with the scaled value representation and a possible
		magnitude scaling annotation suffix.
*/
QString magnitude_of (unsigned long long value);

/**	Get the fully qualified name for a QObject.

	A fully qualified QObject name is the QObject name prepended
	by the names of all its parent QObjects separated by the
	period ('.') character.

	@param	object	A pointer to a QObject.
	@return	A QString containing the fully qualified pathname for the
		object.
*/
QString object_pathname (const QObject* object);

/*------------------------------------------------------------------------------
	URL identifiers
*/
/**	Test if a string provides a valid URL representation.

	A valid URL (Uniform Resource Locator), a type of standard uniform
	resource identifier (URI), was originally specified by RFC 1738
	(http://www.ietf.org/rfc/rfc1738.txt) with subsequent improved RFCs
	from the Network Working Group (e.g. RFC 3986:
	http://www.apps.ietf.org/rfc/rfc3986.html).

	For the purposes of this method a URL is of the form:

	[<i>scheme</i>[<i>host</i>[<i>port</i>]]]<i>path</i>

	where:

	<dl>
	<dt><i>scheme</i>
	<dd>Specifies the network scheme (a.k.a. protocol) to be employed
		when accessing the resource. This method recognizes the (case
		insensitive) "http" or "https", "ftp" or "ftps", "file", and
		"jpip" schemes. The field is terminated with a colon (':')
		delimiter.

		A URL representation string without a scheme is defined by this
		method to not be a valid URL; however a QUrl constructed from
		such a string implicitly has the file scheme by the {@link
		is_URL(const QUrl&) the method that takes a QUrl argument} and is
		therefore defined as a valid URL. (see the Note below)

	<dt><i>host</i>
	<dd>Is the hostname of the system where a server for the scheme is
		expected to be running. Typically a hostname is fully qualified
		with dot separated segments. The field is preceded with a double
		slash ("//") delimiter. The file scheme may omit the host field.

	<dt><i>port</i>
	<dd>Is the port number on the host to be used when connecting to the
		server. Many protcols are associated with a default port number
		that the client system will automatically use when no port numbe
		is sepecified in a URL. The field is preceded with a colon (':')
		delimiter. The default port depends on the scheme.

	<dt><i>path</i>
	<dd>The pathname to the file on the host system containing the
		resource. The pathname typically starts at the server's root
		directory, indicated by a slash ('/'), is followed by one or more
		directory name segments, separated by slash ('/') characters, and
		ends with the name of the resource file. For the file scheme
		a relative path will not start with a slash.

		A URL representation string composed of only a path field will
		have no scheme - <b>but note the {@link is_file_URL(const QUrl&)
		WARNING about MS/Windows pathnames}</b> - which is defined by
		this method to not be a valid URL; however a QUrl constructed
		from such a string implicitly has the file scheme by the {@link
		is_URL(const QUrl&) the method that takes a QUrl argument} and is
		therefore defined as a valid URL. (see the Note below)
	</dl>

	The <i>host</i>, <i>port</i> and <i>path</i> field values are not
	checked, however the representation must conform to the URL
	specification with correct field delimiters and the <i>port</i>
	field, if present, must hav e a numeric value. Also, this method
	requires that both the <i>host</i> and <i>path</i> fields must be
	present - except the <i>host</i> field is optional for the file
	scheme.

	<b>Note</b>: A string that constructs a QUrl without a scheme is not
	a URL, it is a local filesystem pathname. Even though a QUrl without
	a scheme is taken to have a file scheme and thus the path field
	refers to a local filesytem pathname, the distinction in this case is
	that is_URL with a string argument is asking if the string describes
	what is recognized as a valid URL specification, as opposed to a local
	pathname, while {@link is_URL(const QUrl&) is_URL with a QUrl
	argument} is asking if the URL is for a valid and recognized URL
	object.

	<b>N.B.</b>: If the URL does not have one of the {@link
	has_recognized_scheme(const QString&) recognized shemes} it is not
	considered to be a valid URL.

	@param	URL	A string representation of a URL.
	@return	true if the URL string appears to be represent a valid URL
		with a recognized scheme and the expected fields; false otherwise.
	@see is_HTTP_URL(const QString&)
	@see is_FTP_URL(const QString&)
	@see is_file_URL(const QString&)
	@see is_JPIP_URL(const QString&)
	@see is_pathname(const QString&, QString*)
*/
bool is_URL (const QString& URL);

/**	Test for a valid URL.

	<b>N.B.</b>: This method will return true if the URL has a path field
	but no scheme, while {@link is_URL(const QString&) is_URL with a
	string argument} will return false.

	@param	URL	A QUrl.
	@return	true if the URL appears to specify a valid URL with a
		recognized scheme and the expected fields; false otherwise.
	@see is_URL(const QString&)
*/
bool is_URL (const QUrl& URL);

/**	Test if a string provides a valid HTTP URL representation.

	A HTTP URL is a valid URL with a "http" or "https"  (case
	insensitive) scheme and has non-empty host and path fields.

	@param	URL	A string representation of a URL.
	@return	true if the URL string appears to represent a valid HTTP URL
		with the expected fields; false otherwise.
	@see is_URL(const QString&)
	@see is_HTTP_URL(const QUrl&)
*/
bool is_HTTP_URL (const QString& URL);

/**	Test for a valid HTTP URL.

	@param	URL	A QUrl.
	@return	true if the URL appears to specify a valid URL with the
		expected fields; false otherwise.
	@see is_HTTP_URL(const QString&)
*/
bool is_HTTP_URL (const QUrl& URL);

/**	Test if a string provides a valid file URL representation.

	A file URL is a valid URL with the "file" (case insensitive) scheme
	and a non-empty path field. <b>N.B.</b>: If the string does not
	include a scheme field it is not a file URL.

	@param	URL	A string representation of a URL.
	@return	true if the URL string appears to represent a valid file URL
		with a path field; false otherwise.
	@see is_URL(const QString&)
	@see is_file_URL(const QUrl&)
	@see is_pathname(const QString&, QString*)
*/
bool is_file_URL (const QString& URL);

/**	Test for a valid file URL.

	A file URL is a valid URL with the "file" (case insensitive)
	scheme, or an empty scheme, and a non-empty path field.
	<b>N.B.</b>: Unlike the is_file_URL with a string argument, this
	function will return true if the URL has an empty scheme field.

	<b>>>> WARNING <<<</b> A filesystem pathname on MS/Windows that
	starts with a drive specification - e.g. "D:" - will be interpreted
	as having a URL scheme specification that is the drive letter when a
	QUrl is constructed from such a pathname string. Such a QUrl will
	test false by this function, which is not the intended result, but
	will also test false by the is_URL function. It is a reasonable
	presumption in most situations that a string that produces a QUrl
	{@link has_scheme(const QUrl&) without a scheme} or does not have a
	{@link has_recognized_scheme(const QUrl&) recognized scheme} is
	probably a filesystem pathname. Nevertheless, there is still the
	possibility that the string represents a URL with an unrecognized
	scheme; this uncertainty is unavoidable.

	@param	URL	A QUrl.
	@return	true if the URL appears to specify a valid URL with the
		expected fields; false otherwise.
	@see is_file_URL(const QString&)
	@see is_URL(const QUrl&)
*/
bool is_file_URL (const QUrl& URL);

/**	Test if a string provides a valid FTP URL representation.

	A FTP URL is a valid URL with the "FTP" or "FTPS" (case insensitive)
	scheme and a non-empty host and path fields.

	@param	URL	A string representation of a URL.
	@return	true if the URL string appears to represent a valid FTP URL
		with a host and path field; false otherwise.
	@see is_URL(const QString&)
	@see is_FTP_URL(const QUrl&)
*/
bool is_FTP_URL (const QString& URL);

/**	Test for a valid FTP URL.

	@param	URL	A QUrl.
	@return	true if the URL appears to specify a valid URL with the
		expected fields; false otherwise.
	@see is_FTP_URL(const QString&)
*/
bool is_FTP_URL (const QUrl& URL);

/**	Tests if a string provides a valid JPIP URL representation.

	A JPIP URL is a valid URL with the "jpip" (case insensitive) scheme
	and non-empty host and path fields.

	@param	URL	A string representation of a URL.
	@return	true if the URL string appears to represent a valid JPIP URL
		with the expected fields; false otherwise.
	@see is_URL(const QString&)
	@see is_JPIP_URL(const QUrl&)
*/
bool is_JPIP_URL (const QString& URL);

/**	Test for a valid JPIP URL.

	@param	URL	A QUrl.
	@return	true if the URL appears to specify a valid URL with the
		expected fields; false otherwise.
	@see is_JPIP_URL(const QString&)
*/
bool is_JPIP_URL (const QUrl& URL);

/**	Tests if a string represents a URL with a recognized scheme.

	The recognized schemes are (case insensitive): "http" or "https",
	"ftp" or "ftps" "jpip", and "file". The empty scheme is allowed in
	some contexts, but is not a recognized scheme.

	@param	URL	A string representation of a URL.
	@return true if the URL string appears to have a recognized scheme,
		including no scheme; false otherwise.
	@see has_recognized_scheme(const QUrl&)
	@see is_URL(const QString&)
*/
bool has_recognized_scheme (const QString& URL);

/**	Tests if a URL has a recognized scheme.

	The recognized schemes are (case insensitive): "http" or "https",
	"ftp" or "ftps" "jpip", and "file". The empty scheme is allowed in
	some contexts, but is not a recognized scheme.

	@param	URL	A QUrl.
	@return true if the URL has a recognized scheme, including no scheme;
		false otherwise.
	@see has_recognized_scheme(const QString&)
	@see is_URL(const QUrl&)
*/
bool has_recognized_scheme (const QUrl& URL);

/**	Tests if a string represents a URL with a scheme.

	@param	URL	A string representation of a URL.
	@return true if the URL has a non-empty scheme; false otherwise.
	@see has_scheme(const QUrl&)
	@see is_URL(const QString&)
*/
bool has_scheme (const QString& URL);

/**	Tests if a URL has a scheme.

	@param	URL	A QUrl.
	@return true if the URL has a non-empty scheme; false otherwise.
	@see has_scheme(const QString&)
*/
bool has_scheme (const QUrl& URL);

/**	Get a string that appears to be suitable as a filesystem pathname.

	If the name is for {@link is_file_URL(const QString&) a file URL} the
	path field of the URL is the filesystem pathname. If the name is not
	for {@link is_URL(const QString&) a recognized URL} the unaltered
	name is the filesystem pathname. Otherwise there is no filesystem
	pathname.

	@param	name	The QString to be tested.
	@param[out]	pathname	A pointer to a QString that, if non-NULL,
		will receive the filesystem pathname, or remain unchanged if no
		filesystem pathname is found.
	@return	true if the name appears to be suitable as a filesystem
		pathname; false otherwise.
*/
bool is_pathname (const QString& name, QString* pathname = NULL);

/**	Resolve a pathname relative to a source reference.

	The source reference is used as a base to resolve a relative
	pathname. The source reference may be a filesystem pathname
	or URL representation. Its pathname may be absolute or relative.

	The specified pathname may be absolute or relative. An absolute
	pathname replaces the pathname of the source reference. A
	relative pathname is resolved against the source reference
	pathname as a base. For example:

	@code

	QString
		reference;

	reference = resolved_reference
		("http://host/path/to/file",
		 "/absolute/location");
	// reference = "http://host/absolute/location"

	reference = resolved_reference
		("http://host/path/to/file",
		 "another/location")
	// reference = "http://host/path/to/another/location"

	reference = resolved_reference
		("http://host//a/path/to/some/file",
		 "../../resolved");
	// reference = "http://host/a/path/resolved"

	@endcode

	If the specified pathname is a URL reference it is returned
	unchanged.

	@param	source_reference	A QString that provides the source
		reference against which the pathname will be resolved.
	@param	pathname	A QString that provides the pathname to be
		resolved against the source reference.
	@return	A QString that is the pathname resolved against the
		source reference.
*/
QString resolved_reference
	(const QString& source_reference, const QString& pathname);

/*------------------------------------------------------------------------------
	Image File Formats
*/
/**	Get the list of image file format names recognized for read support
	on the host system

	@return	A list of image file format names recognized for read
		support on the host system. Only lower case format names will be
		inluded in the list; each name in the list will be unique; and
		the list will be sorted alphabetically.
	@see image_reader_formats_file_filters()
	@see image_writer_formats()
*/
QStringList image_reader_formats ();

/**	Get a file name filters string of image reader formats supported on
	the host system.

	The string is suitable for use with QFileDialog and its
	selectNameFilter method.

	@return	A QString providing a set of file name filters for the image
		file formats supported for reading on the host system.
	@see image_reader_formats()
*/
QString image_reader_formats_file_filters ();

/**	Get the list of image file format names recognized for write support
	on the host system.

	@return	A list of image file format names recognized for write
		support on the host system. Only lower case format names will be
		inluded in the list; each name in the list will be unique; and
		the list will be sorted alphabetically.
	@see image_writer_formats_file_filters()
	@see image_reader_formats()
*/
QStringList image_writer_formats ();

/**	Get a file name filters string of image writer formats supported on
	the host system.

	The string is suitable for use with QFileDialog and its
	selectNameFilter method.

	@return	A QString providing a set of file name filters for the image
		file formats supported for writing on the host system.
	@see image_writer_formats()
*/
QString image_writer_formats_file_filters ();

/**	Produce a file name filter string for a file format (filename
	extension) name.

	@param	format	A QString containing a format name.
	@return	A QString containing a file name filter suitable for
		use with QFileDialog.
*/
QString file_filter_for (const QString& format);

/**	Produce a file name filters string from a list of file format
	(filename extension) names.

	@param	list	A QStringList containing a list of file format names.
	@return	A QString containing a set of file name filters suitable for
		use with QFileDialog.
*/
QString file_filters_from (QStringList& list);

/*------------------------------------------------------------------------------
	C-string Utilities
*/
/**	Convert a string to its uppercase form.

	@param	a_string	A std::string to be converted.
	@return	A copy of the string in uppercase form.
*/
std::string uppercase (const std::string& a_string);

/**	Remove all occurances of a character from a string.

	@param	a_string	A std::string to be filtered.
	@param	character	The character to be removed.
	@return	A copy of the string with all occurances of the
		character removed.
*/
std::string remove (const std::string& a_string, const char character);

/**	Replace all occurances of a character in a string with a new character.

	@param	a_string	A std::string to be filtered.
	@param	old_character	The character to be replaced.
	@param	new_character	The new character to replace the old character.
	@return	A copy of the string with all occurances of the
		old character replaced with the new character.
*/
std::string replace (const std::string& a_string,
	const char old_character, const char new_character);

/**	Compare two C-strings.

	If both string pointers are NULL the comparison is true. Otherwise,
	if either string pointer is NULL the comparison is false.

	@param	this_string	A C-string.
	@param	that_string	A C-string.
	@param	case_sensitive	If true a case sensitive comparison is done;
		if false the case of the strings is ignored.
	@return	true if the strings match; false otherwise.
*/
bool compare (const char* this_string, const char* that_string,
	bool case_sensitive = false);

}		//	namespace idaeim
#endif	//	QSTRINGS_HH
