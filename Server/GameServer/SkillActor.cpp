#include "pch.h"
#include "SkillActor.h"

SkillActor::SkillActor(GameObjectRef caster, RoomRef room) : GameObject(room), _caster(caster)
{

}

SkillActor::~SkillActor()
{
}

void SkillActor::Init()
{
	Super::Init();
}

void SkillActor::Update(float deltaTime)
{
	Super::Update(deltaTime);
}
