#pragma once

#include "PjCommon.h"
#include "Stage.h"
#include <Batch.h>
#include <memory>

namespace rmkl {

	class Pj;

	enum PjModes { PREDICTED, INTERPOLATED };


	class PjMode
	{
	public:
		virtual void UpdateState(Pj& pj, const PjState& state, const Stage& stage) = 0;
		virtual glm::vec2 GetDrawPos(Pj& pj) = 0;
		virtual PjModes GetType() const = 0;
	};


	class Pj : public PjCommon 
	{
	public:
		Pj(unsigned int id, float x, float y);
		void UpdateState(const PjState& state, const Stage& stage);
		void SetMode(PjModes mode);
		void Draw(Batch& batch);
		PjModes GetMode() const;

	private:
		std::unique_ptr<PjMode> m_Mode;
	};

}