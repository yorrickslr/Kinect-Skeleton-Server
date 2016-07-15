// exclude some windows resources
// otherwise Kinect and ZMQ won't work together
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <Kinect.h>
#include <string>
#include <iostream>
#include <fstream>
#include "server.hpp"
#include "bbox.hpp"
#include <exception>

template<class Interface>
inline void SafeRelease(Interface *& pInterfaceToRelease)
{
	if (pInterfaceToRelease != NULL)
	{
		pInterfaceToRelease->Release();
		pInterfaceToRelease = NULL;
	}
}

void error(IKinectSensor* sensor, std::string message) {
	std::cerr << "ERROR: " << message << std::endl;
	sensor->Close();
	SafeRelease(sensor);
	exit(0);
}

int main(int argc, char** argv) {
	// init zmq server

	Server server("tcp://141.54.147.35:7700");

	// set bounding box
	if (argc > 5) {
		try {
			server.bbox.set(std::stof(argv[1]), std::stof(argv[3]));
			server.bbox.set(std::stof(argv[2]), std::stof(argv[4]));
		} catch(std::exception e) {
			std::cerr << "ERROR: " << e.what() << std::endl;
			return 1;
		}
	}

	// main program
	HRESULT hr;
	std::cout << "starting Kinect..." << std::endl;
	IKinectSensor* sensor = NULL;
	hr = GetDefaultKinectSensor(&sensor);
	if (FAILED(hr))
		error(sensor, "cannot find default kinect");

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
			if(server.send(body))
				break;
		}
		SafeRelease(frame);
	}
}
