cmake_minimum_required(VERSION 3.10)
project(myproj)

add_executable(${PROJECT_NAME} main.c)
#add_library(mylib SHARED lib.c lib.h)

target_link_libraries(${PROJECT_NAME} iio)

