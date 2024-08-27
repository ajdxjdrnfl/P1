// Fill out your copyright notice in the Description page of Project Settings.


#include "PhaseBossDoor.h"
#include "Components/CapsuleComponent.h"
#include "P1/P1Character.h"
#include "P1/P1GameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Components/WidgetComponent.h"

APhaseBossDoor::APhaseBossDoor()
{
	PrimaryActorTick.bCanEverTick = true;

	CapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleComponent"));
	CapsuleComponent->SetupAttachment(DefaultSceneRootComponent);

	WidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("WidgetComponent"));
	WidgetComponent->SetupAttachment(DoorComponent);
	WidgetComponent->SetVisibility(false);

}

void APhaseBossDoor::BeginPlay()
{
	Super::BeginPlay();

	CapsuleComponent->OnComponentBeginOverlap.AddDynamic(this, &APhaseBossDoor::OnCollisionOverlapBegin);
	CapsuleComponent->OnComponentEndOverlap.AddDynamic(this, &APhaseBossDoor::OnCollisionOverlapEnd);
}

void APhaseBossDoor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void APhaseBossDoor::ClearPhase()
{
	if (!bCanInteract)
	{
		bCanInteract = true;
	}
	else
	{
		bIsOpeningDoor = true;
		WidgetComponent->SetVisibility(false);
	}
}

void APhaseBossDoor::Interact()
{
	if (!bCanInteract) return;
	
	{
		Protocol::C_PHASE Pkt;
		SEND_PACKET(Pkt);
	}
}

void APhaseBossDoor::OnOpeningDoorEnd()
{
	bIsAlreadyOpen = true;
}

void APhaseBossDoor::OnCollisionOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	UP1GameInstance* GameInstance = GetWorld()->GetGameInstance<UP1GameInstance>();
	AP1Character* HitCharacter = Cast<AP1Character>(OtherActor);
	if (GameInstance && HitCharacter && GameInstance->IsMyCharacter(HitCharacter->ObjectInfo->object_id()) && HitCharacter)
	{
		if (bIsAlreadyOpen || !bCanInteract) return;
		HitCharacter->SetInteractInterfaceActor(this);
		WidgetComponent->SetVisibility(true);
	}
}

void APhaseBossDoor::OnCollisionOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	UP1GameInstance* GameInstance = GetWorld()->GetGameInstance<UP1GameInstance>();
	AP1Character* HitCharacter = Cast<AP1Character>(OtherActor);
	if (GameInstance && HitCharacter && GameInstance->IsMyCharacter(HitCharacter->ObjectInfo->object_id()) && HitCharacter)
	{
		if (bIsAlreadyOpen) return;
		HitCharacter->SetInteractInterfaceActor(nullptr);
		WidgetComponent->SetVisibility(false);
	}
}
