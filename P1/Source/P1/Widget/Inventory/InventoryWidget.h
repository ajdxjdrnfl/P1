// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventoryWidget.generated.h"

/**
 * 
 */
UCLASS()
class P1_API UInventoryWidget : public UUserWidget
{
	GENERATED_BODY()
	
private:
	UPROPERTY(meta = (BindWidget))
	class UWrapBox* Slots;

public:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class UInventorySlot> SlotWidgetClass;

	UFUNCTION()
	void UpdateInventory();
	
	UFUNCTION()
	void UpdatedInventory();
};
