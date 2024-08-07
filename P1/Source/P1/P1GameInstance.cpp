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
#include "P1/Enemy/EnemyBoss.h"
#include "Containers/Ticker.h"
#include "P1/Skill/Boss/BossPillar.h"

void UP1GameInstance::Init()
{
	ClassCasterMap =
	{
		{ FName("Warrior"), Protocol::CasterType::CASTER_TYPE_WARRIOR },
		{ FName("Mob"), Protocol::CasterType::CASTER_TYPE_MOB },
		{ FName("Boss"), Protocol::CasterType::CASTER_TYPE_BOSS },
	};
		
	InitSkillMap();

	TickDelegateHandle = FTSTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateUObject(this, &UP1GameInstance::Tick));

	Super::Init();
}

void UP1GameInstance::Shutdown()
{
	FTSTicker::GetCoreTicker().RemoveTicker(TickDelegateHandle);

	Super::Shutdown();
}

bool UP1GameInstance::Tick(float DeltaTime)
{
	for (TMap<uint64, ASkillActorBase*>::TIterator Iter = Skills.CreateIterator(); Iter; ++Iter)
	{
		if (Iter->Value == nullptr)
			Iter.RemoveCurrent();
	}

	for (TMap<uint64, ABossPillar*>::TIterator Iter = BossPillars.CreateIterator(); Iter; ++Iter)
	{
		if (Iter->Value == nullptr)
			Iter.RemoveCurrent();
	}

	return false;
}

void UP1GameInstance::InitSkillMap()
{
	if (SkillDataTable == nullptr)
		return;

	SkillInfo.SetNum(Protocol::CasterType_ARRAYSIZE);
	TArray<FName> RowNames = SkillDataTable->GetRowNames();
	for (FName Row : RowNames)
	{
		FString ContextString;
		FSkillsByClass* SkillInfos = SkillDataTable->FindRow<FSkillsByClass>(Row, ContextString);
		for (FSkillInfo& CurrentSkillInfo : SkillInfos->SkillInfos)
		{
			if (Row == FName("Warrior"))
			{
				SkillInfo[Protocol::CASTER_TYPE_WARRIOR].Add(CurrentSkillInfo);
			}
			else if (Row == FName("Boss"))
			{
				SkillInfo[Protocol::CASTER_TYPE_BOSS].Add(CurrentSkillInfo);
			}
			else if (Row == FName("Mob"))
			{
				SkillInfo[Protocol::CASTER_TYPE_MOB].Add(CurrentSkillInfo);
			}

			// Set skillinfo to each skill game default object
			SetSkillInfo(CurrentSkillInfo);
		}
	}
}

void UP1GameInstance::SetSkillInfo(const FSkillInfo& CurrentSkillInfo)
{
	if (CurrentSkillInfo.SkillActorClass == nullptr) return;
	ASkillActorBase* SkillActor = Cast<ASkillActorBase>(CurrentSkillInfo.SkillActorClass->GetDefaultObject());

	if (SkillActor == nullptr) 
		return;

	FVector2D CollisionSize = FVector2D(CurrentSkillInfo.XScale, CurrentSkillInfo.YScale);

	SkillActor->SkillInfo = CurrentSkillInfo;
	SkillActor->SetCollisionSize(CollisionSize);
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
		case Protocol::CASTER_TYPE_BOSS:
			Boss = SpawnBoss(Pkt.info(i), Loc);
			break;
		case Protocol::CASTER_TYPE_STRUCTURE:
			BossPillars.Add({info.object_id(), SpawnBossPillar(Pkt.info(i), Loc) });
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
	AP1Creature* Creature = GetCreature(Pkt);
	if (Creature == nullptr) 
		return;

	FVector SpawnedLocation = FVector(Pkt.skillactor().x(), Pkt.skillactor().y(), Pkt.skillactor().z());
	FRotator SpawnedRotation = FRotator(0, Pkt.skillactor().yaw(), 0);
	FSkillInfo CurrentSkillInfo = SkillInfo[Pkt.caster().castertype()][Pkt.skillid()];
	FTransform SpawnedTransform;
	SpawnedTransform.SetLocation(SpawnedLocation);
	SpawnedTransform.SetRotation(SpawnedRotation.Quaternion());
	
	ASkillActorBase* SkillActor = Cast<ASkillActorBase>(UGameplayStatics::BeginDeferredActorSpawnFromClass(GWorld, CurrentSkillInfo.SkillActorClass, SpawnedTransform));
	if (SkillActor == nullptr)
		return;

	SkillActor->SkillInfo = CurrentSkillInfo;
	SkillActor->ObjectInfo->CopyFrom(Pkt.skillactor());
	SkillActor->InitOnSpawn(Creature);
	SkillActor->InstigatorOfSkill = Creature;
	SkillActor->BindCollisionDelegate();
	
	Creature->SetSpawnedSkill(Pkt.skillid(), SkillActor);

	UGameplayStatics::FinishSpawningActor(SkillActor, SpawnedTransform);
	SkillActor->ActivateSkill();

	Skills.Add(Pkt.skillactor().object_id(), SkillActor);

	
}

