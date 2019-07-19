add_library(PVL SHARED
   Lister.cc
   PVL_Exceptions.cc
   Parameter.cc
   Parser.cc
   Value.cc
   Vectal.cc
)
target_include_directories(PVL PRIVATE "..")
target_link_libraries(PVL Strings Utility)
