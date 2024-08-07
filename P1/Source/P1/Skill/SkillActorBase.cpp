// Fill out your copyright notice in the Description page of Project Settings.


#include "SkillActorBase.h"
#include "P1/P1.h"
#include "P1/P1Creature.h"
#include "P1/P1Character.h"
#include "P1/Enemy/EnemyBase.h"
#include "Particles/ParticleSystem.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "P1/Skill/SkillInstanceBase.h"

ASkillActorBase::ASkillActorBase()
{
	PrimaryActorTick.bCanEverTick = true;

	ObjectInfo = new Protocol::ObjectInfo();
	//SkillInfo = new Protocol::SkillInfo();
}

void ASkillActorBase::BeginPlay()
{
	Super::BeginPlay();
	
	
}

void ASkillActorBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (CurrentLifeTime > 0)
	{
		CurrentLifeTime -= DeltaTime;
	}
	else
	{
		if (this != nullptr)
		{
			Destroy();
			SkillInstance->OnSkillDetroyed();
		}
	}

}

void ASkillActorBase::InitOnSpawn(class AP1Creature* _OwnerCreature)
{
	OwnerCreature = _OwnerCreature;
	CurrentLifeTime = SkillInfo.LifeTime;
	if (CurrentLifeTime == -1) CurrentLifeTime = 100000;
}

void ASkillActorBase::ActivateSkill()
{

}

void ASkillActorBase::SendCollisionPacketToServer(AP1Creature* Creature)
{
	Protocol::C_ATTACK Pkt;
	Protocol::ObjectInfo* CasterInfo = Pkt.mutable_caster();
	Protocol::ObjectInfo* SkillActorInfoLocal = Pkt.mutable_skillactor();
	Protocol::ObjectInfo* VictimInfo = Pkt.mutable_victim();

	AP1Creature* InstigatorLocal = Cast<AP1Creature>(InstigatorOfSkill);

	if (InstigatorLocal == nullptr)
		return;

	UP1GameInstance* P1GameInstance = Cast<UP1GameInstance>(GetWorld()->GetGameInstance());
	if (P1GameInstance == nullptr)
		return;

	if (!P1GameInstance->IsMyCharacter(InstigatorLocal->ObjectInfo->object_id()))
		return;

	if (P1GameInstance->ClassCasterMap.Contains(InstigatorLocal->GetClassType()))
	{
		CasterInfo->set_castertype(P1GameInstance->ClassCasterMap[InstigatorLocal->GetClassType()]);
	}
	if (P1GameInstance->ClassCasterMap.Contains(Creature->GetClassType()))
	{
		VictimInfo->set_castertype(P1GameInstance->ClassCasterMap[Creature->GetClassType()]);
	}

	CasterInfo->CopyFrom(*InstigatorLocal->ObjectInfo);
	VictimInfo->CopyFrom(*Creature->ObjectInfo);
	SkillActorInfoLocal->CopyFrom(*ObjectInfo);

	Pkt.set_counter(Creature->GetCounterState());

	SEND_PACKET(Pkt);
}

void ASkillActorBase::SpawnActivationParticleOnLocation(FVector ActivationLocation)
{
	if (P_Activation == nullptr) return;

	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), P_Activation, GetFloorPos(ActivationLocation));
}

void ASkillActorBase::SpawnActivationParticleOnTarget(AActor* TargetActor)
{
	if (P_Activation == nullptr) return;

	UGameplayStatics::SpawnEmitterAttached(P_Activation, TargetActor->GetRootComponent());
}

void ASkillActorBase::SpawnHitParticleOnLocation(FVector HitLocation)
{
	if (P_Hit == nullptr) return;

	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), P_Hit, GetFloorPos(HitLocation));
}

void ASkillActorBase::SpawnHitParticleOnTarget(AActor* TargetActor)
{
	if (P_Hit == nullptr) return;

	UGameplayStatics::SpawnEmitterAttached(P_Hit, TargetActor->GetRootComponent());
}

UParticleSystemComponent* ASkillActorBase::SpawnHoldParticleOnLocation(FVector HoldLocation)
{
	if (P_Hold == nullptr) return nullptr;

	return UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), P_Hold, GetFloorPos(HoldLocation));
}

UParticleSystemComponent* ASkillActorBase::SpawnHoldParticleOnTarget(AActor* TargetActor)
{
	if (P_Hold == nullptr) return nullptr;

	return UGameplayStatics::SpawnEmitterAttached(P_Hold, TargetActor->GetRootComponent());
}

FVector ASkillActorBase::GetFloorPos(FVector TargetLocation)
{
	FVector ResultPos = TargetLocation;
	FVector Start = TargetLocation;
	FVector End = Start - (10000 * TargetLocation);
	TArray<AActor*> ActorsToIgnore;
	FHitResult OutHit;

	bool bIsHit = UKismetSystemLibrary::LineTraceSingle(
		GetWorld(),
		Start,
		End,
		ETraceTypeQuery::TraceTypeQuery1,
		false,
		ActorsToIgnore,
		EDrawDebugTrace::Persistent,
		OutHit,
		true
		);

	if (bIsHit)
	{
		if (Cast<AP1Creature>(OutHit.GetActor()) || Cast<ASkillActorBase>(OutHit.GetActor())) 
			return ResultPos;

		ResultPos = OutHit.Location;
	}

	return ResultPos;
}
