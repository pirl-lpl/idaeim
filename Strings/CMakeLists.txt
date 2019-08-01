
add_library(Strings Line_Count_Filter.cc Sliding_String.cc String.cc VMS_Records_Filter.cc)
set_property(TARGET Strings PROPERTY POSITION_INDEPENDENT_CODE ON)
target_include_directories(Strings PUBLIC ${PROJECT_SOURCE_DIR})
