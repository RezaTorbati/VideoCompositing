cmake_minimum_required(VERSION 2.8)

project(cppDemo)

find_package(OpenCV REQUIRED)

add_executable(${PROJECT_NAME} "cppDemo.cpp")

target_link_libraries(${PROJECT_NAME} ${OpenCV_LIBS})
