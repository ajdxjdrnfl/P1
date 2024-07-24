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
#include "P1/Enemy/EnemyBase.h"
#include "P1/Enemy/EnemyMob.h"

void UP1GameInstance::Init()
{
	InitSkillMap();
	Super::Init();
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

	Protocol::C_LEAVE_GAME LeavePkt;
	SEND_PACKET(LeavePkt);
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

		if (info.castertype() == Protocol::CASTER_TYPE_MOB)
		{
			AEnemyMob* SpawnedMob = Cast<AEnemyMob>(GetWorld()->SpawnActor(EnemyMobClass, &Loc));

			if (SpawnedMob == nullptr)
				return;

			SpawnedMob->Info->CopyFrom(info);
			SpawnedMob->InitOnSpawn(Pkt.info(i).hp());
			
			Enemies.Add({ info.object_id(), SpawnedMob });
		}
		else if (info.castertype() == Protocol::CASTER_TYPE_WARRIOR)
		{
			AP1Character* SpawnedActor = Cast<AP1Character>(GetWorld()->SpawnActor(CharacterClass, &Loc));

			if (SpawnedActor == nullptr)
				return;

			SpawnedActor->Info->CopyFrom(info);
			SpawnedActor->InitOnSpawn(Pkt.info(i).hp(), Pkt.info(i).stamina());
			
			Characters.Add({ info.object_id(), SpawnedActor });
		}
		
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

void UP1GameInstance::AttackEnemy(Protocol::S_ATTACK& Pkt)
{
	int32 EnemyID = Pkt.victim().object_id();

	FDamageInfo DamageInfo;
	DamageInfo.Damage = Pkt.skillinfo().damage();

	if (Pkt.skillinfo().damage_type() == Protocol::DAMAGE_TYPE_NORMAL)
	{
		DamageInfo.DamageType = EDamageType::Normal;
	}
	else if (Pkt.skillinfo().damage_type() == Protocol::DAMAGE_TYPE_DOT)
	{
		DamageInfo.DamageType = EDamageType::Dot;
	}
	else if (Pkt.skillinfo().damage_type() == Protocol::DAMAGE_TYPE_BUFF)
	{
		DamageInfo.DamageType = EDamageType::Buff;
	}

	if (Pkt.skillinfo().cc_type() == Protocol::CC_TYPE_NORMAL)
	{
		DamageInfo.CCType = ECCType::Normal;
	}
	else if (Pkt.skillinfo().cc_type() == Protocol::CC_TYPE_SLOW) 
	{
		DamageInfo.CCType = ECCType::Slow;
	}
	else if (Pkt.skillinfo().cc_type() == Protocol::CC_TYPE_STUN)
	{
		DamageInfo.CCType = ECCType::Stun;
	}
	else if (Pkt.skillinfo().cc_type() == Protocol::CC_TYPE_AIRBORNE)
	{
		DamageInfo.CCType = ECCType::Airborne;
	}

	Enemies[EnemyID]->SetHealthByDamage(Pkt.victim().hp(), DamageInfo);
}
