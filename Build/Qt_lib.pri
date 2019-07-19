#	Qt_lib.pri
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
#	CVS ID: Qt_lib.pri,v 1.8 2013/10/26 01:56:32 castalia Exp

#	Installation location.
isEmpty (INSTALL_DIR): INSTALL_DIR = ..
! build_pass:message (Default INSTALL_DIR: $$INSTALL_DIR)
isEmpty (LIB_INSTALL_DIR): LIB_INSTALL_DIR = $$INSTALL_DIR/lib
isEmpty (HEADERS_INSTALL_DIR): HEADERS_INSTALL_DIR = $$INSTALL_DIR


#	CONFIG.

shared_and_static {
	#	Build the static library in addtion to the dynamic library.
	CONFIG			+=	shared	#	Primary target is the shared library.
	CONFIG			-=	static	#	staticlib target builds the static library.

	#	The TARGET library is build by the $(TARGET) make target.
	#	The static library is build by the staticlib make target.
	#	The staticlib target is made a dependency of the all make tareget
	#	by providing an additional all target definition.
	all.target			=	all
	all.depends			+=	staticlib
	QMAKE_EXTRA_TARGETS	+=	all
	}

macx {
	#	Host system kernel release build dependency.
	KERNEL_RELEASE = $$system(uname -r)
	contains (MACHINE, i386):contains (KERNEL_RELEASE, 10\\..+) {
		#	Build universal binary on v10.x OS.
		! build_pass:message (Building x86 and ppc universal binary)
		CONFIG		+=	x86 ppc
		}
	}

CONFIG (debug, debug|release) {
	DEFINES			+=	DEBUG_SECTION=$(DEBUG_SECTION)
	
	#	Selective by-QObject name DEBUG.
	DEFINES			+=	DEBUG_OBJECT=$(DEBUG_OBJECT)

	win32 {
		#	Save debugging information in the debug directory
		QMAKE_LFLAGS_DEBUG	+= /MAP:debug/mapfile.txt /MAPINFO:exports
		QMAKE_CXXFLAGS		+=	/Fd"debug/$$TARGET.pdb"
		#	Provide console output on MS/Windows.
		#	Debug output (stdout) is always provided remotely through cygwin,
		#	but not on a local physical console without the console option.
		CONFIG				+=	console
		}

	SUBDIR_SUFFIX	=	_debug
	}
else {
	SUBDIR_SUFFIX	=
	}


#	Subdirectories for build files.

OBJECTS_DIR			=	obj$$SUBDIR_SUFFIX
obj_clean.target	=	obj_clean
obj_clean.commands	=	$(DEL_FILE) -r $$OBJECTS_DIR
QMAKE_EXTRA_TARGETS	+=	obj_clean
distclean.depends	+=	obj_clean

MOC_DIR				=	moc$$SUBDIR_SUFFIX
moc_clean.target	=	moc_clean
moc_clean.commands	=	$(DEL_FILE) -r $$MOC_DIR
QMAKE_EXTRA_TARGETS	+=	moc_clean
distclean.depends	+=	moc_clean

RCC_DIR				=	moc$$SUBDIR_SUFFIX
! contains (RCC_DIR, $$MOC_DIR) {
	rcc_clean.target	=	rcc_clean
	rcc_clean.commands	=	$(DEL_FILE) -r $$RCC_DIR
	QMAKE_EXTRA_TARGETS	+=	rcc_clean
	distclean.depends	+=	rcc_clean
	}


#	Installation.

#		Library files:

target.path			=	$$LIB_INSTALL_DIR
INSTALLS			+=	target
! build_pass:message (LIB_INSTALL_DIR: $$LIB_INSTALL_DIR ($$target.path))

bin_install.target	=	bin_install

shared_library.target			=	shared_library
shared_library_install.target	=	shared_library_install
shared {
	#	shared_library alias for $(TARGET)
	shared_library.depends			=	$(TARGET)
	#	shared_library_install alias for install_target.
	shared_library_install.depends	=	install_target
	#	bin_install alias for static_library_install.
	bin_install.depends				+=	$$shared_library_install.target
	}
else {
	shared_library_install.commands	=	@echo "No shared library"
	}
QMAKE_EXTRA_TARGETS	+=	shared_library_install
QMAKE_EXTRA_TARGETS	+=	shared_library

