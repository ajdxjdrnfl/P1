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
		for (FSkillInfo& CurrentSkillInfo : SkillInfos.SkillInfos)
		{
			CurrentSkillInfo.SkillNum = idx;
			SkillInfo.Add(idx, CurrentSkillInfo);

			// Set skillinfo to each skill game default object
			SetSkillInfo(idx, CurrentSkillInfo);

			idx += 1;
		}
	}
}

void UP1GameInstance::SetSkillInfo(int32 ID, const FSkillInfo& CurrentSkillInfo)
{
	if (CurrentSkillInfo.SkillActorClass == nullptr) return;
	ASkillActorBase* SkillActor = Cast<ASkillActorBase>(CurrentSkillInfo.SkillActorClass->GetDefaultObject());

	if (SkillActor == nullptr) 
		return;

	Protocol::SkillInfo* SkillInfoRef = SkillActor->SkillInfo;

	SkillInfoRef->set_skill_id(ID);
	SkillInfoRef->set_size_x(CurrentSkillInfo.XScale);
	SkillInfoRef->set_size_y(CurrentSkillInfo.YScale);
	SkillInfoRef->set_damage(CurrentSkillInfo.Damage);
	SkillInfoRef->set_cooldown(CurrentSkillInfo.CooldownTime);

	switch (CurrentSkillInfo.CollisionType)
	{
	case ECollisionType::Circle:
		SkillInfoRef->set_collision_type(Protocol::CollisionType::COLLISION_TYPE_CIRCLE);
		break;
	case ECollisionType::Box:
		SkillInfoRef->set_collision_type(Protocol::CollisionType::COLLISION_TYPE_BOX);
		break;
	default:
		SkillInfoRef->set_collision_type(Protocol::CollisionType::COLLISION_TYPE_CIRCLE);
		break;
	}

	switch (CurrentSkillInfo.DamageType)
	{
	case EDamageType::Normal:
		SkillInfoRef->set_damage_type(Protocol::DamageType::DAMAGE_TYPE_NORMAL);
		break;
	case EDamageType::Dot:
		SkillInfoRef->set_damage_type(Protocol::DamageType::DAMAGE_TYPE_DOT);
		break;
	case EDamageType::Buff:
		SkillInfoRef->set_damage_type(Protocol::DamageType::DAMAGE_TYPE_BUFF);
		break;
	default:
		SkillInfoRef->set_damage_type(Protocol::DamageType::DAMAGE_TYPE_NORMAL);
		break;
	}

	switch (CurrentSkillInfo.CCType)
	{
	case ECCType::Normal:
		SkillInfoRef->set_cc_type(Protocol::CCType::CC_TYPE_NORMAL);
		break;
	case ECCType::Slow:
		SkillInfoRef->set_cc_type(Protocol::CCType::CC_TYPE_SLOW);
		break;
	case ECCType::Stun:
		SkillInfoRef->set_cc_type(Protocol::CCType::CC_TYPE_STUN);
		break;
	case ECCType::Airborne:
		SkillInfoRef->set_cc_type(Protocol::CCType::CC_TYPE_AIRBORNE);
		break;
	default:
		SkillInfoRef->set_cc_type(Protocol::CCType::CC_TYPE_NORMAL);
		break;
	}

	switch (CurrentSkillInfo.SkillType)
	{
	case ESkillType::Normal:
		SkillInfoRef->set_skill_type(Protocol::SkillType::SKILL_TYPE_NORMAL);
		break;
	case ESkillType::Charging:
		SkillInfoRef->set_skill_type(Protocol::SkillType::SKILL_TYPE_CHARGING);
		break;
	case ESkillType::Casting:
		SkillInfoRef->set_skill_type(Protocol::SkillType::SKILL_TYPE_CASTING);
		break;
	default:
		SkillInfoRef->set_skill_type(Protocol::SkillType::SKILL_TYPE_NORMAL);
		break;
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
	return MyCharacter->ObjectInfo->object_id() == ID;
}

void UP1GameInstance::SpawnActorByServer(Protocol::S_SPAWN& Pkt)
{
	check(WarriorClass != nullptr);
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

	ASkillActorBase* SkillActor = Cast<ASkillActorBase>(GWorld->SpawnActor(SkillInfo[Pkt.skillinfo().skill_id()].SkillActorClass, &SpawnedLocation, &SpawnedRotation, SpawnParams));
	if (SkillActor == nullptr)
		return;

	Skills.Add(Pkt.skillinfo().skill_id(), SkillActor);

	SkillActor->ObjectInfo->CopyFrom(Pkt.skillactor());
	SkillActor->ActivateSkill();
}

void UP1GameInstance::AttackEnemy(Protocol::S_ATTACK& Pkt)
{
	int32 EnemyID = Pkt.victim().object_id();

	Enemies[EnemyID]->SetHealthByDamage(Pkt.victim().hp());
}

AEnemyMob* UP1GameInstance::SpawnMob(Protocol::ObjectInfo ObjInfo, FVector Loc)
{
	AEnemyMob* SpawnedMob = Cast<AEnemyMob>(GetWorld()->SpawnActor(EnemyMobClass, &Loc));

	if (SpawnedMob == nullptr)
		return nullptr;

	SpawnedMob->ObjectInfo->CopyFrom(ObjInfo);
	SpawnedMob->InitOnSpawn(ObjInfo.hp());
	return SpawnedMob;
}

AP1Character* UP1GameInstance::SpawnCharacter(Protocol::ObjectInfo ObjInfo, FVector Loc)
{
	AP1Character* SpawnedActor = Cast<AP1Character>(GetWorld()->SpawnActor(WarriorClass, &Loc));

	if (SpawnedActor == nullptr)
		return nullptr;

	SpawnedActor->ObjectInfo->CopyFrom(ObjInfo);
	SpawnedActor->InitOnSpawn(ObjInfo.hp(), ObjInfo.stamina());

	return SpawnedActor;
}

