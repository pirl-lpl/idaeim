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

CVS ID: Qstrings.cc,v 1.19 2014/04/16 03:31:59 castalia Exp


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

#include	"Qstrings.hh"

#include	<QImageReader>
#include	<QImageWriter>
#include	<QStringList>
#include	<QFont>
#include	<QFontMetrics>
#include	<QDir>
#include	<QUrl>

#include	<string>




namespace idaeim
{
#ifndef DEFAULT_PATHNAME_WRAP_LENGTH
#define DEFAULT_PATHNAME_WRAP_LENGTH	300
#endif

QString
wrapped_pathname
	(
	const QString&	pathname,
	int				wrap_length,
	const QFont&	font
	)
{
QString
	path (pathname);
if (wrap_length < 50)
	wrap_length = DEFAULT_PATHNAME_WRAP_LENGTH;
QFontMetrics
	font_metrics (font);
int
	length = font_metrics.width (path);
if (length > wrap_length)
	{
	QChar
		separator (QDir::separator ());
	QStringList
		segments (pathname.split (separator));
	QString
		section;
	path.clear ();
	for (int
			index = 0;
			index < segments.count ();
		  ++index)
		{
		if (index)
			section += separator;
		section += segments.at (index);
		length = font_metrics.width (section);
		if (length > wrap_length)
			{
			section += '\n';
			path += section;
			section.clear ();
			}
		}
	if (path.at (path.length () - 1) == '\n')
		path.remove (path.length () - 1, 1);
	if (! section.isEmpty ())
		path += section;
	}
return path;
}


QString
pathname_suffix
	(
	const QString&	source_name,
	const QString&	suffix
	)
{
QString
	pathname (source_name);
QUrl
	URL;
if (is_URL (source_name))
	{
	URL.setUrl (source_name);
	pathname = URL.path ();
	}

//	Index of beginning of basename.
int
	base_index (pathname.lastIndexOf ('/'));
if (base_index < 0)
	base_index = pathname.lastIndexOf ('\\');
++base_index;

//	Suffix w/o prefix before and including delimiter.
QString
	extension (suffix);
int
	suffix_index (extension.lastIndexOf ('.'));
if (suffix_index >= 0)
	extension.remove (0, ++suffix_index);

//	Pathame w/o suffix.
suffix_index = pathname.lastIndexOf ('.');
if (suffix_index++ < base_index)
	{
	//	No existing suffix.
	if (! extension.isEmpty ())
		pathname += '.';
	//	Special case: no pathname suffix and empty extension; leave empty.
	}
else
	pathname.remove (suffix_index, pathname.length () - suffix_index);
pathname += extension;

if (! URL.isEmpty ())
	{
	URL.setPath (pathname);
	pathname = URL.toString ();
	}

return pathname;
}


QString
magnitude_of
	(
	unsigned long long	value
	)
{
static const char* const
	MAGNITUDE = " KMGTPEZ";
double
	amount = value;
const char*
	mag;
for (mag = MAGNITUDE;
	*(mag + 1) &&
		amount >= 1023.5;
	++mag,
		amount /= 1024.0) ;
value = amount + 0.5;
QString
	representation (QString::number (value));
if (*mag != ' ')
	representation += *mag;
return representation;
}


QString
object_pathname
	(
	const QObject*	object
	)
{
QString
	pathname (object->objectName ());
while ((object = object->parent ()))
	pathname.prepend ('.').prepend (object->objectName ());
return pathname;
}

/*------------------------------------------------------------------------------
	C-string Utilities
*/
std::string
uppercase
	(
	const std::string&	a_string
	)
{
std::string
	new_string (a_string);
for (std::string::iterator
		character = new_string.begin (),
		end = new_string.end ();
		character < end;
	  ++character)
	*character = (char)toupper (*character);
return new_string;
}


std::string
remove
	(
	const std::string&	a_string,
	const char			character
	)
{
std::string
	new_string (a_string);
size_t
	index = 0;
while ((index = new_string.find (character, index)) != std::string::npos)
	new_string.erase (index, 1);
return new_string;
}


std::string
replace
	(
	const std::string&	a_string,
	const char			old_character,
	const char			new_character
	)
{
std::string
	new_string (a_string);
for (std::string::iterator
		character = new_string.begin (),
		end = new_string.end ();
		character < end;
	  ++character)
	if (*character == old_character)
		*character =  new_character;
return new_string;
}


bool
compare
	(
	const char*	this_string,
	const char*	that_string,
	bool		case_sensitive
	)
{
if (this_string &&
	that_string)
	{
	const char
		*this_ = this_string,
		*that_ = that_string;
	while (*this_ &&
		   *that_)
		if ((case_sensitive ?
				(*this_++ != *that_++) :
				(toupper (*this_++) != toupper (*that_++))))
			return false;
	if (*this_ == *that_)
		//	EOS of both strings.
		return true;
	//	EOS of only one string.
	return false;
	}
else
if (this_string ||
	that_string)
	//	Only one string is NULL.
	return false;
//	Both strings are NULL.
return true;
}

/*------------------------------------------------------------------------------
	URL identifiers
*/
bool
is_URL
	(
	const QUrl&	URL
	)
{
bool
	OK (false);
if (URL.isValid () &&
	! URL.path ().isEmpty ())
	{
	QString
		scheme (URL.scheme ().toUpper ());
	if ((scheme == "HTTP" ||
		 scheme == "HTTPS" ||
		 scheme == "JPIP" ||
		 scheme == "FTP" ||
		 scheme == "FTPS") &&
		! URL.host ().isEmpty ())
		OK = true;
	else
	if (scheme == "FILE" ||
		scheme.isEmpty ())
		OK = true;
	}
return OK;
}


bool
is_URL
	(
	const QString&	URL
	)
{
QUrl
	url (URL);
return
	! url.scheme ().isEmpty () &&
	is_URL (QUrl (URL));
}


bool
is_HTTP_URL
	(
	const QUrl&	URL
	)
{
QString
	scheme (URL.scheme ().toUpper ());
return
	(URL.isValid () &&
	 (scheme == "HTTP" ||
	  scheme == "HTTPS") &&
	! URL.host ().isEmpty () &&
	! URL.path ().isEmpty ());
}


bool
is_HTTP_URL
	(
	const QString&	URL
	)
{return is_HTTP_URL (QUrl (URL));}


bool
is_file_URL
	(
	const QUrl&	URL
	)
{
return
	(URL.isValid () &&
	(URL.scheme ().toUpper () == "FILE" ||
	 URL.scheme ().isEmpty ()) &&
   ! URL.path ().isEmpty ());
}


bool
is_file_URL
	(
	const QString&	URL
	)
{
QUrl
	url (URL);
return
	! url.scheme ().isEmpty () &&
	is_file_URL (QUrl (URL));
}


bool
is_FTP_URL
	(
	const QUrl&	URL
	)
{
QString
	scheme (URL.scheme ().toUpper ());
return
	(URL.isValid () &&
	 (scheme == "FTP" ||
	  scheme == "FTPS") &&
	! URL.host ().isEmpty () &&
	! URL.path ().isEmpty ());
}


bool
is_FTP_URL
	(
	const QString&	URL
	)
{return is_FTP_URL (QUrl (URL));}


bool
is_JPIP_URL
	(
	const QUrl&	URL
	)
{
return
	(URL.isValid () &&
	 URL.scheme ().toUpper () == "JPIP" &&
   ! URL.host ().isEmpty () &&
   ! URL.path ().isEmpty ());
}


bool
is_JPIP_URL
	(
	const QString&	URL
	)
{return is_JPIP_URL (QUrl (URL));}


bool
has_recognized_scheme
	(
	const QUrl&	URL
	)
{
QString
	scheme (URL.scheme ().toUpper ());
return
	(scheme == "HTTP"  ||
	 scheme == "HTTPS" ||
	 scheme == "JPIP"  ||
	 scheme == "FTP"   ||
	 scheme == "FTPS"  ||
	 scheme == "FILE");
}


bool
has_recognized_scheme
	(
	const QString&	URL
	)
{return has_recognized_scheme (QUrl (URL));}


bool
has_scheme
	(
	const QUrl&	URL
	)
{return ! URL.scheme ().isEmpty ();}


bool
has_scheme
	(
	const QString&	URL
	)
{return has_scheme (QUrl (URL));}


bool
is_pathname
	(
	const QString&	name,
	QString*		pathname)
{
QUrl
	URL (name);
if (is_file_URL (URL))
	{
	if (pathname)
		*pathname = URL.path ();
	return true;
	}
if (! has_recognized_scheme (URL))
	{
	if (pathname)
		*pathname = name;
	return true;
	}
return false;
}


QString
resolved_reference
	(
	const QString&	source_reference,
	const QString&	pathname
	)
{
QString
	reference (QUrl (source_reference).resolved (QUrl (pathname)).toString ());
if (source_reference.startsWith ("../"))
	//	QUrl::resolved strips source_reference of leading parent segment.
	reference.prepend ("../");
return reference;
}

/*------------------------------------------------------------------------------
	Image File Formats
*/
namespace
{
QString
	Image_Reader_Formats_File_Filters,
	Image_Writer_Formats_File_Filters;
QStringList
	*Image_Reader_Formats = NULL,
	*Image_Writer_Formats = NULL;


void
image_formats
	(
	const QList<QByteArray>&	formats,
	QStringList*				list,
	QString&					filters
	)
{
QString
	name;
int
	index = 0,
	entry = 0;
while (index < formats.size ())
	{
	name = formats.at (index++);
	//	Check for a case insensitive duplicate.
	entry = list->size ();
	while (--entry >= 0)
		if (name.compare (list->at (entry), Qt::CaseInsensitive) == 0)
			break;
	if (entry < 0)
		//	New entry; lowercase.
		list->append (name.toLower ());
	}

list->sort ();

filters = file_filters_from (*list);
}
}	//	Local namespace


QString
file_filters_from
	(
	QStringList&	list
	)
{
QString
	filters;
int
	index;
/*
	Special case: format name alias removal.
	N.B.: List entries are presumed to be lowercase.
*/
if ((index = list.indexOf ("jpg")) >= 0)
	{
	if ((list.indexOf ("jpeg")) >= 0)
		list.removeAt (index);
	else
		list.replace (index, QString ("jpeg"));
	}
if ((index = list.indexOf ("tif")) >= 0)
	{
	if ((list.indexOf ("tiff")) >= 0)
		list.removeAt (index);
	else
		list.replace (index, QString ("tiff"));
	}

for (index = 0;
	 index < list.size ();
	 index++)
	{
	if (! filters.isEmpty ())
		filters += ";; ";
	filters += file_filter_for (list.at (index));
	}
return filters;
}


QString
file_filter_for
	(
	const QString&	format
	)
{
QString
	filter;
if (! format.isEmpty ())
	{
	QString
		upper (format.toUpper ());
	if (upper == "JPG")
		upper = "JPEG";
	if (upper == "TIF")
		upper = "TIFF";
	QString
		lower (upper.toLower ());
	filter = upper + " (*." + lower + " *." + upper;
	if (upper == "JPEG")
		filter += " *.jpg *.JPG";
	if (upper == "TIFF")
		filter += " *.tif *.TIF";
	filter += ')';
	}
return filter;
}


QString
image_reader_formats_file_filters ()
{
if (! Image_Reader_Formats)
	image_reader_formats ();
return Image_Reader_Formats_File_Filters;
}


QStringList
image_reader_formats ()
{
if (! Image_Reader_Formats)
	{
	Image_Reader_Formats = new QStringList;
	image_formats
		(QImageReader::supportedImageFormats (),
		 Image_Reader_Formats,
		 Image_Reader_Formats_File_Filters);
	}
return *Image_Reader_Formats;
}


QString
image_writer_formats_file_filters ()
{
if (! Image_Writer_Formats)
	image_writer_formats ();
return Image_Writer_Formats_File_Filters;
}


QStringList
image_writer_formats ()
{
if (! Image_Writer_Formats)
	image_formats
		(QImageWriter::supportedImageFormats (),
		 Image_Writer_Formats = new QStringList,
		 Image_Writer_Formats_File_Filters);
return *Image_Writer_Formats;
}

}	//	namespace idaeim
