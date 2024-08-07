// Fill out your copyright notice in the Description page of Project Settings.


#include "BossPillar.h"
#include "Components/CapsuleComponent.h"
#include "P1/Enemy/EnemyBoss.h"

ABossPillar::ABossPillar()
{
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	CapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleComponent"));
	CapsuleComponent->SetupAttachment(RootComponent);

	ObjectInfo = new Protocol::ObjectInfo();
}

void ABossPillar::BeginPlay()
{
	Super::BeginPlay();


}

void ABossPillar::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABossPillar::OnCollisionOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (AEnemyBoss* Boss = Cast<AEnemyBoss>(OtherActor))
	{
		Boss->UsePillarSkill(FVector2D(GetActorLocation().X, GetActorLocation().Y));
		Destroy();
	}
}

