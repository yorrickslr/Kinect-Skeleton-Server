// exclude some windows resources
// otherwise Kinect and ZMQ won't work together
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <Kinect.h>
#include <string>
#include <iostream>
#include <fstream>
#include <algorithm>
#include "server.hpp"
#include "bbox.hpp"
#include <exception>
#include <ws2tcpip.h>

template<class Interface>
inline void SafeRelease(Interface* pInterfaceToRelease)
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
	std::vector<std::string> args;
	for (int i = 1; i < argc; i++) {
		args.push_back(argv[i]);
	}

	std::string arg_address;
	try {
		arg_address = args.at(0);
	}
	catch (std::exception e) {
		std::cerr << "ERROR: cannot find address in arguments!" << std::endl;
		std::cerr << "       try --help or -h for usage!" << std::endl;
		return 1;
	}

	auto iter = std::find_if(args.begin(), args.end(), [](auto arg) {return arg == "--help" || arg == "-h"; });
	bool arg_help_flag = iter != args.end() ? true : false;

	iter = std::find_if(args.begin(), args.end(), [](auto arg) {return arg == "--bbox" || arg == "-b"; });
	bool arg_bbox_flag = iter != args.end() ? true : false;
	std::vector<float> arg_bbox;
	if (arg_bbox_flag) {
		std::cout << "INFORMATION: reading bbox" << std::endl;
		while(++iter != args.end()) {
			arg_bbox.push_back(std::stof(*iter));
		}
		if (arg_bbox.size() != 4) {
			std::cerr << "ERROR: cannot parse bounding box arguments!" << std::endl;
			std::cerr << "       try --help or -h for usage!" << std::endl;
			return 1;
		}
	}

	if (arg_help_flag) {
		std::cout << "usage: " << argv[0] << " <ip:port> [-b|--bbox <x> <z> <toX> <toZ>]" << std::endl;
		std::cout << std::endl;
		std::cout << "examples:" << std::endl;
		std::cout << "         " << argv[0] << " 127.0.0.1:7700" << std::endl;
		std::cout << "         " << argv[0] << " 127.0.0.1:7700 -b -1 -2 1 2" << std::endl;
		std::cout << "         " << argv[0] << " yourpc:7700 -bbox -1.2 -3.4 5.6 7.8" << std::endl;
		return 0;
	}

	try {
		std::cout << "INFORMATION: binding server to " << arg_address << std::endl;
		Server server("tcp://" + arg_address);

		// set bounding box
		if (arg_bbox_flag) {
			std::cout << "INFORMATION: setting bbox" << std::endl;
			server.bbox.set(arg_bbox.at(0), arg_bbox.at(1));
			server.bbox.set(arg_bbox.at(2), arg_bbox.at(3));
		}

		// main program
		HRESULT hr;
		std::cout << "INFORMATION: starting Kinect..." << std::endl;
		IKinectSensor* sensor = NULL;
		hr = GetDefaultKinectSensor(&sensor);
		if (FAILED(hr))
			error(sensor, "cannot find default kinect");

		hr = sensor->Open();
		if (SUCCEEDED(hr))
			std::cout << "INFORMATION: Kinect is online!" << std::endl;
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
				if (server.send(body))
					break;
			}
			SafeRelease(frame);
		}
	}
	catch (std::exception e) {
		std::cerr << "ERROR: " << e.what() << std::endl;
		return 1;
	}
}
