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
#include "Kismet/GameplayStatics.h"
#include "P1/Skill/SkillActorBase.h"

void UP1GameInstance::Init()
{
	Super::Init();

	InitSkillMap();
}

void UP1GameInstance::InitSkillMap()
{
	if (SkillDataTable == nullptr)
		return;

	int idx = 0;
	TArray<FName> RowNames = SkillDataTable->GetRowNames();
	for (FName Row : RowNames)
	{
		FString ContextString;
		FSkillsByClass SkillInfos = *SkillDataTable->FindRow<FSkillsByClass>(Row, ContextString);
		for (FGeneralSkillInfo SkillInfo : SkillInfos.SkillInfos)
		{
			Skills.Add(idx++, SkillInfo);
		}
	}
}

void UP1GameInstance::ConnectToGameServer()
{
	MyCharacter = Cast<AP1Character>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	check(MyCharacter != nullptr);

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
	return MyCharacter->Info->object_id() == ID;
}

void UP1GameInstance::CharacterSpawn(Protocol::S_SPAWN& Pkt)
{
	for (int32 i = 0; i < Pkt.info_size(); i++)
	{
		check(CharacterClass != nullptr);

		if (Characters.Contains(Pkt.info(i).object_id()))
			continue;

		Protocol::ObjectInfo info = Pkt.info(i);
		FVector Loc = FVector(info.x(), info.y(), info.z());
		AP1Character* SpawnedActor = Cast<AP1Character>(GetWorld()->SpawnActor(CharacterClass, &Loc));
		if (SpawnedActor == nullptr)
			return;
		SpawnedActor->Info->CopyFrom(info);
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

void UP1GameInstance::SkillSpawn(Protocol::S_SKILL& Pkt)
{
	if (IsMyCharacter(Pkt.caster().object_id()))
		return;

	FVector SpawnedLocation = FVector(Pkt.skillactor().x(), Pkt.skillactor().y(), Pkt.skillactor().z());
	FRotator SpawnedRotation = FRotator(0, Pkt.skillactor().yaw(), 0);

	ASkillActorBase* SkillActor = Cast<ASkillActorBase>(GWorld->SpawnActor(Skills[Pkt.skillinfo().skill_id()].SkillActor, &SpawnedLocation, &SpawnedRotation));
	if (SkillActor == nullptr)
		return;

	SkillActor->ActivateSkill();
}
