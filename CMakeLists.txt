cmake_minimum_required(VERSION 2.8)
project(cxx_pva_interface)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

#set(CMAKE_PREFIX_PATH  ${PROJECT_SOURCE_DIR}/3rdparty/opencv3.3.0) #版本冲突可能导致PVADetector类detect函数中resize报错
set(CMAKE_PREFIX_PATH  /home/cuizhou/Downloads/opencv-3.3.0/build_cuda) #版本冲突可能导致PVADetector类detect函数中resize报错
find_package(OpenCV 3.3.0 REQUIRED)
find_package(Boost REQUIRED)

include_directories(
        ${PROJECT_SOURCE_DIR}/include
        ${PROJECT_SOURCE_DIR}/include/PvaDetector
        ${PROJECT_SOURCE_DIR}/include/KCF
        ${PROJECT_SOURCE_DIR}/include/HungarianAlg
        ${PROJECT_SOURCE_DIR}/include/utils)
include_directories(
        ${PROJECT_SOURCE_DIR}/3rdparty/caffe/include
        ${OpenCV_INCLUDE_DIR}
        /usr/local/cuda-8.0/include)
link_directories(${PROJECT_SOURCE_DIR}/3rdparty/caffe/lib)

add_subdirectory(src)
add_subdirectory(executable)