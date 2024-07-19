#include "pch.h"
#include "ServerPacketHandler.h"
#include "BufferReader.h"
#include "BufferWriter.h"
#include "GameSession.h"


PacketHandlerFunc GPacketHandler[UINT16_MAX];

bool Handle_INVALID(PacketSessionRef& session, BYTE* buffer, int32 len)
{
	PacketHeader* header = reinterpret_cast<PacketHeader*>(buffer);
	
	return false;
}

bool Handle_C_LOGIN(PacketSessionRef& session, Protocol::C_LOGIN& pkt)
{

	Protocol::S_LOGIN loginPkt;

	return true;
}

