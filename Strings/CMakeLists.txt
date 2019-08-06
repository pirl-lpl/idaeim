add_library(Strings Line_Count_Filter.cc Sliding_String.cc String.cc VMS_Records_Filter.cc)
set_target_properties(Strings PROPERTIES POSITION_INDEPENDENT_CODE ON WINDOWS_EXPORT_ALL_SYMBOLS ON)
target_include_directories(Strings PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_SOURCE_DIR})
