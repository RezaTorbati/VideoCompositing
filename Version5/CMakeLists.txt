cmake_minimum_required(VERSION 2.8)

project(videoCompositer)

find_package(OpenCV REQUIRED)

include_directories("${PROJECT_SOURCE_DIR}")

set(HEADER_FILES dewarpVideo.cpp dewarpVideo.h)

add_executable(${PROJECT_NAME} videoCompositer.cpp ${HEADER_FILES})
add_executable(dewarper correctDistortion.cpp ${HEADER_FILES})

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -pthread -Wl,--no-as-needed")
target_link_libraries(${PROJECT_NAME} ${OpenCV_LIBS})
target_link_libraries(dewarper ${OpenCV_LIBS})
