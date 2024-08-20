cmake_minimum_required(VERSION 3.10)
project(readB)

add_executable(${PROJECT_NAME} read_buffer.c)
target_link_libraries(${PROJECT_NAME} iio)
