#include "pch.h"
#include "Boss.h"
#include "Collision.h"
#include "Collider.h"
#include "Player.h"
#include "Room.h"

Boss::Boss(RoomRef room) : GameObject(room)
{
	_objectInfo->set_hp(100000.f);
	_objectInfo->set_max_hp(100000.f);
	_objectInfo->set_castertype(Protocol::CASTER_TYPE_BOSS);
}

Boss::~Boss()
{
}

void Boss::Update(float deltaTime)
{

	_attackCooldown += deltaTime;
	_target = FindClosestTarget();

	PlayerRef target = _target.lock();

	if (target == nullptr)
		return;


	Super::Update(deltaTime);
}

void Boss::Init()
{
	Collision* collision = new Collision();
	ColliderCircle* collider = new ColliderCircle();
	collider->_radius = 38.f;
	collision->SetCollider(collider);

	AddComponent(collision);
	Super::Init();
}

void Boss::TakeDamage(GameObjectRef instigator, Protocol::DamageType damageType, float damage)
{
	Super::TakeDamage(instigator, damageType, damage);
}

void Boss::TickIdle()
{
	if (GetState() != Protocol::MOVE_STATE_IDLE)
		return;
	// 1. ü�� üũ �� ��� �ߵ� ���� Ȯ��
	if(false)
	{

	}
	else
	// 2. Target������ �̵� / �⺻����
	{
		_target = FindClosestTarget();
		GameObjectRef target = _target.lock();

		if (target == nullptr)
			return;

		if (GetPos().DistanceSquared(_targetPos) <= _attackRange)
		{
			SetState(Protocol::MOVE_STATE_SKILL, true);
		}
		else
		{
			MoveToTarget(target);
		}
		
	}
}

void Boss::TickRun()
{
	if (GetState() != Protocol::MOVE_STATE_RUN)
		return;

	// ����� ����� ��
	if (GetPos().DistanceSquared(_targetPos) <= 10.f)
	{
		Protocol::ObjectInfo newInfo = *GetObjectInfo();
		newInfo.set_x(_targetPos.x);
		newInfo.set_y(_targetPos.y);
	
		SetObjectInfo(newInfo, true);
		SetState(Protocol::MOVE_STATE_IDLE, true);
	}
	else
	{
		Vector moveVector = (_targetPos - GetPos()).Normalize();
		moveVector = moveVector * _moveSpeed;

		Protocol::ObjectInfo newInfo = *GetObjectInfo();
		newInfo.set_x(GetPos().x + moveVector.x);
		newInfo.set_y(GetPos().y + moveVector.y);
		

		SetObjectInfo(newInfo, true);
	}


}

void Boss::TickSkill()
{
	if (GetState() != Protocol::MOVE_STATE_SKILL)
		return;

	GameObjectRef target = _target.lock();
	if (target == nullptr)
	{
		SetState(Protocol::MOVE_STATE_IDLE, true);
	}

	float distance = target->GetPos().DistanceSquared(GetPos());

	// ���� ���� ��
	if (distance <= _attackRange && _attackCooldown >= 2.f)
	{
		DefaultAttack();
		SetState(Protocol::MOVE_STATE_IDLE, true);
	}
	// ���� ���� ��
	else
	{
		SetState(Protocol::MOVE_STATE_IDLE, true);
	}
	// TODO : ��ų ���� - �⺻ ��ų�� ����

}

void Boss::TickStun()
{
	if (GetState() != Protocol::MOVE_STATE_STUN)
		return;
}

PlayerRef Boss::FindClosestTarget()
{
	RoomRef room = GetRoomRef();

	if (room == nullptr)
		return nullptr;

	return room->FindClosestPlayer(GetPos());
}

void Boss::DefaultAttack()
{
	
	RoomRef room = GetRoomRef();
	
	GameObjectRef target = _target.lock();

	// TODO : ����
	_objectInfo->set_yaw(Utils::GetYawByVector(target->GetPos() - GetPos()));
	room->DoAsync(&Room::HandleSkill, shared_from_this(), (uint64)0);
	
}

void Boss::Rush(GameObjectRef target)
{
	if (target == nullptr)
		return;

	Vector targetPos = target->GetPos();
	Vector rushVector = (targetPos - GetPos()).Normalize();

	rushVector = rushVector * _rushSpeed;

	// TODO : Spawn SkillActor;
	{
		Protocol::S_SKILL skillPkt;
	}
	// TODO : Play AnimMontage;
	{
		Protocol::S_MONTAGE montagePkt;
	}
}

void Boss::DotSkill()
{

}

void Boss::MoveToTarget(GameObjectRef target)
{
	if (target == nullptr)
		return;

	// TODO : �� �����Ϳ� �´� �̵���� �ʿ� 
	_targetPos = target->GetPos();

	SetState(Protocol::MOVE_STATE_RUN, true);
}