#pragma once
#include "ComponentBase.h"

struct SkillTime
{
	uint64 _skillId = 0;
	float _castDelay = 0.f;
	float _duration = 0.f;
	float _coolDown = 0.f;

	SkillTime() 
	{

	}
	SkillTime(uint64 skillId, float castDelay, float duration, float coolDown) :
		_skillId(skillId), _castDelay(castDelay), _duration(duration), _coolDown(coolDown)
	{

	}
};

class SkillTimer : public ComponentBase
{
	using Super = ComponentBase;
public:
	SkillTimer() : Super(EComponentType::ECT_SKILLTIMER)
	{

	}

	virtual void Init() override;

	virtual void Update(float deltaTime) override;

	bool IsAvailable(uint64 skillid);

	bool Cast(uint64 skillid);

	void AddSkillTime(uint64 skillid);

private:
	unordered_map<uint64, SkillTime> _skillTimes;

};

