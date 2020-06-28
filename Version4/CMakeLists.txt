cmake_minimum_required(VERSION 2.8)

project(videoCompositer)

find_package(OpenCV REQUIRED)

add_executable(${PROJECT_NAME} videoCompositer.cpp)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -pthread -Wl,--no-as-needed")
target_link_libraries(${PROJECT_NAME} ${OpenCV_LIBS})
