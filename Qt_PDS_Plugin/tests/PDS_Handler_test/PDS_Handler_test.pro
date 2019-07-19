#	PDS_Handler_test.pro
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
#	CVS ID: PDS_Handler_test.pro,v 1.6 2013/05/11 05:22:28 castalia Exp

TEMPLATE		=	app
TARGET			=	PDS_Handler_test

QT				+=	core \
					network

CONFIG			+=	qt debug

DEPENDPATH		+=	. \
					../..

INCLUDEPATH		+=	. \
					../.. \
					../../../include \
					../../../include/PVL \
					../../../include/Strings \
					../../../include/Utility \
					../../../include/Qt_Utility

LIBS			+= \
	../../../PVL/libPVL.a \
	../../../Strings/libString.a \
	../../../Utility/libidaeim.a \
	../../../Qt_Utility/libQt_Utility.a

HEADERS			+=	PDS_Handler.hh
					
SOURCES			+=	PDS_Handler.cc \
					PDS_Handler_test.cc
