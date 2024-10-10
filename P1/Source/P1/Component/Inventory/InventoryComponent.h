// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "P1/Data/InventorySlotData.h"
#include "InventoryComponent.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnInventoryUpdateDelegate);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class P1_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UInventoryComponent();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	TArray<FInventorySlotData> Slots;

	void Assign(FItemData& ItemData, FInventorySlotData& InventorySlotData);
	void Assign(FInventorySlotData& InventorySlotData, FItemData& ItemData);

public:
	FOnInventoryUpdateDelegate OnInventoryUpdateDelegate;
	bool SwitchSlot(int32 Index1, int32 Index2);
	bool AddToInventory(FItemData ItemData);
	void PopFromInventory(int32 Index, int32 Quantity);
	
	FORCEINLINE TArray<FInventorySlotData> GetSlots() { return Slots; }

};
