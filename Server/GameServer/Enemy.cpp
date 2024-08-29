#include "pch.h"
#include "Enemy.h"
#include "Room.h"
#include "Collision.h"
#include "Collider.h"
#include "ComponentBase.h"
#include "Player.h"
#include "Skill.h"

Enemy::Enemy(RoomRef room, uint64 phaseNumber) : GameObject(room), _phaseNumber(phaseNumber)
{
	_objectInfo->set_hp(50.f);
	_objectInfo->set_max_hp(50.f);
	_objectInfo->set_castertype(Protocol::CASTER_TYPE_MOB);
	_objectInfo->set_speed(_moveSpeed);
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
	_attackCooldown += deltaTime;
	_moveToTargetDelay += deltaTime;

	Super::Update(deltaTime);

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
	RoomRef room = GetRoomRef();

	if (room == nullptr)
		return;

	Vector currentPos = Vector(_objectInfo->x(), _objectInfo->y());

	VectorInt d[8] =
	{ {0,1}, {1, 1}, {1, 0}, {1, -1}, {0,-1}, {-1,-1}, {-1, 0}, {-1, 1} };

	uint8 randomIndex = Utils::GetRandom(0, 7);

	VectorInt gridPos = room->GetGridPos(GetPos());

	if (room->CanGoByDirection(gridPos, randomIndex, true , static_cast<Collision*>(GetComponent(EComponentType::ECT_COLLISION))))
	{
		Vector nextPos = room->GetPosition(gridPos + d[randomIndex]);

		_objectInfo->set_state(Protocol::MOVE_STATE_RUN);

		_targetPos = nextPos;

		_dirtyFlag = true;
	}
	

}

void Enemy::RandomRotate()
{
	float currentYaw = _objectInfo->yaw();

	float nextYaw = Utils::GetRandom(currentYaw - 40.f, currentYaw + 40.f);

	_objectInfo->set_yaw(nextYaw);
	_objectInfo->set_state(Protocol::MOVE_STATE_IDLE);

	_dirtyFlag = true;
}


void Enemy::BroadcastUpdate()
{
	Super::BroadcastUpdate();
}

void Enemy::TakeDamage(GameObjectRef instigator, SkillInfo skillInfo, float damage, bool counter)
{
	Super::TakeDamage(instigator, skillInfo, damage);
}

void Enemy::TickIdle(float deltaTime)
{
	if (GetState() != Protocol::MOVE_STATE_IDLE)
		return;

	_target = FindClosestTarget(_detectRange);

	GameObjectRef target = _target.lock();

	if (target == nullptr)
	{
		if (randomWalkDuration >= randomWalkCooldown)
		{
			int32 random = Utils::GetRandom(0, 1);
			if (random == 0)
			{
				//RandomWalk();
			}
			else if (random == 1)
			{
				RandomRotate();
			}
			
			randomWalkDuration = 0.f;
		}
	
		return;
	}

	_targetPos = target->GetPos();

	if (GetPos().Distance(_targetPos) <= _attackRange && _attackCooldown >= 1.f)
	{
		AttackToTarget(target);
		SetState(Protocol::MOVE_STATE_SKILL, true);
	}
	else if(_moveToTargetDelay >= _moveToTargetCooldown)
	{
		MoveToTarget(target);
	}
}

void Enemy::TickRun(float deltaTime)
{
	RoomRef room = GetRoomRef();

	if (room == nullptr)
		return;

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

		if (!PopTargetPath())
		{
			SetState(Protocol::MOVE_STATE_IDLE);
			_moveToTargetDelay = 0.f;
		}
		_attackCooldown = 0.f;
	}
	else
	{
		Vector moveVector = (_targetPos - GetPos()).Normalize();

		float distance = (_targetPos - GetPos()).Length();

		moveVector = moveVector * min(_moveSpeed * deltaTime, distance);
		
		if (!room->CanGoByVector(static_cast<Collision*>(GetComponent(EComponentType::ECT_COLLISION)), moveVector))
		{
			SetState(Protocol::MOVE_STATE_IDLE, true);
			_moveToTargetDelay = 0.f;
			return;
		}
		Vector nextPos = GetPos() + moveVector;

		float yaw=  Utils::GetYawByVector(moveVector);

		Protocol::ObjectInfo newInfo = *GetObjectInfo();
		newInfo.set_x(nextPos.x);
		newInfo.set_y(nextPos.y);
		newInfo.set_yaw(yaw);

		SetObjectInfo(newInfo, false, true);
	}
}

