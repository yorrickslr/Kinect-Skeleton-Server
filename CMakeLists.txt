cmake_minimum_required (VERSION 2.6)
project(ZMQSkeletonServer)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")

include_directories("C:/Users/cicu7215/documents/zmq/libzmq/include")
include_directories("C:/Program Files/Microsoft SDKs/Kinect/v2.0_1409/inc")

link_directories("C:/Program Files/Microsoft SDKs/Kinect/v2.0_1409/Lib/x64")
link_directories("C:/Users/cicu7215/Documents/zmq/libzmq/bin/x64/Release/v140/dynamic")

add_executable(SkelServer
  main.cpp
  server.hpp
)

target_link_libraries(SkelServer "Kinect20.lib")
target_link_libraries(SkelServer "libzmq.lib")
