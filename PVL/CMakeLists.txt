# the last three in this list are because

add_library(objPVL OBJECT
   Lister.cc
   PVL_Exceptions.cc
   Parameter.cc
   Parser.cc
   Value.cc
   Vectal.cc
)

set(headers
   Lister.hh
   PVL_Exceptions.hh
   Parameter.hh
   Parser.hh
   Value.hh
   Vectal.hh
)

set_target_properties(objPVL PROPERTIES POSITION_INDEPENDENT_CODE ON)

add_library(PVL SHARED $<TARGET_OBJECTS:objPVL>)
add_library(PVL_static STATIC $<TARGET_OBJECTS:objPVL>)

set_target_properties(PVL PROPERTIES PUBLIC_HEADER "${headers}")
set_target_properties(PVL_static PROPERTIES PUBLIC_HEADER "${headers}")

if (WIN32)
	set_target_properties(PVL PROPERTIES WINDOWS_EXPORT_ALL_SYMBOLS ON)
	target_link_options(PVL PRIVATE "/MANIFEST:NO")

	#
	# TODO this hack copies the static lib over the import lib
	# because the latter doesn't have symbols for static data fields
	# add_custom_command(TARGET PVL POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:sllPVL> PVL.lib)
endif()

#target_include_directories(PVL PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_SOURCE_DIR})
target_link_libraries(PVL Utility Strings)

export(TARGETS PVL PVL_static NAMESPACE idaeim:: FILE "PVL-config.cmake")

include(GNUInstallDirs)
install(TARGETS PVL PVL_static
    EXPORT ${PROJECT_NAME}-export
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/PVL
)

install(FILES PVL.hh DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
