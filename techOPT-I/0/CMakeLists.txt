cmake_minimum_required(VERSION 3.16)

project(main)

add_executable(${PROJECT_NAME} 0.c)

find_package(PkgConfig REQUIRED)
pkg_check_modules(FUSE REQUIRED fuse3)
# find_package(FUSE REQUIRED)

link_libraries(${FUSE_LIBRARIES})         # -lfuse3 -lpthread
include_directories(${FUSE_INCLUDE_DIRS}) # -I/usr/include/fuse3
add_compile_options(${FUSE_CFLAGS_OTHER}) 