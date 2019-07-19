/*	PDS_Handler_test

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

CVS ID: PDS_Handler_test.cc,v 1.7 2013/02/24 00:49:37 castalia Exp
*******************************************************************************/
#include	"PDS_Handler.hh"
using idaeim::PDS_Handler;

#include "Qt_Utility/Qlisters.hh"

#include	"PVL.hh"
using idaeim::PVL::Aggregate;

#include	<QtGlobal>
#include	<QApplication>
#include	<QFile>
#include	<QVariant>
#include	<QImage>
#include	<QLabel>
#include	<QScrollArea>

#include	<string>
#include	<iostream>
#include	<iomanip>
#include	<cstdlib>
using namespace std;


int
main
	(
	int		argc,
	char**	argv
	)
{
cout << "Qt version - compiled "
		<< QT_VERSION_STR << " (0x" << hex << QT_VERSION << dec
		<< "), runtime " << qVersion () << endl
	 << PDS_Handler::ID << endl
	 << endl;

QApplication
	application (argc, argv);

if (argc < 2)
	{
	cout << "Usage: " << *argv << " <filename>" << endl;
	exit (1);
	}

QFile
	file (argv[1]);
if (! file.open (QIODevice::ReadOnly))
	{
	cout << "Could not open the file." << endl;
	exit (2);
	}

bool
	readable = PDS_Handler::canRead (&file);
cout << "--- canRead = " << boolalpha << readable << endl;

if (readable)
	{
	cout << "--- constructor" << endl;
	PDS_Handler
		handler;
	cout << "--- setDevice" << endl;
	handler.setDevice (&file);

	cout << "--- option -" << endl;
	QSize
		size (handler.option
			(QImageIOHandler::Size).toSize ());
	cout << "      Size = " << size << endl;
	QImage::Format
		format = (QImage::Format)handler.option
			(QImageIOHandler::ImageFormat).toInt ();
	cout << "      ImageFormat = " << format << endl;
	cout << "      IncrementalReading = " << handler.option
		 	(QImageIOHandler::IncrementalReading).toBool () << endl
		 << "      Animation = " << handler.option
		 	(QImageIOHandler::Animation).toBool () << endl
		 << "      Name = \"" << handler.option
		 	(QImageIOHandler::Name).toString () << '"' << endl
		 << "      Description -" << endl << handler.option
		 	(QImageIOHandler::Description).toString ().replace ("\n\n", "\n")
			<< endl;

	cout << "--- imageCount = "
			<< handler.imageCount () << endl
		 << "--- currentImageNumber = "
		 	<< handler.currentImageNumber () << endl
		 << "--- bands = " << handler.bands () << endl;

	cout << "--- read" << endl;
	QImage
		*image = new QImage (size, format);
	bool
		read = handler.read (image);
	cout << "      read = " << read << endl;

	if (read)
		{
		cout << "--- currentImageNumber = "
		 	 << handler.currentImageNumber () << endl;

		QLabel
			*label = new QLabel;
		label->setPixmap (QPixmap::fromImage (*image));
		QScrollArea
			*scroll_area = new QScrollArea;
		scroll_area->setWidget (label);
		scroll_area->show ();
		application.exec ();
		}
	}
}
