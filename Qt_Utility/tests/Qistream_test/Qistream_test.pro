#	Qistream_test
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
#	CVS ID: Qistream_test.pro,v 1.3 2013/02/24 00:45:15 castalia Exp

TEMPLATE		=	app
TARGET			=	Qistream_test

DEPENDPATH		+=	. \
					../..

CONFIG			+=	qt console debug
QT				+=	network

INCLUDEPATH		+=	. \
					../.. \
					/opt/idaeim/include

unix:LIBS		+=	/opt/idaeim/lib/libidaeim.a
win32:LIBS		+=	d:/cygwin/opt/idaeim/lib/libidaeim.a

HEADERS			+=	Qistream.hh

SOURCES			+=	Qistream.cc \
					Qistream_test.cc
