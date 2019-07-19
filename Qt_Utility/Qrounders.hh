/*	Qrounders

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

CVS ID: Qrounders.hh,v 1.6 2014/01/15 05:23:15 castalia Exp


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

#ifndef QROUNDERS_HH
#define QROUNDERS_HH

#include	<QPoint>
#include	<QPointF>
#include	<QSize>
#include	<QSizeF>
#include	<QRect>
#include	<QRectF>

#include	<cfloat>
#include	<cmath>


namespace idaeim
{
/*------------------------------------------------------------------------------
	Rounders
*/
#ifndef TO_INTEGER_EPSILON
/**	Round down epsilon.

	When rounding a floating point value to an integer one would expect
	integer truncation to be sufficient, however a floating point value
	may be an "epsilon" amount away from an integer value so that
	truncation alone produces the neighboring integer instead. To
	compensate for this effect the floating point value is adjusted in
	the sign direction by the epsilon amount (here set to DBL_EPSILON if
	this macro is defined, otherwise  0.0000001) before truncation.
*/
#ifdef DBL_EPSILON
#define TO_INTEGER_EPSILON	DBL_EPSILON
#else
#define TO_INTEGER_EPSILON	0.0000001
#endif
#endif

/**	Round a floating point value down to the nearest integer.

	<b>N.B.</b>: This function rounds towards zero. For example, 1.1 will
	round down to 1; -2.2 will round down to -2.

	The Qt qRound function rounds to the "nearest integer" which rounds
	down when the real value has a fractional part < .5, and rounds up
	otherwise. This would split a pixel stradling a tile boundary such
	that it occurred twice: once on each side of the boundary. This is
	definately not what is intended. Instead, any partial pixel location
	(in floating point units) should be bound to a single pixel location
	(in integer units), which logically would be the location without any
	fractional part; i.e. "integer truncation" should be used. This would
	create a problem at the zero boundary - the pixels including location
	0.5 and -0.5 would both be at location 0 - but the image space being
	used here is unsigned integer pixel units. Any negative pixel
	locations are defined as outside the image.

	Note: One would expect integer truncation to be sufficient, however a
	floating point value may be an "epsilon" amount away from an integer
	value so that truncation alone produces the neighboring integer
	instead. To compensate for this effect the floating point value is
	adjusted in the sign direction by the #TO_INTEGER_EPSILON amount
	(here set to 0.0000001) before truncation.

	@param	value	A double value.
	@return	The value rounded down (towards zero) to the nearest integer.
	@see	round_up(const double&)
*/
inline int round_down (const double& value)
	{return static_cast<int>(value +
		((value > 0.0) ? TO_INTEGER_EPSILON : -TO_INTEGER_EPSILON));}

/**	Round a floating point value up to the nearest integer.

	<b>N.B.</b>: This function rounds away from zero. For example, 1.1
	will round up to 2; -2.2 will round up to -3.

	@param	value	A double value.
	@return	The value rounded up (away from zero) to the nearest integer.
	@see	round_down(const double&)
*/
inline int round_up (const double& value)
	{return static_cast<int>(value > 0.0 ? ceil (value) : floor (value));}

/**	Round down a QPointF to a QPoint.

	@param	point A QPointF.
	@return A QPoint having the rounded down values of the point.
*/
inline QPoint round_down (const QPointF& point)
	{return QPoint (round_down (point.x ()), round_down (point.y ()));}

/**	Round up a QPointF to a QPoint.

	@param	point A QPointF.
	@return A QPoint having the rounded up values of the point.
*/
inline QPoint round_up (const QPointF& point)
	{return QPoint (round_up (point.x ()), round_up (point.y ()));}

/**	Round down a QSizeF to a QSize.

	@param	size A QSizeF.
	@return A QSize having the rounded down values of the size.
*/
inline QSize round_down (const QSizeF& size)
	{return QSize (round_down (size.width ()), round_down (size.height ()));}

/**	Round up a QSizeF to a QSize.

	@param	size A QSizeF.
	@return A QSize having the rounded up values of the size.
*/
inline QSize round_up (const QSizeF& size)
	{return QSize (round_up (size.width ()), round_up (size.height ()));}

/**	Round down a QRectF to a QRect.

	Both the rectangle top-left point and size are rounded.

	@param	rectangle A QRectF.
	@return A QRect having the rounded down values of the rectangle.
*/
inline QRect round_down (const QRectF& rectangle)
	{return QRect
		(round_down (rectangle.topLeft ()),
		 round_down (rectangle.size ()));}

/**	Round up a QRectF to a QRect.

	Both the rectangle top-left point and size are rounded.

	@param	rectangle A QRectF.
	@return A QRect having the rounded up values of the rectangle.
*/
inline QRect round_up (const QRectF& rectangle)
	{return QRect
		(round_up (rectangle.topLeft ()),
		 round_up (rectangle.size ()));}

double round_to (double value, int decimal_places);

}		//	namespace idaeim
#endif	//	QROUNDERS_HH
