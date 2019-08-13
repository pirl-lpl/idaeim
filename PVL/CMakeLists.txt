# the last three in this list are because

add_library(objPVL OBJECT
   Lister.cc
   PVL_Exceptions.cc
   Parameter.cc
   Parser.cc
   Value.cc
   Vectal.cc
)

set_target_properties(objPVL PROPERTIES POSITION_INDEPENDENT_CODE ON)

add_library(PVL SHARED $<TARGET_OBJECTS:objPVL>)
add_library(sllPVL STATIC $<TARGET_OBJECTS:objPVL>)

if (WIN32)
	set_target_properties(PVL PROPERTIES WINDOWS_EXPORT_ALL_SYMBOLS ON)
	set_target_properties(sllPVL PROPERTIES OUTPUT_NAME libPVL)
	if(${CMAKE_VERSION} VERSION_GREATER_EQUAL "3.13")
		target_link_options(PVL PRIVATE "/MANIFEST:NO")
	else()
		set(CMAKE_SHARED_LINKER_FLAGS "/MANIFEST:NO")
	endif()
	#
	# TODO this hack copies the static lib over the import lib
	# because the latter doesn't have symbols for static data fields
	add_custom_command(TARGET PVL POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:sllPVL> PVL.lib)
else()
	set_target_properties(sllPVL PROPERTIES OUTPUT_NAME PVL)
endif()

target_include_directories(PVL PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_SOURCE_DIR})
target_link_libraries(PVL Utility Strings)
