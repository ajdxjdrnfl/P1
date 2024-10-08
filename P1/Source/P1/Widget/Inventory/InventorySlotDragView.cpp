// Fill out your copyright notice in the Description page of Project Settings.


#include "InventorySlotDragView.h"
#include "P1/SubSystem/GameInstance/ItemSubsystem.h"
#include "Components/Image.h"

void UInventorySlotDragView::NativeConstruct()
{
	Super::NativeConstruct();

	UItemSubsystem* ItemSubsystem = GetGameInstance()->GetSubsystem<UItemSubsystem>();
	if (ItemSubsystem && ItemSubsystem->ItemDataTable)
	{
		FItemTableData* Data = ItemSubsystem->ItemDataTable->FindRow<FItemTableData>(FName(ItemData.ItemID), "");
		if (Data)
		{
			Image->SetBrushFromTexture(Data->Thumbnail);
		}
	}
}
