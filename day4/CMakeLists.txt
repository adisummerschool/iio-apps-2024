cmake_minimum_required(VERSION 3.10)
project(myproj)   

add_executable(${PROJECT_NAME} shock_buffer.c)

target_link_libraries(${PROJECT_NAME} iio)

# mkdir build
# cd build
# cmake ..
# make