#include "pch.h"
#include "ClientPacketHandler.h"
#include "main.h"
#include "Session.h"

PacketHandlerFunc GPacketHandler[UINT16_MAX];

bool Handle_INVALID(PacketSessionRef& session, BYTE* buffer, int32 len)
{
	return false;
}

bool Handle_S_LOGIN(PacketSessionRef& session, Protocol::S_LOGIN& pkt)
{
	Protocol::C_ENTER_GAME enterPkt;
	enterPkt.set_castertype(Protocol::CASTER_TYPE_WARRIOR);
	enterPkt.set_roomtype(Protocol::ROOM_TYPE_FIELD);

	SendBufferRef sendBuffer = ClientPacketHandler::MakeSendBuffer(enterPkt);
	session->Send(sendBuffer);
	return false;
}

bool Handle_S_ENTER_GAME(PacketSessionRef& session, Protocol::S_ENTER_GAME& pkt)
{
	Protocol::ObjectInfo info = pkt.info();

	ClientSessionRef clientSession = static_pointer_cast<ClientSession>(session);
	clientSession->player = make_shared<DummyPlayer>();
	clientSession->player->session = clientSession;

	clientSession->player->info->CopyFrom(pkt.info());
	clientSession->player->defaultX = pkt.info().x();
	clientSession->player->defaultY = pkt.info().y();

	return false;
}

bool Handle_S_SPAWN(PacketSessionRef& session, Protocol::S_SPAWN& pkt)
{
	return false;
}

bool Handle_S_DESPAWN(PacketSessionRef& session, Protocol::S_DESPAWN& pkt)
{
	return false;
}

bool Handle_S_MOVE(PacketSessionRef& session, Protocol::S_MOVE& pkt)
{
	return false;
}

bool Handle_S_SKILL(PacketSessionRef& session, Protocol::S_SKILL& pkt)
{
	return false;
}

bool Handle_S_ATTACK(PacketSessionRef& session, Protocol::S_ATTACK& pkt)
{

	return false;
}

bool Handle_S_MONTAGE(PacketSessionRef& session, Protocol::S_MONTAGE& pkt)
{
	return false;
}

bool Handle_S_DEAD(PacketSessionRef& session, Protocol::S_DEAD& pkt)
{

	return false;
}

bool Handle_S_PHASE(PacketSessionRef& session, Protocol::S_PHASE& pkt)
{
	return false;
}

bool Handle_S_DAMAGE(PacketSessionRef& session, Protocol::S_DAMAGE& pkt)
{
	return false;
}

bool Handle_S_PREDICTSKILL(PacketSessionRef& session, Protocol::S_PREDICTSKILL& pkt)
{
	return false;
}
