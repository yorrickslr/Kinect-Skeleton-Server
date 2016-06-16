// main source for kinect skeleton server
// see README.md for more information

// exclude some windows resources
// otherwise Kinect and ZMQ won't work together
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <Kinect.h>
#include <string>
#include <iostream>
#include "server.hpp"

// release interfaces safely
template<class Interface>
inline void SafeRelease(Interface *& interface)
{
	if (interface != NULL)
	{
		interface->Release();
		interface = NULL;
	}
}

// write error message and safely close kinect sensor
void error(IKinectSensor* sensor, std::string message) {
	std::cerr << "ERROR: " << message << std::endl;
	sensor->Close();
	SafeRelease(sensor);
	exit(0);
}

int main() {
	//initialize the kinect sensor and framereader
	HRESULT hr;
	std::cout << "starting Kinect..." << std::endl;
	IKinectSensor* sensor = NULL;
	hr = GetDefaultKinectSensor(&sensor);
	if (FAILED(hr))
		std::cerr << "ERROR: cannot find default Kinect!" << std::endl;

	hr = sensor->Open();
	if (SUCCEEDED(hr))
		std::cout << "Kinect is online!" << std::endl;
	else
		error(sensor, "Kinect is offline");

	IBodyFrameSource* source = NULL;
	hr = sensor->get_BodyFrameSource(&source);
	if (FAILED(hr))
		error(sensor, "cannot get frame source");

	IBodyFrameReader* reader = NULL;
	hr = source->OpenReader(&reader);
	if (FAILED(hr))
		error(sensor, "cannot open frame reader");

	source->Release();
	source = NULL;

	// start zmq server
	Server server("tcp://141.54.147.35:7700");

	while (true) {
		IBodyFrame* frame = NULL;
		hr = reader->AcquireLatestFrame(&frame);
		if (FAILED(hr)) {
			SafeRelease(frame);
			continue;
		}

		IBody* bodies[BODY_COUNT] = { 0 };
		hr = frame->GetAndRefreshBodyData(BODY_COUNT, bodies);
		if (FAILED(hr))
			error(sensor, "cannot get body data");

		for (IBody* body : bodies) {
			BOOLEAN tracked;
			body->get_IsTracked(&tracked);
			if (!tracked)
				continue;
			Joint joints[JointType_Count];
			body->GetJoints(JointType_Count, joints);
			server.send(joints);
			break;
		}
		SafeRelease(frame);
	}
}
