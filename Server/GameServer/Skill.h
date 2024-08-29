#pragma once
#include "ResourceBase.h"

struct SkillInfo
{
	uint64 skillNum;
	int32 hitNum;

	float coolDown;
	float damage;
	float xscale;
	float yscale;
	
	Protocol::CollisionType collisionType;
	Protocol::CCType ccType;
	Protocol::DamageType damageType;
	Protocol::SkillType skillType;

	float castingTime;
	float ccTime;
	float lifeTime;
	float delayTime;

	bool isPredictable;

}typedef SkillInfo;


class Skill : public ResourceBase
{
public:
	Skill() { };
	virtual ~Skill() { };

	SkillInfo& GetSkillInfo() { return _skillInfo; }
	void SetSkillInfo(SkillInfo skillInfo) { _skillInfo = skillInfo; }

private:
	SkillInfo _skillInfo;
};

