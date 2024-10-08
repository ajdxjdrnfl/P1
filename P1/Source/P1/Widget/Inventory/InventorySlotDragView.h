// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "P1/Data/InventorySlotData.h"
#include "InventorySlotDragView.generated.h"

/**
 * 
 */
UCLASS()
class P1_API UInventorySlotDragView : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;
	FItemData ItemData;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), meta = (AllowPrivateAccess = "true"))
	class UImage* Image;

};
