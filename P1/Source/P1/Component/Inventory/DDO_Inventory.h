// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/DragDropOperation.h"
#include "P1/Data/InventorySlotData.h"
#include "DDO_Inventory.generated.h"

/**
 * 
 */
UCLASS()
class P1_API UDDO_Inventory : public UDragDropOperation
{
	GENERATED_BODY()

public:
	int32 Index;
	FItemData ItemData;
};
