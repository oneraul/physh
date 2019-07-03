#include "Pj.h"
#include "NetworkInterPolatedPj.h"
#include "LocallyPredictedPj.h"
#include "Batch.h"

namespace rmkl {

	Pj::Pj(unsigned int id, float x, float y)
		: PjCommon(id, Rigidbody(x, y, 1))
	{
		m_Mode = std::make_unique<NetworkInterpolatedPj>();
	}

	PjModes Pj::GetMode() const
	{
		return m_Mode->GetType();
	}

	void Pj::UpdateState(const PjState& state, const Stage& stage)
	{
		m_Mode->UpdateState(*this, state, stage);
	}

	void Pj::SetMode(PjModes mode)
	{
		//if (m_Mode)
		//	delete m_Mode;
		switch (mode)
		{
		case PjModes::INTERPOLATED:
			m_Mode.reset(new NetworkInterpolatedPj());
			break;

		case PjModes::PREDICTED:
			m_Mode.reset(new LocallyPredictedPj());
			break;
		}
	}

	void Pj::Draw(Batch& batch)
	{
		glm::vec2 center = m_Mode->GetDrawPos(*this);
		glm::vec2 half = m_Body.m_Collider.Half;
		Rect rect = { center.x - half.x, center.y - half.y, half.x * 2, half.y * 2 };
		glm::vec4 colour(0, 1.0f, 1.0f, 0.5f);
		batch.Draw(rect, colour);
	}
}