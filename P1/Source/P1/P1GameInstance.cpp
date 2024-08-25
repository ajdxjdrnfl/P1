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
#include "AIController.h"
#include "P1/SubSystem/GameInstance/DungeonManagerSubsystem.h"

void UP1GameInstance::Init()
{
	ClassCasterMap =
	{
		{ FName("Warrior"), Protocol::CasterType::CASTER_TYPE_WARRIOR },
		{ FName("Mob"), Protocol::CasterType::CASTER_TYPE_MOB },
		{ FName("Boss"), Protocol::CasterType::CASTER_TYPE_BOSS },
		{ FName("Archer"), Protocol::CasterType::CASTER_TYPE_ARCHER },
	};

	CasterClassMap =
	{
		{ Protocol::CasterType::CASTER_TYPE_WARRIOR, WarriorClass },
		{ Protocol::CasterType::CASTER_TYPE_MOB, EnemyMobClass },
		{ Protocol::CasterType::CASTER_TYPE_BOSS, EnemyBossClass },
		{ Protocol::CasterType::CASTER_TYPE_ARCHER, ArcherClass },
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
			Protocol::CasterType* CasterType = ClassCasterMap.Find(Row);
			if (CasterType == nullptr) continue;

			SkillInfo[*CasterType].Add(CurrentSkillInfo);

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
			Pkt.set_castertype(Protocol::CASTER_TYPE_WARRIOR);
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

bool UP1GameInstance::IsMyCharacter(int32 ID)
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
		if (Characters.Contains(Pkt.info(i).object_id()) || IsMyCharacter(Pkt.info(i).object_id()))
			continue;

		info = Pkt.info(i);
		Loc = FVector(info.x(), info.y(), info.z());

		switch (info.castertype())
		{
		case Protocol::CASTER_TYPE_MOB:
			if (bNoEnemyMode) break;
			Enemies.Add({ info.object_id(), SpawnMob(Pkt.info(i), Loc) });
			break;
		case Protocol::CASTER_TYPE_ARCHER:
		case Protocol::CASTER_TYPE_WARRIOR:
			Characters.Add({ info.object_id(), SpawnCharacter(Pkt.info(i), Loc)});
			break;
		case Protocol::CASTER_TYPE_BOSS:
			if (bNoEnemyMode) break;
			Boss = SpawnBoss(Pkt.info(i), Loc);
			break;
		case Protocol::CASTER_TYPE_STRUCTURE:
			if (bNoEnemyMode) break;
			BossPillars.Add({info.object_id(), SpawnBossPillar(Pkt.info(i), Loc) });
			break;
		default:
			break;
		}
	}
}

void UP1GameInstance::DespawnActorByServer(Protocol::S_DESPAWN& Pkt)
{
	Protocol::ObjectInfo info;

	for (int32 i = 0; i < Pkt.info_size(); i++)
	{
		if (Characters.Contains(Pkt.info(i).object_id()))
			continue;

		info = Pkt.info(i);

		switch (info.castertype())
		{
		case Protocol::CASTER_TYPE_MOB:
			(*Enemies.Find(info.object_id()))->Despawn();
			Enemies.Remove(info.object_id());
			break;
		case Protocol::CASTER_TYPE_ARCHER:
		case Protocol::CASTER_TYPE_WARRIOR:
			(*Characters.Find(info.object_id()))->Despawn();
			Characters.Remove(info.object_id());
			break;
		case Protocol::CASTER_TYPE_BOSS:
			if (bNoEnemyMode) break;
			Boss->Destroy();
			Boss = nullptr;
			break;
		case Protocol::CASTER_TYPE_STRUCTURE:
			if (bNoEnemyMode) break;
			(*BossPillars.Find(info.object_id()))->Destroy();
			BossPillars.Remove(info.object_id());
			break;
		default:
			break;
		}
	}
}