void Enemy::TickSkill(float deltaTime)
{
	RoomRef room = GetRoomRef();

	if (room == nullptr)
		return;

	if (GetState() != Protocol::MOVE_STATE_SKILL)
		return;

	_attackDelay -= deltaTime;

	GameObjectRef target = _target.lock();
	if (target == nullptr)
	{
		SetState(Protocol::MOVE_STATE_IDLE, true);
		return;
	}

	float distance = target->GetPos().Distance(GetPos());

	// 공격 범위 안
	if(_attackDelay <= 0.5f && _sendSkillPacket == false)
	{
		room->DoAsync(&Room::CreateSkillActor, shared_from_this(), (uint64)0, { _targetPos.x, _targetPos.y }, GetObjectInfo()->yaw(), 20.f);
		_sendSkillPacket = true;
	}

	if (_attackDelay <= 0.f)
	{
		_attackCooldown = 0.f;
		SetState(Protocol::MOVE_STATE_IDLE, true);
	}
}

void Enemy::TickStun(float deltaTime)
{
	Super::TickStun(deltaTime);
}

void Enemy::TickDead(float deltaTime)
{
	if (GetState() != Protocol::MOVE_STATE_DEAD)
		return;

	RoomRef room = GetRoomRef();

	if (room == nullptr)
		return;

	_deadElapsedTime += deltaTime;

	if (_deadElapsedTime >= 10.f)
	{
		_deadElapsedTime = 0.f;
		Protocol::S_DESPAWN despawnPkt;
		despawnPkt.add_info()->CopyFrom(*GetObjectInfo());
		room->DoAsync(&Room::HandleDespawn, despawnPkt, true);
	}
}

PlayerRef Enemy::FindClosestTarget(float maxDistance)
{
	RoomRef room = GetRoomRef();

	if (room == nullptr)
		return nullptr;

	return static_pointer_cast<Player>(room->FindClosestPlayer(GetPos(), maxDistance));
}

void Enemy::MoveToTarget(GameObjectRef target)
{
	if (target == nullptr)
		return;

	RoomRef room = GetRoomRef();

	room->GetGridPos(_targetPos);

	vector<VectorInt> path;

	bool found = room->FindPath(GetPos(), target->GetPos(), path, 30);

	if (found)
	{
		if (path.size() > 2)
		{
			PushTargetPath(path);
			PopTargetPath();
			Vector targetVector = (_targetPos - GetPos());
			float yaw = Utils::GetYawByVector(targetVector);
			_objectInfo->set_yaw(yaw);
			SetState(Protocol::MOVE_STATE_RUN, true);
		}
		
	}
	
}

void Enemy::AttackToTarget(GameObjectRef target)
{
	RoomRef room = GetRoomRef();

	if (room == nullptr)
		return;

	if (target == nullptr)
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
	_sendSkillPacket = false;
	
	// yaw set
	_targetPos = target->GetPos();
	float yaw = Utils::GetYawByVector((target->GetPos() - GetPos()).Normalize());

	Protocol::ObjectInfo newInfo = *GetObjectInfo();
	newInfo.set_yaw(yaw);

	SetObjectInfo(newInfo, false, true);
}

void Enemy::PushTargetPath(vector<VectorInt>& path)
{
	while (!_targetPath.empty())
	{
		_targetPath.pop();
	}

	for (int i = 1; i < path.size()-2; i++)
		_targetPath.push(path[i]);

	_targetPathSize = path.size();
}

bool Enemy::PopTargetPath(int32 threshold)
{
	RoomRef room = GetRoomRef();

	if (room == nullptr)
		return false;

	if (_targetPath.empty() || _targetPathSize <= threshold)
		return false;

	VectorInt gridPos = _targetPath.front();
	_targetPath.pop();
	_targetPathSize--;

	if (_targetPath.empty() && _target.lock())
		_targetPos = Vector(_target.lock()->GetObjectInfo()->x(), _target.lock()->GetObjectInfo()->y());
	else
		_targetPos = room->GetPosition(gridPos);

	return true;
}
