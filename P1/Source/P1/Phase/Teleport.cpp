// Fill out your copyright notice in the Description page of Project Settings.


#include "Phase/Teleport.h"
#include "Components/SphereComponent.h"
#include "P1/P1Character.h"
#include "P1/P1GameInstance.h"
#include "Components/WidgetComponent.h"

ATeleport::ATeleport()
{
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root Component"));

	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere Component"));
	SphereComponent->SetupAttachment(RootComponent);

	WidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("WidgetComponent"));
	WidgetComponent->SetupAttachment(RootComponent);
	WidgetComponent->SetVisibility(false);
}

void ATeleport::BeginPlay()
{
	Super::BeginPlay();
	
	SphereComponent->OnComponentBeginOverlap.AddDynamic(this, &ATeleport::OnCollisionOverlapBegin);
	SphereComponent->OnComponentEndOverlap.AddDynamic(this, &ATeleport::OnCollisionOverlapEnd);
}

void ATeleport::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ATeleport::Interact()
{
	if (!bCanInteract) return;

	UP1GameInstance* GameInstance = GetWorld()->GetGameInstance<UP1GameInstance>();
	if (GameInstance)
	{
		Protocol::CasterType CasterTypeToSpawn = GameInstance->GetMyCharacter()->ObjectInfo->castertype();
		{
			Protocol::C_ENTER_GAME Pkt;
			Pkt.set_castertype(CasterTypeToSpawn);
			Pkt.set_roomtype(Protocol::RoomType(LevelID));

			SEND_PACKET(Pkt);
		}
	}
}

void ATeleport::OnCollisionOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	UP1GameInstance* GameInstance = GetWorld()->GetGameInstance<UP1GameInstance>();
	AP1Character* HitCharacter = Cast<AP1Character>(OtherActor);
	if (GameInstance && HitCharacter && GameInstance->IsMyCharacter(HitCharacter->ObjectInfo->object_id()) && HitCharacter)
	{
		if (!bCanInteract) return;
		HitCharacter->SetInteractInterfaceActor(this);
		WidgetComponent->SetVisibility(true);
	}
}

void ATeleport::OnCollisionOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	UP1GameInstance* GameInstance = GetWorld()->GetGameInstance<UP1GameInstance>();
	AP1Character* HitCharacter = Cast<AP1Character>(OtherActor);
	if (GameInstance && HitCharacter && GameInstance->IsMyCharacter(HitCharacter->ObjectInfo->object_id()) && HitCharacter)
	{
		HitCharacter->SetInteractInterfaceActor(nullptr);
		WidgetComponent->SetVisibility(false);
	}
}

