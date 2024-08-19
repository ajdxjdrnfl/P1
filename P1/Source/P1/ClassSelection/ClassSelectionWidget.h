// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
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
	class UButton* SelectRight;

	UPROPERTY(meta = (BindWidget))
	class UButton* SelectLeft;

	UFUNCTION()
	void OnSelectRightClicked();

	UFUNCTION()
	void OnSelectLeftClicked();

};
