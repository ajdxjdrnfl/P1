#include "pch.h"
#include "Boss.h"
#include "Collision.h"
#include "Collider.h"
#include "Player.h"
#include "Room.h"
#include "Structure.h"
#include "Skill.h"

Boss::Boss(RoomRef room, uint64 phaseNumber) : GameObject(room), _phaseNumber(phaseNumber)
{
	_objectInfo->set_hp(1000.f);
	_objectInfo->set_max_hp(1000.f);
	_objectInfo->set_castertype(Protocol::CASTER_TYPE_BOSS);
	_objectInfo->set_speed(_moveSpeed);
}

Boss::~Boss()
{
}

void Boss::Update(float deltaTime)
{
	_target = FindClosestTarget();
	_elapsedPacket += deltaTime;
	
	PlayerRef target = _target.lock();

	if (target == nullptr)
		return;

	Super::Update(deltaTime);

	if (_elapsedPacket >= _updatePacketCooldown)
	{
		_elapsedPacket = 0.f;
		_dirtyFlag = true;
	}

	BroadcastUpdate();
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
	if(_objectInfo->hp() <= 500.f && !_isGimmik && _bossPhase == 1)
	{
		_isGimmik = true;
		_bossPhase = 2;
		SelectSkill();
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

	GameObjectRef target = _target.lock();

	if(target == nullptr)
		SetState(Protocol::MOVE_STATE_IDLE, true);

	float dist = GetPos().Distance(_targetPos);
	// 충분히 가까울 때
	if ( dist <= 10.f)
	{
		Protocol::ObjectInfo newInfo = *GetObjectInfo();
		newInfo.set_x(_targetPos.x);
		newInfo.set_y(_targetPos.y);
	
		SetObjectInfo(newInfo);
		SetState(Protocol::MOVE_STATE_IDLE, true);
		
	}
	else
	{

		float distance = target->GetPos().Distance(GetPos());

		if (distance >= _teleportRange)
		{
			StartTeleport();
			SetState(Protocol::MOVE_STATE_SKILL, true);
			return;
		}

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

	GameObjectRef target = _target.lock();
	if (target == nullptr)
	{
		SetState(Protocol::MOVE_STATE_IDLE, true);
		return;
	}

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
			if (_isGimmik)
			{
				target = _pillars[GetPillarNum()].lock();
			}
			else
			{

			}
			Rush(target, deltaTime);
		}
			break;
		case EBST_SPAWNPILLAS:
			SpawnPillars(deltaTime);
			break;
		case EBST_TELEPORT:
		{
			Vector pos;
			if (_isGimmik)
				pos = { 0.f , 0.f };
			else
			{
				float yaw = Utils::GetRandom(-180.f, 180.f);
				Vector randomVector = Utils::GetVectorByYaw(yaw);

				Collision* victimCollision = static_cast<Collision*>(GetComponent(EComponentType::ECT_COLLISION));
				Collision* pillarCollision = static_cast<Collision*>(target->GetComponent(EComponentType::ECT_COLLISION));

				float radius = static_cast<ColliderCircle*>(victimCollision->GetCollider())->_radius + static_cast<ColliderCircle*>(pillarCollision->GetCollider())->_radius;
				
				randomVector = randomVector* radius;

				pos = target->GetPos() + randomVector;
			}
			Teleport(pos, deltaTime);
		}
			
			break;
		}
		
	}
	
	if (!_forceNext && _attackDelay <= 0.f)
	{
		_attackDelay = 0.f;
		_skillType = EBST_NONE;
		_montageType = MONTAGE_TYPE_NONE;
		SetState(Protocol::MOVE_STATE_IDLE, true);
	}
	
	_forceNext = false;
}

void Boss::TickStun(float deltaTime)
{
	Super::TickStun(deltaTime);

}

void Boss::TickDead(float deltaTime)
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

PlayerRef Boss::FindClosestTarget()
{
	RoomRef room = GetRoomRef();

	if (room == nullptr)
		return nullptr;

	return static_pointer_cast<Player>(room->FindClosestPlayer(GetPos(), FLT_MAX));
}

void Boss::StartDefaultAttack()
{
	_skillType = EBST_DEFAULT;
	_attackDelay = 1.f;
	_skillId = 0;

	
}

void Boss::StartRush()
{
	_skillType = EBST_RUSH;
	_attackDelay = 1.f;

	if (_isGimmik)
	{
		_skillId = 1;
	}
	else
	{
		_skillId = 2;
	}
}

