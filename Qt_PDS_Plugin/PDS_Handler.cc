/*	PDS_Handler

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

CVS ID: PDS_Handler.cc,v 1.18 2014/03/17 05:58:13 castalia Exp
*******************************************************************************/

#include "PDS_Handler.hh"

#include "PVL.hh"
using idaeim::PVL::Parser;
using idaeim::PVL::Aggregate;
using idaeim::PVL::Value;

#include "Qt_Utility/Qistream.hh"
#include "Qt_Utility/Qstrings.hh"
#include "Qt_Utility/Qlisters.hh"
#include "Qt_Utility/misc.hh"

#include <QImage>
#include <QVector>
#include <QPair>
#include <QVariant>
#include <QSize>
#include <QDir>
#include <QFileInfo>
#include <QFile>
#include <QDateTime>
#include <QProcess>

#include <string>
using std::string;
#include <cstring>
using std::memcpy;
#include <sstream>
using std::ostringstream;
#include <iomanip>
using std::endl;
#include <cmath>
#include <limits>
using std::numeric_limits;
#include <climits>
#include <cfloat>
#include <stdexcept>
using std::bad_alloc;




namespace idaeim
{
/*==============================================================================
	Types
*/
typedef Aggregate::Selection		Selection;

typedef QPair<int, const char*>		Value_for_String;
#define MAPPING_VALUE  first
#define MAPPING_STRING second

typedef QVector<Value_for_String>	Value_for_Strings;

/*==============================================================================
	Constants
*/
const char* const
	PDS_Handler::ID =
		"idaeim::PDS_Handler (1.18 2014/03/17 05:58:13)";


const QStringList
	PDS_Handler::FORMATS =
		(QStringList ()
		<< "PDS"
		<< "pds"
		<< "IMG"
		<< "img"
		<< "IMQ"
		<< "imq");

const char* const
	PDS_Handler::QIMAGE_FORMAT_NAME[] =
	{
	"Invalid",
	"Mono",
	"MonoLSB",
	"Indexed8",
	"RGB32",
	"ARGB32",
	"ARGB32_Premultiplied"
	"RGB16",
	"ARGB8565_Premultiplied",
	"RGB666",
	"ARGB6666_Premultiplied",
	"RGB555",
	"ARGB8555_Premultiplied",
	"RGB888",
	"RGB444",
	"ARGB4444_Premultiplied"
	};

QVector<QRgb> PDS_Handler::PALATTE_8BIT (256);

/*==============================================================================
	Local static data
*/
namespace
{
/*..............................................................................
	Image magic.

	Each pair is the file offset, from the beginning of the file, to a
	possible string which, if present, determines that the file has a PDS
	format.

	Note that although the PDS format identification is contained in the
	name of the first parameter of the file's label text, it is the
	initial bytes of the file that are examined before any attempt is
	made to parse label parameters.
*/
static const Value_for_Strings
	MAGIC =
		(Value_for_Strings ()
		//	PDS_VERSION_ID parameter.
		<< Value_for_String (0, "PDS_")
		<< Value_for_String (0, "NJPL1I00")
		<< Value_for_String (2, "NJPL1I")
		<< Value_for_String (0, "CCSD3ZF")
		<< Value_for_String (2, "CCSD3Z")
		//	Old VICAR files.
		<< Value_for_String (0, "LBLSIZE="));

/*..............................................................................
	Compressed image

	In some cases the image data has been stored in a compressed format
	and will need to be decompressed by an external decompression filter
	program before use. If the compression type is unknown, then there
	will be no decompression filter.
*/
enum
	{
	COMPRESSION_TYPE_NONE,
	COMPRESSION_TYPE_HUFFMAN,
	COMPRESSION_TYPE_JPEG,
	COMPRESSION_TYPE_MOC
	};

static const Value_for_Strings
	COMPRESSION_TYPE_MAPPINGS =
		(Value_for_Strings ()
		<< Value_for_String (COMPRESSION_TYPE_HUFFMAN, "HUFFMAN_FIRST_DIFFERENCE")
		<< Value_for_String (COMPRESSION_TYPE_JPEG,    "CLEM-JPEG")
		<< Value_for_String (COMPRESSION_TYPE_MOC,     "MOC-PRED-X-5")
		<< Value_for_String (COMPRESSION_TYPE_MOC,     "MOC-DCT-2")
		/*
			VICAR files have an ENCODING_TYPE parameter,
			but the image data is not compressed.
		*/
		<< Value_for_String (COMPRESSION_TYPE_NONE,	   "INTEGER COSINE"));

/*	Decompression filters.

	The filter strings must be listed in COMPRESSION_TYPE_XXX order.
	Each filter string is a QString arg format string for producing a
	command line in which the arguments will be the input filename and
	the output filename, in that order.
*/
static const char*
	DECOMPRESSION_FILTER[] =
	{
	"echo No decompression.",
	///	format-code 4 = no output label.
	"vdcomp  %1 %2 4",
	//	-n = no output label; -i = full resolution image.
	"clemdcmp -n -i %1 %2",
	/*
		>>> WARNING <<< Always produces output label
		though it should be the same size as the original label.
	*/
	"readmoc %1 %2"
	};

/*	Whether the decompression filter produces a file with a label.

	Unfortunately, not all decompression filters are able to produce an
	output file without the unnecessary - and potentially complicating -
	metadata label.
*/
static const bool
	DECOMPRESSED_FILE_HAS_LABEL[] =
	{
	false,
	false,
	false,
	true
	};

/*..............................................................................
	Data type.

	The presence of one of the mapped strings in the Data_Type_Name
	indicates that the Data_Type is the mapped value, otherwise the
	Data_Type is DATA_TYPE_INTEGER.
*/
enum
	{
	DATA_TYPE_INTEGER,
	DATA_TYPE_UNSIGNED_INTEGER,
	DATA_TYPE_REAL
	};
static const char*
	DATA_TYPE_NAME[] =
	{
	"integer",
	"unsigned integer",
	"real"
	};

static const Value_for_Strings
	DATA_TYPE_MAPPINGS =
		(Value_for_Strings ()
		<< Value_for_String (DATA_TYPE_UNSIGNED_INTEGER, "BYTE")
		<< Value_for_String (DATA_TYPE_UNSIGNED_INTEGER, "UNSIGNED")
		<< Value_for_String (DATA_TYPE_INTEGER,          "HALF")
		<< Value_for_String (DATA_TYPE_REAL,             "REAL")
		<< Value_for_String (DATA_TYPE_REAL,             "FLOAT"));

/*..............................................................................
	Data order.

	The presence of one of the mapped strings at the beginning of the
	Data_Type_Name indicates the byte order of the image data. Initially
	the Data_Order is assumed to be the same as the host machine and
	Swap_Bytes is false, but if the Data_Type_Name results in the
	Data_Order being changed, then Swap_Bytes will be true.
*/
enum
	{
	DATA_ORDER_MSB,
	DATA_ORDER_LSB
	};

static const Value_for_Strings
	DATA_ORDER_MAPPING =
		(Value_for_Strings ()
		<< Value_for_String (DATA_ORDER_MSB, "MSB")
		<< Value_for_String (DATA_ORDER_MSB, "HIGH")
		<< Value_for_String (DATA_ORDER_MSB, "SUN_")
		<< Value_for_String (DATA_ORDER_LSB, "LSB")
		<< Value_for_String (DATA_ORDER_LSB, "LOW")
		<< Value_for_String (DATA_ORDER_LSB, "PC_")
		<< Value_for_String (DATA_ORDER_LSB, "VAX_"));

/*..............................................................................
	Datum format.

	Translation of the Datum_Format string (if any) into Sample_Bits.
*/
static const Value_for_Strings
	DATUM_FORMAT_MAPPING =
		(Value_for_Strings ()
		<< Value_for_String ( 8, "BYTE")
		<< Value_for_String (16, "HALF")
		<< Value_for_String (32, "REAL"));

#ifdef MAXFLOAT
#define MINIMUM_REAL_VALUE				(-MAXFLOAT)
#define MAXIMUM_REAL_VALUE				( MAXFLOAT)
#else
#define MINIMUM_REAL_VALUE				((float)-3.40282346638528860e+38)
#define MAXIMUM_REAL_VALUE				((float) 3.40282346638528860e+38)
#endif

//..............................................................................
}	//	Local namespace

/*==============================================================================
	Private Data

	The Private_Data class encapsulates all of the internal data
	variables used by the PDS_Handler. The data variables are of two
	kinds: A list of parameter Selections and the variables associated
	with them that reflect the selected parameter values, and image data
	access and rendering control variables that are derived from the
	selected parameter values obtained from the data source metadata
	label; the latter may, in some case, be the same as the former.

	Note that the data source metadata label is owned by the PDS_Handler,
	and is not encapsulated in the Private_Data.

	The Private_Data class has only two methods: A method to reset all
	variables to there uninitialized state, and a method to set all
	variables based on values obtained from the source metadata label
	parameter Selections.
*/
class PDS_Handler::Private_Data
{
public:

/*------------------------------------------------------------------------------
	Image data to read
*/
//	Set on successfull completion of a set method.
bool
	Initialized;

//	An external image data file; image data is not in the file with the label.
QIODevice
	*Detached_Source;

//	A temporary file containing decompressed image data.
QFile
	*Decompressed_Source;

//	The DATA_TYPE_XXX code of the image data.
int
	Data_Type;

//	The DATA_ORDER_XXX code of the image data.
int
	Data_Order;
bool
	Swap_Bytes;

//	The type of image data compression of the source.
int
	Compression_Type;

//	The QImage::Format that determines how to treat source image bands.
QImage::Format
	Source_Format;

float
	Gamma;

/*	The starting band number of the last image read.
	This will be -1 before the first image is read.
*/
int
	Current_Band;
//	The starting band number for the next image to be read.
int
	Next_Band;

//	Description of the reason for the last read failure.
string
	Read_Failure_Reason;

/*------------------------------------------------------------------------------
	Image characteristics
*/
Value::Integer_type
	/*..........................................................................
		Label
	*/
	Record_Bytes,			// Record size.
	Label_Records,			// Number of records in the label.
	Header_Records,			// Binary data preceding the image data.
	Image_Record,			// Record number where the image data begins.
	Image_Offset_Bytes,		// Byte offset where the image data begins.

