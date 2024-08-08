#include "pch.h"
#include "Enemy.h"
#include "Room.h"
#include "Collision.h"
#include "Collider.h"
#include "ComponentBase.h"
#include "Player.h"


Enemy::Enemy(RoomRef room) : GameObject(room)
{
	_objectInfo->set_hp(50.f);
	_objectInfo->set_max_hp(50.f);
	_objectInfo->set_castertype(Protocol::CASTER_TYPE_MOB);
	_objectInfo->set_speed(250.f);
}

Enemy::~Enemy()
{
}

void Enemy::Update(float deltaTime)
{
	RoomRef room = GetRoomRef();

	if (room == nullptr)
		return;

	_elapsedPacket += deltaTime;
	randomWalkDuration += deltaTime;

	if (randomWalkDuration >= randomWalkCoolDonw)
	{
		//RandomWalk();
		randomWalkDuration = 0.f;
	}
	
	Super::Update(deltaTime);

	if (_elapsedPacket >= _updatePacketCooldown)
	{
		_elapsedPacket = 0.f;
		_dirtyFlag = true;
	}

	BroadcastUpdate();
}

void Enemy::Init()
{
	Collision* collision = new Collision();
	ColliderCircle* collider = new ColliderCircle();
	collider->_radius = 38.f;
	collision->SetCollider(collider);

	AddComponent(collision);
	Super::Init();
}

void Enemy::RandomWalk()
{
	Vector currentPos = Vector(_objectInfo->x(), _objectInfo->y());
	VectorInt d[9] =
	{
		{-1,-1},
		{-1,1},
		{1,-1},
		{1,1},
		{0,1},
		{1,0},
		{-1,0},
		{0,-1},
		{0,0}
	};

	float randomX = Utils::GetRandom(0.f, 50.f);
	float randomY = Utils::GetRandom(0.f, 50.f);
	
	uint8 randomIndex = Utils::GetRandom(0, 8);

	Vector nextPos = Vector(currentPos.x + randomX * d[randomIndex].x, currentPos.y + randomY * d[randomIndex].y);
	
	_objectInfo->set_x(nextPos.x);
	_objectInfo->set_y(nextPos.y);
	_objectInfo->set_state(Protocol::MOVE_STATE_RUN);

	_dirtyFlag = true;
}


void Enemy::BroadcastUpdate()
{
	Super::BroadcastUpdate();
}

void Enemy::TakeDamage(GameObjectRef instigator, Protocol::DamageType damageType, float damage)
{
	Super::TakeDamage(instigator, damageType, damage);
}

void Enemy::TickIdle(float deltaTime)
{
	if (GetState() != Protocol::MOVE_STATE_IDLE)
		return;

	_target = FindClosestTarget();

	GameObjectRef target = _target.lock();

	if (target == nullptr)
		return;

	_targetPos = target->GetPos();

	if (GetPos().Distance(_targetPos) <= _attackRange)
	{
		//AttackToTarget(target);
		//SetState(Protocol::MOVE_STATE_SKILL, true);
	}
	else
	{

		MoveToTarget(target);
	}
}

void Enemy::TickRun(float deltaTime)
{
	if (GetState() != Protocol::MOVE_STATE_RUN)
		return;

	float dist = GetPos().Distance(_targetPos);
	// 충분히 가까울 때
	if (dist <= 10.f)
	{
		Protocol::ObjectInfo newInfo = *GetObjectInfo();
		newInfo.set_x(_targetPos.x);
		newInfo.set_y(_targetPos.y);

		SetObjectInfo(newInfo);
		SetState(Protocol::MOVE_STATE_IDLE, true);

	}
	else
	{
		Vector moveVector = (_targetPos - GetPos()).Normalize();

		moveVector = moveVector * _moveSpeed * deltaTime;

		Protocol::ObjectInfo newInfo = *GetObjectInfo();
		newInfo.set_x(GetPos().x + moveVector.x);
		newInfo.set_y(GetPos().y + moveVector.y);

		SetObjectInfo(newInfo);
	}
}

void Enemy::TickSkill(float deltaTime)
{
	if (GetState() != Protocol::MOVE_STATE_SKILL)
		return;

	_attackDelay -= deltaTime;
	_attackCooldown += deltaTime;

	GameObjectRef target = _target.lock();
	if (target == nullptr)
	{
		SetState(Protocol::MOVE_STATE_IDLE, true);
	}

	float distance = target->GetPos().Distance(GetPos());

	// 공격 범위 안
	if (_attackDelay <= 0.f && _attackCooldown >= 2.f)
	{
		_attackCooldown = 0.f;
		SetState(Protocol::MOVE_STATE_IDLE, true);
	}
}

void Enemy::TickStun(float deltaTime)
{
	Super::TickStun(deltaTime);
}

PlayerRef Enemy::FindClosestTarget()
{
	RoomRef room = GetRoomRef();

	if (room == nullptr)
		return nullptr;

	return room->FindClosestPlayer(GetPos());
}

void Enemy::MoveToTarget(GameObjectRef target)
{
	if (target == nullptr)
		return;

	// TODO : 맵 데이터에 맞는 이동방식 필요 
	Vector targetPos = target->GetPos();

	Vector targetVector = (targetPos - GetPos());

	_targetPos = targetVector * 0.75 + GetPos();

	float yaw = Utils::GetYawByVector(targetPos - GetPos());
	_objectInfo->set_yaw(yaw);
	SetState(Protocol::MOVE_STATE_RUN, true);
}

void Enemy::AttackToTarget(GameObjectRef target)
{
	RoomRef room = GetRoomRef();

	if (room == nullptr)
		return;

	Protocol::S_MONTAGE montagePkt;
	*montagePkt.mutable_caster() = *GetObjectInfo();
	montagePkt.set_id(0);
	montagePkt.set_isstop(false);
	montagePkt.set_section_num(1);
	montagePkt.set_scalable(true);
	montagePkt.set_duration(1.f);

	_attackDelay = 1.f;
	room->DoAsync(&Room::HandleMontage, montagePkt);
	room->DoAsync(&Room::HandleSkill, shared_from_this(), (uint64)0, { target->GetPos().x, target->GetPos().y }, target->GetObjectInfo()->yaw(), 20.f);
}
