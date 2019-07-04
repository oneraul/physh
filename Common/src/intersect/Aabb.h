#pragma once

#include <glm/vec2.hpp>
#include <vector>

#include "Hit.h"

namespace rmkl {

	class Aabb 
	{
	public:
		glm::vec2 Pos;
		glm::vec2 Half;

	public:
		static Aabb CreateWithMaxMin(glm::vec2 min, glm::vec2 max);

	public:
		Aabb(glm::vec2 pos, glm::vec2 half);
		~Aabb();

		//Serialize();
		//Deserialize();
		//Draw();

		Hit IntersectPoint(glm::vec2 point);
		Hit IntersectSegment(glm::vec2 pos, glm::vec2 delta, float paddingX, float paddingY);
		Hit IntersectAabb(Aabb& box);
		Sweep SweepAabb(Aabb& box, glm::vec2 delta);
		Sweep SweepInto(std::vector<Aabb> staticColliders, glm::vec2 delta);
	};

}
