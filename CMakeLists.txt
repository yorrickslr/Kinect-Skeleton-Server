cmake_minimum_required (VERSION 2.6)
project(ZMQSkeletonServer)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/externals/libzmq/libzmq/include")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/externals/kinect/v2.0_1409/inc")

link_directories("${CMAKE_CURRENT_SOURCE_DIR}/externals/kinect/v2.0_1409/Lib/x64")
link_directories("${CMAKE_CURRENT_SOURCE_DIR}/externals/libzmq/bin/x64/Release/v140/dynamic")

add_executable(SkelServer
  main.cpp
  server.hpp
  message.hpp
  bbox.hpp
)

target_link_libraries(SkelServer "Kinect20.lib")
target_link_libraries(SkelServer "libzmq.lib")