static_library.target			=	static_library
static_library_install.target	=	static_library_install
static {
	#	static_library alias for staticlib.
	static_library.depends			=	$(TARGET)
	#	static_llibrary_install alias for install_target.
	static_library_install.depends	=	install_target

	#	bin_install alias for static_library_install.
	bin_install.depends				=	$$static_library_install.target
	}
else {
	shared_and_static {
		#	Install the static library in addition to the dynamic library.
		unix {
			STATIC_LIB_EXT	=	.a
			}
		else {
			! win32 {
				message ()
				error (Only Unix and Windows platforms are supported.)
				}
			STATIC_LIB_EXT	=	.lib
			}
		STATIC_LIBRARY		=	lib$${TARGET}$${STATIC_LIB_EXT}

		#	N.B.: The {un}install_xxx Makefile targets will only be generated 
		#	if the specified xxx.files exist (the target.files excepted).
		! exists ($$_PRO_FILE_PWD_/$$STATIC_LIBRARY) {
			phony = $$system(touch $$_PRO_FILE_PWD_/$$STATIC_LIBRARY)
			}
		else {
			source_object = $$member(SOURCES)
			source_object = $$replace(source_object, .cc, $$QMAKE_EXT_OBJ)
			exists ($$_PRO_FILE_PWD_/$$OBJECTS_DIR/$$source_object) {
				#	Force static library rebuild
				#	in case the Makefile is being regenerated
				#	but the static library did not exist
				#	and the objects files do exist.
				phony = $$system(rm $$_PRO_FILE_PWD_/$$OBJECTS_DIR/$$source_object)
				}
			}

		static_lib.path		=	$$target.path
		static_lib.files	=	$$STATIC_LIBRARY
		INSTALLS			+=	static_lib

		#	Clean out static library.
		static_library_clean.target	=	static_library_clean
		static_library_clean.commands =	\
			$(DEL_FILE) $$STATIC_LIBRARY
		QMAKE_EXTRA_TARGETS	+= static_library_clean
		distclean.depends	+= $$static_library_clean.target

		#	static_library alias for staticlib.
		static_library.depends	+=	staticlib
		#	static_library_install alias for install_static_lib
		static_library_install.depends	+=	install_static_lib

		#	bin_install includes static_library_install.
		#	Can't added to install_target depends; use bin_install instead.
		bin_install.depends				+=	$$static_library_install.target

		plugin {
			#	The static library target and rules must be added for a plugin.
			staticlib.target	=	staticlib
			staticlib.depends	=	$(OBJECTS)
			staticlib.commands	= \
				-$(DEL_FILE) $$STATIC_LIBRARY$$escape_expand(\\n\\t) \
				$(AR) $$STATIC_LIBRARY $(OBJECTS)
			QMAKE_EXTRA_TARGETS	+=	staticlib
			}
		}
	else {
		static_library_install.commands	=	@echo "No static library"
		}
	}
QMAKE_EXTRA_TARGETS	+=	static_library
QMAKE_EXTRA_TARGETS	+=	static_library_install
QMAKE_EXTRA_TARGETS	+=	bin_install

plugin {
	#	HACK!!!
	#	Workaround for a qmake bug:
	#		When generating Makefiles for custom plugins
	#		distclean does not get {moc,obj}_clean dependencies
	#		and will not accept disclean.depends additions.
	#		This hack provides another distclean target that
	#		does accept the distclean.depends additions.
	#	Sigh
	really_clean.target	=	distclean
	really_clean.depends =	obj_clean moc_clean
	QMAKE_EXTRA_TARGETS	+=	$$really_clean.target
	}


#		Header files:

#	MODULE_NAME is required for pathname to doc files location.
isEmpty (MODULE_NAME): MODULE_NAME = $$basename(_PRO_FILE_PWD_)

isEmpty (HEADERS) {
	warning ()
	warning (No HEADERS!?!)
	error (HEADERS must be specified in $$_PRO_FILE_ before lib.pri is included)
	}

headers.path		=	$$HEADERS_INSTALL_DIR/include/$$MODULE_NAME
headers.files		=	$$HEADERS
INSTALLS			+=	headers
! build_pass:message (HEADERS_INSTALL_DIR: $$HEADERS_INSTALL_DIR ($$headers.path))

#	headers_install install header files w/o building library.
headers_install.target	=	headers_install
headers_install.commands	= \
	@$(CHK_DIR_EXISTS) $$headers.path || \
		$(MKDIR) $$headers.path$$escape_expand(\\n\\t) \
	$(COPY_FILE) $$HEADERS $$headers.path
QMAKE_EXTRA_TARGETS		+=	headers_install
