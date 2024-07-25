// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectileSkillActor.h"
#include "Components/SphereComponent.h"
#include "P1/P1Character.h"
#include "P1/P1.h"
#include "P1/Enemy/EnemyBase.h"
#include "P1/Component/StatComponent/EnemyStatComponent.h"
#include "P1/P1GameInstance.h"

AProjectileSkillActor::AProjectileSkillActor()
{
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	SkillMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SkillMesh"));
	SkillMeshComponent->SetupAttachment(RootComponent);

	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereCollision"));
	SphereComponent->SetupAttachment(RootComponent);
}

void AProjectileSkillActor::BeginPlay()
{
	Super::BeginPlay();

	SphereComponent->OnComponentBeginOverlap.AddDynamic(this, &AProjectileSkillActor::OnCollisionOverlapBegin);
	SphereComponent->OnComponentEndOverlap.AddDynamic(this, &AProjectileSkillActor::OnCollisionOverlapEnd);
}

void AProjectileSkillActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!bIsActivated) 
		return;
	
	if (MoveDistance <= 0) 
		Destroy();

	AddActorWorldOffset(GetActorRotation().Vector() * MoveSpeed);
	MoveDistance -= DeltaTime;
}

void AProjectileSkillActor::ActivateSkill()
{
	Super::ActivateSkill();

	MoveDirection.Normalize();
	bIsActivated = true;
}

void AProjectileSkillActor::SendCollisionPacketToServer(AEnemyBase* Enemy)
{
	Protocol::C_ATTACK Pkt;
	Protocol::ObjectInfo* CasterInfo = Pkt.mutable_caster();
	Protocol::ObjectInfo* SkillActorInfoLocal = Pkt.mutable_skillactor();
	Protocol::ObjectInfo* VictimInfo = Pkt.mutable_victim();

	AP1Creature* InstigatorLocal = Cast<AP1Creature>(GetOwner());

	if (InstigatorLocal == nullptr)
		return;

	UP1GameInstance* P1GameInstance = Cast<UP1GameInstance>(GetWorld()->GetGameInstance());
	if (P1GameInstance == nullptr)
		return;

	if (!P1GameInstance->IsMyCharacter(InstigatorLocal->ObjectInfo->object_id()))
		return;

	if (InstigatorLocal->GetClassType() == 1)
	{
		CasterInfo->set_castertype(Protocol::CASTER_TYPE_WARRIOR);
	}
	else
	{
		// TODO:
	}

	if (Enemy->GetClassType() == 3)
	{
		VictimInfo->set_castertype(Protocol::CASTER_TYPE_BOSS);
	}
	else
	{
		// TODO:
	}
	CasterInfo->set_object_id(InstigatorLocal->ObjectInfo->object_id());
	VictimInfo->set_object_id(Enemy->ObjectInfo->object_id());
	SkillActorInfoLocal->set_object_id(ObjectInfo->object_id());

	SEND_PACKET(Pkt);
}

void AProjectileSkillActor::OnCollisionOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (AEnemyBase* Enemy = Cast<AEnemyBase>(OtherActor))
	{
		SendCollisionPacketToServer(Enemy);
	}
}

void AProjectileSkillActor::OnCollisionOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{

}
