#include "pch.h"
#include "Room.h"
#include "Player.h"
#include "Enemy.h"
#include "SkillActor.h"
#include "GameSession.h"
#include "ObjectUtils.h"

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
	for (int32 i = 0; i < _maxEnemyCount; i++)
	{
		EnemyRef enemy = ObjectUtils::CreateEnemy(GetRoomRef());
		_enemies[enemy->GetObjectInfo()->object_id()] = enemy;
		SetObjectToRandomPos(enemy);
	}
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

bool Room::HandleSkill(Protocol::C_SKILL pkt)
{
	GameObjectRef caster = GetGameObjectRef(pkt.caster().object_id());

	if (caster == nullptr)
		return false;

	SkillActorRef skillActor = ObjectUtils::CreateSkillActor(caster, GetRoomRef());
	Protocol::ObjectInfo* info = skillActor->GetObjectInfo();
	Protocol::ObjectInfo* casterInfo = caster->GetObjectInfo();
	info->set_x(casterInfo->x());
	info->set_y(casterInfo->y());
	info->set_z(casterInfo->z());
	info->set_yaw(casterInfo->yaw());

	// TODO : S_SKILL 구조 수정
	{
		Protocol::S_SKILL skillPkt;
		*skillPkt.mutable_caster() = *caster->GetObjectInfo();
		*skillPkt.mutable_skillactor() = *skillActor->GetObjectInfo();
		*skillPkt.mutable_skillinfo() = pkt.skillinfo();

		SendBufferRef sendBuffer = ServerPacketHandler::MakeSendBuffer(skillPkt);
		Broadcast(sendBuffer, casterInfo->object_id());
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

void Room::LeaveGame(PlayerRef player)
{
	const uint64 id = player->GetObjectInfo()->object_id();

	if (_players.find(id) != _players.end())
		return;

	_players.erase(id);

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

RoomRef Room::GetRoomRef()
{
	return static_pointer_cast<Room>(shared_from_this());
}

GameObjectRef Room::GetGameObjectRef(uint64 id)
{
	if (_players.find(id) != _players.end())
		return _players[id];

	if (_enemies.find(id) != _enemies.end())
		return _players[id];

	if (_skillActors.find(id) != _skillActors.end())
		return _skillActors[id];

	return nullptr;
}
