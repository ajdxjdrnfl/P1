// Fill out your copyright notice in the Description page of Project Settings.


#include "InventorySlot.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "P1/SubSystem/GameInstance/ItemSubsystem.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "P1/Widget/Inventory/InventorySlotDragView.h"
#include "P1/Component/Inventory/DDO_Inventory.h"
#include "Kismet/GameplayStatics.h"
#include "P1/P1Character.h"
#include "P1/Component/Inventory/InventoryComponent.h"

void UInventorySlot::NativeConstruct()
{
	Super::NativeConstruct();

}

FReply UInventorySlot::NativeOnPreviewMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	FReply Reply = Super::NativeOnPreviewMouseButtonDown(InGeometry, InMouseEvent);
	if (ItemData.ItemID == "") return FReply::Unhandled();
	if (InMouseEvent.IsMouseButtonDown(EKeys::LeftMouseButton))
	{
		return UWidgetBlueprintLibrary::DetectDragIfPressed(InMouseEvent, this, EKeys::LeftMouseButton).NativeReply;
	}
	return Reply;
}

/*FReply UInventorySlot::NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	return FReply();
}*/

void UInventorySlot::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation)
{
	Super::NativeOnDragDetected(InGeometry, InMouseEvent, OutOperation);
	if (DragViewClass)
	{
		DragView = Cast<UInventorySlotDragView>(CreateWidget(this, DragViewClass));
		if (DragView)
		{
			if (ItemData.ItemID == "" || ItemData.Quantity == 0) return;
			DragView->ItemData = ItemData;
			UDDO_Inventory* DDO = Cast<UDDO_Inventory>(UWidgetBlueprintLibrary::CreateDragDropOperation(UDDO_Inventory::StaticClass()));
			if (DDO)
			{
				DDO->Index = Index;
				DDO->ItemData = ItemData;

				OutOperation = DDO;
			}
		}
	}
}

bool UInventorySlot::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);
	UInventoryComponent* InventoryComponent = Cast<AP1Character>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0))->GetInventoryComponent();
	if (UDDO_Inventory* DDO = Cast<UDDO_Inventory>(InOperation))
	{
		if (DDO->Index == Index) 
			return false;
		
		InventoryComponent->SwitchSlot(DDO->Index, Index);
	}
	
	return false;
}

void UInventorySlot::NativeOnDragEnter(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
}

void UInventorySlot::NativeOnDragLeave(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
}

void UInventorySlot::SetItemData(FItemData _ItemData)
{
	ItemData.ItemID = _ItemData.ItemID;
	ItemData.Quantity = _ItemData.Quantity;

	UpdateSlot();
}

void UInventorySlot::UpdateSlot()
{
	UItemSubsystem* Subsystem = GetGameInstance()->GetSubsystem<UItemSubsystem>();
	if (ItemData.ItemID == "" || ItemData.Quantity == 0)
	{
		Thumbnail->SetBrushFromTexture(nullptr);
		return;
	}
	FString ContextString = "";
	FItemTableData* ItemTableData = Subsystem->ItemDataTable->FindRow<FItemTableData>(FName(ItemData.ItemID), ContextString);
	if (ItemTableData)
	{
		Thumbnail->SetBrushFromTexture(ItemTableData->Thumbnail);
		QuantityText->SetText(FText::FromString(FString::FromInt(ItemData.Quantity)));
	}
}
