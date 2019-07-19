
add_library(Strings SHARED Line_Count_Filter.cc Sliding_String.cc String.cc VMS_Records_Filter.cc)
target_include_directories(Strings PRIVATE "..")
