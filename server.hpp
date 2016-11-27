#pragma once

#ifndef VRSYS_SKELETON_SERVER
#define VRSYS_SKELETON_SERVER

#include <zmq.hpp>
#include <array>
#include "message.hpp"
#include "bbox.hpp"

struct Server {
	Server(std::string address)
	  : m_ctx(1)
	  , m_socket(m_ctx, ZMQ_PUB)
	{
		uint32_t hwm = 1;
		m_socket.setsockopt(ZMQ_SNDHWM, &hwm, sizeof(hwm));
		try {
			m_socket.bind(address.c_str());
		}
		catch (std::exception e) {
			throw std::logic_error("cannot bind server to address");
		}
	}
	bool send(IBody* body) { //Joint* joints) {
		Joint joints[JointType_Count];
		body->GetJoints(JointType_Count, joints);
		std::array<Message, 25> skeleton;
		if (!bbox.isInside(&joints[0].Position))
			return 0;
		for (int i = 0; i < 25; i++) {
			skeleton[i] = Message{ short(i), &joints[i] };
		}
		HandState state;
		body->get_HandLeftState(&state);
		skeleton[7].grab =  state == 3 ? 1 : 0;
		body->get_HandRightState(&state);
		skeleton[11].grab = state == 3 ? 1 : 0;
		
		size_t len = 25 * sizeof(Message);

		zmq::message_t message(len);
		memcpy(message.data(), skeleton.data(), len);
		m_socket.send(message);
		std::cout << ".";
		return 1;
	}
	zmq::context_t m_ctx;
	zmq::socket_t m_socket;
	Bbox bbox;
};

#endif
