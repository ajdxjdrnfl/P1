#include "pch.h"
#include "SkillTimer.h"

void SkillTimer::Init()
{
	for (auto& item : _skillTimes)
	{
		item.second._duration = item.second._coolDown;
	}
}

void SkillTimer::Update(float deltaTime)
{
	
	for (auto& item : _skillTimes)
	{
		item.second._duration += deltaTime;
	}
}

bool SkillTimer::IsAvailable(uint64 skillId)
{
	if (_skillTimes.find(skillId) == _skillTimes.end())
		return false;

	SkillTime& time = _skillTimes[skillId];

	if (time._duration < time._coolDown)
		return false;

	return true;
}

bool SkillTimer::Cast(uint64 skillId)
{
	if (!IsAvailable(skillId))
		return false;

	SkillTime& time = _skillTimes[skillId];
	
	time._duration = 0.f;

	return true;
}

void SkillTimer::AddSkillTime(uint64 skillid)
{
	return;
}
