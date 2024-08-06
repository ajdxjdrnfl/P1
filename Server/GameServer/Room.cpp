#include "pch.h"
#include "Room.h"
#include "Player.h"
#include "Enemy.h"
#include "SkillActor.h"
#include "GameSession.h"
#include "ObjectUtils.h"
#include "Collision.h"
#include "Boss.h"
#include "Structure.h"
#include "ResourceManager.h"

RoomRef GRoom = make_shared<Room>();

Room::Room()
{
}

Room::~Room()
{
}

void Room::Init()
{
	// TODO : enemy 추가
	_tickManager.Init();
	for (int32 i = 0; i < _maxEnemyCount; i++)
	{
		EnemyRef enemy = ObjectUtils::CreateEnemy(GetRoomRef());
		_enemies[enemy->GetObjectInfo()->object_id()] = enemy;
		SetObjectToRandomPos(enemy);
	}

	_boss = ObjectUtils::CreateBoss(GetRoomRef());
	SetObjectToRandomPos(_boss);
}

void Room::Update(float deltaTime)
{
	_tickManager.Update();

	deltaTime = _tickManager.GetDeltaTime();

	for (auto& item : _players)
	{
		PlayerRef player = item.second;
		player->Update(deltaTime);
	}
	for (auto& item : _enemies)
	{
		EnemyRef enemy = item.second;
		enemy->Update(deltaTime);
	}
	for (auto& item : _skillActors)
	{
		SkillActorRef skillActor = item.second;
		skillActor->Update(deltaTime);
	}

	_boss->Update(deltaTime);

	DoTimer(64, &Room::Update, deltaTime);
}

bool Room::HandleEnterGame(GameSessionRef session)
{
	PlayerRef player = ObjectUtils::CreatePlayer(session, GetRoomRef());
	session->_player.store(player);
	EnterGame(player);

	{
		Protocol::S_ENTER_GAME pkt;
		pkt.set_success(true);

		*pkt.mutable_info() = *player->GetObjectInfo();

		SendBufferRef sendBuffer = ServerPacketHandler::MakeSendBuffer(pkt);
		if (auto session = player->GetSessionRef())
			session->Send(sendBuffer);
		
	}
	
	// 기존 접속한 유저들에게 새로운 유저 접속 전송
	{
		Protocol::S_SPAWN pkt;
		
		Protocol::ObjectInfo* info = pkt.add_info();
		info->CopyFrom(*player->GetObjectInfo());
		
		SendBufferRef sendBuffer = ServerPacketHandler::MakeSendBuffer(pkt);
		Broadcast(sendBuffer, player->GetObjectInfo()->object_id());
	}

	// 접속한 유저에게 존재하는 오브젝트 전송
	{
		Protocol::S_SPAWN pkt;

		for (auto& item : _players)
		{
			PlayerRef player = item.second;
			Protocol::ObjectInfo* info = pkt.add_info();
			info->CopyFrom(*player->GetObjectInfo());
		}

		for (auto& item : _enemies)
		{
			EnemyRef enemy = item.second;
			Protocol::ObjectInfo* info = pkt.add_info();
			info->CopyFrom(*enemy->GetObjectInfo());
		}
		{
			Protocol::ObjectInfo* info = pkt.add_info();
			info->CopyFrom(*_boss->GetObjectInfo());
		}

		// TODO : 새로 접속한 유저에게 스킬 액터들도 같이 스폰할지?

		SendBufferRef sendBuffer = ServerPacketHandler::MakeSendBuffer(pkt);
		if (auto session = player->GetSessionRef())
			session->Send(sendBuffer);

	}
	
	return true;
}

bool Room::HandleLeaveGame(GameSessionRef session)
{
	PlayerRef player = session->_player;
	const uint64 objectId = player->GetObjectInfo()->object_id();
	session->_player.store(nullptr);
	LeaveGame(player);

	// 퇴장하는 플레이어에게 퇴장 사실 전달
	{
		Protocol::S_DESPAWN pkt;

		for (auto& item : _players)
		{
			pkt.add_object_ids(item.first);
		}

		SendBufferRef sendBuffer = ServerPacketHandler::MakeSendBuffer(pkt);

		if (session)
			session->Send(sendBuffer);
	}
	
	{
		/*Protocol::S_LEAVE_GAME pkt;

		SendBufferRef sendBuffer = ServerPacketHandler::MakeSendBuffer(pkt);

		if (session)
			session->Send(sendBuffer);*/
	}

	// 다른 플레이어들에게 퇴장 사실 전달
	{
		Protocol::S_DESPAWN pkt;
		pkt.add_object_ids(objectId);

		SendBufferRef sendBuffer = ServerPacketHandler::MakeSendBuffer(pkt);
		Broadcast(sendBuffer, objectId);
	}
	
	return false;
}

