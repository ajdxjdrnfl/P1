#include "pch.h"
#include "Boss.h"
#include "Collision.h"
#include "Collider.h"
#include "Player.h"
#include "Room.h"

Boss::Boss(RoomRef room) : GameObject(room)
{
	_objectInfo->set_hp(1000.f);
	_objectInfo->set_max_hp(1000.f);
	_objectInfo->set_castertype(Protocol::CASTER_TYPE_BOSS);
}

Boss::~Boss()
{
}

void Boss::Update(float deltaTime)
{
	_target = FindClosestTarget();
	_elapsedPacket += deltaTime;
	_prevStepElapsedTime += deltaTime;

	PlayerRef target = _target.lock();

	if (target == nullptr)
		return;

	if (_elapsedPacket >= _updatePacketCooldown)
	{
		_elapsedPacket = 0.f;
		_dirtyFlag = true;
	}

	if (_prevStepElapsedTime > _nextStepCooldown)
	{
		Super::Update(deltaTime);
	}

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

void Boss::TickIdle(float deltaTime)
{
	if (GetState() != Protocol::MOVE_STATE_IDLE)
		return;
	// 1. 체력 체크 후 기믹 발동 여부 확인
	if(false)
	{

	}
	else
	// 2. Target쪽으로 이동 / 기본공격
	{
		_target = FindClosestTarget();
		GameObjectRef target = _target.lock();

		if (target == nullptr)
			return;

		_targetPos = target->GetPos();

		if (GetPos().Distance(_targetPos) <= _attackRange)
		{
			//SelectSkill();
			//SetState(Protocol::MOVE_STATE_SKILL, true);
		}
		else
		{
			MoveToTarget(target);
		}
		
	}
}

void Boss::TickRun(float deltaTime)
{
	if (GetState() != Protocol::MOVE_STATE_RUN)
		return;

	float dist = GetPos().Distance(_targetPos);
	// 충분히 가까울 때
	if ( dist <= 10.f)
	{
		Protocol::ObjectInfo newInfo = *GetObjectInfo();
		newInfo.set_x(_targetPos.x);
		newInfo.set_y(_targetPos.y);
	
		SetObjectInfo(newInfo);
		SetState(Protocol::MOVE_STATE_IDLE, true);
		_prevStepElapsedTime = 0.f;
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

// 스킬 딜레이로 재조정
void Boss::TickSkill(float deltaTime)
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
	if (distance <= _attackRange && _attackCooldown >= 2.f)
	{
		switch (_skillType)
		{
		case EBST_DEFAULT:
			DefaultAttack(target);
			break;
		case EBST_RUSH:
			Rush(target);
			break;
		}
		_attackCooldown = 0.f;
		SetState(Protocol::MOVE_STATE_IDLE, true);
		_prevStepElapsedTime = 0.f;
	}
	// 공격 범위 밖

	if (_attackDelay <= 0.f)
	{
		_attackDelay = 0.f;
		_skillType = EBST_NONE;
		_montageType = MONTAGE_TYPE_NONE;
		SetState(Protocol::MOVE_STATE_IDLE, true);
	}
	// TODO : 스킬 구현 - 기본 스킬을 먼저

}

void Boss::TickStun(float deltaTime)
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

void Boss::SelectSkill()
{
	_skillType = EBST_DEFAULT;
	_attackDelay = 2.f;

}

void Boss::DefaultAttack(GameObjectRef target)
{
	RoomRef room = GetRoomRef();

	switch(_montageType)
	{
	case MONTAGE_TYPE_NONE:
	{
		Protocol::S_MONTAGE montagePkt;
		*montagePkt.mutable_caster() = *GetObjectInfo();
		montagePkt.set_id(0);
		montagePkt.set_isstop(false);
		montagePkt.set_section_num(1);
		montagePkt.set_scalable(true);
		montagePkt.set_duration(1.f);
		
		_attackDelay = 1.f;
		room->DoAsync(&Room::HandleMontage, montagePkt);
		room->DoAsync(&Room::HandleSkill, shared_from_this(), (uint64)0, {target->GetPos().x, target->GetPos().y}, target->GetObjectInfo()->yaw(), 30.f);
		
		_montageType = MONTAGE_TYPE_START;
	}
	break;

	default:
		break;
	}
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

	if (_attackDelay < 0.f)
	{
		switch (_montageType)
		{
		case MONTAGE_TYPE_START:
			_montageType = MONTAGE_TYPE_ING;
			{

			}
			break;

		case MONTAGE_TYPE_ING:
			_montageType = MONTAGE_TYPE_END;
			{

			}
			break;

		case MONTAGE_TYPE_END:

			break;
		}

	}

	switch (_montageType)
	{
	case MONTAGE_TYPE_START:

		break;

	case MONTAGE_TYPE_ING:

		break;

	case MONTAGE_TYPE_END:

		break;
	default:
		SetState(Protocol::MOVE_STATE_IDLE, true);
		break;
	}
}

void Boss::Rush_START(GameObjectRef target)
{

}

void Boss::Rush_ING(GameObjectRef target)
{

}

void Boss::Rush_END(GameObjectRef target)
{

}

void Boss::DotSkill(GameObjectRef target)
{

}

void Boss::MoveToTarget(GameObjectRef target)
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