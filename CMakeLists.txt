cmake_minimum_required(VERSION 3.13)

project(idaeim VERSION 2.3.4 DESCRIPTION "C++ PVL Parser Library")

# this is so sources in each subdir can refer to
# headers in other subdirs relative to this dir
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory(Utility)
add_subdirectory(Strings)
add_subdirectory(PVL)

set(CMAKE_EXPORT_PACKAGE_REGISTRY ON)
export(PACKAGE ${PROJECT_NAME})

include(CMakePackageConfigHelpers)

write_basic_package_version_file("${PROJECT_NAME}-config-version.cmake" VERSION ${PROJECT_VERSION}
       COMPATIBILITY SameMajorVersion)

configure_package_config_file("${CMAKE_CURRENT_SOURCE_DIR}/Config.cmake.in"
  "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-config.cmake"
  INSTALL_DESTINATION "${CMAKE_INSTALL_DATADIR}/${PROJECT_NAME}/cmake")

include(GNUInstallDirs)

install(EXPORT ${PROJECT_NAME}-export FILE "${PROJECT_NAME}-exports.cmake"
        NAMESPACE ${PROJECT_NAME}:: DESTINATION "${CMAKE_INSTALL_DATADIR}/${PROJECT_NAME}/cmake")

# install config files
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-config-version.cmake"
              "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-config.cmake"
        DESTINATION "${CMAKE_INSTALL_DATADIR}/${PROJECT_NAME}/cmake")
