// Fill out your copyright notice in the Description page of Project Settings.


#include "SkillActors.h"
#include "Components/BoxComponent.h"
#include "P1/P1Character.h"
#include "P1/P1PlayerController.h"
#include "P1/Enemy/EnemyBase.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/SphereComponent.h"

AProjectileSkill::AProjectileSkill()
{
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	SkillMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SkillMesh"));
	SkillMeshComponent->SetupAttachment(RootComponent);

	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereCollision"));
	SphereComponent->SetupAttachment(RootComponent);
}

void AProjectileSkill::BeginPlay()
{
	Super::BeginPlay();

	SphereComponent->OnComponentBeginOverlap.AddUniqueDynamic(this, &AProjectileSkill::OnCollisionOverlapBegin);
}

void AProjectileSkill::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!bIsActivated)
		return;

	if (MoveDistance <= 0)
		Destroy();

	AddActorWorldOffset(GetActorRotation().Vector() * MoveSpeed);
	MoveDistance -= DeltaTime;
}

void AProjectileSkill::ActivateSkill()
{
	Super::ActivateSkill();

	MoveDirection.Normalize();
	bIsActivated = true;
}

void AProjectileSkill::OnCollisionOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (AP1Creature* Creature = Cast<AP1Creature>(OtherActor))
	{
		SendCollisionPacketToServer(Creature);
	}
}

AOnLocationSkill::AOnLocationSkill()
{
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	BoxCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollision"));
	BoxCollision->SetupAttachment(RootComponent);
	BoxCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AOnLocationSkill::BeginPlay()
{
	BoxCollision->OnComponentBeginOverlap.AddUniqueDynamic(this, &AOnLocationSkill::OnCollisionOverlapBegin);
}

void AOnLocationSkill::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (LivingTime <= 0)
	{
		Destroy();
	}
	else
	{
		LivingTime -= DeltaTime;
	}
}

void AOnLocationSkill::OnCollisionOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (AP1Creature* Creature = Cast<AP1Creature>(OtherActor))
	{
		SendCollisionPacketToServer(Creature);
	}
}
