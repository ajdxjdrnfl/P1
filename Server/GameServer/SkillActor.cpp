#include "pch.h"
#include "SkillActor.h"
#include "Collision.h"
#include "Collider.h"
#include "ComponentBase.h"
#include "ResourceManager.h"
#include "Room.h"

SkillActor::SkillActor(GameObjectRef caster, RoomRef room) : GameObject(room), _caster(caster)
{
	_objectInfo->set_castertype(Protocol::CASTER_TYPE_NONE);
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

	if (_skill == nullptr)
		return;

	RoomRef room = GetRoomRef();

	if (room == nullptr)
		return;

	_elapsedTime += deltaTime;

	if (!_isSpawned)
	{
		if(_elapsedTime >= GetDelayTime())
			SpawnSkill();
		
		return;
	}

	_lifeTime += deltaTime;
	
	if (_skill->GetSkillInfo().lifeTime <= _lifeTime)
	{
		_lifeTime = 0.f;
		SetState(Protocol::MOVE_STATE_DEAD);
	}

}

void SkillActor::TickDead(float deltaTime)
{
	if (GetState() == Protocol::MOVE_STATE_DEAD)
		return;

	RoomRef room = GetRoomRef();

	if (room == nullptr)
		return;

	Protocol::S_DESPAWN despawnPkt;
	despawnPkt.add_info()->CopyFrom(*GetObjectInfo());
	room->DoAsync(&Room::HandleDespawn, despawnPkt, true);
}

void SkillActor::SpawnSkill()
{
	RoomRef room = GetRoomRef();

	if (room == nullptr)
		return;

	_isSpawned = true;
	room->HandleSkill(static_pointer_cast<SkillActor>(shared_from_this()));
}

void SkillActor::SetCollisionBySkillId(Protocol::CasterType casterType, const uint64& skillId, float damage, bool counter)
{
	_skill = GResourceManager.GetSkill(casterType, skillId);

	assert(_skill != nullptr);
	
	SkillInfo skillInfo = _skill->GetSkillInfo();
	_damage = damage;

	Collision* collision = new Collision();
	if (skillInfo.collisionType == EColliderType::COLLIDER_BOX)
	{
		ColliderBox* box = new ColliderBox();
		box->extent.x = skillInfo.xscale;
		box->extent.y = skillInfo.yscale;
		collision->SetCollider(box);
	}
	else if (skillInfo.collisionType == EColliderType::COLLIDER_CIRCLE)
	{
		ColliderCircle* circle = new ColliderCircle();
		circle->_radius = skillInfo.xscale;
		collision->SetCollider(circle);
	}

	AddComponent(collision);
}