void UP1GameInstance::AttackTarget(Protocol::S_ATTACK& Pkt)
{
	int32 TargetID = Pkt.victim().object_id();
	int32 InstigatorID = Pkt.caster().object_id();

	AP1Creature* InstigatorCreature = GetCreature(Pkt, true);
	AP1Creature* VictimCreature = GetCreature(Pkt, false);
	if (InstigatorCreature == nullptr || VictimCreature == nullptr)
		return;

	FSkillInfo InstigatorSkillInfo = InstigatorCreature->GetSkillInfoByIndex(Pkt.skillid());
	VictimCreature->SetCreatureStateByServer(InstigatorSkillInfo);
	VictimCreature->SetHealthByDamage(Pkt.victim().hp());
}

void UP1GameInstance::PlayMontage(Protocol::S_MONTAGE& Pkt)
{
	GetCreature(Pkt)->PlayAnimMontageByServer(Pkt);
}

void UP1GameInstance::KillCreature(Protocol::S_DEAD& pkt)
{
	AP1Creature* CreatureToDie = GetCreature(pkt);
	if (CreatureToDie == nullptr) return;

	CreatureToDie->Die();
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

AEnemyBoss* UP1GameInstance::SpawnBoss(Protocol::ObjectInfo ObjInfo, FVector Loc)
{
	AEnemyBoss* SpawnedActor = Cast<AEnemyBoss>(GetWorld()->SpawnActor(EnemyBossClass, &Loc));

	if (SpawnedActor == nullptr)
		return nullptr;

	SpawnedActor->ObjectInfo->CopyFrom(ObjInfo);
	// TODO: Boss hp
	SpawnedActor->InitOnSpawn(ObjInfo.hp());

	return SpawnedActor;
}

ABossPillar* UP1GameInstance::SpawnBossPillar(Protocol::ObjectInfo ObjInfo, FVector Loc)
{
	ABossPillar* SpawnedActor = Cast<ABossPillar>(GetWorld()->SpawnActor(BossPillarClass, &Loc));

	if (SpawnedActor == nullptr)
		return nullptr;

	SpawnedActor->ObjectInfo->CopyFrom(ObjInfo);

	return SpawnedActor;
}

AP1Creature* UP1GameInstance::GetCreature(Protocol::S_SKILL& Pkt)
{
	switch (Pkt.caster().castertype())
	{
	case Protocol::CASTER_TYPE_BOSS:
		return Boss;
		break;
	case Protocol::CASTER_TYPE_MAGE:
	case Protocol::CASTER_TYPE_WARRIOR:
		return Characters[Pkt.caster().object_id()];
		break;
	case Protocol::CASTER_TYPE_MOB:
		return Enemies[Pkt.caster().object_id()];
		break;
	default:
		break;
	}
	return nullptr;
}

AP1Creature* UP1GameInstance::GetCreature(Protocol::S_MONTAGE& Pkt)
{
	switch (Pkt.caster().castertype())
	{
	case Protocol::CASTER_TYPE_BOSS:
		return Boss;
		break;
	case Protocol::CASTER_TYPE_MAGE:
	case Protocol::CASTER_TYPE_WARRIOR:
		return Characters[Pkt.caster().object_id()];
		break;
	case Protocol::CASTER_TYPE_MOB:
		return Enemies[Pkt.caster().object_id()];
		break;
	default:
		break;
	}
	return nullptr;
}

AP1Creature* UP1GameInstance::GetCreature(Protocol::S_ATTACK& Pkt, bool isCaster)
{
	Protocol::ObjectInfo ObjInfo = isCaster ? Pkt.caster() : Pkt.victim();
	switch (ObjInfo.castertype())
	{
	case Protocol::CASTER_TYPE_BOSS:
		return Boss;
		break;
	case Protocol::CASTER_TYPE_MAGE:
	case Protocol::CASTER_TYPE_WARRIOR:
		return Characters[ObjInfo.object_id()];
		break;
	case Protocol::CASTER_TYPE_MOB:
		return Enemies[ObjInfo.object_id()];
		break;
	default:
		break;
	}
	return nullptr;
}

AP1Creature* UP1GameInstance::GetCreature(Protocol::S_DEAD& Pkt)
{
	switch (Pkt.info().castertype())
	{
	case Protocol::CASTER_TYPE_BOSS:
		return Boss;
		break;
	case Protocol::CASTER_TYPE_MAGE:
	case Protocol::CASTER_TYPE_WARRIOR:
		return Characters[Pkt.info().object_id()];
		break;
	case Protocol::CASTER_TYPE_MOB:
		return Enemies[Pkt.info().object_id()];
		break;
	default:
		break;
	}
	return nullptr;
}


