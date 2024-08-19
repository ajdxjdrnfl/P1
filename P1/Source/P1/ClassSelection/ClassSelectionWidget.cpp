// Fill out your copyright notice in the Description page of Project Settings.


#include "ClassSelectionWidget.h"
#include "Components/Button.h"
#include "Components/Image.h"

void UClassSelectionWidget::NativeConstruct()
{
	Super::NativeConstruct();

	SelectLeft->OnClicked.AddDynamic(this, &UClassSelectionWidget::OnSelectLeftClicked);
	SelectRight->OnClicked.AddDynamic(this, &UClassSelectionWidget::OnSelectRightClicked);
}

void UClassSelectionWidget::OnSelectRightClicked()
{
	ClassIndex++;
	if (ClassIndex == ClassMaterials.Num()) ClassIndex = 0;
	MeshCaptureImage->SetBrushFromMaterial(ClassMaterials[ClassIndex]);
}

void UClassSelectionWidget::OnSelectLeftClicked()
{
	ClassIndex--;
	if (ClassIndex == -1) ClassIndex = ClassMaterials.Num() - 1;
	MeshCaptureImage->SetBrushFromMaterial(ClassMaterials[ClassIndex]);
}
