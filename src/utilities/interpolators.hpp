#pragma once

#include "glm.hpp"
#include <functional>
#include <array>

//interpolator arguments follows the standard: x + y * width + z * width*width 
//where width = 2. 

template <typename Type>
class Interpolator
{
    public:
        static Type nearestNeigbor(const std::array<Type, 8>& corners, const glm::vec3& point);
        static Type trilinear(const std::array<Type, 8>& corners, const glm::vec3& point);
};

template <typename Type>
Type Interpolator<Type>::nearestNeigbor(const std::array<Type, 8>& corners, const glm::vec3& point)
{
    //std::cout << "now doing nearest neigh\n";

    //std::cout << "point is at " << glm::to_string(point) << "\n";

    //std::cout << "000: " << corners[0] << "\n";
    //std::cout << "100: " << corners[1] << "\n";
    //std::cout << "010: " << corners[2] << "\n";
    //std::cout << "110: " << corners[3] << "\n";
    //std::cout << "001: " << corners[4] << "\n";
    //std::cout << "101: " << corners[5] << "\n";
    //std::cout << "011: " << corners[6] << "\n";
    //std::cout << "111: " << corners[7] << "\n";

    uint32_t index = (point.x < 0.4999f ? 0 : 1) +
                     (point.y < 0.4999f ? 0 : 2) +
                     (point.z < 0.4999f ? 0 : 4);

    return corners[index];
}

template <typename Type>
Type Interpolator<Type>::trilinear(const std::array<Type, 8>& corners, const glm::vec3& point)
{
	float c00 = corners[0] * (1.0f - point.x) + corners[1] * point.x;
	float c10 = corners[2] * (1.0f - point.x) + corners[3] * point.x;
	float c01 = corners[4] * (1.0f - point.x) + corners[5] * point.x;
	float c11 = corners[6] * (1.0f - point.x) + corners[7] * point.x;

	float c0 = c00 * (1.0f - point.y) + c10 * point.y;
	float c1 = c01 * (1.0f - point.y) + c11 * point.y;
	
	float c = c0 * (1.0f - point.z) + c1 * point.z;
	
	return c;
}
