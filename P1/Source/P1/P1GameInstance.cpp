// Fill out your copyright notice in the Description page of Project Settings.


#include "P1GameInstance.h"
#include "Common/TcpSocketBuilder.h"
#include "Sockets.h"
#include "Serialization/ArrayWriter.h"
#include "SocketSubsystem.h"
#include "Network/PacketSession.h"
#include "Protocol.pb.h"
#include "ClientPacketHandler.h"
#include "P1Character.h"
#include "P1ObjectBase.h"

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

bool UP1GameInstance::IsMyCharacter(uint64 ID)
{
	return MyCharacter->GetObjectBase()->ObjectID == ID;
}

void UP1GameInstance::CharacterSpawn(Protocol::S_SPAWN& Pkt)
{
	for (int32 i = 0; i < Pkt.info_size(); i++)
	{
		Protocol::ObjectInfo info = Pkt.info(i);

		FVector Loc = FVector(info.x(), info.y(), info.z());
		AP1Character* SpawnedActor = Cast<AP1Character>(GetWorld()->SpawnActor(AP1Character::StaticClass(), &Loc));
		SpawnedActor->Info.CopyFrom(info);
		// SpawnedActor->GetObjectBase()->ObjectID = info.object_id();

		Characters.Add({ info.object_id(), SpawnedActor });
	}
}

void UP1GameInstance::CharacterMove(Protocol::S_MOVE& Pkt)
{
	AP1Character* Character = *Characters.Find(Pkt.info().object_id());

	if (Character == nullptr)
		return;

	//Character->MoveByServer();
}
