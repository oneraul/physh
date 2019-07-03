#pragma once

#include "PjState.h"
#include "physics/ForceEmitter.h"
#include <set>

namespace rmkl {

	class PjCommon
	{
	public:
		PjCommon(int id, Rigidbody body);
		virtual ~PjCommon();

		PjState SerializeState(int tick) const;
		void FixedUpdate(const Input& input, const Stage& stage);
		void ApplyStageSnapshot(int tick, const Stage& stage);
		void UseAbility(const Input& input);
		void UpdateCd();

		inline int GetId() const { return m_Id; }
		inline Rigidbody GetBody() const { return m_Body; }

	public:
		int Spritesheet;
		int Palette;
		std::set<PjState, PjStateCompareTick> History;

	private:
		int m_Id;
		int m_Cd;
		Rigidbody m_Body;

	private:
		static const int s_CdDuration;
	};

}