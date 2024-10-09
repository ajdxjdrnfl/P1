// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemBase.h"
#include "P1/P1Character.h"
#include "P1/Component/Inventory/InventoryComponent.h"
#include "P1/Component/Inventory/ItemDataComponent.h"
#include "Components/SphereComponent.h"

AItemBase::AItemBase()
{
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>("RootComponent");

	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
	SphereComponent->SetupAttachment(RootComponent);

	ItemDataComponent = CreateDefaultSubobject<UItemDataComponent>(TEXT("ItemDataComponent"));
}

void AItemBase::BeginPlay()
{
	Super::BeginPlay();
	
	SphereComponent->OnComponentBeginOverlap.AddDynamic(this, &AItemBase::OnCollisionOverlapBegin);
	SphereComponent->OnComponentEndOverlap.AddDynamic(this, &AItemBase::OnCollisionOverlapEnd);
}

void AItemBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AItemBase::Interact(UInventoryComponent* InventoryComponent)
{
	ItemDataComponent->Interact(InventoryComponent);
}

void AItemBase::OnCollisionOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (AP1Character* Character = Cast<AP1Character>(OtherActor))
	{
		Character->GetOverlappedItems().Add(this);
	}
}

void AItemBase::OnCollisionOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (AP1Character* Character = Cast<AP1Character>(OtherActor))
	{
		Character->GetOverlappedItems().Remove(this);
	}
}

