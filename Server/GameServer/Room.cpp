#include "pch.h"
#include "Room.h"
#include "Player.h"
#include "Enemy.h"
#include "SkillActor.h"
#include "GameSession.h"
#include "ObjectUtils.h"
#include "Collision.h"
#include "Boss.h"

RoomRef GRoom = make_shared<Room>();

Room::Room()
{
}

Room::~Room()
{
}

void Room::Init()
{
	// TODO : enemy �߰�
	for (int32 i = 0; i < _maxEnemyCount; i++)
	{
		EnemyRef enemy = ObjectUtils::CreateEnemy(GetRoomRef());
		_enemies[enemy->GetObjectInfo()->object_id()] = enemy;
		SetObjectToRandomPos(enemy);
	}

	_boss = ObjectUtils::CreateBoss(GetRoomRef());
}

void Room::Update(float deltaTime)
{
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
	
	// ���� ������ �����鿡�� ���ο� ���� ���� ����
	{
		Protocol::S_SPAWN pkt;
		
		Protocol::ObjectInfo* info = pkt.add_info();
		info->CopyFrom(*player->GetObjectInfo());
		
		SendBufferRef sendBuffer = ServerPacketHandler::MakeSendBuffer(pkt);
		Broadcast(sendBuffer, player->GetObjectInfo()->object_id());
	}

	// ������ �������� �����ϴ� ������Ʈ ����
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

		// TODO : ���� ������ �������� ��ų ���͵鵵 ���� ��������?

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

	// �����ϴ� �÷��̾�� ���� ��� ����
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

	// �ٸ� �÷��̾�鿡�� ���� ��� ����
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

	// TODO : �� �� �ִ� ������ ����
	// ����� ������ ������ �ٷ� �̵�

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

	return HandleSkill(caster, pkt.skillid());
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

	if (!victimCollision->CheckCollision(skillActorCollision))
		return false;

	SkillActorRef skillActorCast = static_pointer_cast<SkillActor>(skillActor);

	victim->TakeDamage(skillActor, skillActorCast->GetSkillInfo().damageType, skillActorCast->GetSkillInfo().damage);
	
	{
		Protocol::S_ATTACK attackPkt;
		*attackPkt.mutable_caster() = *caster->GetObjectInfo();
		*attackPkt.mutable_victim() = *victim->GetObjectInfo();
		*attackPkt.mutable_skillactor() = *skillActor->GetObjectInfo();
		attackPkt.set_skillid(skillActorCast->GetSkillInfo().skillNum);
		
		SendBufferRef sendBuffer = ServerPacketHandler::MakeSendBuffer(attackPkt);
		Broadcast(sendBuffer);
	}
}

bool Room::HandleMontage(Protocol::C_MONTAGE pkt)
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

		SendBufferRef sendBuffer = ServerPacketHandler::MakeSendBuffer(montagePkt);
		Broadcast(sendBuffer);
	}

	return true;
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

bool Room::HandleSkill(GameObjectRef caster, uint64 skillid)
{
	if (caster == nullptr)
		return false;

	SkillActorRef skillActor = ObjectUtils::CreateSkillActor(caster, GetRoomRef());
	Protocol::ObjectInfo* info = skillActor->GetObjectInfo();
	Protocol::ObjectInfo* casterInfo = caster->GetObjectInfo();
	info->set_x(casterInfo->x());
	info->set_y(casterInfo->y());
	info->set_z(casterInfo->z());
	info->set_yaw(casterInfo->yaw());
	skillActor->SetCollisionBySkillId(casterInfo->castertype(), skillid);

	SpawnSkill(skillActor);

	// TODO : S_SKILL ���� ����
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

	if (_players.find(id) != _players.end())
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
	player->GetObjectInfo()->set_z(500.f);

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

	return nullptr;
}
