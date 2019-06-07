#include "pch.h"
#include "RectForceEmitterPulsating.h"

namespace rmkl {

	RectForceEmitterPulsating::RectForceEmitterPulsating(Aabb aabb, glm::vec2 dir, float impulseMagnitude, int period, int delta)
		: m_Aabb(aabb), m_ImpulseMagnitude(impulseMagnitude), m_Dir(dir), m_Impulse(dir * impulseMagnitude),
		m_Period(period), m_Delta(delta)
	{
	}

	bool RectForceEmitterPulsating::Update(int tick)
	{
		return GetPhase(tick) == 0;
	}

	void RectForceEmitterPulsating::ApplyForce(Rigidbody& body)
	{
		if (m_Aabb.IntersectAabb(body.m_Collider).Intersects)
			body.AddImpulse(m_Impulse);
	}

	Aabb RectForceEmitterPulsating::GetDebugAabb(int tick)
	{
		return m_Aabb;
	}

	glm::vec4 RectForceEmitterPulsating::GetDebugColour(int tick)
	{
		float a = (float)GetPhase(tick) / m_Period;
		return { 0, 1.0f, 0.5f, a * 0.5f };
	}


}