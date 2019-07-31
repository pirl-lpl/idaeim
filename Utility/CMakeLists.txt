
add_library(Utility Checker.cc Exception.cc Internet_Checksum.cc)
set_property(TARGET Utility PROPERTY POSITION_INDEPENDENT_CODE ON)
