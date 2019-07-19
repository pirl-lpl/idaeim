#	Copyright (C) 2012-2014  Bradford Castalia, idaeim studio.
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
#	CVS ID: Qt_PDS_Plugin.pro,v 1.17 2014/03/17 05:58:13 castalia Exp

TEMPLATE 		=	lib
TARGET			=	PDS

#	Choose ONE of the following library build directives.
#CONFIG			+=	shared_and_static	#	Both dynamic and static libraries
CONFIG			+=	shared				#	Dynamic (shared) library only
#CONFIG			+=	static				#	Static library only	

#	Plugin library installation location.
! isEmpty (PLUGIN_LIB_INSTALL_DIR): LIB_INSTALL_DIR = $$PLUGIN_LIB_INSTALL_DIR
isEmpty (LIB_INSTALL_DIR) {
		 LIB_INSTALL_DIR = $$[QT_INSTALL_PLUGINS]/imageformats
	}
! build_pass:message (LIB_INSTALL_DIR: $$LIB_INSTALL_DIR)
plugin_dir = $$basename(LIB_INSTALL_DIR)
! contains (plugin_dir, imageformats) {
	! build_pass:warning (LIB_INSTALL_DIR should end with \"imageformats\")
	! build_pass:warning (" This is not required but Qt applications")
	! build_pass:warning (" will only search for image format plugins")
	! build_pass:warning (" in paths containing an imageformats subdirectory")
	}


#	Qt framework support configuration:

QT				+=	core \
					network

#		Required:
CONFIG			+=	qt \
					plugin

#		Optional:
CONFIG			+=	warn_on
#CONFIG			+=	release
#CONFIG			+=	debug
CONFIG			+=	debug_and_release

#		Include support for large files.
unix:CONFIG		+=	largefile


#	Module VERSION.
! include (../Build/Qt_versions.pri) {
	! build_pass:warning (-----------------------)
	! build_pass:warning (---> VERSION set to 0.0)
	! build_pass:warning (-----------------------)
	VERSION = 0.0
	}


#	Special compiler flags.

QMAKE_CXXFLAGS	-=	-Wall

macx {
   QMAKE_CXX = clang++
#        QMAKE_CXXFLAGS += -v

# ! build_pass:message (Building universal binary)
#                CONFIG  +=      x86
        }

#	External (non-Qt) dependencies:

INCLUDEPATH += \
	. \
	.. \
	../PVL \
	../Strings \
	../Utility \
	../Qt_Utility

HEADERS += PDS_Handler.hh \
		   PDS_Plugin.hh

SOURCES += PDS_Handler.cc \
		   PDS_Plugin.cc

LIBS	+= \
	-L../PVL -lPVL \
	-L../Strings -lString \
	-L../Utility -lidaeim \
	-L../Qt_Utility -lQt_Utility

#LIBS    += \
#        ../PVL/libPVL.a \
#        ../Strings/libString.a \
#        ../Utility/libidaeim.a \
#        ../Qt_Utility/libQt_Utility.a



#	Library build configuration.
! include (../Build/Qt_lib.pri) {
	warning ()
	error (The required ../Build/Qt_lib.pri is missing!)
	}

#	Documentation build configuration.
! include (../Build/Qt_docs.pri) {
	warning ()
	error (The required ../Build/Qt_docs.pri is missing!)
	}
