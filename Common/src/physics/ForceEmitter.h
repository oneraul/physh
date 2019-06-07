#pragma once

#include "Rigidbody.h"
#include "glm/vec4.hpp"

namespace rmkl {

	class ForceEmitter
	{
	public:

		// Shared code between server/client
		static void UpdateEmitters(Stage& stage);

	public:
		virtual bool Update(int tick) = 0; // returns true if it wants to se a new snapshot
		virtual void ApplyForce(Rigidbody& body) = 0;

		virtual Aabb GetDebugAabb(int tick) = 0;
		virtual glm::vec4 GetDebugColour(int tick) = 0;
	};
}