bool Room::HandleMove(Protocol::C_MOVE pkt)
{
	const uint64 objectId = pkt.info().object_id();

	if (_players.find(objectId) == _players.end())
		return false;

	// TODO : 갈 수 있는 곳인지 검증
	// 현재는 움직인 곳으로 바로 이동

	PlayerRef player = _players[objectId];
	player->SetObjectInfo(pkt.info());

	Protocol::S_MOVE movePkt;
	{
		Protocol::ObjectInfo* info = movePkt.mutable_info();
		info->CopyFrom(pkt.info());
	}
	
	SendBufferRef sendBuffer = ServerPacketHandler::MakeSendBuffer(movePkt);
	Broadcast(sendBuffer);

	return true;

}

bool Room::HandleSkillPkt(Protocol::C_SKILL pkt)
{
	GameObjectRef caster = GetGameObjectRef(pkt.caster().object_id());
	
	Skill* skill =  GResourceManager.GetSkill(pkt.caster().castertype(), pkt.skillid());

	if (skill == nullptr)
		return false;

	float damage = skill->GetSkillInfo().damage;

	if (skill->GetSkillInfo().skillType == Protocol::SKILL_TYPE_CHARGING)
		damage = pkt.damage();

	return HandleSkill(caster, pkt.skillid(), { pkt.x(), pkt.y() }, pkt.yaw(), damage);
}

bool Room::HandleAttack(Protocol::C_ATTACK pkt)
{
	GameObjectRef victim = GetGameObjectRef(pkt.victim().object_id());
	GameObjectRef skillActor = GetGameObjectRef(pkt.skillactor().object_id());
	GameObjectRef caster = GetGameObjectRef(pkt.caster().object_id());

	if (victim == nullptr || skillActor == nullptr || caster == nullptr)
		return false;

	Collision* victimCollision = static_cast<Collision*>(victim->GetComponent(EComponentType::ECT_COLLISION));
	Collision* skillActorCollision = static_cast<Collision*>(skillActor->GetComponent(EComponentType::ECT_COLLISION));

	victimCollision->SetPos({pkt.victim().x(), pkt.victim().y()});
	skillActorCollision->SetPos({pkt.skillactor().x(), pkt.skillactor().y()});

	if (!victimCollision->CheckCollision(skillActorCollision))
	{
		victimCollision->SetPos(victim->GetPos());
		skillActorCollision->SetPos(skillActor->GetPos());
		return false;
	}

	SkillActorRef skillActorCast = static_pointer_cast<SkillActor>(skillActor);

	victim->TakeDamage(skillActor, skillActorCast->GetSkillInfo().damageType, skillActorCast->GetDamage());
	
	{
		Protocol::S_ATTACK attackPkt;
		*attackPkt.mutable_caster() = *caster->GetObjectInfo();
		*attackPkt.mutable_victim() = *victim->GetObjectInfo();
		*attackPkt.mutable_skillactor() = *skillActor->GetObjectInfo();
		attackPkt.set_skillid(skillActorCast->GetSkillInfo().skillNum);
		attackPkt.set_damage(skillActorCast->GetDamage());
		
		SendBufferRef sendBuffer = ServerPacketHandler::MakeSendBuffer(attackPkt);
		Broadcast(sendBuffer);
	}

	victimCollision->SetPos(victim->GetPos());
	skillActorCollision->SetPos(skillActor->GetPos());
	
	return true;
}

bool Room::HandleMontagePkt(Protocol::C_MONTAGE pkt)
{
	GameObjectRef caster = GetGameObjectRef(pkt.caster().object_id());

	if (caster == nullptr)
		return false;

	{
		Protocol::S_MONTAGE montagePkt;
		*montagePkt.mutable_caster() = *caster->GetObjectInfo();
		montagePkt.set_isstop(pkt.isstop());
		montagePkt.set_id(pkt.id());
		montagePkt.set_section_num(pkt.section_num());
		montagePkt.set_scalable(false);

		return HandleMontage(montagePkt);
	}
}

void Room::HandleDead(GameObjectRef gameObject)
{
	if (RemoveObject(gameObject))
	{
		Protocol::S_DESPAWN despawnPkt;

		despawnPkt.add_object_ids(gameObject->GetObjectInfo()->object_id());

		SendBufferRef sendBuffer = ServerPacketHandler::MakeSendBuffer(despawnPkt);
		Broadcast(sendBuffer);
	}
	
}

