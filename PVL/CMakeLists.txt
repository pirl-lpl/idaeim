# the last three in this list are because

set(srcPVL
   Lister.cc
   PVL_Exceptions.cc
   Parameter.cc
   Parser.cc
   Value.cc
   Vectal.cc
)

# MSVC needs the definition of the static symbols
# to link (EXPORT_ALL_SYMBOLS is only classes and methods)
# via the IMPLIB, which is what cmake uses to link
# target dependencies
  if (WIN32)
	list(APPEND srcPVL
   ../Strings/Sliding_String.cc
   ../Strings/String.cc
   ../Utility/Exception.cc)
endif()

add_library(objPVL OBJECT ${srcPVL})

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
else()
	set_target_properties(sllPVL PROPERTIES OUTPUT_NAME PVL)
endif()

target_include_directories(PVL PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_SOURCE_DIR})
target_link_libraries(PVL Utility Strings)
