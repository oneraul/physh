#pragma once

#include "ForceEmitter.h"

namespace rmkl {

	class RectForceEmitterContinuous : public ForceEmitter
	{
	public:
		RectForceEmitterContinuous(Aabb aabb, glm::vec2 dir, float forceMagnitude);
		~RectForceEmitterContinuous();

		virtual bool Update(int tick) override;
		virtual void ApplyForce(Rigidbody& body) override;

		virtual Aabb GetDebugAabb(int tick) override;
		virtual glm::vec4 GetDebugColour(int tick) override;

	private:
		Aabb m_Aabb;
		float m_ForceMagnitude;
		glm::vec2 m_Dir;
		glm::vec2 m_Force;
	};
}