void Boss::SelectSkill()
{
	
	RoomRef room = GetRoomRef();

	if (room == nullptr)
		return;

	if (_isGimmik)
	{
		StartTeleport();
		SetState(Protocol::MOVE_STATE_SKILL, true);
		return;
	}

	else
	{
		PlayerRef player = static_pointer_cast<Player>(room->FindClosestPlayer(GetPos(), FLT_MAX));

		_target = player;

		float distance = player->GetPos().Distance(GetPos());

		if(distance <= _attackRange)
		{
			StartDefaultAttack();
			StartRush();
			SetState(Protocol::MOVE_STATE_SKILL, true);
			return;
		}
		else if(distance >= _teleportRange)
		{
			StartTeleport();
			SetState(Protocol::MOVE_STATE_SKILL, true);
			return;
		}
		else
		{
			SetState(Protocol::MOVE_STATE_IDLE, true);
			return;
		}
	}

	
}

void Boss::DefaultAttack(GameObjectRef target)
{
	RoomRef room = GetRoomRef();

	if (_attackDelay < 0.f)
	{
		switch (_montageType)
		{
		case MONTAGE_TYPE_NONE:
		{
			float yaw = Utils::GetYawByVector((target->GetPos() - GetPos()).Normalize());
			Protocol::ObjectInfo info;
			info.CopyFrom(*GetObjectInfo());
			info.set_yaw(yaw);
			SetObjectInfo(info, false, true);

			_targetPos = target->GetPos();
			Protocol::S_MONTAGE montagePkt;
			*montagePkt.mutable_caster() = *GetObjectInfo();
			montagePkt.set_id(_skillId);
			montagePkt.set_isstop(false);
			montagePkt.set_section_num(1);
			montagePkt.set_scalable(true);
			montagePkt.set_duration(2.f);

			_attackDelay = 0.5f;
			room->HandleMontage(montagePkt);

			_montageType = MONTAGE_TYPE_ING;
		}
		break;
		case MONTAGE_TYPE_ING:
		{
			_attackDelay = 1.5f;
			room->CreateSkillActor(shared_from_this(), (uint64)0, { _targetPos.x, _targetPos.y }, GetObjectInfo()->yaw(), 30.f);
			_montageType = MONTAGE_TYPE_END;
		}
		case MONTAGE_TYPE_END:
			Protocol::S_MONTAGE montagePkt;
			*montagePkt.mutable_caster() = *GetObjectInfo();
			montagePkt.set_id(_skillId);
			montagePkt.set_isstop(true);
			montagePkt.set_section_num(0);
			room->HandleMontage(montagePkt);
			break;
		}
	}
	
}

