#pragma once

#include "PjCommon.h"
#include "Stage.h"
#include <Batch.h>
#include <memory>

namespace rmkl {

	class Pj;

	enum PjModes { PREDICTED, INTERPOLATED, DUMMY };


	class PjMode
	{
	public:
		virtual void ProcessStateUpdate(Pj& pj, const PjState& state, const Stage& stage) = 0;
		virtual void Update(Pj& pj, float dt) = 0;
		virtual glm::vec2 GetDrawPos(const Pj& pj) const = 0;
		virtual PjModes GetType() const = 0;
	};


	class Pj : public PjCommon 
	{
	public:
		Pj(unsigned int id, float x, float y);
		void Update(float dt);
		void ProcessStateUpdate(const PjState& state, const Stage& stage);
		void SetMode(PjModes mode);
		void Draw(Batch& batch) const;
		PjModes GetMode() const { return m_Mode->GetType(); }
		glm::vec2 GetDrawPos() const { return m_Mode->GetDrawPos(*this); }

	private:
		std::unique_ptr<PjMode> m_Mode;
	};

}