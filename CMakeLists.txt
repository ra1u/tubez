
cmake_minimum_required (VERSION 2.6)

project (tubez)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(tubez INTERFACE)
target_include_directories(tubez INTERFACE src)

#todo optionally include this
add_subdirectory(example)

