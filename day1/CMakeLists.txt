cmake_minimum_required(VERSION 3.10)
project(myproj)

add_executable(${PROJECT_NAME} main.c)
add_library(mylib lib.h lib.c)


target_link_libraries(${PROJECT_NAME} iio)
target_link_libraries(${PROJECT_NAME} mylib)