	/*..........................................................................
		Image
	*/
	Image_Dimensions[3],
	#define COLUMNS			0
	#define LINES			1
	#define BANDS			2

	Sample_Bytes,
	Sample_Bits,
	Sample_Bit_Mask,

	Line_Prefix_Bytes,		// Amount to skip before a data line.
	Suffix_Multiplier,		// Bytes per suffix item (0 == 1).
	Suffix_Bytes[3];		// Amount to skip after ...
	#define LINE_SUFFIX		0
	#define COLUMN_SUFFIX	1
	#define BAND_SUFFIX		2

Value::Integer_type
	EOL;

Value::Real_type
	Data_Limits[2];
	#define LIMIT_MIN		0
	#define LIMIT_MAX		1

Value::String_type
	Image_Data_Filename,
	Image_Name,
	Data_Type_Name,
	Datum_Format,
	Compression_Method;

//	Label parameter name-to-variable mappings.
QVector<Selection>
	Selections;


/*	Constructs Private_Data for a PDS_Handler.

	All internal variables are reset to the uninitialized values, and
	the list of metadata parameter Selections is initialized.
*/
Private_Data ()
	:
	Detached_Source (NULL),
	Decompressed_Source (NULL)
{
reset ();

/*	Parameter Selections

	Each Parameter Selection list entry specifies the name of a parameter
	that may be found in the PDS label and the location where the
	parameter value(s) are to be stored. The PDS label is searched for
	each parameter name and, when found, value(s) are assigned to the
	variable location.

	Thus it is possible when a parameter is found that its value could
	replace that found from a previous parameter that was found and
	linked to the same variable.

	1) A Selection name is an absolute or relative Parameter pathname:
	absolute pathnames start with a slash character ('/') and may contain
	a slash delimited sequence of Paramater group names before the final
	Parameter name; a relative pathname does not start with a slash
	though it may contain a leading sequence of Parameter group names. An
	absolute pathname is fully qualified such that the selected Parameter
	may only occur at that pathname; a relative pathname is for a
	Parameter that may occur withing any Parameter group, including the
	top level (root) group. Only Assignment Parameters will be selected.

	2) A Selection variables must be (or can be cast to) one of the
	PVL::Value types:

		-Integer_type
		-Real_type
		-String_type

	The variable will receive the value(s) from the Parameter at the
	matching with Value(s) of the matching data type.

	Variables not assigned a value do not have their existing value
	changed. This enables unassigned variables to be detected by having a
	special initialization value, or to be initialized with a default
	value.

	3) A Selection count indicates how many values are expected. The
	default count is 1. A count greater than one implies that the
	Selection variable is for the first element of an array of at least
	count elements. If the selected Parameter does not have an Array of
	at least count Values with the required data type the remaining array
	values will remain unchanged. Values are assigned to the variable in
	the order they are encountered in the Parameter Array.

	>>> CAUTION <<< The order matters: All Selection entries are searched
	for and, when found, replace values that may have been obtained from
	previously found Selections. The first Assignment Parameter at the
	matching pathname is selected; duplicates are not seen.
*/
Selections
	/*
		The key information being sought here is the location of the
		image data in the file; i.e. the image offset bytes. In some
		products this is determined by finding the size of the label that
		immediately preceeds the image data, in others it is provided
		directly with a "^" pointer parameter. In either case, the value
		may be a byte or a record count. For the latter a record size in
		bytes, and possibly the number of records in the label, is
		provided so the image data location can be calculated.
	*/
	<< Selection ("RECORD_BYTES",
		Record_Bytes)
	<< Selection ("/RECSIZE",
		Record_Bytes)
	<< Selection ("HEADER_RECORD_BYTES",
		Record_Bytes)
	<< Selection ("HEADER_RECORDS",
		Label_Records)
	<< Selection ("LABEL_RECORDS",
		Label_Records)
	<< Selection ("/NLB",
		Header_Records)
	<< Selection ("/^QUBE",
		Image_Record)
	<< Selection ("/^SPECTRAL_QUBE",
		Image_Record)
	<< Selection ("LBLSIZE",
		Image_Offset_Bytes)
	/*
		The following image products use the /^IMAGE parameter with
		the specified meaning of the value:

			Mars Pathfinder Lander			Image offset records
			Clementine 1					Image offset bytes
			Viking Lander 1					Image offset records
			Voyager 2						Image offset records
			Mars Global Surveyor			Image offset records
			Mars Orbiter Laser Altimeter	Image filename (detached)
			HiRISE							Image offset bytes

		When the value is in records there is always a RECORD_BYTES
		parameter (Record_Bytes gets its value); when the value is
		in bytes no RECORD_BYTES parameter is provided (none needed),
		and the bytes value is a count (offset + 1).

		Note that when the value is a string it refers to a detached
		image data file.
	*/
	<< Selection ("^IMAGE",
		Image_Record)
	<< Selection ("^IMAGE",
		Image_Data_Filename)

	/*......................................................................
		Image.
	*/
	/*	The dimensions of the image.
	*/
	<< Selection ("LINE_SAMPLES",
		Image_Dimensions[COLUMNS])
	<< Selection ("NS",
		Image_Dimensions[COLUMNS])
	<< Selection ("IMAGE/LINE_SAMPLES",
		Image_Dimensions[COLUMNS])
	<< Selection ("NL",
		Image_Dimensions[LINES])
	<< Selection ("IMAGE_LINES",
		Image_Dimensions[LINES])
	<< Selection ("LINES",
		Image_Dimensions[LINES])
	<< Selection ("IMAGE/LINES",
		Image_Dimensions[LINES])
	<< Selection ("NB",
		Image_Dimensions[BANDS])
	<< Selection ("IMAGE/BANDS",
		Image_Dimensions[BANDS])
	<< Selection ("/QUBE/CORE_ITEMS",
		*Image_Dimensions, 3)
	<< Selection ("/SPECTRAL_QUBE/CORE_ITEMS",
		*Image_Dimensions, 3)

	/*	Some products have non-pixel data attached before (prefix) and/or
		after (suffix) the lines, columns, and/or bands of the image data.
		While this data is not used here, it must be considered when
		calculating the location of the image data in the file.
	*/
	<< Selection ("IMAGE/LINE_PREFIX_BYTES",
		Line_Prefix_Bytes)
	<< Selection ("NBB",
		Line_Prefix_Bytes)
	<< Selection ("LINE_SUFFIX_BYTES",
		*Suffix_Bytes)
	<< Selection ("SUFFIX_ITEMS",
		*Suffix_Bytes, 3)
	<< Selection ("SUFFIX_BYTES",
		Suffix_Multiplier)

	/*	The Data_Type_Name is used to determine the type of pixel data
		(integer or real) and the byte ordering (MSB or LSB).
	*/
	<< Selection ("DATA_TYPE",
		Data_Type_Name)
	<< Selection ("ITEM_TYPE",
		Data_Type_Name)
	<< Selection ("INTFMT",
		Data_Type_Name)
	<< Selection ("IMAGE/SAMPLE_TYPE",
		Data_Type_Name)
	<< Selection ("/QUBE/CORE_ITEM_TYPE",
		Data_Type_Name)
	<< Selection ("/SPECTRAL_QUBE/CORE_ITEM_TYPE",
		Data_Type_Name)

	/*	The size of a pixel (in bits and bytes).
	*/
	<< Selection ("/QUBE/CORE_ITEM_BYTES",
		Sample_Bytes)
	<< Selection ("/SPECTRAL_QUBE/CORE_ITEM_BYTES",
		Sample_Bytes)
	<< Selection ("IMAGE/SAMPLE_BITS",
		Sample_Bits)
	<< Selection ("IMAGE/SAMPLE_BIT_MASK",
		Sample_Bit_Mask)
	//	Old VICAR files have a string that is mapped to the integer size.
	<< Selection ("/FORMAT",
		Datum_Format)

	/*	Image data larger than 8-bits per pixel must be scaled. However,
		some products contain special pixel values that cause the data
		scaling to blow up unless they are excluded. This is where the
		non-default data scaling limits may be found.
	*/
	<< Selection ("/QUBE/CORE_VALID_MINIMUM",
		Data_Limits[LIMIT_MIN])
	<< Selection ("/SPECTRAL_QUBE/CORE_VALID_MAXIMUM",
		Data_Limits[LIMIT_MAX])
	<< Selection ("/IMAGE/MINIMUM",
		Data_Limits[LIMIT_MIN])
	<< Selection ("/IMAGE/SAMPLE_BIT_MASK",
		Data_Limits[LIMIT_MAX])

	/*	Compressed image data requires an external decompression utility.
		See below for the mapping of the compression method to the
		appropriate decompression procedure.
	*/
	<< Selection ("ENCODING_TYPE",
		Compression_Method)

	//	Various alternatives for the image name.

	<< Selection ("PRODUCER_ID",
		Image_Name)
	<< Selection ("SPACECRAFT_NAME",
		Image_Name)
	<< Selection ("MISSION",
		Image_Name)
	<< Selection ("INSTRUMENT_HOST_NAME",
		Image_Name)
	<< Selection ("TARGET",
		Image_Name)
	<< Selection ("TARGET_NAME",
		Image_Name)
	<< Selection ("TARGET_BODY",
		Image_Name)
	<< Selection ("MISSION_PHASE",
		Image_Name)
	<< Selection ("MISSION_PHASE_NAME",
		Image_Name)
	<< Selection ("MISSION_NAME",
		Image_Name)
	<< Selection ("PA",
		Image_Name)
	<< Selection ("INSTRUMENT_ID",
		Image_Name)
	<< Selection ("INSTRUMENT_NAME",
		Image_Name)
	<< Selection ("SENSOR",
		Image_Name)
	<< Selection ("IMAGE_ID",
		Image_Name)
	<< Selection ("PICNO",
		Image_Name)
	<< Selection ("PRODUCT_ID",
		Image_Name)

	/*	Post-image label.

		If the EOL parameter is present in the header label, has a
		non-zero value, and all the other parameters necessary to
		determine the location of the EOL label after the end of the
		image data are present, then an attempt is made to reposition the
		data source at the first byte after the end of the image data and
		then parse the remainder of the file for parameters. If this is
		successful the EOL parameters are appended to the Label
		Parameters as an Aggregate named "EOL".
	*/
	<< Selection ("/EOL",
		EOL)

	<< Selection ();
}


/*	Destroys the Private_Data.

	If a Decompressed_Source has been set it is removed and deleted.
	If a Detached_Source has been set it is closed and deleted.
*/
~Private_Data ()
{
if (Decompressed_Source)
	{
	Decompressed_Source->remove ();
	delete Decompressed_Source;
	}
if (Detached_Source)
	{
	Detached_Source->close ();
	delete Detached_Source;
	}
}


/*	Reset all data to its uninitialized state.

	The Initialized flag is set to false.

	If a Decompressed_Source has been set it is removed, deleted, and
	reset to NULL.

	If a Detached_Source has been set it is closed, deleted, and reset
	to NULL.

	All other internal variables are reset to their appropriate
	uninitialized state.

	<b>N.B.</b>: The Read_Failure_Reason is not reset.

	It is appropriate to reset when a device is bound to the handler.
*/
void
reset ()
{
Initialized = false;

if (Detached_Source)
	{
	Detached_Source->close ();
	delete Detached_Source;
	Detached_Source = NULL;
	}

if (Decompressed_Source)
	{
	Decompressed_Source->remove ();
	delete Decompressed_Source;
	Decompressed_Source = NULL;
	}

Data_Type_Name.clear ();
Data_Type = DATA_TYPE_UNSIGNED_INTEGER;
Datum_Format.clear ();
Data_Order = (Q_BYTE_ORDER == Q_BIG_ENDIAN) ? DATA_ORDER_MSB : DATA_ORDER_LSB;
Swap_Bytes = false;

Compression_Type = COMPRESSION_TYPE_NONE;
Compression_Method.clear ();

Source_Format = QImage::Format_Invalid;

Gamma = 0.0;

Current_Band = -1;
Next_Band = 0;

Record_Bytes =
Label_Records =
Header_Records =
Image_Record =
Image_Offset_Bytes = 0;

Image_Dimensions[COLUMNS] =
Image_Dimensions[LINES] =
Image_Dimensions[BANDS] = 0;

Sample_Bytes =
Sample_Bits =
Sample_Bit_Mask = 0;

Line_Prefix_Bytes = 0;
Suffix_Multiplier = 1;
Suffix_Bytes[LINE_SUFFIX] =
Suffix_Bytes[COLUMN_SUFFIX] =
Suffix_Bytes[BAND_SUFFIX] = 0;

EOL = 0;

Data_Limits[LIMIT_MIN] =
Data_Limits[LIMIT_MAX] = idaeim::INVALID_VALUE;

Image_Data_Filename.clear ();
Image_Name.clear ();
}


/*	Set all internal variables.

	The PDS_Handler metadata parameters must already have been read
	from the data source label.

	The list of metadata parameter Selections is used to select
	values for their associated variables.

	The the selected values are evaluated to provide values for
	dependent variables.

	<b>N.B.</b>: All image characterization data is reset before
	the label variables are set.
*/
bool
set
	(
	const PDS_Handler*	handler
	)
{
//	Safety check.
if (! handler->metadata ())
	return false;

reset ();
Read_Failure_Reason.clear ();

//	Select parameter values from the label.
handler->metadata ()->select (Selections.data ());

/*..............................................................................
	Data compression.
*/
if (! Compression_Method.empty ())
	{
	bool
		matched = false;
	QString
		text (Compression_Method.c_str ());
	foreach (const Value_for_String& mapping, COMPRESSION_TYPE_MAPPINGS)
		{
		if (text.contains (mapping.MAPPING_STRING, Qt::CaseInsensitive))
			{
			Compression_Type = mapping.MAPPING_VALUE;
			matched = true;
			break;
			}
		}
	if (! matched)
		{
		ostringstream
			reason;
		reason << ID << endl
			<< "Unknown source image data compression method: "
			/*
				Note: The c_str method is used here (and elsewhere)
				because the use of the Value::String_type, even though it
				is a typedef for std::string, with the ostringstream is
				confusing to the operator<< on Mac OSX. This is not a
				problem on other Unix systems (e.g. Linux), and the use
				of std::string is not a problem on Mac OSX; so the Mac
				OSX compiler is not getting the actual type of
				Value::String_type.
			*/
				<< Compression_Method.c_str ();
		Read_Failure_Reason = reason.str ();
		return false;
		}
	}

/*..............................................................................
	Data type.
*/
if (! Data_Type_Name.empty ())
	{
	QString
		text;
	if (! Datum_Format.empty ())
		{
		//	When the Datum_Format is present it takes precedence.
		text = Datum_Format.c_str ();
		}
	else
		{
		text = Data_Type_Name.c_str ();
		}

	foreach (const Value_for_String& mapping, DATA_TYPE_MAPPINGS)
		{
		if (text.contains (mapping.MAPPING_STRING, Qt::CaseInsensitive))
			{
			Data_Type = mapping.MAPPING_VALUE;
			break;
			}
		}
	}

/*..............................................................................
	Data order.
*/
if (! Data_Type_Name.empty ())
	{
	QString
		text (Data_Type_Name.c_str ());
	foreach (const Value_for_String& mapping, DATA_ORDER_MAPPING)
		{
		if (text.startsWith (mapping.MAPPING_STRING))
			{
			if (Data_Order != mapping.MAPPING_VALUE)
				{
				Data_Order = mapping.MAPPING_VALUE;
				Swap_Bytes = true;
				}
			break;
			}
		}
	}

/*..............................................................................
	Datum format (pixel bits and bytes).
*/
if (! Datum_Format.empty ())
	{
	foreach (const Value_for_String& mapping, DATUM_FORMAT_MAPPING)
		{
		if (Datum_Format == mapping.MAPPING_STRING)
			{
			Sample_Bits = mapping.MAPPING_VALUE;
			break;
			}
		}
	if (! Sample_Bits)
		{
		ostringstream
			reason;
		reason << ID << endl
			<< "Invalid datum format: " << Datum_Format.c_str ();
		Read_Failure_Reason = reason.str ();
		return false;
		}
	}
else
if (Sample_Bit_Mask)
	//	Count the bits in the Sample_Bit_Mask.
	for (Sample_Bits = 0;
		 Sample_Bit_Mask;
		 Sample_Bit_Mask >>= 1,
		 Sample_Bits++) ;

if (! Sample_Bits &&
	Sample_Bytes)
	{
	Sample_Bits = Sample_Bytes * 8;
	}
else
if (! Sample_Bytes)
	{
	Sample_Bytes = Sample_Bits / 8;
	if (Sample_Bits % 8)
		Sample_Bytes++;	//	Add the "odd", partially used, byte.
	}

//	Sanity check.
bool
	OK = false;
switch (Data_Type)
	{
	case DATA_TYPE_INTEGER:
	case DATA_TYPE_UNSIGNED_INTEGER:
		if
			(
			   Sample_Bytes == sizeof (char)
			#if (SHRT_MAX != INT_MAX)
			|| Sample_Bytes == sizeof (short)
			#endif
			|| Sample_Bytes == sizeof (int)
			#if (INT_MAX != LONG_MAX)
			|| Sample_Bytes == sizeof (long)
			#endif
			#if (LONG_MAX != LLONG_MAX)
			|| Sample_Bytes == sizeof (long long)
			#endif
			)
			OK = true;
		break;
	case DATA_TYPE_REAL:
		if
			(
			#if (FLT_MANT_DIG != DBL_MANT_DIG )
			   Sample_Bytes == sizeof (float) ||
			#endif
			   Sample_Bytes == sizeof (double)
			#if (DBL_MANT_DIG  != LDBL_MANT_DIG)
			|| Sample_Bytes == sizeof (long double)
			#endif
			)
			OK = true;
		break;
	}
if (! OK)
	{
	ostringstream
		reason;
	reason << ID << endl
		<< "A " << Sample_Bytes << " byte pixel sample" << endl
		<< "is not consistent with the " << DATA_TYPE_NAME[Data_Type]
			<< " image data type.";
	Read_Failure_Reason = reason.str ();
	return false;
	}

if (Sample_Bytes == 1)
	{
	//	Single byte data is always unsigned and has no byte swapping.
	Data_Type = DATA_TYPE_UNSIGNED_INTEGER;
	Swap_Bytes = false;
	}

/*..............................................................................
	Image dimensions sanity check.
*/
if (! Image_Dimensions[COLUMNS] ||
	! Image_Dimensions[LINES] ||
	! Sample_Bytes)
	{
	ostringstream
		reason;
	reason << ID << endl
		<< "Unable to determine the image dimensions.";
	Read_Failure_Reason = reason.str ();
	return false;
	}
if (Image_Dimensions[BANDS] == 0)
	Image_Dimensions[BANDS] = 1;

/*..............................................................................
	Valid data limits.
*/
if (is_invalid (Data_Limits[LIMIT_MIN]))
	{
	if (Data_Type == DATA_TYPE_UNSIGNED_INTEGER)
		Data_Limits[LIMIT_MIN] = 0.0;
	else
	if (Data_Type == DATA_TYPE_INTEGER)
		Data_Limits[LIMIT_MIN] =
			((Value::Integer_type)-1 << (Sample_Bits - 1));
	else
		{
		//	DATA_TYPE_REAL
		#if (FLT_MANT_DIG != DBL_MANT_DIG)
		if (Sample_Bytes == sizeof (float))
			Data_Limits[LIMIT_MIN] = -numeric_limits<float>::min ();
		else
		#endif
		if (Sample_Bytes == sizeof (double))
			Data_Limits[LIMIT_MIN] = -numeric_limits<double>::min ();
		#if (DBL_MANT_DIG != LDBL_MANT_DIG)
		else
		if (Sample_Bytes == sizeof (long double) &&
			sizeof (Data_Limits[LIMIT_MIN]) >= sizeof (long double))
			Data_Limits[LIMIT_MIN] = -numeric_limits<long double>::min ();
		#endif
		}
	}

if (is_invalid (Data_Limits[LIMIT_MAX]))
	{
	if (Data_Type == DATA_TYPE_UNSIGNED_INTEGER)
		Data_Limits[LIMIT_MAX] =
			~((Value::Integer_type)-1 << Sample_Bits);
	else
	if (Data_Type == DATA_TYPE_INTEGER)
		Data_Limits[LIMIT_MAX] =
			~((Value::Integer_type)-1 << (Sample_Bits - 1));
	else
		{
		//	DATA_TYPE_REAL
		#if (FLT_MANT_DIG != DBL_MANT_DIG )
		if (Sample_Bytes == sizeof (float))
			Data_Limits[LIMIT_MAX] = numeric_limits<float>::max ();
		else
		#endif
		if (Sample_Bytes == sizeof (double))
			Data_Limits[LIMIT_MAX] = numeric_limits<double>::max ();
		#if (DBL_MANT_DIG  != LDBL_MANT_DIG)
		else
		if (Sample_Bytes == sizeof (long double) &&
			sizeof (Data_Limits[LIMIT_MAX]) >= sizeof (long double))
			Data_Limits[LIMIT_MAX] = numeric_limits<long double>::max ();
		#endif
		}
	}

/*..............................................................................
	Image data location.

	Note: A detached image data file has an Image_Offset_Bytes of 0.
*/
if (! Image_Offset_Bytes &&
	Image_Data_Filename.empty ())
	{
	if (Image_Record &&
		Record_Bytes)
		{
		//	Convert image location from record number to byte offset.
		Image_Offset_Bytes =
			Record_Bytes * (Image_Record - 1);
		}
	else
	if (Label_Records &&
		Record_Bytes)
		{
		//	The image data begins after the label.
		Image_Offset_Bytes = Record_Bytes * Label_Records;
		}
	else
	if (Image_Record > 300)
		{
		/*
			Clementine-1 data from the NRL and MRO/HiRISE data has an
			^IMAGE parameter that is a byte count (as long as the value
			is reasonable). In other (numeric) cases, this parameter
			has a record count value.

			Note: This special ^IMAGE value does have a <BYTES> units
			attached; this should probably be used in the future.

			N.B. The value is a byte count, not offset.
		*/
		Image_Offset_Bytes = Image_Record - 1;
		}
	else
		{
		ostringstream
			reason;
		reason << ID << endl
			<< "Unable to determine the image data location.";
		Read_Failure_Reason = reason.str ();
		return false;
		}
	}

//	Add any binary label records (a VICAR artifact).
Image_Offset_Bytes += Header_Records * Record_Bytes;

/*..............................................................................
	Suffix items.

	The size of each suffix item, in bytes, is the item count from its
	parameter value times the Suffix_Multiplier; this results in a
	Suffix_Bytes value.

	The line suffix bytes (Suffix_Bytes[LINE_SUFFIX]) occur after the end
	of each line ("sideplanes"). Note that if there are line suffix bytes
	there should be no Line_Prefix_Bytes, which occur before the start of
	each line.

	The column suffix bytes (Suffix_Bytes[COLUMN_SUFFIX]) are grouped at
	the end of each band ("bottomplanes"). Each image column (line
	sample) has its set of column suffix bytes; effectively the column
	suffix data adds Suffix_Bytes[COLUMN_SUFFIX] lines of single byte
	data to each band.

	The band suffix bytes (Suffix_Bytes[BANDS]) are grouped at the end of
	the image band data ("backplanes"). Each pixel has its set of band
	suffix bytes; effectively the band suffix bytes adds
	Suffix_Bytes[BANDS] bands of single byte data to the image data
	block. (Note: no case of a data source with band suffix items has
	been seen by the developer of this software, and the only cases that
	have been seen of files with extended labels do not have backplanes,
	so band suffix data is ignored for image display purposes.)
*/
Suffix_Bytes[LINE_SUFFIX]   *= Suffix_Multiplier;
Suffix_Bytes[COLUMN_SUFFIX] *= Suffix_Multiplier;
Suffix_Bytes[BAND_SUFFIX]   *= Suffix_Multiplier;	//	NOT TOTAL BYTES.
//	Convert column suffix bytes to size of column suffix bottomplanes.
Suffix_Bytes[COLUMN_SUFFIX] *= Image_Dimensions[COLUMNS];

if (EOL)
	{
	/*..........................................................................
		EOL parameters.

		The EOL parameters should be located immediately following the
		image data.

		Note that files with EOL parameters are expected to have no
		backplane (Suffix_Bytes[BAND_SUFFIX]) data.
	*/
      Value::Integer_type
		EOL_offset =
			//	Beginning of image data.
			Image_Offset_Bytes
			//	Bands
			+ (Image_Dimensions[BANDS]
				//	Band bytes.
				*  (Image_Dimensions[LINES]
				* ((Image_Dimensions[COLUMNS] * Sample_Bytes)
					//	Sideplanes
					+ Line_Prefix_Bytes
					+ Suffix_Bytes[LINE_SUFFIX]))
				//	Bottomplanes
				+ Suffix_Bytes[COLUMN_SUFFIX]);

	Aggregate*
		EOL_parameters = PDS_Handler::label (handler->device (), (qint64) EOL_offset);
	if (EOL_parameters)
		{
		EOL_parameters->name ("EOL");
		handler->Label->add (EOL_parameters);
		}
	}

/*..............................................................................
	Default QImage::Format for reading multi-band images.
*/
Source_Format =
	((Image_Dimensions[BANDS] % 3) == 0) ?
		QImage::Format_RGB32 :
		QImage::Format_Indexed8;


Initialized = true;

return true;
}

};	//	Private_Data

/*==============================================================================
	Constructors
*/
PDS_Handler::PDS_Handler ()
	:
	Label (NULL),
	Data (new PDS_Handler::Private_Data)
{
      for (int i = 0 ; i < 256 ; i++) PALATTE_8BIT[i] = QColor(i,i,i).rgb();
}


PDS_Handler::~PDS_Handler ()
{
delete Data;
if (Label)
	delete Label;
}

/*------------------------------------------------------------------------------
	Options
*/
bool
PDS_Handler::supportsOption
	(
	ImageOption	option
	)
	const
{
return
	option == Size ||
	option == ImageFormat ||
	option == Gamma ||
	option == Name ||
	option == Description ||
	option == IncrementalReading ||
	option == Animation;
}


QVariant
PDS_Handler::option
	(
	ImageOption	option
	)
	const
{
QVariant
	value;
if (Label ||
	read_label ())
	{
	switch (option)
		{
		case Size:
			value =
				QSize
					(Data->Image_Dimensions[COLUMNS],
					 Data->Image_Dimensions[LINES]);
			break;
		case ImageFormat:
			value = Data->Source_Format;
			break;
		case Gamma:
			value = Data->Gamma;
			break;
		case Name:
			value = Data->Image_Name.c_str ();
			break;
		case Description:
			value = parameters_to_Description (*Label);
			break;
		case IncrementalReading:
			value =
				((Data->Source_Format != QImage::Format_Indexed8 &&
				//	More than one three band images.
				 Data->Image_Dimensions[BANDS] > 3)
				||
				//	Multiple single band images.
				 Data->Image_Dimensions[BANDS] > 0);
			break;
		case Animation:
			value = false;
			break;
		default:
			break;
		}
	}
return value;
}


void
PDS_Handler::setOption
	(
	ImageOption		option,
	const QVariant&	value
	)
{
	bool
		OK;
if (option == ImageFormat)
	{
	int
		format = value.toInt (&OK);
	if (OK &&
		(format == QImage::Format_Indexed8 ||
		 format == QImage::Format_RGB32 ||
		 format == QImage::Format_ARGB32 ||
		 format == QImage::Format_ARGB32_Premultiplied))
		{
		Data->Source_Format = (QImage::Format)format;
		}
	}
else
if (option == Gamma)
	{
	float
		gamma = value.toFloat (&OK);
	if (OK)
		{
		Data->Gamma = gamma;
		}
	}
}


int
PDS_Handler::imageCount () const
{
int
	count = -1;
if (Label ||
	read_label ())
	{
	count =
		(Data->Source_Format == QImage::Format_Indexed8) ?
			Data->Image_Dimensions[BANDS] :
			((Data->Image_Dimensions[BANDS] / 3)
				+ ((Data->Image_Dimensions[BANDS] % 3) ? 1 : 0));
	}
return count;
}


int
PDS_Handler::bands () const
{
if (! Label &&
	! read_label ())
	return -1;
return Data->Image_Dimensions[BANDS];
}


int
PDS_Handler::currentImageNumber () const
{
return
	(Data->Source_Format == QImage::Format_Indexed8 ||
	Data->Current_Band < 0) ?
	Data->Current_Band :
	Data->Current_Band / 3;
}


bool
PDS_Handler::jumpToImage
	(
	int		image_number
	)
{
bool
	jumped = false;
if (image_number >= 0 &&
	image_number < imageCount ())
	{
	Data->Next_Band = to_band_number (image_number);
	jumped = true;
	}
return jumped;
}


bool
PDS_Handler::jumpToNextImage ()
{return jumpToImage (currentImageNumber () + 1);}

/*------------------------------------------------------------------------------
	Reading
*/
void
PDS_Handler::setDevice
	(
	QIODevice*	device
	)
{
if (device &&
	! device->isSequential ())
	{
	QImageIOHandler::setDevice (device);
	read_label ();
	}
}


bool
PDS_Handler::canRead () const
{
if ((Data->Initialized &&
	 Data->Next_Band != bands ()) ||
	canRead (device ()))
	{
	setFormat (FORMATS[0].toLocal8Bit ());
	return true;
	}
return false;
}


bool
PDS_Handler::read_label () const
{
if (Label)
	{
	delete Label;
	Label = NULL;
	}
Data->reset ();

if ((Label = label (device ())))
	{
	if (! Data->set (this))
		{
		//	Not a valid PDS image label.
		delete Label;
		Label = NULL;
		Data->reset ();
		}
	}
else
	{
	ostringstream
		reason;
	reason << ID << endl
		<< "Unable to obtain a metadata label from the source data device.";
	Data->Read_Failure_Reason = reason.str ();
	}
return (Label != NULL);
}


bool
PDS_Handler::read
	(
	QImage*	image
	)
{
bool
	image_read = false;
Data->Read_Failure_Reason.clear ();

if (image)
	{
	if (device ())
		{
		if (Label ||
			read_label ())
			{
			QImage::Format
				destination_format = image->format ();
			if (destination_format == QImage::Format_Invalid ||
				destination_format == QImage::Format_Indexed8 ||
				destination_format == QImage::Format_RGB32 ||
				destination_format == QImage::Format_ARGB32 ||
				destination_format == QImage::Format_ARGB32_Premultiplied)
				image_read = read_image_data (image);
			else
				{
				ostringstream
					reason;
				reason << ID << endl
					<< "The QImage has an unsupported format: "
						<< destination_format << endl
					<< "Only the 8-bit and 32-bit formats are supported.";
				Data->Read_Failure_Reason = reason.str ();
				}
			}
		else
			{
			ostringstream
				reason;
			reason << ID << endl
				<< "No PDS image label could be obtained.";
			Data->Read_Failure_Reason = reason.str ();
			}
		}
	else
		{
		ostringstream
			reason;
		reason << ID << endl
			<< "No image data source.";
		Data->Read_Failure_Reason = reason.str ();
		}
	}
else
	{
	ostringstream
		reason;
	reason << ID << endl
		<< "No QImage provided.";
	Data->Read_Failure_Reason = reason.str ();
	}
return image_read;
}

/*==============================================================================
	Helpers
*/
bool
PDS_Handler::read_image_data
	(
	QImage*	image
	)
{
if (Data->Next_Band == bands ())
	{
	ostringstream
		reason;
	reason << ID << endl
		<< "No more images.";
	Data->Read_Failure_Reason = reason.str ();
	return false;
	}

//	QImage destination setup.
QImage::Format
	destination_format = image->format ();
if (destination_format == QImage::Format_Invalid)
	{
	//	Use the Source_Format by default.
	destination_format = Data->Source_Format;
	}
else
if (Data->Current_Band < 0 &&	//	First image read.
	Data->Source_Format != destination_format &&
	bands () > 2)
	{
	//	Coerce the Source_Format to the destination format.
	Data->Source_Format = destination_format;
	}

//	CAUTION: band_read_count depends on Source_Format.
int
	band_count = band_read_count ();

if (destination_format == QImage::Format_Indexed8 &&
	band_count > 1)
	{
	ostringstream
		reason;
	reason << ID << endl
		<< "Can't read " << band_count
			<< " bands from the image data source into a single band "
			<< QIMAGE_FORMAT_NAME[destination_format] << " image.";
	Data->Read_Failure_Reason = reason.str ();
	return false;
	}

QImage
	*destination = image;
if (image->isNull () ||
	image->width ()  != Data->Image_Dimensions[COLUMNS] ||
	image->height () != Data->Image_Dimensions[LINES])
	{
	//	Provide a suitable QImage.
	destination = new QImage
		(Data->Image_Dimensions[COLUMNS],
		 Data->Image_Dimensions[LINES],
		 destination_format);
	}

if (destination_format == QImage::Format_Indexed8)
    destination->setColorTable(PALATTE_8BIT);

//	Data source setup.
QIODevice
	*source = device ();

if (Data->Detached_Source)
	{
	source = Data->Detached_Source;
	}
else
if (! Data->Image_Data_Filename.empty ())
	{
	//	Switch to the detached image data file.
	if ((Data->Detached_Source
			= detached_data_source (Data->Image_Data_Filename.c_str ())))
		{
		source = Data->Detached_Source;
		//	The external file does not have a label.
		Data->Image_Offset_Bytes = 0;
		}
	else
		{
		if (destination != image)
			delete destination;
		return false;
		}
	}

if (Data->Decompressed_Source)
	{
	source = Data->Decompressed_Source;
	}
else
if (Data->Compression_Type)
	{
	//	Switch to the decompressed data file.
	if ((Data->Decompressed_Source = decompressed_data_source (source)))
		{
		source = Data->Decompressed_Source;

		if (! DECOMPRESSED_FILE_HAS_LABEL[Data->Compression_Type])
			Data->Image_Offset_Bytes = 0;
			/*
				>>> CAUTION <<< Experimentation with the readmoc utility
				- the only one that will not produce a decompressed data
				file without a label - indicates that the output label
				is the same size as the source label. If this is found to
				be an unreliable assumption then the Image_Offset_Bytes
				will have to be determined from the output label.
			*/

		//	The file contains only raw image data.
		Data->Line_Prefix_Bytes =
		Data->Suffix_Bytes[LINE_SUFFIX] =
		Data->Suffix_Bytes[COLUMN_SUFFIX] =
		Data->Suffix_Bytes[BAND_SUFFIX] = 0;

		if (Data->Detached_Source)
			{
			//	The decompressed file effectively replaces the external source.
			Data->Detached_Source->close ();
			delete Data->Detached_Source;
			Data->Detached_Source = NULL;
			}
		}
	else
		{
		if (destination != image)
			delete destination;
		return false;
		}
	}

if (! source->isOpen () &&
	/*
		Open the source if it has become closed.

		If the source is a file that was memory mapped
		on a previous image read it may have been automatically closed
		(some versions of MS/Windows are known to do this).
	*/
	! source->open (QIODevice::ReadOnly))
	{
	ostringstream
		reason;
	reason << ID << endl
		<< "The data source was closed and could not be opened.";
	Data->Read_Failure_Reason = reason.str ();
	if (destination != image)
		delete destination;
	return false;
	}

QFile
	*file = dynamic_cast<QFile*>(source);

/*------------------------------------------------------------------------------
	Image data storage.
*/
uchar
	*image_data = NULL,
	*mapped = NULL;
long long
	image_data_amount
		= (long long)band_count
		* Data->Image_Dimensions[LINES]
		* Data->Image_Dimensions[COLUMNS]
		* Data->Sample_Bytes,
	band_data_amount
		=  (Data->Image_Dimensions[LINES]
		* ((Data->Image_Dimensions[COLUMNS] * Data->Sample_Bytes)
			//	Sideplanes
			+ Data->Line_Prefix_Bytes
			+ Data->Suffix_Bytes[LINE_SUFFIX]))
		//	Bottomplanes
		+ Data->Suffix_Bytes[COLUMN_SUFFIX];

//	Position of the image data to be read in the source.
qint64
	position = (qint64)
		//	Beginning of image data.
		Data->Image_Offset_Bytes
		//	Beginning of Next_Band.
		+ (Data->Next_Band * band_data_amount);

if (file &&
	source->size () < (position + (band_count * band_data_amount)))
	{
	ostringstream
		reason;
	reason << ID << endl
		<< "Insufficient image data available:" << endl
		<< "Starting at position " << position << " of the source "
			<< (band_count * band_data_amount)
			<< " bytes of image data are expected," << endl
		<< "but the image data source only contains "
			<< source->size () << " bytes.";
	Data->Read_Failure_Reason = reason.str ();
	if (destination != image)
		delete destination;
	return false;
	}

if (Data->Sample_Bytes == 1 &&
	destination_format == QImage::Format_Indexed8)
	{
	/*	The data can be read directly into the QImage.

		Note that if the band_count is 1 but the destination QImage
		format is not Format_Indexed8 then the image data that is read
		will need to be replicated across all of the destination bands,
		which is done during BSQ to BIP reorganization. An optimization
		would check for this case, read the data directly into the
		destination QImage, and then replicate the single pixel sample in
		the other two pixel samples for all pixels.
	*/
	image_data = destination->bits ();
	}
else
	{
	/*	Provide an image data buffer.

		A buffer for the entire image is needed because the the min,max
		of all data values will need to be found before each data value
		is then scaled from the data range into 8-bit display values.

		An alternative is to read the source twice: first to find the
		image data range; second to scale the data values.

		Note that this is a time vs space decision. The data buffer
		method is faster than the read twice method which does not
		require any data buffer.
	*/
	try {image_data = new uchar[image_data_amount];}
	catch (bad_alloc&)
		{
		ostringstream
			reason;
		reason << ID << endl
			<< "Couldn't allocate a " << magnitude_of (image_data_amount)
				<< " (" << image_data_amount << ") byte source data buffer.";
		Data->Read_Failure_Reason = reason.str ();
		if (destination != image)
			delete destination;
		return false;
		}

	}

/*------------------------------------------------------------------------------
	Read the image data.
*/
bool
	data_read = false;
if (file)
	{
	/*	Try to memory map to the file.

		Memory mapping could provide faster access to the file's image data.
		However, this requires that the data resides in a local file,
		which can not be assured.
	*/
	mapped = file->map (position, (band_count * band_data_amount));
	}
if (mapped)
	data_read = read_data (mapped, image_data, destination);
else
	data_read = read_data (source, image_data, destination);

/*------------------------------------------------------------------------------
	Massage the image data.
*/
if (data_read)
	{
	/*	Write massaged data directly into the destination image when possible.

		Single band data can easily be written to the destination image.

		Multi-band data can be written into the destination image data,
		but this requires getting the byte order of the QRgb pixel data
		type correct. This optimization is left for later.
	*/
	QImage
		*to_destination = NULL;
		/*	(destination_format == QImage::Format_Indexed8) ?
			destination : NULL; */
   if (destination_format == QImage::Format_Indexed8)
   {
      to_destination = destination;
      to_destination->setColorTable(PALATTE_8BIT);
   }

	if (Data->Sample_Bytes > 1)
		{
		long long
			amount = image_data_amount / Data->Sample_Bytes;

		if (Data->Swap_Bytes)
			{
			//	Reverse the order of all pixel bytes.
			swap_bytes (image_data, amount, Data->Sample_Bytes);
			}

		//	Collapse multi-byte pixel data to single byte data.
		switch (Data->Data_Type)
			{
			case DATA_TYPE_INTEGER:
				#if (SHRT_MAX != INT_MAX)
				if (Data->Sample_Bytes == sizeof (short))
					collapse_data ((short*)image_data, to_destination);
				else
				#endif
				if (Data->Sample_Bytes == sizeof (int))
					collapse_data ((int*)image_data, to_destination);
				#if (INT_MAX != LONG_MAX)
				else
				if (Data->Sample_Bytes == sizeof (long))
					collapse_data ((long*)image_data, to_destination);
				#endif
				#if (LONG_MAX != LLONG_MAX)
				else
				if (Data->Sample_Bytes == sizeof (long long))
					collapse_data ((long long*)image_data, to_destination);
				#endif
				break;
			case DATA_TYPE_UNSIGNED_INTEGER:
				#if (USHRT_MAX != UINT_MAX)
				if (Data->Sample_Bytes == sizeof (unsigned short))
					collapse_data ((unsigned short*)image_data, to_destination);
				else
				#endif
				if (Data->Sample_Bytes == sizeof (unsigned int))
					collapse_data ((unsigned int*)image_data, to_destination);
				#if (UINT_MAX != ULONG_MAX)
				else
				if (Data->Sample_Bytes == sizeof (unsigned long))
					collapse_data ((unsigned long*)image_data, to_destination);
				#endif
				#if (ULONG_MAX != ULLONG_MAX)
				else
				if (Data->Sample_Bytes == sizeof (unsigned long long))
					collapse_data ((unsigned long long*)image_data,
						to_destination);
				#endif
				break;
			case DATA_TYPE_REAL:
				#if (FLT_MANT_DIG != DBL_MANT_DIG )
				if (Data->Sample_Bytes == sizeof (float))
					collapse_data ((float*)image_data, to_destination);
				else
				#endif
				if (Data->Sample_Bytes == sizeof (double))
					collapse_data ((double*)image_data, to_destination);
				#if (DBL_MANT_DIG  != LDBL_MANT_DIG)
				else
				if (Data->Sample_Bytes == sizeof (long double))
					collapse_data ((long double*)image_data, to_destination);
				#endif
				break;
			}
		}

	if (! to_destination)
		{
		//	Reorganize band sequential into band interleaved by pixel data.
		BSQ_to_BIP (image_data, destination);
		}

	if (destination != image)
		{
		*image = *destination;
		}

	/*	Update the image band locations in the source.

		CAUTION: The band_read_count and currentImageNumber
		are dependent on these values.
	*/
	Data->Next_Band += band_count;
	if (Data->Current_Band < 0)
		Data->Current_Band = 0;
	else
		Data->Current_Band += band_count;
	}

/*------------------------------------------------------------------------------
	Cleanup.
*/
if (image_data != destination->constBits ())
	{
	delete[] image_data;
	}

if (image != destination)
	{
	delete destination;
	}

if (mapped)
	{
	if (! file->unmap (mapped))
		{
		ostringstream
			reason;
		reason << ID << endl
			<< ">>> WARNING <<< Releasing the memory map at address "
				<< (void*)mapped << " failed" << endl
			<< " for file: " << file->fileName ();
		Data->Read_Failure_Reason = reason.str ();
		//???	Perhaps this warning should be written to cerr.
		}
	}

return data_read;
}


bool
PDS_Handler::read_data
	(
	QIODevice*	source,
	uchar*		image_data,
	QImage*		destination
	)
{
bool
	image_buffering = (image_data == destination->constBits ());
//	Position the source at the beginning of the first line of image data.
std::size_t
	line_bytes = Data->Image_Dimensions[COLUMNS] * Data->Sample_Bytes;
qint64
	position = (qint64)
		//	Beginning of image data.
		Data->Image_Offset_Bytes
		//	Beginning of Next_Band.
		+ (Data->Next_Band
			* (Data->Image_Dimensions[LINES]
			* (line_bytes
				//	Sideplanes
				+ Data->Line_Prefix_Bytes
				+ Data->Suffix_Bytes[LINE_SUFFIX]))
			//	Bottomplanes
			+ Data->Suffix_Bytes[COLUMN_SUFFIX]);

if (! source->seek (position))
	{
	ostringstream
		reason;
	reason << ID << endl
		<< "Seeking to beginning of source image data band " << Data->Next_Band
			<< " at position " << position << " failed." << endl
		<< source->errorString ();
	Data->Read_Failure_Reason = reason.str ();
	return false;
	}

image_data -= line_bytes;
int
	band_count = band_read_count (),
	band,
	line;

for (band = 0;
	 band < band_count;
	 band++)
	 {
	for (line = 0;
		 line < Data->Image_Dimensions[LINES];
		 line++)
		{
		//	Skip over any line prefix.
		if (Data->Line_Prefix_Bytes &&
			! source->seek
				(position = source->pos () + Data->Line_Prefix_Bytes))
			{
			ostringstream
				reason;
			reason << ID << endl
				<< "Skpping the " << Data->Line_Prefix_Bytes
					<< " byte prefix at line " << line;
			if (band_count > 1)
				reason << " of band " << band;
			reason << " to position " << position << " failed." << endl
				<< source->errorString ();
			Data->Read_Failure_Reason = reason.str ();
			return false;
			}

		//	Read the line of image data.
		if (image_buffering)
			/*
				When the image data is being read directly into the QImage
				data the address of the line data storage needs to be
				obtained from the QImage because it alignment contraints
				may affect the location of each line data storage section.
			*/
			image_data = destination->scanLine (line);
		else
			image_data += line_bytes;

		if ((std::size_t)(source->read ((char*)image_data, line_bytes))
				!= line_bytes)
			{
			ostringstream
				reason;
			reason << ID << endl
				<< "Reading the " << line_bytes << " byte line data " << line;
			if (band_count > 1)
				reason << " of band " << band;
			reason << " to position " << position << " failed." << endl
				<< source->errorString ();
			Data->Read_Failure_Reason = reason.str ();
			return false;
			}

		//	Skip over any line suffix bytes.
		if (Data->Suffix_Bytes[LINE_SUFFIX] &&
			! source->seek
				(position = source->pos () + Data->Suffix_Bytes[LINE_SUFFIX]))
			{
			ostringstream
				reason;
			reason << ID << endl
				<< "Skpping the " << Data->Suffix_Bytes[LINE_SUFFIX]
					<< " byte suffix at line " << line;
			if (band_count > 1)
				reason << " of band " << band;
			reason << " to position " << position << " failed." << endl
				<< source->errorString ();
			Data->Read_Failure_Reason = reason.str ();
			return false;
			}
		}

	//	Skip over any column suffix bytes.
	if (Data->Suffix_Bytes[COLUMN_SUFFIX] &&
		(band + 1) < band_count &&
		! source->seek
			(position = source->pos () + Data->Suffix_Bytes[COLUMN_SUFFIX]))
		{
		ostringstream
			reason;
		reason << ID << endl
			<< "Skpping the " << Data->Suffix_Bytes[COLUMN_SUFFIX]
				<< " byte column suffix data of band " << band
				<< " to position " << position << " failed." << endl
			<< source->errorString ();
		Data->Read_Failure_Reason = reason.str ();
		return false;
		}
	}
return true;
}


bool
PDS_Handler::read_data
	(
	const uchar*	source_data,
	uchar*			image_data,
	QImage*			destination
	)
{
bool
	image_buffering = (image_data == destination->constBits ());
std::size_t
	line_bytes = Data->Image_Dimensions[COLUMNS] * Data->Sample_Bytes;
image_data -= line_bytes;	//	pre-decrement in case of no image_buffering.
int
	band_count = band_read_count (),
	band,
	line;

for (band = 0;
	 band < band_count;
   ++band)
	{
	for (line = 0;
		 line < Data->Image_Dimensions[LINES];
	   ++line)
		{
		//	Skip over any line prefix bytes.
		source_data += Data->Line_Prefix_Bytes;

		//	Copy the line of image data.
		if (image_buffering)
			/*
				When the image data is being read directly into the
				QImage data the address of the line data storage needs to
				be obtained from the QImage because alignment contraints
				may affect the location of each line's data storage
				section.
			*/
			image_data = destination->scanLine (line);
		else
			image_data += line_bytes;
		memcpy (image_data, source_data, line_bytes);
		source_data += line_bytes;

		//	Skip over any line suffix bytes.
		source_data += Data->Suffix_Bytes[LINE_SUFFIX];
		}
	//	Skip over any column suffix bytes.
	source_data += Data->Suffix_Bytes[COLUMN_SUFFIX];
	}
return true;
}


void
PDS_Handler::swap_bytes
	(
	unsigned char*			data,
	const long long&		groups,
	const unsigned int&		size
	)
{
if (! data)
	return;
register unsigned char
	datum,
	*first,
	*last,
	*end = data + (groups * size);
while (data < end)
	{
	first = data;
	data += size;
	last  = data;
	while (first < --last)
		{
		datum    = *first;
		*first++ = *last;
		*last    = datum;
		}
	}
}

template<typename Sample_Data_Type>
void
PDS_Handler::collapse_data
	(
	Sample_Data_Type*	image_data,
	QImage*				destination
	)
{
Sample_Data_Type
	datum,
	min_valid = (Sample_Data_Type)(Data->Data_Limits[LIMIT_MIN]),
	max_valid = (Sample_Data_Type)(Data->Data_Limits[LIMIT_MAX]);
if (Data->Gamma == 0.0)
	Data->Gamma = (max_valid == min_valid) ?
		0.0 : 255.0 / (max_valid - min_valid);
float
	scale = Data->Gamma;

uchar
	*scaled_data = (uchar*)image_data;
int
	band_count = band_read_count (),
	band,
	line,
	column;

for (band = 0;
	 band < band_count;
   ++band)
	{
	for (line = 0;
		 line < Data->Image_Dimensions[LINES];
	   ++line)
		{
		if (destination)
			/*
				When the scaled image data is being writen directly into the
				QImage data the address of the line data storage needs to
				be obtained from the QImage because alignment contraints
				may affect the location of each line's data storage section.
			*/
			scaled_data = destination->scanLine (line);

		for (column = 0;
			 column < Data->Image_Dimensions[COLUMNS];
		   ++column)
			{
			datum = *image_data++;
			if (datum < min_valid)
				*scaled_data++ = 0;
			else
			if (datum > max_valid)
				*scaled_data++ = 255;
			else
				{
				datum -= min_valid;
				datum *= scale;
				if (datum < min_valid)
					*scaled_data++ = 0;
				else
				if (datum > max_valid)
					*scaled_data++ = 255;
				else
					*scaled_data++ = (uchar)datum;
				}
			}
		}
	}
}


void
PDS_Handler::BSQ_to_BIP
	(
	uchar*	image_data,
	QImage*	destination
	) const
{
int
	line,
	column,
	band,
	band_count = band_read_count ();
uchar
	*band_sample,
	datum = 0;
QRgb
	*pixel_data,
	pixel;
long long
	band_bytes =
		Data->Image_Dimensions[LINES] *
		Data->Image_Dimensions[COLUMNS];
bool
	filling = (band_count < 3),
	replicate = (Data->Source_Format == QImage::Format_Indexed8);

for (line = 0;
	 line < Data->Image_Dimensions[LINES];
   ++line)
	{
	pixel_data = (QRgb*)(destination->scanLine (line));
	for (column = 0;
		 column < Data->Image_Dimensions[COLUMNS];
	   ++column)
		{
		band_sample = image_data;
		pixel = 0xFF;
		for (band = 0;
			 band < band_count;
		   ++band,
			 band_sample += band_bytes)	//	Next band.
			{
			pixel <<= 8;
			pixel |= *band_sample;
			}

		if (filling)
			{
			if (replicate)
				//	Replicate the single band source sample.
				datum = *image_data;
			while (band++ < 3)
				{
				pixel <<= 8;
				pixel |= datum;
				}
			}
		*pixel_data++ = pixel;
		++image_data;
		}
	}
}


QIODevice*
PDS_Handler::detached_data_source
	(
	const QString&	pathname
	) const
{
QIODevice
	*detached_source = NULL;
QString
	detached_pathname;

if (QFileInfo (pathname).isRelative ())
	{
	//	Get the pathname for the directory where the label file is located.
	QFile
		*file = dynamic_cast<QFile*>(device ());
	if (file)
		{
		QFileInfo
			file_info (file->fileName ());
		if (file_info.exists () &&
			file_info.isFile () &&
			file_info.isReadable ())
			detached_pathname =
				file_info.canonicalPath () + '/' + pathname;
		else
			{
			ostringstream
				reason;
			reason << ID << endl
				<< "Unable to obtain the image data from the detached file: "
					<< pathname << endl
				<< "Couldn't determine the pathname to the PDS label file: "
					<< file->fileName ();
			Data->Read_Failure_Reason = reason.str ();
			}
		}
	else
		{
		ostringstream
			reason;
		reason << ID << endl
			<< "Unable to obtain the image data from the detached file: "
				<< pathname << endl
			<< "The PDS label does not appear to be from a local file.";
		Data->Read_Failure_Reason = reason.str ();
		}
	}

if (! detached_pathname.isEmpty ())
	{
	detached_source = new QFile (detached_pathname);
	if (! detached_source->open (QIODevice::ReadOnly))
		{
		ostringstream
			reason;
		reason << ID << endl
			<< "Unable to open the detached image data file: "
				<< detached_pathname;
		Data->Read_Failure_Reason = reason.str ();
		delete detached_source;
		detached_source = NULL;
		}
	}
return detached_source;
}


QFile*
PDS_Handler::decompressed_data_source
	(
	QIODevice*	source
	) const
{
QFile
	*decompressed_source = NULL;
if (Data->Compression_Type &&
	Data->Compression_Type < (int)(sizeof DECOMPRESSION_FILTER / sizeof (char*)))
	{
	QFile
		*file = dynamic_cast<QFile*>(source);
	if (file)
		{
		//	Source pathname.
		QFileInfo
			file_info (file->fileName ());
		if (file_info.exists () &&
			file_info.isFile () &&
			file_info.isReadable ())
			{
			//	Decompressed filename.
			QString
				pathname (QDir::tempPath ());
			if (! pathname.endsWith (QDir::separator ()))
				pathname += QDir::separator ();
			pathname += "PDS_decompressed_";
			pathname += file_info.fileName ();
			pathname += '-';
			pathname += QDateTime::currentDateTime ()
				.toString ("dd.MM.yyyyThh:mm:ss.zzz");

			//	Assemble and run the command line.
			QString
				command =
					QString (DECOMPRESSION_FILTER[Data->Compression_Type])
					.arg (file_info.canonicalFilePath ())
					.arg (pathname);
			int
				exit_status = QProcess::execute (command);
			if (exit_status == 0)
				{
				//	Open the decompressed data file.
				decompressed_source = new QFile (pathname);
				if (! decompressed_source->open (QIODevice::ReadOnly))
					{
					ostringstream
						reason;
					reason << ID << endl
						<< "Couldn't open the decompressed file: "
							<< pathname << endl
						<< decompressed_source->errorString ();
					Data->Read_Failure_Reason = reason.str ();
					decompressed_source->remove ();
					delete decompressed_source;
					decompressed_source = NULL;
					}
				}
			else
				{
				ostringstream
					reason;
				reason << ID << endl
					<< "Decompressing the "
						<< Data->Compression_Method.c_str ()
						<< " image data" << endl
					<< "using command " << command << endl
					<< "failed with exit status " << exit_status << '.';
				Data->Read_Failure_Reason = reason.str ();
				}
			}
		else
			{
			ostringstream
				reason;
			reason << ID << endl
				<< "Unable to decompress the "
					<< Data->Compression_Method.c_str ()
					<< " image data." << endl
				<< "because the source file " << file_info.canonicalFilePath ()
				<< "does not appear to be for an existing readable file!";
			Data->Read_Failure_Reason = reason.str ();
			}
		}
	else
		{
		ostringstream
			reason;
		reason << ID << endl
			<< "Unable to decompress the "
				<< Data->Compression_Method.c_str ()
				<< " image data" << endl
			<< "because the source does not appear to be from a local file.";
		Data->Read_Failure_Reason = reason.str ();
		}
	}
return decompressed_source;
}


bool
PDS_Handler::canRead
	(
	QIODevice*	device
	)
{
bool
	readable = false;
static int
	magic_size = -1;

if (device &&
	device->isOpen () &&
	device->isReadable () &&
	! device->isSequential ())
	{
	qint64
		position = device->pos ();
	if (position != 0)
		//	Must read from the beginning of the source.
		device->seek (0);

	Value_for_String
		magic;
	if (magic_size < 0)
		{
		int
			size;
		foreach (magic, MAGIC)
			{
			size = magic.MAPPING_VALUE + strlen (magic.MAPPING_STRING);
			if (magic_size < size)
				magic_size = size;
			}
		}
	QByteArray
		file_magic (device->peek (magic_size));
	if (file_magic.length () == magic_size)
		{
		foreach (magic, MAGIC)
			{
			if (file_magic.mid (magic.MAPPING_VALUE)
					.startsWith (magic.MAPPING_STRING))
				{
				readable = true;
				break;
				}
			}
		}

	if (position != 0)
		device->seek (position);
	}
return readable;
}


Aggregate*
PDS_Handler::label
	(
	QIODevice*	device,
	qint64		location
	)
{
Aggregate
	*metadata = NULL;
if (device &&
	device->isOpen () &&
	device->isReadable () &&
	! device->isSequential ())
	{
	qint64
		position = device->pos ();
	if (position != location &&
		! device->seek (location))
		{
		return NULL;
		}

	Qistream
		qistream (device);
	Parser
		parser (qistream);
	try
		{
		metadata = new Aggregate (parser, Parser::CONTAINER_NAME);
		}
	catch (idaeim::Exception except)
		{
		}

	//	Restore the previous device position.
	device->seek (position);
	}
return metadata;
}

QString
PDS_Handler::parameters_to_Description
	(
	const Aggregate&	parameters
	)
{
QString
	description;
ostringstream
	value_string;
for (Aggregate::Depth_Iterator
		parameter = (const_cast<Aggregate&>(parameters)).begin_depth (),
		last      = (const_cast<Aggregate&>(parameters)).end_depth ();
		parameter != last;
	  ++parameter)
	{
	if (parameter->is_Aggregate ())
		continue;

	if (! description.isEmpty ())
		//
		description += "\n\n";
	description += QString::fromStdString (parameter->pathname ());
	description += ':';
	if (parameter->is_Assignment ())
		{
		value_string.str ("");

		value_string << parameter->value ();
		description += ' ';
		description += QString::fromStdString (value_string.str ());

		/*	Work around for broken QImageReader:

			According to the documentation for the
			QImageIOHandler::Description ImageOption the format of
			all key,value pairs is:

			key:value\n\n

			Note that the ':' is the required delimiter of the key and
			value; no embedded spaces are required; and every pair,
			including the last, is terminated by two newline characters.

			The QImageReaderPrivate::getText() method produces a QMap of
			name,value QString pairs by parsing the return value of the
			Description option: The Description string is split into
			QString segments on the two newline characters (note that
			this results in a final empty segment). For each segment, if
			the index of the ':' delimiter is non-negative and the index
			of any space character (' ') is less than the delimiter index
			the resulting key,value pair in the QMap is "Description" and
			the entire segment QString::simplified; otherwise the
			key,value pair is the part of the segment before the
			delimiter and the part after the segment QString::simplified.

			The conditional determining the key,value pair is incorrect!
			For example, the typical segment "key:value" will result in a
			"Description" key and a "key:value" value. This can be
			expected to occur for most, if not all, segments in the
			formatted Description string, and so only the last
			"Description" segment will remain in the QMap; all other
			key:value pairs will be lost.

			To work around this problem the two newline characters are
			only placed between key, value pairs; none are included after
			the last pair. And an extra space character is added to the
			end of the value string so "key:value " will be correctly
			parsed; the extra space will be removed when the value string
			is simplified.
		*/
		}
	}
return description;
}


int
PDS_Handler::band_read_count () const
{
return
	(Data->Next_Band == Data->Image_Dimensions[BANDS]) ?
		0 :
		((Data->Source_Format == QImage::Format_Indexed8) ?
			1 :
			qMin (3, (int)(Data->Image_Dimensions[BANDS] - Data->Next_Band)));
}


int
PDS_Handler::to_band_number
	(
	int		image_number
	) const
{
return ((Data->Source_Format == QImage::Format_Indexed8) ?
		image_number :
		(image_number * 3));
}


int
PDS_Handler::to_image_number
	(
	int		band_number
	) const
{
return ((Data->Source_Format == QImage::Format_Indexed8) ?
		band_number :
		(band_number / 3));
}

}	//	namespace idaeim
