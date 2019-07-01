#include "pch.h"
#include "PjCommon.h"
#include "Utils.h"

namespace rmkl {

	const int PjCommon::s_CdDuration = utils::secondsToTicks(2.0f);

	PjCommon::PjCommon(int id, Rigidbody body)
		: m_Id(id), m_Body(body), m_Cd(0), Spritesheet(0), Palette(0)
	{
	}

	PjCommon::~PjCommon()
	{
	}

	PjState PjCommon::SerializeState(int tick) const
	{
		return PjState(tick, m_Id, m_Body.m_Pos, m_Body.m_InputV, m_Body.m_NonInputV);
	}
	
	void PjCommon::FixedUpdate(Input& input, Stage& stage)
	{
		UpdateCd();
		UseAbility(input);
		ApplyStageSnapshot(input.Tick, stage);
		m_Body.FixedUpdate(input, stage);
	}
	
	void PjCommon::ApplyStageSnapshot(int tick, Stage& stage)
	{
		StageSnapshot snapshot = stage.History.at(tick);
		for (ForceEmitter* emitter : snapshot.Emitters)
		{
			emitter->ApplyForce(m_Body);
		}
	}

	void PjCommon::UseAbility(Input& input)
	{
		if (input.Space == 1 && m_Cd == 0)
		{
			m_Cd = s_CdDuration;
			// TODO
		}
	}
	void PjCommon::UpdateCd()
	{
		m_Cd = std::max((--m_Cd), 0);
	}

}