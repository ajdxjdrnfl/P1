#include "pch.h"
#include "SkillTimer.h"

void SkillTimer::Init()
{
}

void SkillTimer::Update(float deltaTime)
{
	_duration += deltaTime;
}

bool SkillTimer::IsAvailable()
{
	if (_duration < _coolDown)
		return false;

	return true;
}

bool SkillTimer::Cast()
{
	return false;
}
