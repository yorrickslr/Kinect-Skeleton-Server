#pragma once

#ifndef VRSYS_SKELETON_BBOX
#define VRSYS_SKELETON_BBOX

struct Bbox {
	void set(float x, float z) {
		minX = x < minX ? x : minX;
		minZ = z < minZ ? z : minZ;
		maxX = x > maxX ? x : maxX;
		maxZ = z > maxZ ? z : maxZ;
		isset = true;
	}
	bool checkX(float x) {
		return x > minX && x < maxX;
	}
	bool checkZ(float z) {
		return z > minZ && z < minZ;
	}
	bool isInside(CameraSpacePoint* pos) {
		/*
		std::cout << "x: " << minX << " & " << maxX << std::endl;
		std::cout << "z: " << minZ << " & " << maxZ << std::endl;
		std::cout << "   " << x << " & " << z << std::endl;
		*/
		if (!isset) {
			return 1;
		}
		float x = pos->X;
		float z = pos->Z;
		if (x < minX || x > maxX) {
			return 0;
		}
		if (z < minZ || z > maxZ) {
			return 0;
		}
		return 1;
	}
	float minX = INFINITY;
	float minZ = INFINITY;
	float maxX = -INFINITY;
	float maxZ = -INFINITY;
	bool isset = false;
};

#endif