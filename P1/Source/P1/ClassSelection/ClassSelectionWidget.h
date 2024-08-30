// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "P1/P1.h"
#include "ClassSelectionWidget.generated.h"

/**
 * 
 */
UCLASS()
class P1_API UClassSelectionWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	int32 ClassIndex;

	UPROPERTY(EditAnywhere)
	TArray<UMaterialInterface*> ClassMaterials;

	UPROPERTY(meta = (BindWidget))
	class UImage* MeshCaptureImage;

	UPROPERTY(meta = (BindWidget))
	class UButton* RightButton;

	UPROPERTY(meta = (BindWidget))
	class UButton* LeftButton;

	UPROPERTY(meta = (BindWidget))
	class UButton* SelectButton;

	UFUNCTION()
	void OnRightButtonClicked();

	UFUNCTION()
	void OnLeftButtonClicked();

	UFUNCTION()
	void OnSelectButtonClicked();

	TMap<int32, Protocol::CasterType> ClassMap;

};
