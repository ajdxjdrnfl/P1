// Fill out your copyright notice in the Description page of Project Settings.


#include "SkillSlotWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Kismet/KismetMaterialLibrary.h"
#include "Kismet/KismetStringLibrary.h"
#include "P1/SubSystem/GameInstance/SkillManagerSubSystem.h"

void USkillSlotWidget::NativePreConstruct()
{
	Super::NativePreConstruct();
	
	CooldownMatInstance = UKismetMaterialLibrary::CreateDynamicMaterialInstance(GetWorld(), CircleCooldownMaterial);
	CooldownImage->SetBrushFromMaterial(CooldownMatInstance);
	CooldownMatInstance->SetScalarParameterValue(FName("Percentage"), 0.0f);
	SetCooldownVisibility(false);
}

void USkillSlotWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	if (!bIsActivated) return;

	RemainingCooldownTime -= InDeltaTime;

	if (RemainingCooldownTime <= 0)
	{
		if (USkillManagerSubSystem* SkillSubSystem = GetGameInstance()->GetSubsystem<USkillManagerSubSystem>())
		{
			SkillSubSystem->SkillCanUseMap[SkillNum] = true;
			bIsActivated = false;
		}
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
	int32 FloorVal = FMath::FloorToInt(value);
	FString Str = FString::FromInt(FloorVal);
	return FText::FromString(Str);
}

void USkillSlotWidget::ActivateSlot(float time)
{
	if (bIsActivated)
		return;

	DefaultCooldownTime = RemainingCooldownTime = time;

	if (USkillManagerSubSystem* SkillSubSystem = GetGameInstance()->GetSubsystem<USkillManagerSubSystem>())
	{
		SkillSubSystem->SkillCanUseMap[SkillNum] = false;
		bIsActivated = true;
	}
	
	SetCooldownVisibility(true);
}

void USkillSlotWidget::SetCooldownVisibility(bool bIsVisible)
{
	ESlateVisibility SlateVisible = bIsVisible ? ESlateVisibility::Visible : ESlateVisibility::Hidden;
	TimeText->SetVisibility(SlateVisible);
	CooldownImage->SetVisibility(SlateVisible);
}

void USkillSlotWidget::SetSkill(/*TODO: SkillInfo*/)
{
	// SkillImage = SkillInfo.Image;
	// ...
}

void USkillSlotWidget::SetKeyText(FString Key)
{
	KeyText->SetText(FText::FromString(Key));
}
