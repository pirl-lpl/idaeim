add_library(PVL SHARED
   Lister.cc
   PVL_Exceptions.cc
   Parameter.cc
   Parser.cc
   Value.cc
   Vectal.cc
)

set_target_properties(PVL PROPERTIES WINDOWS_EXPORT_ALL_SYMBOLS ON POSITION_INDEPENDENT_CODE ON)
target_include_directories(Strings PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(PVL Strings Utility)
