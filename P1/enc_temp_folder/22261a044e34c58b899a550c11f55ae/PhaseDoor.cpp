// Fill out your copyright notice in the Description page of Project Settings.


#include "PhaseDoor.h"

APhaseDoor::APhaseDoor()
{
	PrimaryActorTick.bCanEverTick = true;

	DefaultSceneRootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	DoorComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Door"));
	DoorComponent->SetupAttachment(DefaultSceneRootComponent);
}

void APhaseDoor::BeginPlay()
{
	Super::BeginPlay();
	
	DefaultDist = OpeningDist;
	TargetLocation = GetActorLocation() - FVector(0, 0, OpeningDist);
}

void APhaseDoor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bIsOpeningDoor)
	{
		if (FMath::IsNearlyEqual(TargetLocation.Z, GetActorLocation().Z, 5))
		{
			OnOpeningDoorEnd();
			return;
		}

		FVector NextLocation = FMath::VInterpTo(GetActorLocation(), TargetLocation, DeltaTime, OpeningSpeed);
		SetActorLocation(NextLocation);
	}
}

void APhaseDoor::OpenDoor()
{
	bIsOpeningDoor = true;
}

void APhaseDoor::ClearPhase()
{
	OpenDoor();
}

void APhaseDoor::OnOpeningDoorEnd()
{
	bIsOpeningDoor = false;
}

