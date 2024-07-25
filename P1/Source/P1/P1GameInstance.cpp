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
		for (FGeneralSkillInfo CurrentSkillInfo : SkillInfos.SkillInfos)
		{
			SkillInfo.Add(idx++, CurrentSkillInfo);
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

void UP1GameInstance::SpawnActorByServer(Protocol::S_SPAWN& Pkt)
{
	check(CharacterClass != nullptr);
	check(EnemyMobClass != nullptr);

	Protocol::ObjectInfo info;
	FVector Loc;

	for (int32 i = 0; i < Pkt.info_size(); i++)
	{
		if (Characters.Contains(Pkt.info(i).object_id()))
			continue;

		info = Pkt.info(i);
		Loc = FVector(info.x(), info.y(), info.z());

		switch (info.castertype())
		{
		case Protocol::CASTER_TYPE_MOB:
			Enemies.Add({ info.object_id(), SpawnMob(Pkt.info(i), Loc) });
			break;
		case Protocol::CASTER_TYPE_WARRIOR:
			Characters.Add({ info.object_id(), SpawnCharacter(Pkt.info(i), Loc) });
			break;
		default:
			break;
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
	FVector SpawnedLocation = FVector(Pkt.skillactor().x(), Pkt.skillactor().y(), Pkt.skillactor().z());
	FRotator SpawnedRotation = FRotator(0, Pkt.skillactor().yaw(), 0);

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = Characters[Pkt.caster().object_id()];
	SpawnParams.Instigator = Characters[Pkt.caster().object_id()];

	ASkillActorBase* SkillActor = Cast<ASkillActorBase>(GWorld->SpawnActor(SkillInfo[Pkt.skillinfo().skill_id()].SkillActor, &SpawnedLocation, &SpawnedRotation, SpawnParams));
	if (SkillActor == nullptr)
		return;

	Skills.Add(Pkt.skillinfo().skill_id(), SkillActor);

	SkillActor->Info->CopyFrom(Pkt.skillactor());
	SkillActor->ActivateSkill();
}

void UP1GameInstance::AttackEnemy(Protocol::S_ATTACK& Pkt)
{
	int32 EnemyID = Pkt.victim().object_id();
	FDamageInfo DamageInfo = MakeDamageInfo(Pkt);

	Enemies[EnemyID]->SetHealthByDamage(Pkt.victim().hp(), DamageInfo);
}

AEnemyMob* UP1GameInstance::SpawnMob(Protocol::ObjectInfo ObjInfo, FVector Loc)
{
	AEnemyMob* SpawnedMob = Cast<AEnemyMob>(GetWorld()->SpawnActor(EnemyMobClass, &Loc));

	if (SpawnedMob == nullptr)
		return nullptr;

	SpawnedMob->Info->CopyFrom(ObjInfo);
	SpawnedMob->InitOnSpawn(ObjInfo.hp());
	return SpawnedMob;
}

AP1Character* UP1GameInstance::SpawnCharacter(Protocol::ObjectInfo ObjInfo, FVector Loc)
{
	AP1Character* SpawnedActor = Cast<AP1Character>(GetWorld()->SpawnActor(CharacterClass, &Loc));

	if (SpawnedActor == nullptr)
		return nullptr;

	SpawnedActor->Info->CopyFrom(ObjInfo);
	SpawnedActor->InitOnSpawn(ObjInfo.hp(), ObjInfo.stamina());

	return SpawnedActor;
}

FDamageInfo UP1GameInstance::MakeDamageInfo(Protocol::S_ATTACK& Pkt)
{
	FDamageInfo DamageInfo;
	DamageInfo.Damage = Pkt.skillinfo().damage();

	switch (Pkt.skillinfo().damage_type())
	{
	case Protocol::DAMAGE_TYPE_NORMAL:
		DamageInfo.DamageType = EDamageType::Normal;
		break;
	case Protocol::DAMAGE_TYPE_DOT:
		DamageInfo.DamageType = EDamageType::Dot;
		break;
	case Protocol::DAMAGE_TYPE_BUFF:
		DamageInfo.DamageType = EDamageType::Buff;
		break;
	default:
		break;
	}

	switch (Pkt.skillinfo().cc_type())
	{
	case Protocol::CC_TYPE_NORMAL:
		DamageInfo.CCType = ECCType::Normal;
		break;
	case Protocol::CC_TYPE_SLOW:
		DamageInfo.CCType = ECCType::Slow;
		break;
	case Protocol::CC_TYPE_STUN:
		DamageInfo.CCType = ECCType::Stun;
		break;
	case Protocol::CC_TYPE_AIRBORNE:
		DamageInfo.CCType = ECCType::Airborne;
		break;
	default:
		break;
	}

	return DamageInfo;
}
