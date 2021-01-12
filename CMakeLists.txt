cmake_minimum_required(VERSION 2.8)
find_package(OpenCV 3 REQUIRED)
#find_package(OpenCV 3 REQUIRED COMPONENTS core imgcodecs videoio)
# TransmitterReceiver Path Variables
# required files transmitter_receiver/src/transmitterReceiver.cpp,
#       transmitter_receiver/include/transmitterReceiver.h, and
#       transmitter_receiver/src/video_transmitter_receiver_test.cpp
set(TR_PROJ_DIR /home/pi/symbiotic2/projects/robotics/crawler/ros/ws/src/transmitter_receiver)
set(TR_PROJ_SRC ${TR_PROJ_DIR}/src)
set(TR_INCLUDE_DIRS ${TR_PROJ_DIR}/include ${OpenCV_INCLUDE_DIRS})
set(TR_PROJ_LIB ${TR_PROJ_SRC}/transmitterReceiver.cpp)
set(HEADER_FILES src/dewarpVideo.cpp src/dewarpVideo.h ${TR_INCLUDE_DIRS})
include_directories("${PROJECT_SOURCE_DIR}" ${TR_INCLUDE_DIRS})
# Declare TransmitterReceiver library
add_library(transmitter_receiver ${TR_INCLUDE_DIRS} ${TR_PROJ_LIB})

add_executable(videoCompositer src/videoCompositer.cpp ${HEADER_FILES} ${TR_INCLUDE_DIRS})
add_executable(dewarper src/correctDistortion.cpp ${HEADER_FILES})
add_executable(calibrate src/getCalibrationMats.cpp ${HEADER_FILES})
add_executable(compress src/compressRaw.cpp ${HEADER_FILES})

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -std=c++11 -pthread -Wl,--no-as-needed")
target_link_libraries(videoCompositer ${OpenCV_LIBS} transmitter_receiver rt)
target_link_libraries(dewarper ${OpenCV_LIBS})
target_link_libraries(calibrate ${OpenCV_LIBS})
target_link_libraries(compress ${OpenCV_LIBS})

# Declare TransmitterReceiver video test executable
#add_executable(video_tr_test ${TR_PROJ_SRC}/video_transmitter_receiver_test.cpp ${TR_INCLUDE_DIRS})
# Specify libraries to link a library or executable target against
#target_link_libraries(video_tr_test transmitter_receiver ${OpenCV_LIBS} rt)
