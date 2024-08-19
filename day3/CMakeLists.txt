
cmake_minimum_required(VERSION 3.10)
project(day3)   

add_executable(${PROJECT_NAME} main.c)

target_link_libraries(${PROJECT_NAME} iio)

# mkdir build
# cd build
# cmake ..
# make