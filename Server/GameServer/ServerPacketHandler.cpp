#include "pch.h"
#include "ServerPacketHandler.h"
#include "BufferReader.h"
#include "BufferWriter.h"
#include "GameSession.h"
#include "Player.h"
#include "Room.h"
#include "GameRoomManager.h"

PacketHandlerFunc GPacketHandler[UINT16_MAX];

bool Handle_INVALID(PacketSessionRef& session, BYTE* buffer, int32 len)
{
	PacketHeader* header = reinterpret_cast<PacketHeader*>(buffer);
	
	return false;
}

bool Handle_C_LOGIN(PacketSessionRef& session, Protocol::C_LOGIN& pkt)
{
	auto gameSession = static_pointer_cast<GameSession>(session);

	if (gameSession == nullptr)
		return false;

	LOG(pkt);
	GRoomManager.EnterGame(gameSession);

	return true;
}

bool Handle_C_LEAVE_GAME(PacketSessionRef& session, Protocol::C_LEAVE_GAME& pkt)
{
	auto gameSession = static_pointer_cast<GameSession>(session);

	if (gameSession == nullptr)
		return false;

	LOG(pkt);
	GRoomManager.ExitGame(gameSession);
	
	return true;
}

bool Handle_C_MOVE(PacketSessionRef& session, Protocol::C_MOVE& pkt)
{
	auto gameSession = static_pointer_cast<GameSession>(session);

	if (gameSession == nullptr)
		return false;

	PlayerRef player = gameSession->_player.load();
	if (player == nullptr)
		return false;

	RoomRef room = player->GetRoomRef();
	if (room == nullptr)
		return false;

	LOG(pkt);
	room->DoAsync(&Room::HandleMove, pkt);

	return true;
}

bool Handle_C_SKILL(PacketSessionRef& session, Protocol::C_SKILL& pkt)
{
	auto gameSession = static_pointer_cast<GameSession>(session);

	if (gameSession == nullptr)
		return false;

	PlayerRef player = gameSession->_player.load();
	if (player == nullptr)
		return false;

	RoomRef room = player->GetRoomRef();
	if (room == nullptr)
		return false;

	LOG(pkt);
	room->DoAsync(&Room::HandleSkillPkt, pkt);
	return true;
}

bool Handle_C_ATTACK(PacketSessionRef& session, Protocol::C_ATTACK& pkt)
{
	auto gameSession = static_pointer_cast<GameSession>(session);

	if (gameSession == nullptr)
		return false;

	PlayerRef player = gameSession->_player.load();
	if (player == nullptr)
		return false;

	RoomRef room = player->GetRoomRef();
	if (room == nullptr)
		return false;

	LOG(pkt);
	room->DoAsync(&Room::HandleAttack, pkt);
	return true;
}

bool Handle_C_MONTAGE(PacketSessionRef& session, Protocol::C_MONTAGE& pkt)
{
	auto gameSession = static_pointer_cast<GameSession>(session);

	if (gameSession == nullptr)
		return false;

	PlayerRef player = gameSession->_player.load();
	if (player == nullptr)
		return false;

	RoomRef room = player->GetRoomRef();
	if (room == nullptr)
		return false;

	LOG(pkt);
	room->DoAsync(&Room::HandleMontagePkt, pkt);
	return true;
}
