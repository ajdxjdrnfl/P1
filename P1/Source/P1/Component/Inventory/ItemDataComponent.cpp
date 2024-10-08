// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemDataComponent.h"
#include "P1/Component/Inventory/InventoryComponent.h"

UItemDataComponent::UItemDataComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

void UItemDataComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

void UItemDataComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UItemDataComponent::Interact(UInventoryComponent* InventoryComponent)
{
	InventoryComponent->AddToInventory(FItemData(Quantity, ItemID.RowName.ToString()));
}

