#include "pch.h"
#include "SkillActor.h"
#include "Collision.h"
#include "Collider.h"
#include "ComponentBase.h"

SkillActor::SkillActor(GameObjectRef caster, RoomRef room) : GameObject(room), _caster(caster)
{
	_skillinfo = new Protocol::SkillInfo();
	_objectInfo->set_castertype(Protocol::CASTER_TYPE_NONE);
}

SkillActor::~SkillActor()
{
	delete _skillinfo;
	_skillinfo = nullptr;
}

void SkillActor::Init()
{
	Super::Init();
}

void SkillActor::Update(float deltaTime)
{
	Super::Update(deltaTime);
}

void SkillActor::SetCollisionBySkillInfo(const Protocol::SkillInfo& skillinfo)
{
	_skillinfo->CopyFrom(skillinfo);

	Collision* collision = new Collision();
	if (skillinfo.collision_type() == EColliderType::COLLIDER_BOX)
	{
		ColliderBox* box = new ColliderBox();
		box->extent.x = skillinfo.size_x();
		box->extent.y = skillinfo.size_y();
		collision->SetCollider(box);
	}
	else if (skillinfo.collision_type() == EColliderType::COLLIDER_CIRCLE)
	{
		ColliderCircle* circle = new ColliderCircle();
		circle->_radius = skillinfo.size_x();
		collision->SetCollider(circle);
	}

	AddComponent(collision);

}
