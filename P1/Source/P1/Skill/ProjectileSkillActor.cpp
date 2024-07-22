// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectileSkillActor.h"
#include "Components/SphereComponent.h"

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

	SphereComponent->OnComponentBeginOverlap.AddDynamic(this, &AProjectileSkillActor::OnSphereOverlapBegin);
	SphereComponent->OnComponentEndOverlap.AddDynamic(this, &AProjectileSkillActor::OnSphereOverlapEnd);
}

void AProjectileSkillActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!bIsActivated) 
		return;
	
	if (MoveDistance <= 0) 
		Destroy();

	AddActorWorldOffset(MoveDirection * MoveSpeed);
	MoveDistance -= MoveDirection.Size() * MoveSpeed;
}

void AProjectileSkillActor::MoveProjectile(FVector Direction, float Speed, float Dist, FSkillInfo InfoOfSkill)
{
	MoveDirection = Direction;
	MoveDirection.Normalize();

	MoveSpeed = Speed;
	MoveDistance = Dist;
	bIsActivated = true;
}

void AProjectileSkillActor::OnSphereOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// TODO: enemy 인지 character 인지 enum으로 구분
	
}

void AProjectileSkillActor::OnSphereOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{

}
