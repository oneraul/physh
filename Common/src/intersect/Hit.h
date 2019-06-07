#pragma once

namespace rmkl {

	class Aabb;

	struct Hit
	{
		bool Intersects;
		glm::vec2 Pos;
		glm::vec2 Delta;
		glm::vec2 Normal;
		float Time;
		Aabb* Collider;

		Hit() : Intersects(false), Pos{0, 0}, Delta{0, 0}, Normal{0, 0}, Time(0), Collider(nullptr) {}
	};

	struct Sweep
	{
		Hit Hit;
		glm::vec2 Pos;
		float Time;

		Sweep() : Hit(), Pos{0, 0}, Time(0) {}
	};
}