add_library(Strings Line_Count_Filter.cc Sliding_String.cc String.cc VMS_Records_Filter.cc)
set_target_properties(Strings PROPERTIES WINDOWS_EXPORT_ALL_SYMBOLS ON POSITION_INDEPENDENT_CODE ON)
target_include_directories(Strings PUBLIC ${PROJECT_SOURCE_DIR} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
