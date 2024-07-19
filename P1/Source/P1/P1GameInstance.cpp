// Fill out your copyright notice in the Description page of Project Settings.


#include "P1GameInstance.h"
#include "Common/TcpSocketBuilder.h"
#include "Sockets.h"
#include "Serialization/ArrayWriter.h"
#include "SocketSubsystem.h"
#include "Network/PacketSession.h"
#include "Protocol.pb.h"
#include "ClientPacketHandler.h"

void UP1GameInstance::ConnectToGameServer()
{

	ClientPacketHandler::Init();
	Socket = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateSocket(TEXT("Stream"), TEXT("Client Socket"));

	FIPv4Address Ip;
	FIPv4Address::Parse(IpAddress, Ip);

	TSharedRef<FInternetAddr> InternetAddr = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();
	InternetAddr->SetIp(Ip.Value);
	InternetAddr->SetPort(Port);

	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Connectiong To Server")));

	bool Connected = Socket->Connect(*InternetAddr);

	if (Connected)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Connection Success")));

		// TODO : Session
		GameServerSession = MakeShared<PacketSession>(Socket);
		GameServerSession->Run();

		// TODO : 로비에서 캐릭터 선택
		{
			Protocol::C_LOGIN Pkt;
			SendBufferRef SendBuffer = ClientPacketHandler::MakeSendBuffer(Pkt);
			SendPacket(SendBuffer);
		}
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Connection Failed")));
	}
}


void UP1GameInstance::DisconnectFromGameServer()
{
	if (Socket == nullptr || GameServerSession == nullptr)
		return;

	//Protocol::C_LEAVE_GAME LeavePkt;
	//SEND_PACKET(LeavePkt);
}

void UP1GameInstance::HandleRecvPackets()
{
	if (Socket == nullptr || GameServerSession == nullptr)
		return;

	GameServerSession->HandleRecvPackets();
}

void UP1GameInstance::SendPacket(SendBufferRef SendBuffer)
{
	if (Socket == nullptr || GameServerSession == nullptr)
		return;

	GameServerSession->SendPacket(SendBuffer);
}