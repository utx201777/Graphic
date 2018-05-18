#pragma once
#include <cmath>
#include <glm\glm.hpp>
#include <iostream>

namespace Geometry
{
	// 点是否在直线上
	bool inSameLine(glm::vec3 p1, glm::vec3 p2, glm::vec3 p);

	// 点是否在线段上
	bool inSameSegment(glm::vec3 p1, glm::vec3 p2, glm::vec3 p);

	// 点是否在三角形内
	bool inSameTriangle(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, glm::vec3 p);

	// 点到直线的距离
	float distance2Line(glm::vec3 p1, glm::vec3 p2, glm::vec3 p);

	// 点到线段的距离
	float distance2Segment(glm::vec3 p1, glm::vec3 p2, glm::vec3 p);

	// 点到三角形的距离
	float distance2Triangle(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, glm::vec3 p);

	// 是否构成三角形
	bool isTriangle(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3);
}