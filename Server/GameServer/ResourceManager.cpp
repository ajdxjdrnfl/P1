#include "pch.h"
#include "ResourceManager.h"
#include "SKill.h"

ResourceManager GResourceManager;

static string nameKey = "Name";
static string skillInfosKey = "SkillInfos";

// skillinfo
static string skillNumLocalKey = "SkillNumLocal";
static string hitNumKey = "HitNum";
static string coolDownKey = "CooldownTime";
static string damageKey = "Damage";
static string xscaleKey = "XScale";
static string yscaleKey = "YScale";
static string collisionTypeKey = "CollisionType";
static string ccTypeKey = "CCType";
static string damageTypeKey = "DamageType";
static string skillTypeKey = "SkillType";
static string castingTimeKey = "CastingTime";


void ResourceManager::Init()
{
	_configPath = filesystem::current_path() / "..\\Resources";
	LoadSkillInfos("Skill");
}

void ResourceManager::LoadSkillInfos(const string& path)
{
	filesystem::path fullPath = _configPath / path;

	filesystem::directory_iterator itr(fullPath);
	while (itr != filesystem::end(itr)) {
		const filesystem::directory_entry& entry = *itr;

		ifstream ifs;

		ifs.open(entry.path());

		json jsonObjects;
		ifs >> jsonObjects;

		for (auto& json : jsonObjects)
		{
			string name = json[nameKey].get<string>();
			if (name == "Mage")
			{
				SkillInfoParser(Protocol::CASTER_TYPE_MAGE, json[skillInfosKey]);
			}
			else if (name == "Warrior")
			{
				SkillInfoParser(Protocol::CASTER_TYPE_WARRIOR, json[skillInfosKey]);
			}
			else if (name == "Boss")
			{
				SkillInfoParser(Protocol::CASTER_TYPE_BOSS, json[skillInfosKey]);
			}
			else if (name == "Mob")
			{
				SkillInfoParser(Protocol::CASTER_TYPE_MOB, json[skillInfosKey]);
			}
		}

		ifs.close();
		itr++;
	}
}

void ResourceManager::SkillInfoParser(Protocol::CasterType casterType, json& jsonObjects)
{
	for (auto& json : jsonObjects)
	{
		uint64 skillNum = json[skillNumLocalKey].get<uint64>();
		int32 hitNum = json[hitNumKey].get<int32>();

		float coolDown = json[coolDownKey].get<float>();
		float damage = json[damageKey].get<float>();
		float xscale = json[xscaleKey].get<float>();
		float yscale = json[yscaleKey].get<float>();

		string collisionTypeString = json[collisionTypeKey].get<string>();
		string ccTypeString = json[ccTypeKey].get<string>();
		string damageTypeString = json[damageTypeString].get<string>();
		string skillTypeString = json[skillTypeKey].get<string>();
		
		Protocol::CollisionType collisionType = ConvertStringToCollisionType(collisionTypeString);
		Protocol::CCType ccType = ConvertStringToCCType(ccTypeString);
		Protocol::DamageType damageType = ConvertStringToDamageType(damageTypeString);
		Protocol::SkillType skillType = ConvertStringToSkillType(skillTypeString);
		

		float castingTime = json[castingTimeKey].get<float>();

		Skill* skill = new Skill();
		skill->SetSkillInfo({ skillNum, hitNum, coolDown, damage, xscale, yscale, collisionType, ccType, damageType, skillType, castingTime });
		_skills[casterType][skillNum] = skill;
	}
}

Protocol::CollisionType ResourceManager::ConvertStringToCollisionType(const string& str)
{
	if (str == "Circle")
		return Protocol::COLLISION_TYPE_CIRCLE;

	else if(str == "Box")
		return Protocol::COLLISION_TYPE_BOX;

	return Protocol::COLLISION_TYPE_NONE;
}

Protocol::CCType ResourceManager::ConvertStringToCCType(const string& str)
{
	if (str == "Normal")
		return Protocol::CC_TYPE_NORMAL;	
	else if (str == "Airborne")
		return Protocol::CC_TYPE_AIRBORNE;
	else if (str == "Slow")
		return Protocol::CC_TYPE_SLOW;
	else if (str == "Stun")
		return Protocol::CC_TYPE_STUN;
	
	return Protocol::CC_TYPE_NORMAL;
}

Protocol::DamageType ResourceManager::ConvertStringToDamageType(const string& str)
{
	if (str == "Normal")
		return Protocol::DAMAGE_TYPE_NORMAL;
	else if (str == "Buff")
		return Protocol::DAMAGE_TYPE_BUFF;
	else if (str == "Dot")
		return Protocol::DAMAGE_TYPE_DOT;

	return Protocol::DAMAGE_TYPE_NORMAL;
}

Protocol::SkillType ResourceManager::ConvertStringToSkillType(const string& str)
{
	if (str == "Normal")
		return Protocol::SKILL_TYPE_NORMAL;
	else if (str == "Casting")
		return Protocol::SKILL_TYPE_CASTING;
	else if (str == "Hold")
		return Protocol::SKILL_TYPE_HOLD;
	else if (str == "Charging")
		return Protocol::SKILL_TYPE_CHARGING;

	return Protocol::SKILL_TYPE_NORMAL;
}
