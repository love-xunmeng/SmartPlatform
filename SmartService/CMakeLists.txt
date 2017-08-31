PROJECT(SmartService)
cmake_minimum_required(VERSION 3.8)
add_compile_options(-std=c++14)
aux_source_directory(./src DIR_SRCS)
SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_ROOT}/bin)
include_directories(
			${BOOST_INCLUDEDIR}
			${OPENCV_INCLUDE_DIR}
			${PROJECT_ROOT}/3rdparty-linux/image_stone
			${PROJECT_ROOT}/3rdparty-linux/rapidjson/include
			${PROJECT_ROOT}/3rdparty-linux/cpp_redis/include
			${PROJECT_ROOT}/3rdparty-linux/FreeImage/build/usr/include)
link_directories(${OPENCV_LIBRARY_DIR}
			${PROJECT_ROOT}/3rdparty-linux/cpp_redis/lib
			${PROJECT_ROOT}/3rdparty/FreeImage/build/usr/lib)
find_package(Boost REQUIRED COMPONENTS
		filesystem
		regex
		system
		thread
)
add_executable(SmartService ${DIR_SRCS})
target_link_libraries(SmartService ${Boost_LIBRARIES} ${OpenCV_LIBRARIES} cpp_redis tacopie opencv_core opencv_highgui opencv_imgproc opencv_imgcodecs freeimage pthread)
