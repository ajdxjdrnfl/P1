#pragma once
#include "ComponentBase.h"

class SkillTimer : public ComponentBase
{
public:
	SkillTimer(uint64 skillId, float castDelay, float duration, float coolDown) :
		_skillId(skillId), _castDelay(castDelay), _duration(duration), _coolDown(coolDown)
	{

	}

	virtual void Init() override;

	virtual void Update(float deltaTime) override;

	bool IsAvailable();

	bool Cast();

private:
	uint64 _skillId = 0;
	float _castDelay = 0.f;
	float _duration = 0.f;
	float _coolDown = 0.f;

};

