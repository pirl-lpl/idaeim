/**	PDS_Plugin

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

CVS ID: PDS_Handler.hh,v 1.16 2014/03/17 05:58:13 castalia Exp
*******************************************************************************/

#ifndef PDS_HANDLER_HH
#define PDS_HANDLER_HH

#include <QImageIOHandler>
#include <QImage>
#include <QColor>
#include <QVector>

//	Forward references.
class QFile;

namespace idaeim
{
//	Forward reference.
namespace PVL {
class Aggregate;
}
using idaeim::PVL::Aggregate;


/**	A <i>PDS_Handler</i> implements a QImageIOHandler for reading PDS
	image data.

	The PDS_Handler is a read-only QImageIOHandler; the write method
	is not implemented.

	The PDS file format is used by the NASA Planetary Data System
	(http://pds.nasa.gov/) to store image data (and other types of data)
	obtained from science instruments. Unfortunately, though there are
	ostensible specifications for the PDS file format, in practice it has
	been used rather inconsistently, and the PDS management and data
	providers have remained stubbornly uncooperative and obtuse towards
	efforts to help acheive clear standards and provide generically
	useful tools. The PDS_Handler incorporates decades of experience by
	the author using the PDS format and developing software tools to work
	with it. The data access techniques used here are known to work with
	a very wide range of PDS image data going back to the VICAR products
	and up to the current HiRISE and other contemporary products.
	Nevertheless, the unreliability inherent in the PDS format means that
	there can be no guarantee that all PDS products will be correctly
	accessiby by this PDS_Handler.

	The implementation of the PDS_Handler is based on the implementation
	of the xvPVL module of the PIRL enhanced xv application
	(http://pirlwww.lpl.arizona.edu/software/xv/).

	PDS science data and QImage display data -

	Because PDS image files contain science data there are various
	challenges in using it for display purposes with a QImage.

	Metadata:

	A PDS image file always contains a label providing metadata that
	describes the image data. The label is a set of parameters - a name
	and value - represented in Parameter Value Language. PVL is plain
	ASCII text. The idaeim::PVL classes and associated libraries are used
	to parse the PVL label and obtain the parameter values.

	The parameter names in the PVL for the values describing the
	structure of the image data are not the same in all PDS data
	products, and the values needed to determine the data structure do
	not have the same meaning in all PDS data products. The PDS_Handler
	includes a table of known parameter names for the needed values and
	the logic to reduce what is found in the label to a succinct set of
	image data stucture definition values. If a problematic PDS data
	product is encountered additions to the Parameter Selection table,
	and possibly the Private_Data:set method, should suffice in dealing
	with it.

	Because the metadata characterizes the image data it is required to
	read the image data, as well as to provide information necessary
	to support various other PDS_Handler/QImageIOHandler methods.

	All of the PVL label parameters may be obtained by using the
	Description option. The QImageReader class provides a textKeys method
	to get the fully qualified pathnames of all parameters, and a text
	method to get the text representation of a value associated with a
	particular named parameter. <b>N.B.</b>: However, the textKeys are
	not provided in the order in which they occur in the label (they
	are alphabetically ordered), and duplicate parameter pathnames
	are not allowed.

	Data types:

	A QImage requires unsigned integer image data of no more than one
	byte per pixel sample. PDS image data samples may be unsigned or
	signed integers, or real (floating point) types, composed of more
	than one byte. Multi-byte data may be in big-endian (MSB) or
	little-endian (LSB) order. The PDS_Hander will manage multi-byte
	sample data by swapping values to the host system data order, if
	needed, and then collapsing the values to a single byte by scaling
	them using the Gamma option cooeficient. The Gamma cooeficient may be
	user specified or the PDS_Hander will determine the value based on
	the limits of valid image sample data values obtained from the PDS
	label.

	Scaling multi-byte sample data values to single byte values will
	almost certainly mean a loss of image fidelity in the displayed
	image. This is unavoidable.

	Image bands:

	A pseudo color QImage will have one band of image data that is
	treated as an index into a lookup table of three band red-green-blue
	values. A full color QImage will have three bands of image data -
	plus an alpha transparency value that the PDS_Handler will always set
	to opaque (0xFF) - of red, green, and blue pixel samples. The full
	color image data is band interleaved by pixel (BIP) such than each
	line of the image data contains pixel values composed of all four -
	alpha plus RGB - band samples.

	PDS image data is always band sequential (BSQ) ordered: All the
	sample values for a band occur before the sample values for the next
	band. The PDS_Handler will reorder the BSQ source image data into BIP
	QImage data as needed. In addition, a single band of source image
	data can be read into a full color QImage and the sample data of the
	source will be replicated for all the pixel samples of the
	destination.

	Multi-band PDS image data presents an ambiguity in how it is to be
	treated when mapped into a QImage: as a sequence of single band
	pseudo color (8-bit pixel data as index values into a color lookup
	table), or a set of three band full color images where the pixel data
	are red, green blue sample values. A PDS image with less than three
	bands of image data is always treated as a sequence of pseudo color
	images. By default a PDS iamge with a multiple of three bands is
	treated as one or more full color images; otherwise it is treated as
	a sequence of pseudo color images. However, the ImageFormat option
	may be set to force the PDS_Handler to treat the latter case as full
	color images; the last image in this case will have the missing
	sample data values set to zero. Alternatively, the QImage passed to
	the {@link read(QImage*) read method} may be used to specify the
	desired image format for the image data that is read.

	Other capabilities -

	The QImageIOHandler methods to access multiple images work
	consistently with how the multi-band PDS image data is being treated.
	The PDS_Handler provides a method to determine the total number of
	{@link bands()const image data bands} present in the source file.

	If an attempt to read image data fails for any reason a description
	of the problem is available as a short text string.

	<b>>>> CAUTION <<<</b> Do not {@link setDevice(QIODevice*) set the
	source image data device} after any PDS_Handler accessors, including
	the read method, have been used. The device, when set, provides the
	source label metadata that characterizes the image data, which once
	read is not read again for the life of the PDS_Handler object.
	Changing the device will probably change the iamge data source and
	its image metatdata and pixel data after these have already been
	obtained and for a different data source that has already been used
	to establish the image access values.

	@author		Bradford Castalia, idaeim
	@version	1.16
*/
class PDS_Handler
:	public QImageIOHandler
{
public:
/*==============================================================================
	Constants
*/
//!	Class identification name with source code version and date.
static const char* const
	ID;

//!	Recognized image formats as filename extensions.
static const QStringList
	FORMATS;

//!	Names corresponding the QImage::Format values.
static const char* const
	QIMAGE_FORMAT_NAME[];

static QVector<QRgb> PALATTE_8BIT;

/*==============================================================================
	Constructors
*/
/**	Constructs a PDS_Handler.

	The PDS_Handler has no image data source device bound to it.

	A PDS_Handler is expected to be constructed by the PDS_Plugin create
	method that will {@link setDevice(QIODevice*) set the device} used as
	the source of data. <b>N.B.</b>: The handler's initialization can
	only be done when the data source device is set.
*/
PDS_Handler ();

~PDS_Handler ();

/*==============================================================================
	QImageIOHandler methods implementations
*/
/**	Set the device that will be used as the source of image metadata
	and pixel data.

	@param	device	A pointer to the QIODevice that will be used as the
		source of image metadata and pixel data. If non-NULL the image
		metadata label is read from the device and the image
		characterization is initialized from it.
	@see canRead(QIODeivce*)
*/
void setDevice (QIODevice* device);

/*	Test if an ImageOption is supported.

	The following options are supported for obtaining their value:

	-Size
	-ImageFormat
	-Gamma
	-Name
	-Description
	-IncrementalReading
	-Animation

	@param	option	A QImageIOHandler::ImageOption code.
	@return	true if the option is supported; false otherwise.
	@see	option(ImageOption)const
	@see	setOption(ImageOption, const QVariant&)
*/
virtual bool supportsOption (ImageOption option) const;

/**	Set an ImageOption.

	The PDS_Handler only supports image {@link read(QImage*) reading}.
	Nevertheless, two options may be set to control how the image
	data is read:

	-ImageFormat

	The QImage::Format to use when  an image is {@link read(QImage*)
	read} and the specified destination QImage is null.

	Only the 8-bit pseudo color (color table index) QImage format -
	Format_Indexed8 - and the 32-bit full color formats - Format_RGB32,
	Format_ARGB32, and Format_ARGB32_Premultiplied - are supported.

	The ImageFormat option also specifies how a multi-band source image
	is to be treated: For Format_Indexed8 each band is a separate image;
	otherwise each group of three bands constitutes an effective image.

	For an image with a multiple of three {@link bands()const bands} the
	default image format is the full color Format_RGB32; otherwise it is
	the pseudo color Format_Indexed8. Format_Indexed8 may be specified to
	force using one source image band for each image read; or a full
	color format may be specified to force using up to three bands for
	each image read.

	Automatic ImageFormat -

	To assist applications that indirectly use a PDS_Handler via an
	interface that does not provide the ability to set options (e.g.
	QImageReader) but does provide a method to specify the destination
	QImage for image data that is read, the specified QImage format for
	the first image read - i.e. before any images have yet been read -
	can be used to automatically set the ImageFormat. In this case the
	QImage must not have the Format_Invalid and the source image data
	must contain more than three bands. The automatically set ImageFormat
	remains in effect for any subsequent images read unless specifically
	changed by setting the ImageFormat option.

	<b>N.B.</b>: Multiple source image bands can not be read into a
	Format_Indexed8 destination image; i.e. if the ImageFormat is not
	Format_Indexed8 (i.e. it is a 32-bit full color format), and there is
	more than one band remaining in the source image data, reading into a
	destination image that is Format_Indexed8 will fail. Conversely, when
	the ImageFormat is Format_Indexed8 reading into a destination image
	that is a full color format is acceptable; in this case the image
	data of the single band that is read will be replicated across all
	three destination QImage bands.

	-Gamma

	The multi-byte image data scaling factor.

	Multi-byte image pixel sample data is scaled to single byte data
	required by a QImage. The Gamma value may be  set to any non-zero
	value override the default multi-byte scaling factor.

	A Gamma of zero (the initial default) will cause the appropriate
	value to be determined when a multi-byte image is read using the
	valid data range:

	Gamma = 255.0 / (max_valid - min_valid); or 0 if max_valid == min_valid

	The vaid data range may be specified in the image label parameters,
	determined from the number of bits per sample, or based on the limits
	derived from the image data type and and the number of bytes per
	sample. Note that image data always occurs in byte multiples but the
	data may be specified as occupying less than all of the bits of each
	pixel sample.

	<b>N.B.</b>: All scaled image data values will be clipped to be
	within the valid data range.

	@param	option	A QImageIOHandler::ImageOption code specifying the
		option value to obtain.
	@param	value	A QVariant containing a value appropriate for the
		specified option.
	@see	setOption(ImageOption, const QVariant&)
*/
virtual void setOption (ImageOption option, const QVariant& value);

/**	Get the value of an ImageOption.

	The following options are supported:

<dl>
<dt>Size
<dd>A QSize is returned with the width and height of the source image.

	This will be an invalid, Null, QSize if a device has not been
	specified from which a {@link label(QIODevice*, qint64) PDS label}
	was successfully obtained.

	Note: The number of image bands is provided by the {@link bands()
	bands} method.

<dt>ImageFormat
<dd>A QImage::Format code indicating the default format to use when an
	image is {@link read(QImage*) read} and the specified destination
	QImage is null.

	By default the value will be Format_Invalid if a device has not been
	specified from which a {@link label(QIODevice*, qint64) PDS label}
	was successfully obtained. The default value will be Format_RGB32 if
	the image has a multiple of three {@link bands()const bands};
	otherwise it will be Format_Indexed8.

	The value may be {@link setOption(ImageOption, const QVariant&) set}.

<dt>Gamma
<dd>A float that provides a multi-byte image data scaling factor.

	Image data of more than one byte per pixel sample (a full color image
	is composed of three samples - red, green, blue - for each pixel; a
	pseudo color image has one sample per pixel) must be scaled into a
	single byte per sample for the QImage. The default Gamma is zero
	until the first image is {@link read(QImage*) read} when it is set
	using the valid data range which may be specified in the image label
	parameters, determined from the number of bits per sample, or based
	on the limits derived from the image data type and and the number of
	bytes per sample.

	The value may be {@link setOption(ImageOption, const QVariant&) set}.

<dt>Name
<dd>A QString providing a name for the image. If no name can be determined
	this will be an empty string.

<dt>Description
<dd>A QString containing a sequence of {@link parameters_to_Description()
	parameter representations} for the PDS label {@link metadata()const
	metadata}. This will be an empty if a device has not been specified
	from which a {@link label(QIODevice*, qint64) PDS label} was
	successfully obtained.

<dt>IncrementalReading
<dd>A bool that indicates if multiple images can be read from the image
	data source. The value will be true if the ImageFormat is not
	Format_Indexed8 (i.e. it is for an RGB image) and the image has
	more than one set of three bands; or the image has multiple bands
	for a Format_Indexed8 ImageFormat.

	Note that the value of this option may change if the ImageFormat
	option is set to a different value.

<dt>Animation
<dd>A bool that will always be false. This is to indicate that though
	IncrementalReading might be true the image is not actully an
	animation.
</dl>

	@param	option	A QImageIOHandler::ImageOption code specifying the
		option value to obtain.
	@return	A QVariant contain the option value. This will be an invalid
		QVariant if the option is not {@link
		supportsOption(ImageOption)const supported}.
	@see	setOption(ImageOption, const QVariant&)
*/
virtual QVariant option (ImageOption option) const;

/**	Get the effective number of images available from the data source.

	The number of effective images available depends on the value of the
	ImageFormat {@link option(ImageOption)const option} and the total
	number of {@link bands()const image bands}: If the ImageFormat is
	QImage::Format_Indexed8 then the effective images is the same as the
	number of bands; otherwise it is the number of bands divided by
	three.

	@return	The effective number of images available from the data source.
*/
virtual int imageCount () const;

/**	Get the current image number.

	The current image is the last image {@link read(QImage*) read}. If
	the first image has not read the current image number is -1;
	otherwise the current image number is the effective image number,
	starting with zero for the first image read, of the current image.

	The effective image number depends on the value of the ImageFormat
	{@link option(ImageOption)const option} and the total number of
	{@link bands()const image bands}: If the ImageFormat is
	QImage::Format_Indexed8 then the effective image number is the same
	as the image band number; otherwise it is the number of the first
	band in the image divided by three.

	@return	The effective image number of the current image. The first
		image read is image number zero. If the first image has not been
		read the effective image number is -1.
	@see	imageCount()const
*/
virtual int currentImageNumber () const;

/**	Set the number of the next image to be {@link read(QImage*) read}.

	@param	image_number	The number of the next image to read.
	@return	true if image_number is valid; otherwise false and
		the next image number is unchanged.
*/
virtual bool jumpToImage (int image_number);

/**	Increment the number of the next image to be {@link read(QImage*) read}.

	@return	true if the {@link currentImageNumber()const current image}
		is less than the {@link imageCount()const image count}; otherwise
		false and the next image number is unchanged.
	@see	jumpToImage(int)
*/
virtual bool jumpToNextImage ();

/**	Test if this QImageIOHandler can read the data source from the
	device bound to this PDS_Handler.

	<b>N.B.</b>: The device bound to this PDS_Handler must be open,
	readable, and not sequential or no test will be done. If the device
	is not positioned at the beginning of the stream it will be set to
	the beginning before the test is done and restored to its former
	position afterwards.

	If the {@link currentImageNumber()const current image} is the last
	available image, then the device content will not be tested and
	false will be returned.

	@return	true if it appears that a PDS image can be {@link read(QImage*)
		read} from the device; false otherwise.
	@see canRead(QIODevice* device);
*/
virtual bool canRead () const;

/**	Read the image data into a QImage.

	The validity of all the image data access values is confirmed,
	compressed source data is decompressed if needed, and the source
	image data is read. Any image data massaging - possible byte swapping
	of multi-byte pixel sample data, scaling of multi-byte sample data
	into single byte QImage sample data, and reording of band sequential
	(BSQ) source image data into band interleaved by pixel (BIP) QImage
	data - is applied as needed.

	@param	image	A pointer to a QImage to receive the image data.
		This may be a "null" QImage in which case an appropriate
		QImage is provided and assigned to the specified QImage.
	@return	true if the image data was successfully read into the
		QImage; false otherwise and a {@link read_failure_reason()const
		reason for the failure} will be available.
*/
virtual bool read (QImage *image);

/*==============================================================================
	Accessors
*/
/**	Get the PDS image metadata.

	Note: The image metadata can also be obtained via a
	QImageIOHandler::Description {@link option(ImageOption)const option
	query}. In this case the metadata will be formatted as a single
	{@link parameters_to_Description(const Aggregate&) Description
	string}.

	@return	A pointer to an Aggregate of Parameters read from the PDS
		image label. This will be NULL if no label is available.
	@see	label(QIODevice*)
*/
inline const Aggregate* metadata () const
	{return Label;}

/**	Get the number of bands in the image data source.

	Each image band is one pixel deep.

	An image with three bands may be read as a single RGB image or
	incrementally as three single band (QImage::Format_Indexed8) images.
	The effective number of images that can be read for the current
	ImageFormat {@link option(ImageOption)const option} is provided as
	the {@link imageCount()const image count}.

	@return	The total number of image bands in the image data source.
	@see	setOption(ImageOption, const QVariant&)
*/
int bands () const;

/**	A description of the reason for the last read failure.

	If the last {@link read(QImage*) image read} failed the reason
	description will include the class {@link #ID} followed by a brief
	explanation of why the image could not be read; otherwise, if the
	last read was successful, the description string will be empty.

	@return	A QString providing a description of the reason that the
		the last image read failed. This will be the empty string if
		the last read succeeded.
*/
QString read_failure_reason () const;

/*==============================================================================
	Helpers
*/
/**	Test a data source for containing a PDS label.

	<b>N.B.</b>: The device must be open, readable, and not sequential or
	no device content check will be attempted. If the device is not
	positioned at the beginning of the stream it will be set to the
	beginning before the test is done and restored to its former position
	afterwards.

	The first several bytes are obtained from the device using a peek
	(the device position is not altered). These bytes are checked for
	"magic" characters:

	- "PDS_"
	- "LBLSIZE="
	- "NJPL1I00"
	- "CCSD3ZF"

	The last two sequences of magic characters may optionally be
	preceeded by two bytes of a binary record size value, as used by old
	VICAR labels.

	The device content check is not definative. The contents of the
	{@link label(QIODevice*, qint64) PDS label} needs to be examined to
	be sure, and even then the image data may not be readable for some
	reason.

	@param	device	A pointer to a QIODevice to have its content checked.
		If NULL false is returned.
	@return	true if a match with any of the "magic" characters is found
		at the beginning of the device content; false if the required
		number of bytes could not be obtained from the device or no magic
		match was found.
*/
static bool canRead (QIODevice* device);

/**	Read a PDS label from a data source.

	The parameters read from the data source are not checked for being
	a valid PDS image label.

	<b>N.B.</b>: The device must be open, readable, and not sequential or
	no reading will be attempted. If the device is not positioned at the
	specified location in the stream it will be set to that location before
	it is read and restored to its former position after the label is read.
	Failure to seek the device to the label location will result in a
	NULL return value.

	@param	device	A pointer to a QIODevice from which to read the
		PDS label Parameters. If NULL nothing is done.
	@param	location	The location of the label in the data source.
	@return	A pointer to an Aggregate of Parameters. This will be NULL
		if no label was obtained.
	@see	canRead(QIODevice*)
*/
static Aggregate* label (QIODevice* device, qint64 location = 0);

/**	Convert an Aggregate of Parameters to a Description string.

	A QImageIOHandler::ImageOption Description is a QString in which each
	image metadata parameter is formatted thus:

	"pathname: value"

	Where pathname is the absolute pathname of the Parameter in the
	Aggregate and value is the text representation of the Parameter
	Value. <b>N.B.</b>: The parameter value representation will always
	have an extra leading space character (see the QImageReader work
	around description, below).

	Each parameter:value pair in the Description string sequence is
	separated from the next pair by two newline characters; the last
	parameter does not end with the speparator characters. <b>N.B.</b>:
	This is not the same as the documentation for the
	QImageIOHandler::ImageOption Description option.

	Work around for broken QImageReader:

	The QImageReader text and textKeys methods rely on parsing the the
	QImageIOHandler::ImageOption Description QString into a QMap.
	However, this is done in a manner that is inconsistent with the
	documentation for the key:value Description format:

	The Description string is split into QString segments on the two
	newline characters (note that this results in a final empty segment).
	For each segment, if the index of the ':' delimiter is non-negative
	and the index of the space character (' ') is less than the delimiter
	index (note that when no space character is present this index will
	be -1) the resulting key,value pair in the QMap will have a
	"Description" key and the value will be the entire segment
	QString::simplified; otherwise the key is the expected part of the
	segment before the delimiter and the value is the part at the second
	character after the delimiter (!?!) QString::simplified.
	QString::simplified removes leading and trailing whitespace and all
	other whitespace is reduced to a single space character.

	The conditional determining the key,value pair is incorrect, as is
	the selection of the value part string! For example, the typical
	segment "key:value" will result in a "Description" key and a
	"key:value" value. This can be expected to occur for most, if not
	all, segments in the formatted Description string, and so only the
	last "Description" segment will remain in the QMap; all other
	key:value pairs will be lost. Even if the the value contains a space
	character it will be incorrectly selected as "alue" instead of
	"value".

	To work around this problem the two newline characters are only
	placed between key:value segments; none are included after the last
	segment. And an extra space character is added to the beginning of
	the value string so "key: value" will be correctly parsed; the extra
	space will be removed when the value string is simplified.

	@param	parameters	A reference to the Aggregate of Parameters to be
		converted to a QImageIOHandler::Description format string.
	@return	A QString containing the sequence of parameter
		representations. This will be the empty string if the Aggregate
		does not contain a Assignment Parameter.
*/
static QString parameters_to_Description (const Aggregate& parameters);


private:

/**	Read the image metadata.

	Any existing {@link metadata()const metadata parameters} are deleted,
	and all internal image access variables (Private_Data) are reset to
	their uninitialized values.

	The {@link QImageIOHandler::device()const source device} content will
	be read, starting at the beginning of the data stream, to obtain the
	{@link label(QIODevice*, qint64) PDS image label} containing the
	metadata parameters. The initial position of the device will be
	restored after reading the label.

	Selected label metadata parameters are used to set their associated
	internal variables. These are then  evaluated to produce the image
	data access values required to read the image data.

	All values are checked for validity. If any of the label parameters
	do not appear to be valid the label parameters are deleted and the
	internal values are reset.

	If the primary label contains an "EOL" parameter with a non-zero
	value, an attempt is made to read the extended label starting at the
	first byte following the image data to the end of the source file.
	The additional metadata parameters are added to the primary metadata
	parameters in their own EOL group.

	@return	true if the PDS label metadata parameters were successfully
		obtained from the data source and all internal image data
		access values were found to be valid; false otherwise.
	@see	metadata()const
*/
bool read_label () const;

/**	Get a detached image data source.

	This method is used to handle the case where the PDS label file does
	not include the image data, but instead refers to a separate file
	containing only the image data. The label and image data are
	detached.

	If the pathname is absolute it is used as given. For a relative
	pathname the device bound to this QImageIOHandler is used as a QFile
	to obtain the absolute pathname to the directory containing the PDS
	label file which is prepended to the relative pathname.

	<b>N.B.</b>: It would be desirable to also support a QNetworkReply
	device bound to the QImageIOHandler.

	The pathname is used to open a QFile for reading

	@param	pathname	A QString containing the pathname (absolute
		or relative) for the detached image data source.
	@return	A pointer to a QIODevice opened for reading the image
		data. This will be NULL if the detached file could not
		be found or opened; in which case the read_failure_reason
		will describe the problem.
*/
QIODevice* detached_data_source (const QString& pathname) const;

/**	Get a decompressed data source.

	This method is used to handle the case where the PDS label indicates
	that the image data is compressed.

	The specified source is used as a QFile to obtain the absolute
	pathname to the file containing the compressed image data. A pathname
	to a file to receive the decompressed image data is generated with
	this format:

	tempPath/PDS_decompressed_filename-dd.MM.yyyyThh:mm:ss.zzz

	where:

	-tempPath is the absolute pathname for the system's temporary
		directory.
	-filename is the filename from the source pathname.
	-dd.MM.yyyy is the current date in day, month and year values.
	-hh:mm:ss.zzz is the current time in hour, minute, second and
		fractional second values.

	The DECOMPRESSION_FILTER for the Compression_Type is used to generate
	a command line including the compressed data source pathname and the
	decompressed data destination pathname. The command is executed as a
	separate process. If the exit status from the process is zero
	(success) the pathname for the decompressed image data is used to
	open a QFile for reading.

	<b>N.B.</b>: There is no guarantee that the decompressed image data
	file will not have an attached label. This depends on the
	DECOMPRESSION_FILTER that is used. A DECOMPRESSED_FILE_HAS_LABEL
	array of bool values corresponding the DECOMPRESSION_FILTER array is
	expected to provide this information.

	@param	source	A pointer to a QIODevice, which must be a QFile, from
		which the compressed image data can be obtained. This device does
		not need to be open.
	@return	A pointer to a QFile opened for reading from the decompressed
		image data file. This will be NULL if the Compression_Type value
		is invalid, the source pathname could not be determined, the
		decompression filter process did not complete successfully, or
		the file containing the decompressed image data could not be
		opened for reading; in case of failure a read_failure_reason will
		be provided and no decompressed data file will be left behind.
*/
QFile* decompressed_data_source (QIODevice* source) const;

/**	Read the image data into the destination QImage.

	This method manages the transfer of source image data into the
	destination QImage.

	If the Next_Band value equals the total number of image bands then
	nothing is done and false is returned.

	If the specified QImage is Format_Invalid the destination QImage
	Format is set to the Source_Format; otherwise the format of the
	specified QImage is used. In the latter case if the first image is
	being read and the number of source image bands is greater than 2 the
	Source_Format is set to the Format of the QImage.

	If the destination image is not a null QImage and has a size
	corresponding the dimensions of the source image it is used as-as;
	however, if the Source_Format is not Format_Indexed8 but the
	destination image is, then nothing is done and false is returned.
	Otherwise an appropriate temporary QImage is provided to receive the
	data that is read.

	If a detached image data pathname was specified in the PDS label then
	the pathname is used to obtain a detached_data_source that
	substitutes for (but does not permanently replace) the device bound
	to this QImageIOHandler. If a Detached_Source had already been
	obtained from reading a previous image in the same data source, then
	it is reused without needing to setup the detached_data_source again.

	If the source image data is compressed, as indicated by the
	Compression_Type, a decompressed_data_source is obtained that becomes
	the image data source to be read. If a Decompressed_Source had
	already been obtained from reading a previous image in the same data
	source, then it is reused without needing to setup the
	decompressed_data_source again.

	The image data source is then opened for reading if it is not already
	open. If the source is found to have insufficient data beyond the
	expected position of the start of image data to be read nothing is
	done and false is returned.

	Next the data buffer to receive the image data that is read is
	provided. This will be the data buffer internal to the destination
	QImage if the single byte, single band image is being read (in this
	case the image data can be transferred directly into the QImage).
	Otherwise a data buffer is allocated that is large enough to contain
	the band_read_count. Note:

	An attempt is made to provide an address map to the image data source
	that coincides with the image data to be read. If this succeeds the
	image data will be read via the address map, otherwise the image data
	will be read via the QIODevice of the source.

	After the image data has been successfully read from the source the
	data is massaged as needed: Multi-byte pixel samples may needed to
	have their byte order swapped (swap_bytes). Multi-byte samples will
	be collapsed (collapse_data) to single byte values by data scaling
	with the image data buffer containing the data that was read being
	treated as an array of the appropriate data type elements
	corresponding the Data_Type and Sample_Bytes determined from the
	label parameters.

	If the source image data was not read directly into the destination
	QImage then the contents of the image data buffer are copied into the
	QImage while reording the band sequential (BSQ) source data to band
	interleaved by pixel (BIP) QImage data.

	If the destination QImage is not the same as the QImage specified by
	the caller the former is assigned to the latter.

	Finally the Current_Band and Next_Band values are updated and cleanup
	- deleting a temporary image data buffer and QImage, and unmapping
	from the source device if that was done - is completed.

	@param	image	A pointer to a QImage that is the destination
		of the image data that is read.
		<b>>>> WARNING <<<</b> Must not be NULL.
	@return	true if the source image data was successfully read and
		deposited into the specified image.
*/
bool read_image_data (QImage* image);

/**	Read image data from a source device into a data buffer.

	<b>N.B.</b>: The source device must be seekable. The seeks are used
	to position the device at the beginning of the image data to be read
	and skip forward over non-image data (line prefix and suffix bytes,
	and column suffix bytes) embedded in the image data structure.

	@param	source	A pointer to the QIODevice that is the source of
		image data.
		<b>>>> WARNING <<<</b> Must not be NULL.
	@param	data_buffer	A pointer to an unsigned char array that will
		receive the image data that is read. The array must be large
		enough to hold at least the amount of data determined by the
		image dimensions, pixel sample size, band_read_count.
		<b>>>> WARNING <<<</b> Must not be NULL.
	@param	destination	A pointer to the QImage that is the destination
		of the final image data. If the destination data buffer is the
		same as the specified data buffer then the address of the
		begining of each QImage line (scanLine) will be used to set the
		output data buffer address for the line to allow for QImage data
		buffer line data alignment adjustment.
		<b>>>> WARNING <<<</b> Must not be NULL.
*/
bool read_data (QIODevice* source,
	uchar* data_buffer, QImage* destination);

/**	Read image data from an address mapped source device into a data buffer.

	@param	source_data	A pointer to the address mapped to the beginning
		of the image data in the source device.
		<b>>>> WARNING <<<</b> Must not be NULL.
	@param	data_buffer	A pointer to an unsigned char array that will
		receive the image data that is read. The array must be large
		enough to hold at least the amount of data determined by the
		image dimensions, pixel sample size, band_read_count.
		<b>>>> WARNING <<<</b> Must not be NULL.
	@param	destination	A pointer to the QImage that is the destination
		of the final image data. If the destination data buffer is the
		same as the specified data buffer then the address of the
		begining of each QImage line (scanLine) will be used to set the
		output data buffer address for the line to allow for QImage data
		buffer line data alignment adjustment.
		<b>>>> WARNING <<<</b> Must not be NULL.
*/
bool read_data (const uchar* source_data,
	uchar* data_buffer, QImage* destination);

/**	Swap the bytes in a set of data items.

	The byte groups are swapped in place. The first byte of each group is
	swapped with the last byte in the group, the second byte is swapped
	with the next-to-last byte in the group, etc. until all bytes within
	the group have been reorderd. Each byte group is swapped in the same
	way.

	@param	data	A pointer to the first byte of the data to be
		swapped. If NULL nothing is done.
	@param	groups	The number of groups to be reordered.
	@param	size	The number of bytes per group.
*/
static void swap_bytes (unsigned char* data,
	const long long& groups, const unsigned int& size);

/**	Collapse multi-byte pixel sample data to single byte data.

	If the Gamma value is zero it is reset based on the Data_Limits:

	Gamma = 255.0 / (Data_Limits[LIMIT_MAX] - Data_Limits[LIMIT_MIN])

	or Gamma = zero if the data limits are the same, in which case
	all of the collapsed data will have a value of zero.

	Each datum from the source data buffer is scaled using the Gamma
	value:

	datum = Gamma * (datum - Data_Limits[LIMIT_MIN])

	However, if the source datum is less than Data_Limits[LIMIT_MIN] it
	is set to 0, and a source datum greater than Data_Limits[LIMIT_MAX]
	is set to 255, instead of being scaled. And a scaled datum is also
	clipped to the Data_Limits.

	@param	image_data	An array of Pixel_Data_Type values that
		contains the source data to be collapsed.
		<b>>>> WARNING <<<</b> Must not be NULL.
	@param	destination	The QImage where the collapsed data is to
		be written. If NULL the collapsed data will overwrite the
		data buffer.
*/
template<typename Pixel_Data_Type> void collapse_data
	(Pixel_Data_Type* image_data, QImage* destination);

/**	Reorder band sequential (BSQ) image data to band interleaved by
	pixel (BIP) image data.

	The source BSQ image data must be 1 byte per sample, and the buffer
	must contain band_read_count amount of data. The destination QImage
	must be a 32-bit full color format. The source image data is copied
	into the destination QImage. If the band_read_count is less than 3
	the remaining destination pixel sample values are zero unless the
	Source_Format is Format_Indexed8 in which case the single source band
	sample value is replicated across all destination bands.

	@param	image_data	The source image data buffer.
		<b>>>> WARNING <<<</b> Must not be NULL.
	@param	destination	A pointer to the destination QImage that will
		receive the image data.
		<b>>>> WARNING <<<</b> Must not be NULL.
*/
void BSQ_to_BIP (uchar* image_data, QImage* destination) const;

/*	Get the number of image bands to read.

	If the next band to read is equal to the total number of {@link
	bands() bands} in the image data, then zero is returned.

	If the ImageFormat option (Source_Format) is Format_Indexed8 then 1
	is retured; otherwise the minimum of 3 and the remaining number of
	bands in the image data is returned.

	@return	The number of image bands to be read.
*/
int band_read_count () const;

/**	Convert an effective image number to a source band number.

	The band number depends on the Source_Format used when the image is
	read: If the format is Format_Indexed8 then the band number is the
	same as the image number; otherwise it is the image number multiplied
	by 3.

	@param	image_number	An effective image number. Should not
		be negative.
	@return	A source image band number.
*/
int to_band_number (int image_number) const;

/**	Convert a source band number to an effective image number.

	The effective image number depends on the Source_Format used when the
	image is read: If the format is Format_Indexed8 then the effective
	image number is the same as the band number; otherwise it is the band
	number divided by 3.

	@param	band_number	A source image band number. Should not
		be negative.
	@return	An effective image number.
*/
int to_image_number (int band_number) const;

/*==============================================================================
	Data
*/
private:

mutable Aggregate*
	Label;

class Private_Data;
mutable Private_Data
	*Data;

};		//	PDS_Handler

}		//	namespace idaeim
#endif	//	PDS_HANDLER_HH
