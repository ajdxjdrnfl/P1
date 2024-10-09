// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/Inventory/InventoryComponent.h"
#include "P1/Item/ItemBase.h"
#include "P1/P1.h"

UInventoryComponent::UInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
	Slots.SetNum(20);
	for (int i = 0; i < 20; i++)
	{
		Slots[i].Index = i;
		Slots[i].Quantity = 0;
	}
}


void UInventoryComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UInventoryComponent::Assign(FItemData& ItemData, FInventorySlotData& InventorySlotData)
{
	ItemData.ItemID = InventorySlotData.ItemID;
	ItemData.Quantity += InventorySlotData.Quantity;
}

void UInventoryComponent::Assign(FInventorySlotData& InventorySlotData, FItemData& ItemData)
{
	InventorySlotData.ItemID = ItemData.ItemID;
	InventorySlotData.Quantity += ItemData.Quantity;

	if (InventorySlotData.Quantity > 999)
	{
		int32 Rest = InventorySlotData.Quantity - 999;
		InventorySlotData.Quantity = 999;
		ItemData.Quantity = Rest;

		AddToInventory(ItemData);
	}
}

bool UInventoryComponent::SwitchSlot(int32 Index1, int32 Index2)
{
	if (!Slots.IsValidIndex(Index1) || !Slots.IsValidIndex(Index2)) 
		return false;

	Swap(Slots[Index1], Slots[Index2]);

	OnInventoryUpdateDelegate.Broadcast();

	return true;
}

bool UInventoryComponent::AddToInventory(FItemData ItemData)
{
	for (FInventorySlotData& Slot : Slots)
	{
		if (Slot.Quantity == 0 || (ItemData.ItemID == Slot.ItemID && Slot.Quantity < 1000))
		{
			//Protocol::C_GETITEM Pkt;
			//SEND_PACKET(Pkt);
			Assign(Slot, ItemData);

			OnInventoryUpdateDelegate.Broadcast();

			return true;
		}
	}
	return false;
}

void UInventoryComponent::PopFromInventory(int32 Index, int32 Quantity)
{
	if (!Slots.IsValidIndex(Index) || Slots[Index].Quantity == 0)
		return;

	//Protocol::C_DROPITEM Pkt;
	//SEND_PACKET(Pkt);

	FItemData Tmp;
	Slots[Index].Quantity -= Quantity;
	Assign(Tmp, Slots[Index]);

	OnInventoryUpdateDelegate.Broadcast();
}

