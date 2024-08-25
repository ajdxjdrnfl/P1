// Fill out your copyright notice in the Description page of Project Settings.


#include "PhaseDoor.h"

APhaseDoor::APhaseDoor()
{
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	DoorComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Door"));
	DoorComponent->SetupAttachment(RootComponent);
}

void APhaseDoor::BeginPlay()
{
	Super::BeginPlay();
	
}

void APhaseDoor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bIsOpeningDoor)
	{
		FVector TargetLocation = FVector(GetActorLocation().X, GetActorLocation().Y, GetActorLocation().Z - OpeningDist);
		OpeningDist -= DeltaTime;
		if (OpeningDist <= 0)
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

