#include "pch.h"
#include "RectForceEmitterContinuous.h"

namespace rmkl {

	RectForceEmitterContinuous::RectForceEmitterContinuous(Aabb aabb, glm::vec2 dir, float forceMagnitude)
		: m_Aabb(aabb), m_ForceMagnitude(forceMagnitude), m_Dir(dir), m_Force(dir * forceMagnitude)
	{
	}

	RectForceEmitterContinuous::~RectForceEmitterContinuous() {}

	bool RectForceEmitterContinuous::Update(int tick)
	{
		return true;
	}

	void RectForceEmitterContinuous::ApplyForce(Rigidbody& body)
	{
		if (m_Aabb.IntersectAabb(body.m_Collider).Intersects)
			body.AddForce(m_Force);
	}

	Aabb RectForceEmitterContinuous::GetDebugAabb(int tick)
	{
		return m_Aabb;
	}

	glm::vec4 RectForceEmitterContinuous::GetDebugColour(int tick)
	{
		return { 0, 1.0f, 0.5f, 0.5f };
	}
}
