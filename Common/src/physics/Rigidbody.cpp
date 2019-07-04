#include "pch.h"
#include "Rigidbody.h"
#include <glm/gtx/norm.hpp>

namespace rmkl {

	using glm::vec2;

	Rigidbody::Rigidbody(float x, float y, float mass = 1.0f)
		: m_Mass(mass)
		, m_MaxInputV(2.5f)
		, m_FrictionU(1.0f)
		, m_Pos{x, y}
		, m_InputV{0, 0}
		, m_NonInputV{0, 0}
		, m_Collider({x, y}, {0.2f, 0.2f})
	{
		float inputTimeToReachMaxV = 0.1f;
		float inputTimeToDecelerate = 0.2f;
		m_InputA = m_MaxInputV / inputTimeToReachMaxV;
		m_InputFrictionDeceleration = -m_MaxInputV / inputTimeToDecelerate;
	}

	Rigidbody::~Rigidbody()
	{
	}

	void Rigidbody::SetPos(vec2 pos) 
	{
		m_Pos = pos;
		m_Collider.Pos = pos;
	}

	void Rigidbody::FixedUpdate(const Input& rawInput, const Stage& stage)
	{
		float dt = 1.0f / FIXED_UPDATE_FPS;

		vec2 input(rawInput.X, rawInput.Y);
		if (input.x != 0 || input.y != 0) 
			input = normalize(input);

		// calculate the input acceleration/deceleration, component-wise
		vec2 inputA(0 , 0);
		if (input.x != 0) 
			inputA.x = input.x * m_InputA;
		else if(m_InputV.x != 0)
		{
			inputA.x = normalize(m_InputV).x * m_InputFrictionDeceleration * m_FrictionU;
			if (std::fabs(inputA.x * dt) > std::fabs(m_InputV.x))
				m_InputV.x = inputA.x = 0;
		}

		if (input.y != 0)
			inputA.y = input.y * m_InputA;
		else if (m_InputV.y != 0)
		{
			inputA.y = normalize(m_InputV).y * m_InputFrictionDeceleration * m_FrictionU;
			if (std::fabs(inputA.y * dt) > std::fabs(m_InputV.y))
				m_InputV.y = inputA.y = 0;
		}

		// update the V contribution from input
		m_InputV += inputA * dt;
		if (glm::length2(m_InputV) > m_MaxInputV * m_MaxInputV)
			m_InputV = glm::normalize(m_InputV) * m_MaxInputV;

		// update the v contribution from external sources...
		vec2 nonInputF(0, 0);
		for (vec2 force : m_Forces)
			nonInputF += force;
		vec2 nonInputImpulseF(0, 0);
		for (vec2 impulse : m_Impulses)
			nonInputImpulseF += impulse;
		m_NonInputV += (nonInputF / m_Mass) * dt + (nonInputImpulseF / m_Mass);

		// ...and apply friction to external forces
		if (m_NonInputV.x != 0 || m_NonInputV.y != 0)
		{
			float frictionF = 30 * m_FrictionU;
			float magnitude = std::min((frictionF / m_Mass) * dt, length(m_NonInputV));
			m_NonInputV -= normalize(m_NonInputV) * magnitude;
		}

		// composite v
		vec2 v = m_InputV + m_NonInputV;

		// collision detection + response
		if (v.x != 0 || v.y != 0) 
		{
			Sweep nearest = m_Collider.SweepInto(stage.Walls, v * dt);

			// actual movement
			SetPos(nearest.Pos);

			if (nearest.Hit.Intersects) 
			{
				// correct v
				glm::vec2 tangent = { -nearest.Hit.Normal.y, nearest.Hit.Normal.x };
				m_InputV = tangent * dot(m_InputV, tangent);
				m_NonInputV = tangent * dot(m_NonInputV, tangent);
				v = m_InputV + m_NonInputV;

				// slide along the wall
				Sweep slideNearest = m_Collider.SweepInto(stage.Walls, v * dt);
				SetPos(slideNearest.Pos);
			}

		}

		// clear the forces
		m_Forces.clear();
		m_Impulses.clear();
		m_FrictionU = 1.0f;
	}

}
