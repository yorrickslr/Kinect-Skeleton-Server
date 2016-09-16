cmake_minimum_required (VERSION 2.6)
project(ZMQSkeletonServer)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/externals/kinect/v2.0_1409/inc")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/externals/libzmq/include")

link_directories("${CMAKE_CURRENT_SOURCE_DIR}/externals/kinect/v2.0_1409/Lib/x64")
link_directories("${CMAKE_CURRENT_SOURCE_DIR}/externals/libzmq/bin/x64/Release/v140/dynamic")

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

add_executable(SkelServer
  main.cpp
  server.hpp
  message.hpp
  bbox.hpp
)

target_link_libraries(SkelServer "Kinect20.lib")
target_link_libraries(SkelServer "libzmq.lib")

add_custom_target(ZMQCopyBinaries
	COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/externals/libzmq/bin/x64/Release/v140/dynamic/libzmq.dll ${CMAKE_BINARY_DIR}/Release
)
add_dependencies(SkelServer ZMQCopyBinaries)