#	Qstrings_test
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
#	CVS ID: Qstrings_test.pro,v 1.3 2013/12/17 03:36:18 castalia Exp

TEMPLATE		=	app
TARGET			=	Qstrings_test

CONFIG			+=	qt console debug

INCLUDEPATH		+=	. \
					../.. \
					/opt/idaeim/include

SOURCES			+=	Qstrings_test.cc

unix:LIBS		+=	../../libQt_Utility.a \
					/opt/idaeim/lib/libidaeim.a
