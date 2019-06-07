#pragma once

#include "PjState.h"
#include "physics/ForceEmitter.h"
#include <map>

namespace rmkl {

	class PjCommon
	{
	public:
		PjCommon(int id, Rigidbody body);
		virtual ~PjCommon();

		PjState SerializeState(int tick) const;
		void FixedUpdate(Input& input, Stage& stage);
		void ApplyStageSnapshot(int tick, Stage& stage);
		void UseAbility(Input& input);
		void UpdateCd();

	public:
		int m_Id;
		Rigidbody m_Body;
		int Spritesheet;
		int Palette;
		std::map<int, PjState> m_History;

	private:
		float m_Cd;

	private:
		static const float s_CdDuration;
	};

}