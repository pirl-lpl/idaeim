add_library(PVL SHARED
   Lister.cc
   PVL_Exceptions.cc
   Parameter.cc
   Parser.cc
   Value.cc
   Vectal.cc
)
set_property(TARGET PVL PROPERTY POSITION_INDEPENDENT_CODE ON)
target_include_directories(PVL PUBLIC ${PROJECT_SOURCE_DIR})
target_link_libraries(PVL Strings Utility)
