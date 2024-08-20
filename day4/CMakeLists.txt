# CMakeList.txt

cmake_minimum_required(VERSION 3.10)
project(shock_buffer)
#project(read_voltage)

#add_executable(${PROJECT_NAME} main.c)
#add_executable(${PROJECT_NAME} read_voltage.c)
add_executable(${PROJECT_NAME} shock_buffer.c)
target_link_libraries(${PROJECT_NAME} iio)


# mkdir build
# cd build
# cmake ..
# make 