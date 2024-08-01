#pragma once
#include "nlohmann/json.hpp"

using namespace nlohmann;

class ResourceManager
{
public:
	void Init();

	class Skill* GetSkill(Protocol::CasterType casterType, uint64 skillId) { return _skills[casterType][skillId]; }

private:
	void LoadSkillInfos(const string& path);
	void SkillInfoParser(Protocol::CasterType casterType, json& jsonObjects);

private:
	Protocol::CollisionType ConvertStringToCollisionType(const string& str);
	Protocol::CCType ConvertStringToCCType(const string& str);
	Protocol::DamageType ConvertStringToDamageType(const string& str);
	Protocol::SkillType ConvertStringToSkillType(const string& str);

private:
	filesystem::path _configPath;
	map<uint64, class Skill*> _skills[Protocol::CasterType_ARRAYSIZE];
};

extern ResourceManager GResourceManager;