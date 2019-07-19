/*	Qlisters

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

CVS ID: Qlisters.hh,v 1.3 2013/02/24 00:45:15 castalia Exp
*******************************************************************************/

#ifndef QLISTERS_HH
#define QLISTERS_HH

#include	<QPoint>
#include	<QPointF>
#include	<QSize>
#include	<QSizeF>
#include	<QRect>
#include	<QRectF>
#include	<QColor>
#include	<QString>

#include	<ostream>
#include	<iomanip>


/*------------------------------------------------------------------------------
	Qt objects listers (output operators)
*/
/**	QPoint output operator.

	The point is written as the decimal coordinate values:

	XXXx, YYYy

	@param	stream	A std::ostream reference where the text
		representation will be written.
	@param	point	The QPoint to be output.
	@return	The stream.
*/
inline std::ostream& operator<< (std::ostream& stream,
	const QPoint& point)
	{return stream << point.x () << "x, " << point.y () << 'y';}

/**	QPointF output operator.

	The point is written as the decimal coordinate values:

	XXXx, YYYy

	@param	stream	A std::ostream reference where the text
		representation will be written.
	@param	point	The QPointF to be output.
	@return	The stream.
*/
inline std::ostream& operator<< (std::ostream& stream,
	const QPointF& point)
	{return stream << point.x () << "x, " << point.y () << 'y';}

/**	QSize output operator.

	The size is written as the decimal width and height:

	WWWw, HHHh

	@param	stream	A std::ostream reference where the text
		representation will be written.
	@param	size	The QSize to be output.
	@return	The stream.
*/
inline std::ostream& operator<< (std::ostream& stream,
	const QSize& size)
	{return stream << size.width () << "w, " << size.height () << 'h';}

/**	QSizeF output operator.

	The size is written as the decimal width and height:

	WWWw, HHHh

	@param	stream	A std::ostream reference where the text
		representation will be written.
	@param	size	The QSizeF to be output.
	@return	The stream.
*/
inline std::ostream& operator<< (std::ostream& stream,
	const QSizeF& size)
	{return stream << size.width () << "w, " << size.height () << 'h';}

/**	QRect output operator.

	The rectangle is written as the decimal left (x), top (y) coordinate
	and width, height (size):

	XXXx, YYYy, WWWw, HHHh

	@param	stream	A std::ostream reference where the text
		representation will be written.
	@param	rectangle	The QRect to be output.
	@return	The stream.
*/
inline std::ostream& operator<< (std::ostream& stream,
	const QRect& rectangle)
	{return 
		stream << rectangle.x () << "x, " << rectangle.y () << "y, "
			   << rectangle.width () << "w, " << rectangle.height () << 'h';}

/**	QRectF output operator.

	The rectangle is written as the decimal x (left), y (top) coordinate
	and width, height (size):

	XXXx, YYYy, WWWw, HHHh

	@param	stream	A std::ostream reference where the text
		representation will be written.
	@param	rectangle	The QRectF to be output.
	@return	The stream.
*/
inline std::ostream& operator<< (std::ostream& stream,
	const QRectF& rectangle)
	{return 
		stream << rectangle.x () << "x, " << rectangle.y () << "y, "
			   << rectangle.width () << "w, " << rectangle.height () << 'h';}

/**	QColor output operator.

	The color is written as the hex Alpha-Red-Green-Blue value of the color
	as eight uppercase characters preceeded by a crosshatch ('#') character:

	\#AARRGGBB

	@param	stream	A std::ostream reference where the text
		representation will be written.
	@param	color	The QColor to be output.
	@return	The stream.
*/
inline std::ostream& operator<< (std::ostream& stream,
	const QColor& color)
	{return
		stream << std::hex << std::uppercase << std::setfill ('0')
			   << '#' << std::setw (8) << color.rgba ()
			   << std::dec << std::nouppercase << std::setfill (' ');}

/*	QString output operator.

	The QString is written as its C-string form.

	@param	stream	A std::ostream reference where the text
		representation will be written.
	@param	qstring	A QString.
	@return	The stream.
*/
inline std::ostream& operator<< (std::ostream& stream,
	const QString& qstring)
	{return stream << qPrintable (qstring);}

#endif	//	QLISTERS_HH

