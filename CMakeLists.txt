cmake_minimum_required(VERSION 3.11)

project(idaeim VERSION 2.3.4 DESCRIPTION "C++ PVL Parser Library")

# this is so sources in each subdir can refer to
# headers in other subdirs relative to this dir
include_directories(${CMAKE_SOURCE_DIR})

add_subdirectory(Utility)
add_subdirectory(Strings)
add_subdirectory(PVL)

include(CMakePackageConfigHelpers)
write_basic_package_version_file(${PROJECT_NAME}-config-version.cmake VERSION ${PACKAGE_VERSION}
   COMPATIBILITY SameMajorVersion
)

#install(EXPORT ${PROJECT_NAME}
#   FILE ${PROJECT_NAME}Targets.cmake
#   NAMESPACE ${PROJECT_NAME}::
#   DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}-${PACKAGE_VERSION})

set(CMAKE_EXPORT_PACKAGE_REGISTRY ON)
export(TARGETS PVL Strings Utility
	   NAMESPACE ${PROJECT_NAME}:: FILE ${PROJECT_NAME}-config.cmake)
export(PACKAGE ${PROJECT_NAME})


