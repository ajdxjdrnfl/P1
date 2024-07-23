// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectileSkillActor.h"
#include "Components/SphereComponent.h"
#include "P1/P1Character.h"
#include "P1/P1.h"

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

void AProjectileSkillActor::OnCollisionOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// TODO:
}

void AProjectileSkillActor::OnCollisionOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{

}
