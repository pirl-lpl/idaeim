#	Qt_versions.pri
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
#	CVS ID: Qt_versions.pri,v 1.9 2013/10/17 08:16:54 castalia Exp

#	Host system machine architecture.
MACHINE = $$system(uname -m)
! build_pass:message (Host machine architecture: $$MACHINE)
DEFINES +=	MACHINE=$$MACHINE

#	Host system OS.
HOST_OS = $$system(uname -s)
KERNEL_RELEASE = $$system(uname -r)
! build_pass:message (Host operating system: $$HOST_OS $$KERNEL_RELEASE)
contains (HOST_OS, CYGWIN.+): OS = WIN
else: OS = $$join(HOST_OS, _)
DEFINES +=	OS=$$OS

#	Software versions.

! build_pass:message (Qt version: $$[QT_VERSION])
! build_pass:message (qmake version: $$[QMAKE_VERSION])

#	Module version.
! exists ($$_PRO_FILE_PWD_/VERSION) {
	exists ($$_PRO_FILE_PWD_/../VERSION) {
		phony = $$system(ln -s ../VERSION $$_PRO_FILE_PWD_/VERSION)
		}
	}
exists ($$_PRO_FILE_PWD_/VERSION) {
	VERSION = $$system(tail -1 $$_PRO_FILE_PWD_/VERSION)
	isEmpty (VERSION) | ! count (VERSION, 1) {
		! build_pass:warning (----------------------------)
		! build_pass:warning (!!!> No version number found)
		! build_pass:warning (---> VERSION set to 0.0     )
		! build_pass:warning (----------------------------)
		VERSION = 0.0
		}
	}
else {
	! build_pass:warning (-----------------------)
	! build_pass:warning (!!!> No VERSION file   )
	! build_pass:warning (---> VERSION set to 0.0)
	! build_pass:warning (-----------------------)
	VERSION = 0.0
	}

MODULE_NAME = $$basename(_PRO_FILE_PWD_)

! build_pass:message ($$MODULE_NAME version: $$VERSION)
DEFINES +=	MODULE_VERSION=$$VERSION

sp = $$quote(" ")
VERSION_Report.target = VERSION_Report
VERSION_Report.commands	= @echo $$quote(\"$$sp$$sp$$sp$$MODULE_NAME v$$VERSION\")
QMAKE_EXTRA_TARGETS += VERSION_Report
