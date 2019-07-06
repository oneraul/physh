#pragma once

#include "Input.h"
#include "Stage.h"

namespace rmkl {

	class Rigidbody
	{
	typedef glm::vec2 vec2;

	public:
		Rigidbody(float x, float y, float mass);
		~Rigidbody();

		inline vec2 GetPos() const { return m_Pos; }
		void SetPos(vec2 pos);
		void FixedUpdate(const Input& input, const Stage& stage);
		inline void ApplyFrictionCoeficient(float u) { m_FrictionU *= u; }
		inline void AddForce(vec2 force) { m_Forces.emplace_back(force); }
		inline void AddImpulse(vec2 impulse) { m_Impulses.emplace_back(impulse); }

	public:
		float m_Mass;
		float m_InputA;
		float m_InputFrictionDeceleration;
		vec2 m_InputV;
		vec2 m_NonInputV;
		Aabb m_Collider;

	private:
		vec2 m_Pos;
		float m_MaxInputV;
		float m_FrictionU;
		std::vector<vec2> m_Forces;
		std::vector<vec2> m_Impulses;
	};

}
