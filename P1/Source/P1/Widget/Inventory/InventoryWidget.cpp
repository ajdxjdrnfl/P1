// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryWidget.h"
#include "Components/WrapBox.h"
#include "Components/Image.h"
#include "P1/P1Character.h"
#include "P1/Component/Inventory/InventoryComponent.h"
#include "Kismet/GameplayStatics.h"
#include "P1/Widget/Inventory/InventorySlot.h"
#include "P1/Component/Inventory/DDO_Inventory.h"

void UInventoryWidget::NativeConstruct()
{
	Super::NativeConstruct();

	UpdateInventory();
}

void UInventoryWidget::NativeDestruct()
{
	Super::NativeDestruct();

	AP1Character* P1Character = Cast<AP1Character>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	if (P1Character)
	{
		UInventoryComponent* InventoryComponent = P1Character->GetInventoryComponent();
		InventoryComponent->OnInventoryUpdateDelegate.Clear();
	}
}

void UInventoryWidget::UpdateInventory()
{
	Slots->ClearChildren();
	AP1Character* P1Character = Cast<AP1Character>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	if (P1Character)
	{
		UInventoryComponent* InventoryComponent = P1Character->GetInventoryComponent();
		if (InventoryComponent && SlotWidgetClass)
		{
			TArray<FInventorySlotData> _Slots = InventoryComponent->GetSlots();
			for (int i = 0; i < _Slots.Num(); i++)
			{
				UInventorySlot* SlotWidget = CreateWidget<UInventorySlot>(GetWorld(), SlotWidgetClass);
				if (SlotWidget)
				{
					SlotWidget->ItemData = FItemData(_Slots[i].Quantity, _Slots[i].ItemID);
					SlotWidget->UpdateSlot();
					SlotWidget->Index = i;
					Slots->AddChildToWrapBox(SlotWidget);
				}
			}

			if (!InventoryComponent->OnInventoryUpdateDelegate.IsBound())
				InventoryComponent->OnInventoryUpdateDelegate.AddUFunction(this, FName("UpdatedInventory"));
		}
	}
}

void UInventoryWidget::UpdatedInventory()
{
	Slots->ClearChildren();
	UpdateInventory();
}
