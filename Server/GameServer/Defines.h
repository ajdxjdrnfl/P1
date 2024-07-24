#pragma once

#define SEND_PACKET(pkt)													\
	SendBufferRef sendBuffer = ServerPacketHandler::MakeSendBuffer(pkt);	\
	session->Send(sendBuffer);

#define USING_SHARED_PTR(name) \
	using name##Ref = shared_ptr<class name>;

#define LOG(pkt) LogUtils::LogPacket(__FUNCTION__, pkt);