void Boss::Rush(GameObjectRef target, float deltaTime)
{
	if (!_isGimmik && target == nullptr)
		return;

	RoomRef room = GetRoomRef();


	if (room == nullptr)
		return;

	if (_attackDelay <= 0.f)
	{
		Protocol::S_MONTAGE montagePkt;

		switch (_montageType)
		{
		case MONTAGE_TYPE_NONE:
			_montageType = MONTAGE_TYPE_START;
			{
				Protocol::ObjectInfo info;
				info.CopyFrom(*GetObjectInfo());
				info.set_speed(_rushSpeed);
				SetObjectInfo(info, false, true);
			}
			if(_isGimmik)
			{
				// 준비자세
				*montagePkt.mutable_caster() = *GetObjectInfo();
				montagePkt.set_id(_skillId);
				montagePkt.set_section_num(4);
				montagePkt.set_isstop(false);
				montagePkt.set_scalable(true);
				montagePkt.set_duration(2.f);
				room->HandleMontage(montagePkt);
				_attackDelay = 2.f;
			}
			else
			{
				// 준비자세
				*montagePkt.mutable_caster() = *GetObjectInfo();
				montagePkt.set_id(_skillId);
				montagePkt.set_section_num(1);
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
				_rushVector = (target->GetPos() - GetPos()).Normalize();
				// TODO : 스킬액터 스폰
				room->DoAsync(&Room::CreateSkillActor, shared_from_this(), _skillId, GetPos(), _objectInfo->yaw(), 100.f);
			}
			if(_isGimmik)
			{
				// 달려가기
				*montagePkt.mutable_caster() = *GetObjectInfo();
				montagePkt.set_id(_skillId);
				montagePkt.set_section_num(5);
				montagePkt.set_isstop(false);
				montagePkt.set_scalable(true);
				montagePkt.set_duration(10.f);
				room->HandleMontage(montagePkt);
				_attackDelay = 10.f;
			}
			else
			{
				// 달려가기
				*montagePkt.mutable_caster() = *GetObjectInfo();
				montagePkt.set_id(_skillId);
				montagePkt.set_section_num(2);
				montagePkt.set_isstop(false);
				montagePkt.set_scalable(true);
				montagePkt.set_duration(2.f);
				room->HandleMontage(montagePkt);
				_attackDelay = 2.f;
			}
			break;

		case MONTAGE_TYPE_ING:
			_montageType = MONTAGE_TYPE_END;
			if(_isGimmik)
			{
				// 후딜레이
				*montagePkt.mutable_caster() = *GetObjectInfo();
				montagePkt.set_id(_skillId);
				montagePkt.set_section_num(6);
				montagePkt.set_isstop(false);
				montagePkt.set_scalable(true);
				montagePkt.set_duration(2.f);
				room->HandleMontage(montagePkt);
				_attackDelay = 2.f;
			}
			else
			{
				// 후딜레이 - 기믹이 아닐때
				/**montagePkt.mutable_caster() = *GetObjectInfo();
				montagePkt.set_id(_skillId);
				montagePkt.set_section_num(3);
				montagePkt.set_isstop(false);
				montagePkt.set_scalable(true);
				montagePkt.set_duration(2.f);
				room->HandleMontage(montagePkt);
				*/
				_attackDelay = 1.f;
			}
			break;

		case MONTAGE_TYPE_END:
			{
			*montagePkt.mutable_caster() = *GetObjectInfo();
			montagePkt.set_id(_skillId);
			montagePkt.set_section_num(0);
			montagePkt.set_isstop(true);
			room->HandleMontage(montagePkt);
			}
			if (_isGimmik && _pillarCount < _pillars.size())
			{
				_montageType = MONTAGE_TYPE_NONE;
				StartRush();
			}
			else
			{
				{
					Protocol::ObjectInfo info;
					info.CopyFrom(*GetObjectInfo());
					info.set_speed(_moveSpeed);
					SetObjectInfo(info, false , true);
				}
				if (_isGimmik)
				{

					_isGimmik = false;

					Protocol::S_DESPAWN despawnPkt;
					for (int32 i = 0; i < _pillars.size(); i++)
					{
						StructureRef pillar = _pillars[i].lock();
						if (pillar)
							despawnPkt.add_info()->CopyFrom(*pillar->GetObjectInfo());

					}
					room->DoAsync(&Room::HandleDespawn, despawnPkt, true);
				}
			}
		
			break;
		}

	}

	switch (_montageType)
	{
	case MONTAGE_TYPE_START:
		Rush_START(target, deltaTime);
		break;

	case MONTAGE_TYPE_ING:
 		Rush_ING(target, deltaTime);
		break;

	case MONTAGE_TYPE_END:
		Rush_END(target, deltaTime);
		break;

	}
}

void Boss::Rush_START(GameObjectRef target, float deltaTime)
{
	// 패링 처리
	{
		Protocol::ObjectInfo info;
		info.CopyFrom(*GetObjectInfo());
		info.set_yaw(Utils::GetYawByVector((target->GetPos() - GetPos()).Normalize()));
		SetObjectInfo(info, false, true);
;	}
}

void Boss::Rush_ING(GameObjectRef target, float deltaTime)
{
	if (target == nullptr)
		return;

	RoomRef room = GetRoomRef();

	if (room == nullptr)
		return;

	if (_isGimmik)
	{
		float dist = GetPos().Distance(target->GetPos());

		Collision* victimCollision = static_cast<Collision*>(GetComponent(EComponentType::ECT_COLLISION));
		Collision* pillarCollision = static_cast<Collision*>(target->GetComponent(EComponentType::ECT_COLLISION));

		// pillar와 충돌이 일어났을때
		if (victimCollision->CheckCollision(pillarCollision))
		{
			_attackDelay = 0.0f;
			_forceNext = true;

			Vector moveVector = (GetPos() - target->GetPos()).Normalize();
			float radius = static_cast<ColliderCircle*>(victimCollision->GetCollider())->_radius + static_cast<ColliderCircle*>(pillarCollision->GetCollider())->_radius;

			moveVector = moveVector * radius;

			Protocol::ObjectInfo newInfo = *GetObjectInfo();
			newInfo.set_x(GetPos().x + moveVector.x);
			newInfo.set_y(GetPos().y + moveVector.y);

			SetObjectInfo(newInfo, false, true);
			{
				room->CreateSkillActor(shared_from_this(), 1, target->GetPos(), 0.f, 100.f);
			}

			{
				Protocol::S_DESPAWN despawnPkt;
				despawnPkt.add_info()->CopyFrom(*target->GetObjectInfo());
				room->HandleDespawn(despawnPkt, true);
			}
		}
		else
		{

			Vector moveVector = (target->GetPos() - GetPos()).Normalize();

			moveVector = moveVector * _rushSpeed * deltaTime;

			Protocol::ObjectInfo newInfo = *GetObjectInfo();
			newInfo.set_x(GetPos().x + moveVector.x);
			newInfo.set_y(GetPos().y + moveVector.y);

			SetObjectInfo(newInfo, false, true);
		}
	}

	else
	{
		// TODO : 플레이어와 충돌 처리
		Vector moveVector = _rushVector;
		moveVector = moveVector * _rushSpeed * min(deltaTime, _attackDelay);
		if(room->CanGoByVector(static_cast<Collision*>(GetComponent(EComponentType::ECT_COLLISION)), moveVector))
		{
			Protocol::ObjectInfo newInfo = *GetObjectInfo();
			newInfo.set_x(GetPos().x + moveVector.x);
			newInfo.set_y(GetPos().y + moveVector.y);

			SetObjectInfo(newInfo, false, true);
		}
		else
		{
 			_attackDelay = 0.f;
			_forceNext = true;
		}
	}
	
}

