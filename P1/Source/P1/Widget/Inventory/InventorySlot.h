// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "P1/Data/InventorySlotData.h"
#include "InventorySlot.generated.h"

/**
 * 
 */
UCLASS()
class P1_API UInventorySlot : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	virtual FReply NativeOnPreviewMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	//virtual FReply NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation) override;
	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	virtual void NativeOnDragEnter(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation);
	virtual void NativeOnDragLeave(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation);

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* QuantityText;

	UPROPERTY(meta = (BindWidget))
	class UImage* Thumbnail;

	FItemData ItemData;
	int32 Index;

	void SetItemData(FItemData _ItemData);

	UPROPERTY(EditAnywhere, Category = Editable)
	TSubclassOf<class UUserWidget> DragViewClass;

	UPROPERTY()
	class UInventorySlotDragView* DragView;

	void UpdateSlot();

};
