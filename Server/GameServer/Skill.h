#pragma once
#include "ResourceBase.h"

struct SkillInfo
{

}typedef SkillInfo;


class Skill : public ResourceBase
{
public:
	Skill() { };
	virtual ~Skill() { };

	virtual void Load(const string& path);

private:
	SkillInfo skillInfo;
};

