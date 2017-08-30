PROJECT(SmartProxy)
cmake_minimum_required(VERSION 3.8)
aux_source_directory(./src DIR_SRCS)
SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_ROOT}/bin)
include_directories(${PROJECT_ROOT}/3rdparty/crow/include
			${PROJECT_ROOT}/3rdparty/cpp_redis/include
			${PROJECT_ROOT}/3rdparty/rapidjson/include
			${BOOST_INCLUDEDIR})
link_directories(/home/michael/work/3rdparty/cpp_redis/lib)

find_package(Boost REQUIRED COMPONENTS
		filesystem
		regex
		system
		thread
)
add_executable(SmartProxy ${DIR_SRCS})
target_link_libraries(SmartProxy ${Boost_LIBRARIES} cpp_redis tacopie pthread)

