cmake_minimum_required(VERSION 3.16)

project(test_zip)

add_executable(test_zip "test.c")

find_library(libzip REQUIRED)
target_link_libraries(test_zip ${LIBZIP_LIB})