void Boss::Rush_END(GameObjectRef target, float deltaTime)
{

}

int32 Boss::GetPillarNum()
{
	if (GameObjectRef pillar = _pillars[_pillarNum].lock())
	{
		return _pillarNum;
	}

	_pillarNum = Utils::GetRandom(0, (int32)_pillars.size() - 1);
	_pillarCount = 0;
	while (_pillarCount < _pillars.size())
	{
		if (GameObjectRef pillar = _pillars[_pillarNum].lock())
		{
			break;
		}
		else
		{
			_pillarCount++;
			_pillarNum = (_pillarNum + 1) % _pillars.size();
		}
	}
	return _pillarNum;
}

void Boss::StartDot()
{
	_skillType = EBST_DOT;
	_attackDelay = 1.f;
	_skillId = 3;
}

void Boss::DotSkill(GameObjectRef target, float deltaTime)
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
				*montagePkt.mutable_caster() = *GetObjectInfo();
				montagePkt.set_id(_skillId);
				montagePkt.set_section_num(1);
				montagePkt.set_isstop(false);
				montagePkt.set_scalable(true);
				montagePkt.set_duration(1.f);
				room->HandleMontage(montagePkt);
				_attackDelay = 1.f;
			}
			// Predict 패킷 전송
			{
				Protocol::S_PREDICTSKILL predictPkt;
				predictPkt.mutable_caster();
				predictPkt.skillid();
				predictPkt.set_x(target->GetPos().x);
				predictPkt.set_y(target->GetPos().y);
				predictPkt.set_z(100.f);
				predictPkt.set_yaw(GetObjectInfo()->yaw());
				
				room->DoAsync(&Room::HandlePredictSkill, predictPkt);
			}
			// 스킬 액터 생성
			{
				room->DoAsync(&Room::CreateSkillActor, shared_from_this(), _skillId, { _targetPos.x, _targetPos.y }, GetObjectInfo()->yaw(), 30.f);
			}
			break;
		case MONTAGE_TYPE_START:
			_montageType = MONTAGE_TYPE_ING;
			{
				*montagePkt.mutable_caster() = *GetObjectInfo();
				montagePkt.set_id(_skillId);
				montagePkt.set_section_num(2);
				montagePkt.set_isstop(false);
				montagePkt.set_scalable(true);
				montagePkt.set_duration(1.f);
				room->HandleMontage(montagePkt);
				_attackDelay = 2.f;
			}
			break;

		case MONTAGE_TYPE_ING:
			_montageType = MONTAGE_TYPE_END;
			{
				*montagePkt.mutable_caster() = *GetObjectInfo();
				montagePkt.set_id(_skillId);
				montagePkt.set_section_num(0);
				montagePkt.set_isstop(true);
				room->HandleMontage(montagePkt);
				_attackDelay = 1.f;
			}
			break;

		case MONTAGE_TYPE_END:

			break;
		}

	}

	switch (_montageType)
	{
	case MONTAGE_TYPE_START:
		DotSkill_START(target, deltaTime);
		break;
	case MONTAGE_TYPE_ING:
		DotSkill_ING(target, deltaTime);
		break;

	case MONTAGE_TYPE_END:
		DotSkill_END(target, deltaTime);
		break;
	}
}


void Boss::DotSkill_START(GameObjectRef target, float deltaTime)
{

}