bool Room::HandleSkill(GameObjectRef caster, uint64 skillid, Vector skillActorPos, float yaw, float damage)
{
	if (caster == nullptr)
		return false;

	SkillActorRef skillActor = ObjectUtils::CreateSkillActor(caster, GetRoomRef());
	Protocol::ObjectInfo* info = skillActor->GetObjectInfo();
	Protocol::ObjectInfo* casterInfo = caster->GetObjectInfo();
	info->set_x(skillActorPos.x);
	info->set_y(skillActorPos.y);
	info->set_z(100.f);
	info->set_yaw(yaw);
	skillActor->SetCollisionBySkillId(casterInfo->castertype(), skillid, damage);

	SpawnSkill(skillActor);

	// TODO : S_SKILL 구조 수정
	{
		Protocol::S_SKILL skillPkt;
		*skillPkt.mutable_caster() = *caster->GetObjectInfo();
		*skillPkt.mutable_skillactor() = *skillActor->GetObjectInfo();
		skillPkt.set_skillid(skillid);

		SendBufferRef sendBuffer = ServerPacketHandler::MakeSendBuffer(skillPkt);
		Broadcast(sendBuffer);
	}
	return true;
}

bool Room::HandleMontage(Protocol::S_MONTAGE pkt)
{
	SendBufferRef sendBuffer = ServerPacketHandler::MakeSendBuffer(pkt);
	Broadcast(sendBuffer);
	return false;
}

bool Room::HandleSpawn(Protocol::S_SPAWN pkt)
{
	SendBufferRef sendBuffer = ServerPacketHandler::MakeSendBuffer(pkt);
	Broadcast(sendBuffer);
	return false;
}

StructureRef Room::SpawnStructure(Vector pos)
{
	StructureRef structure = ObjectUtils::CreateStructure(GetRoomRef());
	
	structure->GetObjectInfo()->set_x(pos.x);
	structure->GetObjectInfo()->set_y(pos.y);
	structure->GetObjectInfo()->set_z(100.f);
	structure->GetObjectInfo()->set_yaw(0.f);

	uint64 id = structure->GetObjectInfo()->object_id();
	_structures[id] = structure;


	return structure;
}

void Room::EnterGame(PlayerRef player)
{
	const uint64 id = player->GetObjectInfo()->object_id();

	if (_players.find(id) != _players.end())
		return;

	_players[id] = player;
	SetObjectToRandomPos(player);
}

void Room::SpawnSkill(SkillActorRef skillActor)
{
	const uint64 id = skillActor->GetObjectInfo()->object_id();

	if (_skillActors.find(id) != _skillActors.end())
		return;

	_skillActors[id] = skillActor;
}

void Room::LeaveGame(PlayerRef player)
{
	const uint64 id = player->GetObjectInfo()->object_id();

	if (_players.find(id) == _players.end())
		return;

	_players.erase(id);

}

bool Room::RemoveObject(GameObjectRef gameObject)
{
	const uint64 id = gameObject->GetObjectInfo()->object_id();

	if (_players.find(id) != _players.end())
		return true;

	if (_enemies.find(id) != _enemies.end())
		return true;

	if (_skillActors.find(id) != _skillActors.end())
		return true;

	return false;
}

void Room::SetObjectToRandomPos(GameObjectRef player)
{
	player->GetObjectInfo()->set_x(Utils::GetRandom(-500.f, 500.f));
	player->GetObjectInfo()->set_y(Utils::GetRandom(-500.f, 500.f));
	player->GetObjectInfo()->set_z(100.f);

	player->GetObjectInfo()->set_yaw(Utils::GetRandom(0.f, 100.f));
}

void Room::Broadcast(SendBufferRef sendBuffer, uint64 exceptId)
{
	for (auto& item : _players)
	{
		PlayerRef player = item.second;
		
		if (player->GetObjectInfo()->object_id() == exceptId)
			continue;
		
		if (GameSessionRef gameSession = player->GetSessionRef())
		{
			gameSession->Send(sendBuffer);
		}
	}
}

PlayerRef Room::FindClosestPlayer(Vector pos)
{
	PlayerRef result = nullptr;
	
	for (auto& item : _players)
	{
		if (result == nullptr)
			result = item.second;
		else
		{
			float dist1 = pos.Distance(result->GetPos());
			float dist2 = pos.Distance(item.second->GetPos());

			if (dist1 > dist2)
				result = item.second;
		}
	}

	return result;
}

RoomRef Room::GetRoomRef()
{
	return static_pointer_cast<Room>(shared_from_this());
}

GameObjectRef Room::GetGameObjectRef(uint64 id)
{
	if (_players.find(id) != _players.end())
		return _players[id];

	if (_enemies.find(id) != _enemies.end())
		return _enemies[id];

	if (_skillActors.find(id) != _skillActors.end())
		return _skillActors[id];

	if (_boss->GetObjectInfo()->object_id() == id)
		return _boss;

	if (_structures.find(id) != _structures.end())
		return _structures[id];

	return nullptr;
}

