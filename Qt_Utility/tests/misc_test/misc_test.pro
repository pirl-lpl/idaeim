#	misc_test
#
#	Copyright (C) 2013  Bradford Castalia, idaeim studio.
#
#	This file is part of the idaeim libraries.
#
#	The idaeim libraries are free software; you can redistribute them
#	and/or modify them under the terms of the GNU Lesser General Public
#	License as published by the Free Software Foundation, either version
#	3 of the License, or (at your option) any later version.
#
#	The idaeim libraries are distributed in the hope that they will be
#	useful, but WITHOUT ANY WARRANTY; without even the implied warranty
#	of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
#	Lesser General Public License for more details.
#
#	You should have received a copy of the GNU Lesser General Public
#	License along with this program. If not, see
#	<http://www.gnu.org/licenses/>.
#
#	CVS ID: misc_test.pro,v 1.2 2014/02/06 02:05:23 castalia Exp

TEMPLATE		=	app
TARGET			=	misc_test

CONFIG			+=	qt console debug

INCLUDEPATH		+=	../.. \
					../../..

SOURCES			+=	misc_test.cc

LIBS			+=	../../libQt_Utility.a \
					../../../Utility/libidaeim.a
