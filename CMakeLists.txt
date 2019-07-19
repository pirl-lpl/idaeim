cmake_minimum_required(VERSION 3.11)

project(idaeim VERSION 2.3.4 DESCRIPTION "C++ PVL Parser Library")

add_subdirectory(Strings)
add_subdirectory(Utility)
add_subdirectory(PVL)


# allows dependent projects to use our headers
# by direct include from this directory
# TODO should this be a target property rather than a project global?
set(CMAKE_INCLUDE_CURRENT_DIR_IN_INTERFACE ON)

include(CMakePackageConfigHelpers)
write_basic_package_version_file(${PROJECT_NAME}-config-version.cmake VERSION ${PACKAGE_VERSION}
   COMPATIBILITY SameMajorVersion
)

export(TARGETS PVL Strings Utility NAMESPACE ${PROJECT_NAME}:: FILE ${PROJECT_NAME}-config.cmake)
export(PACKAGE ${PROJECT_NAME})