void Boss::DotSkill_ING(GameObjectRef target, float deltaTime)
{

}

void Boss::DotSkill_END(GameObjectRef target, float deltaTime)
{

}


void Boss::StartSpawnPillars()
{
	_skillType = EBST_SPAWNPILLAS;
	_attackDelay = 2.f;
	_skillId = 1;
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
				montagePkt.set_id(_skillId);
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
			if (_isGimmik)
			{
				_montageType = MONTAGE_TYPE_NONE;
				StartRush();
			}
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
	RoomRef room = GetRoomRef();

	if (room == nullptr)
		return;

	_skillType = EBST_TELEPORT;
	_skillId = 1;

	_attackDelay = 0.5f;
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
				// 아래로 들어가기
				Protocol::S_MONTAGE montagePkt;
				*montagePkt.mutable_caster() = *GetObjectInfo();
				montagePkt.set_id(_skillId);
				montagePkt.set_isstop(false);
				montagePkt.set_section_num(1);
				montagePkt.set_scalable(true);
				montagePkt.set_duration(2.f);
				room->HandleMontage(montagePkt);
				_attackDelay = 2.5f;
			}
			break;

		case MONTAGE_TYPE_START:
			_montageType = MONTAGE_TYPE_ING;
			{
				// 위치로 순간이동
				Protocol::ObjectInfo info;
				info.CopyFrom(*GetObjectInfo());

				info.set_x(pos.x);
				info.set_y(pos.y);
				info.set_yaw(0.f);

				SetObjectInfo(info, true, true);
				_attackDelay = 1.f;
			}
			break;

		case MONTAGE_TYPE_ING:
			_montageType = MONTAGE_TYPE_END;
			{
				// 위로 올라오기
				Protocol::S_MONTAGE montagePkt;
				*montagePkt.mutable_caster() = *GetObjectInfo();
				montagePkt.set_id(_skillId);
				montagePkt.set_isstop(false);
				montagePkt.set_section_num(2);
				montagePkt.set_scalable(true);
				montagePkt.set_duration(1.f);
				room->HandleMontage(montagePkt);
				_attackDelay = 2.5f;
			}
			break;
		case MONTAGE_TYPE_END:
		{
			
			if (_isGimmik)
			{
				_montageType = MONTAGE_TYPE_NONE;
				StartSpawnPillars();
			}
			else
			{
				Protocol::S_MONTAGE montagePkt;
				*montagePkt.mutable_caster() = *GetObjectInfo();
				montagePkt.set_id(_skillId);
				montagePkt.set_isstop(true);
				montagePkt.set_section_num(0);
				room->HandleMontage(montagePkt);
			}
		}
		break;
		}
	}
}

void Boss::MoveToTarget(GameObjectRef target)
{
	if (target == nullptr)
		return;

	RoomRef room = GetRoomRef();

	room->GetGridPos(_targetPos);

	vector<VectorInt> path;

	bool found = room->FindPath(GetPos(), target->GetPos(), path, 30);

	if (found)
	{
		if (path.size() > 1)
		{
			_targetPos = room->GetPosition(path[1]);
			Vector targetVector = (_targetPos - GetPos());
			float yaw = Utils::GetYawByVector(targetVector);
			_objectInfo->set_yaw(yaw);
			SetState(Protocol::MOVE_STATE_RUN, true);
		}

	}
}

void Boss::TakeDamage(GameObjectRef instigator, SkillInfo skillInfo, float damage, bool counter)
{

	RoomRef room = GetRoomRef();

	switch (_skillType)
	{
	case EBossSkillType::EBST_RUSH:
	{
		// 패링처리
		if (counter && _isGimmik == false)
		{
			_montageType = MONTAGE_TYPE_END;
			_attackDelay = 0.f;

			Protocol::S_MONTAGE montagePkt;
			*montagePkt.mutable_caster() = *GetObjectInfo();
			montagePkt.set_id(_skillId);
			montagePkt.set_section_num(0);
			montagePkt.set_isstop(true);
			room->HandleMontage(montagePkt);

			Protocol::ObjectInfo info;
			info.CopyFrom(*GetObjectInfo());
			info.set_speed(_moveSpeed);
			SetObjectInfo(info, false, true);

			SetState(Protocol::MOVE_STATE_STUN, true);
			SetCCTime(2.f);
		}
		switch (_montageType)
		{
		case MONTAGE_TYPE_START:
			// TODO : 패링처리
			
			break;
		case MONTAGE_TYPE_ING:
		{
			
		}
			break;
		}
	}
		break;
	}



	Super::TakeDamage(instigator, skillInfo, damage, counter);

}