void UP1GameInstance::MoveByServer(Protocol::S_MOVE& Pkt)
{
	if (Pkt.info().castertype() == Protocol::CASTER_TYPE_BOSS)
	{
		Boss->SetMoveValueByServer(Pkt);
	}
	else if (Pkt.info().castertype() == Protocol::CASTER_TYPE_MOB)
	{
		(*Enemies.Find(Pkt.info().object_id()))->SetMoveValueByServer(Pkt);
	}
	else if(Pkt.info().castertype() == Protocol::CASTER_TYPE_WARRIOR ||
			Pkt.info().castertype() == Protocol::CASTER_TYPE_ARCHER)
	{
		(*Characters.Find(Pkt.info().object_id()))->SetMoveValueByServer(Pkt);

		/*if (MyCharacter->ObjectInfo->object_id() == Pkt.info().object_id() && Pkt.info().state() == Protocol::MOVE_STATE_IDLE)
		{
			Protocol::C_MOVE Pkt;
			Protocol::ObjectInfo* TargetInfo = Pkt.mutable_targetinfo();
			Protocol::ObjectInfo* ObjInfo = Pkt.mutable_info();
			ObjInfo->CopyFrom(*MyCharacter->ObjectInfo);
			TargetInfo->CopyFrom(*MyCharacter->ObjectInfo);

			SEND_PACKET(Pkt);
		}*/
	}
}

void UP1GameInstance::SkillSpawn(Protocol::S_SKILL& Pkt)
{
	AP1Creature* Creature = GetCreature(Pkt);
	if (Creature == nullptr) 
		return;

	FVector SpawnedLocation = FVector(Pkt.skillactor().x(), Pkt.skillactor().y(), Pkt.skillactor().z());
	FRotator SpawnedRotation = FRotator(Creature->GetActorRotation().Pitch, Pkt.skillactor().yaw(), Creature->GetActorRotation().Roll);
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
	AP1Creature* Creature = GetCreature(Pkt);
	if (Creature == nullptr) return;

	Creature->PlayAnimMontageByServer(Pkt);
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
	SpawnedMob->InitOnSpawn(ObjInfo);
	return SpawnedMob;
}

AP1Character* UP1GameInstance::SpawnCharacter(Protocol::ObjectInfo ObjInfo, FVector Loc)
{
	UClass* ClassToSpawn = *CasterClassMap.Find(ObjInfo.castertype());
	if (ClassToSpawn == nullptr)
		return nullptr;

	AP1Character* SpawnedActor = Cast<AP1Character>(GetWorld()->SpawnActor(ClassToSpawn, &Loc));

	if (SpawnedActor == nullptr)
		return nullptr;

	SpawnedActor->ObjectInfo->CopyFrom(ObjInfo);
	SpawnedActor->InitOnSpawn(ObjInfo);
	SpawnedActor->TargetInfo->set_x(ObjInfo.x());
	SpawnedActor->TargetInfo->set_y(ObjInfo.y());
	SpawnedActor->TargetInfo->set_z(ObjInfo.z());

	return SpawnedActor;
}

AEnemyBoss* UP1GameInstance::SpawnBoss(Protocol::ObjectInfo ObjInfo, FVector Loc)
{
	AEnemyBoss* SpawnedActor = Cast<AEnemyBoss>(GetWorld()->SpawnActor(EnemyBossClass, &Loc));

	if (SpawnedActor == nullptr)
		return nullptr;

	SpawnedActor->ObjectInfo->CopyFrom(ObjInfo);
	// TODO: Boss hp
	SpawnedActor->InitOnSpawn(ObjInfo);

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

void UP1GameInstance::SetPhase(Protocol::S_PHASE& pkt)
{
	UDungeonManagerSubsystem* DungeonManager = UGameplayStatics::GetGameInstance(this)->GetSubsystem<UDungeonManagerSubsystem>();
	if (DungeonManager == nullptr) return;
	DungeonManager->OpenPhaseDoor(pkt.phasenumber());
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
	case Protocol::CASTER_TYPE_ARCHER:
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
	case Protocol::CASTER_TYPE_ARCHER:
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
	case Protocol::CASTER_TYPE_ARCHER:
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
	case Protocol::CASTER_TYPE_ARCHER:
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


