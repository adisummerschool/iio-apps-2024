cmake_minimum_required(VERSION 3.10)
project(myproj)

add_executable(${PROJECT_NAME} read_voltage.c)
target_link_libraries(${PROJECT_NAME} iio)
