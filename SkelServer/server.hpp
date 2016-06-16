// zmq server for kinect skeleton server

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
		size_t len = 16 * sizeof(float);
		std::array<float, 16> matrix;
		matrix.fill(0);
		// initialize identity matrix
		matrix[0] = 1;
		matrix[5] = 1;
		matrix[10] = 1;
		matrix[15] = 1;
		// set transformation values
		matrix[12] = joints[11].Position.X;
		matrix[13] = joints[11].Position.Y;
		matrix[14] = joints[11].Position.Z;
		zmq::message_t message(len);
		memcpy(message.data(), matrix.data(), len);
		m_socket.send(message);
	}
	zmq::context_t m_ctx;
	zmq::socket_t m_socket;
};

#endif
