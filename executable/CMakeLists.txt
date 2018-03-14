include_directories(/usr/include/freetype2)
add_executable(pvademo pvademo.cpp ../src/OpenCVputTextChinese/OpencvText.cpp)
target_link_libraries(pvademo myfUtils ${OpenCV_LIBS} freetype pva tools caffe glog boost_system )


