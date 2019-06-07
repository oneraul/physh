#pragma once

#include "ForceEmitter.h"

namespace rmkl {

	class RectForceEmitterPulsating : public ForceEmitter
	{
	public:
		RectForceEmitterPulsating(Aabb aabb, glm::vec2 dir, float impulseMagnitude, int period, int delta);

		virtual bool Update(int tick) override;
		virtual void ApplyForce(Rigidbody& body) override;

		virtual Aabb GetDebugAabb(int tick) override;
		virtual glm::vec4 GetDebugColour(int tick) override;

	private:
		int inline GetPhase(int tick) const { return (tick % m_Period) + m_Delta; }

	private:
		Aabb m_Aabb;
		float m_ImpulseMagnitude;
		glm::vec2 m_Dir;
		glm::vec2 m_Impulse;
		int m_Period; // how often to pulse (in physics ticks)
		int m_Delta;  // when in the update period to pulse
	};
}