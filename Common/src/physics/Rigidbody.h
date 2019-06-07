#pragma once

#include "Input.h"
#include "Stage.h"

namespace rmkl {


	class Rigidbody
	{
	public:
		Rigidbody(float x, float y, float mass);
		~Rigidbody();

		void SetPos(glm::vec2 pos);
		void FixedUpdate(Input& input, Stage& stage);
		inline void ApplyFrictionCoeficient(float u) { m_FrictionU *= u; }
		inline void AddForce(glm::vec2 force) { m_Forces.emplace_back(force); }
		inline void AddImpulse(glm::vec2 impulse) { m_Impulses.emplace_back(impulse); }

	public:
		float m_Mass;
		float m_InputA;
		float m_InputFrictionDeceleration;
		glm::vec2 m_Pos;
		glm::vec2 m_InputV;
		glm::vec2 m_NonInputV;
		Aabb m_Collider;

	private:
		float m_MaxInputV;
		float m_FrictionU;
		std::vector<glm::vec2> m_Forces;
		std::vector<glm::vec2> m_Impulses;
	};
}