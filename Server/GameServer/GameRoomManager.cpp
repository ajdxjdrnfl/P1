#include "pch.h"
#include "GameRoomManager.h"
#include "Player.h"
#include "Room.h"
#include "Field.h"
#include "Raid.h"
#include "GameSession.h"

GameRoomManager GRoomManager;
atomic<int64> GameRoomManager::s_idGenerator = 1;

GameRoomManager::GameRoomManager()
{

}

GameRoomManager::~GameRoomManager()
{

}

void GameRoomManager::Update(float deltaTime)
{
	WRITE_LOCK;
	/*for (auto& p : _rooms)
	{
		RoomRef room = p.second;
		room->DoAsync(&Room::Update, deltaTime);
	}*/

	GRaid->Update(deltaTime);
	GField->Update(deltaTime);
}

void GameRoomManager::Init()
{
	GRaid->Init();
	GField->Init();
}

void GameRoomManager::EnterGame(GameSessionRef session, Protocol::C_ENTER_GAME pkt)
{
	WRITE_LOCK;

	// 이미 Room에 들어가 있다면 종료
	PlayerRef player = session->_player.load();

	if (player)
	{
		if (RoomRef room = player->GetRoomRef())
			room->HandleLeaveGame(session);
	}

	RoomRef room;

	switch (pkt.roomtype())
	{
	case Protocol::ROOM_TYPE_FIELD:
		room = GField;
		break;

	case Protocol::ROOM_TYPE_DUNGEON:
		room = GRaid;
		break;
	}

	if(room)
		room->DoAsync(&Room::HandleEnterGame, session, pkt);
}

void GameRoomManager::EnterGame(GameSessionRef session, Protocol::C_LOGIN pkt)
{
	WRITE_LOCK;
	RoomRef room = GField;

	Protocol::C_ENTER_GAME enterPkt;
	enterPkt.set_castertype(Protocol::CASTER_TYPE_ARCHER);

	if (room)
		room->DoAsync(&Room::HandleEnterGame, session, enterPkt);
}

void GameRoomManager::ExitGame(GameSessionRef session)
{
	WRITE_LOCK;
	RoomRef room = session->_player.load()->GetRoomRef();

	if (room)
		room->DoAsync(&Room::HandleLeaveGame, session);
}

void GameRoomManager::EnterRaid(GameSessionRef session)
{
	WRITE_LOCK;
	RoomRef room = GRaid;

	RoomRef prevRoom = session->_player.load()->GetRoomRef();

	/*if (room)
		room->DoAsync(&Room::HandleEnterGame, session, enterPkt);
	*/
	
	if (prevRoom)
		prevRoom->DoAsync(&Room::HandleLeaveGame, session);
}

bool GameRoomManager::ChangeRoom(PlayerRef player)
{
	return false;
}

RoomRef GameRoomManager::CreateRoom(ERoomType roomType)
{
	RoomRef room;

	uint64 roomId = s_idGenerator.fetch_add(1);

	switch (roomType)
	{
	case ERoomType::ERT_FIELD:
		room = make_shared<Field>(roomId);
		break;
	case ERoomType::ERT_RAID:
		room = make_shared<Raid>(roomId);
		break;
	}
	
	{
		WRITE_LOCK;
		_rooms[roomType][roomId] = room;
	}
	
	return room;
}
