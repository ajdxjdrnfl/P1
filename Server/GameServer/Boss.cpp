#include "pch.h"
#include "Boss.h"
#include "Collision.h"
#include "Collider.h"
#include "Player.h"
#include "Room.h"
#include "Structure.h"

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
			SelectSkill();
			SetState(Protocol::MOVE_STATE_SKILL, true);
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
	if (true)
	{
		switch (_skillType)
		{
		case EBST_DEFAULT:
			DefaultAttack(target);
			break;
		case EBST_RUSH:
		{
			GameObjectRef pillar = _pillars[_pillarNum].lock();
			Rush(pillar, deltaTime);
		}
			break;
		case EBST_SPAWNPILLAS:
			SpawnPillars(deltaTime);
			break;
		case EBST_TELEPORT:
			Teleport({0.f, 0.f}, deltaTime);
			break;
		}
		_attackCooldown = 0.f;
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

void Boss::StartDefaultAttack()
{
	_attackDelay = 2.f;
}

void Boss::StartRush()
{
	{
		Protocol::S_MONTAGE montagePkt;
		*montagePkt.mutable_caster() = *GetObjectInfo();
		montagePkt.set_id(1);
		montagePkt.set_isstop(false);
		montagePkt.set_section_num(1);
		montagePkt.set_scalable(true);
		montagePkt.set_duration(2.f);
	}
	{
		Protocol::ObjectInfo info;
		info.CopyFrom(*GetObjectInfo());
		info.set_x(0.f);
		info.set_y(0.f);

		SetObjectInfo(info, true, true);
	}
	_attackDelay = 2.f;

	
}

void Boss::SelectSkill()
{
	// Default
	//_skillType = EBST_DEFAULT;
	//StartDefaultAttack();

	// Rush
	//_skillType = EBST_RUSH;
	//StartRush();

	// SpawnPillars

	_skillType = EBST_TELEPORT;
	StartTeleport();
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

void Boss::Rush(GameObjectRef target, float deltaTime)
{
	if (target == nullptr)
		return;

	RoomRef room = GetRoomRef();


	if (room == nullptr)
		return;

	if (_attackDelay < 0.f)
	{
		Protocol::S_MONTAGE montagePkt;

		switch (_montageType)
		{
		case MONTAGE_TYPE_NONE:
			_montageType = MONTAGE_TYPE_START;
			{
				// 준비자세
				*montagePkt.mutable_caster() = *GetObjectInfo();
				montagePkt.set_id(1);
				montagePkt.set_section_num(4);
				montagePkt.set_isstop(false);
				montagePkt.set_scalable(true);
				montagePkt.set_duration(2.f);
				room->HandleMontage(montagePkt);
				_attackDelay = 2.f;
			}
			break;
		case MONTAGE_TYPE_START:
			_montageType = MONTAGE_TYPE_ING;
			{
				// 달려가기
				*montagePkt.mutable_caster() = *GetObjectInfo();
				montagePkt.set_id(1);
				montagePkt.set_section_num(5);
				montagePkt.set_isstop(false);
				montagePkt.set_scalable(true);
				montagePkt.set_duration(10.f);
				room->HandleMontage(montagePkt);
				_attackDelay = 10.f;
			}
			break;

		case MONTAGE_TYPE_ING:
			_montageType = MONTAGE_TYPE_END;
			{
				// 후딜레이
				*montagePkt.mutable_caster() = *GetObjectInfo();
				montagePkt.set_id(1);
				montagePkt.set_section_num(6);
				montagePkt.set_isstop(false);
				montagePkt.set_scalable(true);
				montagePkt.set_duration(2.f);
				room->HandleMontage(montagePkt);
				_attackDelay = 2.f;
			}
			break;

		case MONTAGE_TYPE_END:
			
			break;
		}

	}

	switch (_montageType)
	{
	case MONTAGE_TYPE_START:
		Rush_START(target, deltaTime);
		break;

	case MONTAGE_TYPE_ING:
	{
		Rush_ING(target, deltaTime);
	}
		break;

	case MONTAGE_TYPE_END:
		Rush_END(target, deltaTime);
		break;

	}
}

void Boss::Rush_START(GameObjectRef target, float deltaTime)
{
	// 패링 처리

}

void Boss::Rush_ING(GameObjectRef target, float deltaTime)
{
	if (target == nullptr)
		return;

	float dist = GetPos().Distance(target->GetPos());
	// 충분히 가까울 때
	if (dist <= 10.f)
	{
		Protocol::ObjectInfo newInfo = *GetObjectInfo();
		newInfo.set_x(target->GetPos().x);
		newInfo.set_y(target->GetPos().y);

		SetObjectInfo(newInfo, false, true);
		_attackDelay = 0.f;
	}
	else
	{
		Vector moveVector = (target->GetPos() - GetPos()).Normalize();

		moveVector = moveVector * _rushSpeed * deltaTime;

		Protocol::ObjectInfo newInfo = *GetObjectInfo();
		newInfo.set_x(GetPos().x + moveVector.x);
		newInfo.set_y(GetPos().y + moveVector.y);

		SetObjectInfo(newInfo);
	}
}

void Boss::Rush_END(GameObjectRef target, float deltaTime)
{

}

void Boss::DotSkill(GameObjectRef target)
{

}


void Boss::StartSpawnPillars()
{
	
	_attackDelay = 2.f;
}


void Boss::SpawnPillars(float deltaTime)
{
	RoomRef room = GetRoomRef();

	if (room == nullptr)
		return;
	
	if (_attackDelay <= 0.f)
	{
		switch (_montageType)
		{
		case MONTAGE_TYPE_NONE:
			_montageType = MONTAGE_TYPE_START;
			{
				// 필라 스폰
				Protocol::S_MONTAGE montagePkt;
				*montagePkt.mutable_caster() = *GetObjectInfo();
				montagePkt.set_id(1);
				montagePkt.set_isstop(false);
				montagePkt.set_section_num(3);
				montagePkt.set_scalable(true);
				montagePkt.set_duration(3.f);
				room->HandleMontage(montagePkt);
				_attackDelay = 2.f;
			}
			break;

		case MONTAGE_TYPE_START:
			_montageType = MONTAGE_TYPE_ING;
			{
				Protocol::S_SPAWN spawnPkt;

				Vector d[4] =
				{
					{1, 1},
					{1, -1},
					{-1, 1},
					{-1, -1}
				};

				for (int32 i = 0; i < 4; i++)
				{
					Vector pos = d[i] * 1000;
					StructureRef structure = room->SpawnStructure(pos);
					_pillars.push_back(structure);
					Protocol::ObjectInfo* info = spawnPkt.add_info();
					info->CopyFrom(*structure->GetObjectInfo());

				}

				room->DoAsync(&Room::HandleSpawn, spawnPkt);
				_attackDelay = 1.f;
			}
			break;

		case MONTAGE_TYPE_ING:
			_montageType = MONTAGE_TYPE_END;
			_attackDelay = 1.f;
			break;
		case MONTAGE_TYPE_END:
		{
			_skillType = EBST_RUSH;
			_montageType = MONTAGE_TYPE_NONE;
			_pillarNum = Utils::GetRandom(0, (int32)_pillars.size() - 1);
			StartRush();
		}
			break;
		}
	}
	
	switch (_montageType)
	{
	case MONTAGE_TYPE_START:
		SpawnPillars_START();
		break;

	case MONTAGE_TYPE_ING:
		SpawnPillars_ING();
		break;

	case MONTAGE_TYPE_END:
		SpawnPillars_END();
		break;
	}
		
	
}

void Boss::SpawnPillars_START()
{

}

void Boss::SpawnPillars_ING()
{

}

void Boss::SpawnPillars_END()
{

}


void Boss::StartTeleport()
{
	// 아래로 들어가기
	RoomRef room = GetRoomRef();

	if (room == nullptr)
		return;

	{
		Protocol::S_MONTAGE montagePkt;
		*montagePkt.mutable_caster() = *GetObjectInfo();
		montagePkt.set_id(1);
		montagePkt.set_isstop(false);
		montagePkt.set_section_num(1);
		montagePkt.set_scalable(true);
		montagePkt.set_duration(2.f);
		room->HandleMontage(montagePkt);
	}

	_attackDelay = 3.f;
}

void Boss::Teleport(Vector pos, float deltaTime)
{
	RoomRef room = GetRoomRef();

	if (room == nullptr)
		return;

	if (_attackDelay <= 0.f)
	{
		switch (_montageType)
		{
		case MONTAGE_TYPE_NONE:
			_montageType = MONTAGE_TYPE_START;
			{
				Protocol::ObjectInfo info;
				info.CopyFrom(*GetObjectInfo());

				info.set_x(pos.x);
				info.set_y(pos.y);
				info.set_yaw(0.f);

				SetObjectInfo(info, true, true);
				_attackDelay = 1.f;
			}
			break;

		case MONTAGE_TYPE_START:
			_montageType = MONTAGE_TYPE_ING;
			{
				// 위로 올라오기
				Protocol::S_MONTAGE montagePkt;
				*montagePkt.mutable_caster() = *GetObjectInfo();
				montagePkt.set_id(1);
				montagePkt.set_isstop(false);
				montagePkt.set_section_num(2);
				montagePkt.set_scalable(true);
				montagePkt.set_duration(2.f);
				room->HandleMontage(montagePkt);
				_attackDelay = 2.f;
			}
			break;

		case MONTAGE_TYPE_ING:
			_montageType = MONTAGE_TYPE_END;
			_attackDelay = 1.f;
			break;
		case MONTAGE_TYPE_END:
		{
			_skillType = EBST_SPAWNPILLAS;
			_montageType = MONTAGE_TYPE_NONE;
			StartSpawnPillars();
		}
		break;
		}
	}
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

void Boss::TakeDamage(GameObjectRef instigator, Protocol::DamageType damageType, float damage)
{
	Super::TakeDamage(instigator, damageType, damage);

	RoomRef room = GetRoomRef();

	switch (_skillType)
	{
	case EBossSkillType::EBST_RUSH:
	{
		switch (_montageType)
		{
		case MONTAGE_TYPE_ING:
		{
			_montageType = MONTAGE_TYPE_END;
			{
				Protocol::S_MONTAGE montagePkt;
				*montagePkt.mutable_caster() = *GetObjectInfo();
				montagePkt.set_id(1);
				montagePkt.set_section_num(4);
				montagePkt.set_isstop(false);
				montagePkt.set_scalable(true);
				montagePkt.set_duration(2.f);
				room->HandleMontage(montagePkt);
				_attackDelay = 2.f;
			}
		}
			break;
		}
	}
		break;
	}
}