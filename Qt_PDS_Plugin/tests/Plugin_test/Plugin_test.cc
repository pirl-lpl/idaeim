/*	Plugin_test

Copyright (C) 2013 Bradford Castalia, idaeim studio.

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

CVS ID: Plugin_test.cc,v 1.10 2013/10/18 01:55:50 castalia Exp
*******************************************************************************/

#include <QApplication>
#include <QFileInfo>
#include <QImageReader>
#include <QSplitter>
#include <QTextEdit>
#include <QSize>
#include <QImage>
#include <QPixmap>
#include <QLabel>
#include <QScrollArea>
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSpinBox>
#include <QMessageBox>

#include <iostream>
#include <iomanip>
#include <cstdlib>
using namespace std;


std::ostream& operator<< (std::ostream& stream,
	const QString& qstring)
	{return stream << qPrintable (qstring);}

std::ostream& operator<< (std::ostream& stream,
	const QSize& size)
	{return stream << size.width () << "w, " << size.height () << 'h';}


const char* const
	FORMAT_NAME[] =
	{
	"Invalid",
	"Mono",
	"Mono_LSB",
	"Indexed8",
	"RGB32",
	"ARGB32",
	"ARGB32_Premultiplied",
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

const char* const
format_name
	(
	int		format
	)
{
if (format < 0 ||
	format > 15)
	return "unknown";
return FORMAT_NAME[format];
}


class Plugin_test
:	public QWidget
{
Q_OBJECT

public:

Plugin_test
	(
	QImageReader*	reader,
	int				format
	)
	:	QWidget (),
		Image_Reader (reader),
		Image (NULL),
		Image_Label (new QLabel),
		Image_Selection (new QSpinBox)
{
QVBoxLayout*
	layout = new QVBoxLayout (this);

QSplitter*
	split_panel = new QSplitter (Qt::Vertical, this);
split_panel->setChildrenCollapsible (true);

QTextEdit
	*metadata = new QTextEdit;
QString
	text;
foreach (const QString& key, Image_Reader->textKeys ())
	{
	if (key.isEmpty ())
		continue;
	text += key;
	text += " = ";
	text += Image_Reader->text (key);
	text += '\n';
	}
cout << "Metadata parameters -";
if (text.isEmpty ())
	cout << " none" << endl;
else
	cout << endl << text;
metadata->setPlainText (text);
metadata->setReadOnly (true);
split_panel->addWidget (metadata);

QSize
	image_size (Image_Reader->size ());

cout << "Image format -" << endl
	 << "    given: " << format << " - "
	 	<< format_name (format) << endl;
QImage::Format
	image_format
		((format == 1) ? QImage::Format_Indexed8 :
		((format == 3) ? QImage::Format_RGB32 :
		                 QImage::Format_Invalid));
cout << "  default: " << Image_Reader->imageFormat ()
	 	<< " - " << format_name (Image_Reader->imageFormat ()) << endl;
if (image_format == QImage::Format_Invalid)
	Image = new QImage;
else
	{
	cout << "  applied: " << image_format
	 	<< " - " << format_name (image_format) << endl;
	Image = new QImage (image_size, image_format);
	}
cout << "   actual: " << Image->format ()
		<<  " - " << format_name (Image->format ()) << endl;

cout << "Image size: " << image_size << endl;
Image_Label->resize (image_size);

QScrollArea
	*scroll_area = new QScrollArea;
scroll_area->setWidget (Image_Label);
split_panel->addWidget (scroll_area);

layout->addWidget (split_panel);

QHBoxLayout
	*box_layout = new QHBoxLayout;
Image_Selection->setRange (0, Image_Reader->imageCount () - 1);
Image_Selection->setValue (0);
connect (Image_Selection,
	SIGNAL (valueChanged (int)),
	SLOT (select_image (int)));
box_layout->addWidget (new QLabel ("Image: "));
box_layout->addWidget (Image_Selection, 1);
box_layout->addWidget (new QLabel
	(QString ("of ") + QString::number (Image_Reader->imageCount ()) + " total"));
box_layout->addStretch (100);
box_layout->addWidget (new QLabel ("Format: "));
box_layout->addWidget
	(Format_Label = new QLabel (format_name (Image->format ())));

layout->addLayout (box_layout);
}


public slots:

void
select_image
	(
	int		image_number
	)
{
bool
	OK = false;
if (image_number == (Image_Reader->currentImageNumber () + 1))
	OK = Image_Reader->jumpToNextImage ();
else
	OK = Image_Reader->jumpToImage (image_number);
if (! OK &&
	image_number != 0)
	QMessageBox::warning (this, "Plugin_test",
		QString ("Can't jump to image number ") +
		QString::number (image_number));
else
if (! Image_Reader->read (Image))
	QMessageBox::warning (this, "Plugin_test",
		QString ("Couldn't read image number ") +
		QString::number (image_number));
else
	{
	Image_Label->setPixmap (QPixmap::fromImage (*Image));
	Format_Label->setText (format_name (Image->format ()));
	cout << "Image read" << endl
		 << "   format: " << Image->format ()
		 	<< " - " << format_name (Image->format ()) << endl
		 << "  imageCount = " << Image_Reader->imageCount () << endl;
	}
}


private:

QImageReader
	*Image_Reader;

QImage
	*Image;
QLabel
	*Image_Label;

QSpinBox
	*Image_Selection;
QLabel
	*Format_Label;
};
#include "Plugin_test.moc"



int
main
	(
	int		count,
	char**	arguments
	)
{
cout << "Qt version - compiled "
        << QT_VERSION_STR << " (0x" << hex << QT_VERSION << dec
        << "), runtime " << qVersion () << endl
     << endl;

QApplication
    application (count, arguments);
QString
	pathname;
int
	format = 0;

while (--count)
	{
	if (*arguments[count] == '-')
		{
		switch (*(arguments[count] + 1))
			{
			case '1':	//	Read single band images.
				format = 1;
				break;
			case '3':	//	Read three band images.
				format = 3;
				break;
			default:
				cout << "Unknown option: " << arguments[count] << endl;
			case 'H':
			case 'h':
				Usage:
				cout << "Usage: " << *arguments
						<< " [-1 | -3] [<pathname>]" << endl;
				exit (1);
			}
		}
	else
	if (pathname.isEmpty ())
		pathname = arguments[count];
	else
		{
		cout << "Multiple pathnames -" << endl
			 << pathname << endl
			 << "and" << endl
			 << arguments[count] << endl;
		goto Usage;
		}
	}

QFileInfo
	file_info ("../../libPDS.dylib");
if (file_info.exists ())
	{
	//	Add the parent directory to the application's library path.
	cout << "libPDS.so exists in directory "
			<< file_info.absolutePath () << endl;
	#if defined (Q_OS_WIN32)
	#define imageformats "../../imageformats.lnk"
	#else
	#define imageformats "../../imageformats"
	#endif
	if (! QFileInfo (imageformats).exists () &&
		! QFile::link (".", imageformats))
		{
		cout << "Could not create the link: " << imageformats << endl;
		exit (2);
		}
	application.addLibraryPath (file_info.absolutePath ());
	}

cout << "Supported image formats -" << endl;
foreach (const QByteArray& entry, QImageReader::supportedImageFormats ())
	cout << QString (entry) << ' ';
cout << endl;
if (! QImageReader::supportedImageFormats ().contains ("PDS"))
	{
	cout << "The PDS plugin was not found." << endl
		 << "Make sure the libPDS.plugin library was built." << endl;
	exit (3);
	}

if (! pathname.isEmpty ())
	{
	QImageReader
		*reader = new QImageReader (pathname);
	if (! reader->canRead ())
		{
		cout << "Can't read " << pathname << endl;
		exit (4);
		}

	Plugin_test
		*plugin_test = new Plugin_test (reader, format);
	plugin_test->select_image (0);
	plugin_test->show ();
	return application.exec ();
	}
}
