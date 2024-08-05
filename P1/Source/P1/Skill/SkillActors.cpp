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

void AProjectileSkill::BindCollisionDelegate()
{
	SphereComponent->OnComponentBeginOverlap.AddUniqueDynamic(this, &AProjectileSkill::OnCollisionOverlapBegin);
}

void AProjectileSkill::SetCollisionSize(FVector2D SizeToSet)
{
	SphereComponent->SetSphereRadius(SizeToSet.X);
}

void AProjectileSkill::OnCollisionOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	SphereComponent->UpdateOverlaps();
	if (AP1Creature* Creature = Cast<AP1Creature>(OtherActor))
	{
		SendCollisionPacketToServer(Creature);
	}
}

AOnLocationSkill::AOnLocationSkill()
{
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	BoxCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollision"));
	BoxCollision->SetupAttachment(RootComponent);

	SphereCollsion = CreateDefaultSubobject<USphereComponent>(TEXT("SphereCollision"));
	SphereCollsion->SetupAttachment(RootComponent);

}

void AOnLocationSkill::BeginPlay()
{
	Super::BeginPlay();
}

void AOnLocationSkill::BindCollisionDelegate()
{
	BoxCollision->OnComponentBeginOverlap.AddUniqueDynamic(this, &AOnLocationSkill::OnCollisionOverlapBegin);
}

void AOnLocationSkill::SetCollisionSize(FVector2D SizeToSet)
{
	BoxCollision->Bounds.BoxExtent.X = SizeToSet.X;
	BoxCollision->Bounds.BoxExtent.Y = SizeToSet.Y;
}

void AOnLocationSkill::OnCollisionOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (AP1Creature* Creature = Cast<AP1Creature>(OtherActor))
	{
		SendCollisionPacketToServer(Creature);
	}
}

void AOnLocationSkillByTick::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	for (AP1Creature* Creature : TargetCreatures)
	{
		SendCollisionPacketToServer(Creature);
	}
	
}

void AOnLocationSkillByTick::OnCollisionOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OwnerCreature == nullptr) return;
	bool check1 = Cast<AEnemyBase>(OtherActor) && Cast<AP1Character>(OwnerCreature);
	bool check2 = Cast<AEnemyBase>(OwnerCreature) && Cast<AP1Character>(OtherActor);
	if (!check1 && !check2) return;

	TargetCreatures.Add(Cast<AP1Creature>(OtherActor));
}

void AOnLocationSkillByTick::OnCollisionOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OwnerCreature == nullptr) return;
	bool check1 = Cast<AEnemyBase>(OtherActor) && Cast<AP1Character>(OwnerCreature);
	bool check2 = Cast<AEnemyBase>(OwnerCreature) && Cast<AP1Character>(OtherActor);
	if (!check1 && !check2) return;

	TargetCreatures.Remove(Cast<AP1Creature>(OtherActor));
}
