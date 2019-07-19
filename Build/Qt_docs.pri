#	docs.pri
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
#	CVS ID: Qt_docs.pri,v 1.10 2013/10/17 07:33:18 castalia Exp.

isEmpty (DOCS_INSTALL_DIR) {
	isEmpty (INSTALL_DIR): DOCS_INSTALL_DIR = ..
	else:                  DOCS_INSTALL_DIR = $$INSTALL_DIR
	}
		

#	VERSION is required for Doxyfile.template sed to Doxyfile.
isEmpty (VERSION) {
	! include ($$PWD/Qt_versions.pri) {
		! build_pass:warning (-----------------------)
		! build_pass:warning (---> VERSION set to 0.0)
		! build_pass:warning (-----------------------)
		VERSION = 0.0
		}
	}
#	MODULE_NAME is required for pathname to doc files location.
isEmpty (MODULE_NAME): MODULE_NAME = $$basename(_PRO_FILE_PWD_)

#	Location of the doc files.
DOCS_HTML_DIR		=	docs/html/idaeim/$$MODULE_NAME
! exists ($$_PRO_FILE_PWD_/$$DOCS_HTML_DIR) {
	#	Make the doc files directory.
	#	Necessary to have the INSTALLS entry included.
	phony = $$system(mkdir -p $$_PRO_FILE_PWD_/$$DOCS_HTML_DIR)
	}


#	Generate VERSION specific Doxyfile from Doxyfile.template.
Doxyfile.target		=	Doxyfile
Doxyfile.depends	=	Doxyfile.template
Doxyfile.commands	= \
	@echo$$escape_expand(\\n\\t) \
	@echo "--- Creating Doxyfile"$$escape_expand(\\n\\t) \
	@$(DEL_FILE) Doxyfile$$escape_expand(\\n\\t) \
	sed \
		-e \'s/_VERSION_/$$VERSION/\' \
		Doxyfile.template > Doxyfile
QMAKE_EXTRA_TARGETS	+=	Doxyfile

#	Generate MODULE specific Doxyfile.template file.
! exists ($$PWD/Doxyfile.template) {
	error (The Doxyfile.template file is missing from the Build directory!)
	}
Doxyfile_template.target	=	Doxyfile.template
Doxyfile_template.commands	= \
	@echo$$escape_expand(\\n\\t) \
	@echo "--- Creating Doxyfile.template"$$escape_expand(\\n\\t) \
	sed \
		-e 's/_MODULE_/$$MODULE_NAME/g' \
		$$PWD/Doxyfile.template > Doxyfile.template
QMAKE_EXTRA_TARGETS	+=	Doxyfile_template

#	Template file for basic idaeim software description.
! exists ($$PWD/Overview.doxygen) {
	error (The Doxyfile.template file is missing from the Build directory!)
	}
Overview.target		=	Overview.doxygen
Overview.commands	= \
	@echo$$escape_expand(\\n\\t) \
	@echo "--- Creating Overview.doxygen"$$escape_expand(\\n\\t) \
	$(COPY_FILE) $$PWD/Overview.doxygen Overview.doxygen
QMAKE_EXTRA_TARGETS	+=	Overview

#	Generate docs files using doxygen.
doc_files.target	=	$$DOCS_HTML_DIR/index.html
doc_files.depends	=	$$Doxyfile.target $$Overview.target
doc_files.commands	= \
	@echo$$escape_expand(\\n\\t) \
	@echo "=== $$MODULE_NAME documentation"$$escape_expand(\\n\\t) \
	@$(CHK_DIR_EXISTS) $$DOCS_HTML_DIR || \
		$(MKDIR) $$DOCS_HTML_DIR$$escape_expand(\\n\\t) \
	doxygen Doxyfile
QMAKE_EXTRA_TARGETS	+=	doc_files

docs.target			=	docs
docs.depends		=	$$doc_files.target
QMAKE_EXTRA_TARGETS	+=	docs

#	Install docs files.
DOCS_PATH			=	$$DOCS_INSTALL_DIR/$$DOCS_HTML_DIR
docs.path			=	$$DOCS_PATH/..
docs.files			=	$$DOCS_HTML_DIR
INSTALLS			+=	docs
! build_pass:message (DOCS_INSTALL_DIR: $$DOCS_INSTALL_DIR ($$DOCS_PATH))

#	Docs install depends on doc files being generated.
install_docs.target		=	install_docs
install_docs.depends	+=	docs
QMAKE_EXTRA_TARGETS		+=	install_docs

#	docs_install installs docs files w/o building library.
docs_install.target		=	docs_install
docs_install.depends	=	$$docs.target
docs_install.commands	= \
	@$(CHK_DIR_EXISTS) $$DOCS_PATH || \
		$(MKDIR) $$DOCS_PATH$$escape_expand(\\n\\t) \
	$(COPY_DIR) $$DOCS_HTML_DIR $$DOCS_PATH/..
QMAKE_EXTRA_TARGETS		+=	docs_install

#	Clean out doc files.
docs_clean.target	=	docs_clean
docs_clean.commands	=	$(DEL_FILE) -r $$docs.target $$Doxyfile.target
QMAKE_EXTRA_TARGETS	+=	docs_clean
#	distclean dependent on docs_clean.
distclean.target	=	distclean
distclean.depends	+=	$$docs_clean.target
QMAKE_EXTRA_TARGETS	+=	distclean

