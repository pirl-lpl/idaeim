
add_library(Utility Checker.cc Exception.cc Internet_Checksum.cc)
set_target_properties(Utility PROPERTIES WINDOWS_EXPORT_ALL_SYMBOLS ON POSITION_INDEPENDENT_CODE ON)

