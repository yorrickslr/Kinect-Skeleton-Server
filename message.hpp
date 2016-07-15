#pragma once

#ifndef VRSYS_SKELETON_BUNDLE
#define VRSYS_SKELETON_BUNDLE

#include <array>

struct Message {
	Message()
		:id{ -1 }
	{
		matrix.fill(0);
		// initialize identity matrix
		matrix[0] = 1;
		matrix[5] = 1;
		matrix[10] = 1;
		matrix[15] = 1;
	}

	Message(short in_id, Joint* joint)
		: id{ in_id }
	{
		matrix.fill(0);
		// initialize identity matrix
		matrix[0] = 1;
		matrix[5] = 1;
		matrix[10] = 1;
		matrix[15] = 1;
		// set coordinates
		matrix[12] = joint->Position.X;
		matrix[13] = joint->Position.Y;
		matrix[14] = joint->Position.Z;
		status = joint->TrackingState > 1 ? 1 : 0;
	}

	void setTransform(float x, float y, float z) {
		matrix[12] = x;
		matrix[13] = y;
		matrix[14] = z;
	}

	short id;
	std::array<float, 16> matrix;
	bool status = 0; //dummy
	bool grab = 0;
};

#endif
