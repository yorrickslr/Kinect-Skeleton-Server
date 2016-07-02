#pragma once

#ifndef VRSYS_SKELETON_SERVER
#define VRSYS_SKELETON_SERVER

#include <zmq.hpp>
#include <array>

struct Server {
	Server(std::string address)
	  : m_ctx(1)
	  , m_socket(m_ctx, ZMQ_PUB)
	{
		uint32_t hwm = 1;
		m_socket.setsockopt(ZMQ_SNDHWM, &hwm, sizeof(hwm));

		m_socket.bind(address.c_str());
	}
	void send(Joint* joints) {
		// size of 24 4x4-matrices
		//size_t len = 384 * sizeof(float);
		size_t len = 16 * sizeof(float);
		std::array<float, 16> matrix;
		matrix.fill(0);
		//for (int i = 0; i < 24; i++) {
			float temp[16] = { 0 };
			// initialize identity matrix
			temp[0] = 1;
			temp[5] = 1;
			temp[10] = 1;
			temp[15] = 1;
			// set transformation
			//temp[3] = joints[11].Position.X;
			//temp[7] = joints[11].Position.Y;
			//temp[11] = joints[11].Position.Z;
			temp[12] = joints[11].Position.X;
			temp[13] = joints[11].Position.Y;
			temp[14] = joints[11].Position.Z;
			for (int k = 0; k < 16; k++) {
				matrix[k] = temp[k];
			}
		//}
		zmq::message_t message(len);
		memcpy(message.data(), matrix.data(), len);
		m_socket.send(message);
		std::cout << ".";
	}
	zmq::context_t m_ctx;
	zmq::socket_t m_socket;
};

#endif
