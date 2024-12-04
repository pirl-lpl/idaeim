add_library(objUtility OBJECT
Checker.cc Exception.cc Internet_Checksum.cc)

set(headers Types.hh STRINGIFY.hh
Checker.hh Exception.hh Internet_Checksum.hh
)

set_target_properties(objUtility PROPERTIES POSITION_INDEPENDENT_CODE ON)

add_library(Utility SHARED $<TARGET_OBJECTS:objUtility>)
add_library(Utility_static STATIC $<TARGET_OBJECTS:objUtility>)

if (WIN32)
    set_target_properties(Utility PROPERTIES WINDOWS_EXPORT_ALL_SYMBOLS ON)
    target_link_options(Utility PRIVATE "/MANIFEST:NO")
    #
    # TODO this hack copies the static lib over the import lib
    # because the latter doesn't have symbols for static data fields
    #add_custom_command(TARGET Utility POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:sllUtility> Utility.lib)
endif()

#target_include_directories(Utility INTERFACE
# $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
# $<INSTALL_INTERFACE:include>)

set_target_properties(Utility PROPERTIES PUBLIC_HEADER "${headers}")
set_target_properties(Utility_static PROPERTIES PUBLIC_HEADER "${headers}")

export(TARGETS Utility Utility_static NAMESPACE idaeim:: FILE "Utility-config.cmake")

include(GNUInstallDirs)
install(TARGETS Utility Utility_static
    EXPORT ${PROJECT_NAME}-export
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/Utility
)
