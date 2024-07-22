#include "pch.h"
#include "Room.h"
#include "Player.h"
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
}

void Room::Update()
{
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

		SendBufferRef sendBuffer = ServerPacketHandler::MakeSendBuffer(pkt);
		if (auto session = player->GetSessionRef())
			session->Send(sendBuffer);

	}
	
	return true;
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

void Room::EnterGame(PlayerRef player)
{
	const uint64 id = player->GetObjectInfo()->object_id();

	if (_players.find(id) != _players.end())
		return;

	_players[id] = player;
	SetPlayerToRandomPos(player);
}

void Room::SetPlayerToRandomPos(PlayerRef player)
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
