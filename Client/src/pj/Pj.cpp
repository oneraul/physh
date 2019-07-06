#include "Pj.h"
#include "NetworkInterPolatedPj.h"
#include "LocallyPredictedPj.h"
#include "DummyMode.h"
#include "Batch.h"

namespace rmkl {

	Pj::Pj(unsigned int id, float x, float y)
		: PjCommon(id, Rigidbody(x, y, 1))
		, m_Mode(std::make_unique<NetworkInterpolatedPj>())
	{
	}

	void Pj::Update(float dt)
	{
		m_Mode->Update(*this, dt);
	}

	void Pj::ProcessStateUpdate(const PjState& state, const Stage& stage)
	{
		m_Mode->ProcessStateUpdate(*this, state, stage);
	}

	void Pj::SetMode(PjModes mode)
	{
		switch (mode)
		{
			case PjModes::INTERPOLATED: m_Mode.reset(new NetworkInterpolatedPj()); break;
			case PjModes::PREDICTED:	m_Mode.reset(new LocallyPredictedPj()); break;
			case PjModes::DUMMY:		m_Mode.reset(new DummyMode()); break;
			default: throw("invalid pj mode");
		}
	}

	void Pj::Draw(Batch& batch) const
	{
		glm::vec2 center = m_Mode->GetDrawPos(*this);
		glm::vec2 half = GetBody().m_Collider.Half;
		Rect rect = { center.x - half.x, center.y - half.y, half.x * 2, half.y * 2 };

		glm::vec4 colour;
		switch (GetMode())
		{
			case PjModes::PREDICTED:	colour = glm::vec4(1.0f, 0, 0, 0.3f); break;
			case PjModes::INTERPOLATED: colour = glm::vec4(0, 1.0f, 0, 0.3f); break;
			case PjModes::DUMMY:		colour = glm::vec4(0, 0, 1.0f, 0.3f); break;
		}

		batch.Draw(rect, colour);
	}

}
