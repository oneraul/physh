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

	public:
		int m_Id;
		Rigidbody m_Body;
		int Spritesheet;
		int Palette;
		std::set<PjState, PjStateCompareTick> History;

	private:
		int m_Cd;

	private:
		static const int s_CdDuration;
	};

}