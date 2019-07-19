#	Qt_Utility.pro
#
#	Copyright (C) 2013-2014  Bradford Castalia, idaeim studio.
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
#	CVS ID: Qt_Utility.pro,v 1.18 2014/04/17 04:53:17 castalia Exp

TEMPLATE 		=	lib
TARGET			=	Qt_Utility

#	Choose ONE of the following library build directives.
#CONFIG			+=	shared_and_static	#	Both dynamic and static libraries
CONFIG			+=	shared				#	Dynamic (shared) library only
#CONFIG			+=	static				#	Static library only	


#	Qt framework support configuration.

QT				+=	core \
					network

#		Required:
CONFIG			+=	qt \
					thread
#		Optional:
CONFIG			+=	warn_on
#CONFIG			+=	release
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

#        QMAKE_CXXFLAGS += -fvisibility=hidden -fvisibility-inlines-hidden

#                ! build_pass:message (Building universal binary)
#                CONFIG  +=      x86
        }

#	Disable MSVC "Function call with parameters that may be unsafe" warning.
win32: DEFINES		+= _SCL_SECURE_NO_WARNINGS


#	External (non-Qt) dependencies:

INCLUDEPATH += \
	..


#	Source code files:

HEADERS += \
	Log_Lock.hh \
	Network_Access_Manager.hh \
	Network_Status.hh \
	PDS_Metadata.hh \
	Qistream.hh \
	Qlisters.hh \
	Qrounders.hh \
	Qstrings.hh \
	Synchronized_Event.hh \
	URL_Checker.hh \
	URL_Fetcher.hh \
	misc.hh

SOURCES += \
	Log_Lock.cc \
	Network_Access_Manager.cc \
	Network_Status.cc \
	PDS_Metadata.cc \
	Qistream.cc \
	Qrounders.cc \
	Qstrings.cc \
	Synchronized_Event.cc \
	URL_Checker.cc \
	URL_Fetcher.cc \
	misc.cc

LIBS += \
	-L../PVL \
		-lPVL \
	-L../Strings \
		-lString \
	-L../Utility \
		-lidaeim \


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

