# Kinect-Skeleton-Server
server for sending kinect skeleton data to avango daemon

---
Build (Visual Studio 2015 only)
---
you will need:

 - ZMQ4 include files for C++ `zmq.hpp`
 - ZMQ4 library files `libzmq.lib` `libzmq.dll`
 - Kinect SDK 2.0 `kinect20.dll` `Kinect.h`

Run `cmake -G "Visual Studio 14 2015 Win64"` to create the Visual Studio project.

Run `msbuild -p:Configuration=Release` to build the program or open the solution file.

In order ro run the application, please copy `libzmq.dll` to the `Release` directory