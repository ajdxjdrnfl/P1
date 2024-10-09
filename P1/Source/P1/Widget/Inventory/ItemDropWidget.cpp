// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemDropWidget.h"
#include "P1/Component/Inventory/DDO_Inventory.h"
#include "Kismet/GameplayStatics.h"
#include "P1/P1Character.h"
#include "P1/Component/Inventory/InventoryComponent.h"

bool UItemDropWidget::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);

	UInventoryComponent* InventoryComponent = Cast<AP1Character>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0))->GetInventoryComponent();
	if (UDDO_Inventory* DDO = Cast<UDDO_Inventory>(InOperation))
	{
		if (InventoryComponent->GetSlots().IsValidIndex(DDO->Index))
		{
			//InventoryComponent->GetSlots()[DDO->Index].ItemID = FString("");
			//InventoryComponent->GetSlots()[DDO->Index].Quantity = 0;

			InventoryComponent->PopFromInventory(DDO->Index, InventoryComponent->GetSlots()[DDO->Index].Quantity);

			return true;
		}
	}

	return false;
}
