// Fill out your copyright notice in the Description page of Project Settings.


#include "SkillSlotWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Kismet/KismetMaterialLibrary.h"
#include "Kismet/KismetStringLibrary.h"

void USkillSlotWidget::NativePreConstruct()
{
	Super::NativePreConstruct();
	
	CooldownMatInstance = UKismetMaterialLibrary::CreateDynamicMaterialInstance(GetWorld(), CircleCooldownMaterial);
	CooldownImage->SetBrushFromMaterial(CooldownMatInstance);
	CooldownMatInstance->SetScalarParameterValue(FName("Percentage"), 0.0f);
	SetCooldownVisibility(true);
}

void USkillSlotWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	if (!bIsActivated) return;

	RemainingCooldownTime -= InDeltaTime;

	if (RemainingCooldownTime <= 0)
	{
		bIsActivated = false;
		SetCooldownVisibility(false);
	}

	if (CooldownMatInstance)
	{
		float RemainingTimeRate = RemainingCooldownTime / DefaultCooldownTime;
		CooldownMatInstance->SetScalarParameterValue(FName("Percentage"), RemainingTimeRate);
		TimeText->SetText(FloatToText1DecimalPlace(RemainingCooldownTime));
	}
}

FText USkillSlotWidget::FloatToText1DecimalPlace(float value)
{
	FString Str = FString::SanitizeFloat(value);
	UKismetStringLibrary::GetSubstring(Str, 0, 3);
	return FText::FromString(Str);
}

void USkillSlotWidget::SetCooldownTime(float time)
{
	DefaultCooldownTime = RemainingCooldownTime = time;
	bIsActivated = true;
	
}

void USkillSlotWidget::SetCooldownVisibility(bool bIsVisible)
{
	ESlateVisibility SlateVisible = bIsVisible ? ESlateVisibility::Visible : ESlateVisibility::Hidden;
	TimeText->SetVisibility(SlateVisible);
	CooldownImage->SetVisibility(SlateVisible);
}
