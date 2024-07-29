// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterWidgetComponent.h"
#include "P1/Widget/CharacterOverlayWidget.h"
#include "P1/Widget/Stat/CharacterStatWidget.h"
#include "P1/P1Character.h"
#include "P1/Component/StatComponent/CharacterStatComponent.h"
#include "P1/Widget/CastingSkillWidget.h"

void UCharacterWidgetComponent::BeginPlay()
{
	Super::BeginPlay();

	OpenOverlayWidget();

	// TODO: 
	Cast<AP1Character>(GetOwner())->GetStatComponent()->OnCharacterStaminaChangedDelegate.AddDynamic(this, &UCharacterWidgetComponent::OnCharacterStaminaChanged);
	Cast<AP1Character>(GetOwner())->GetStatComponent()->OnCharacterHealthChangedDelegate.AddDynamic(this, &UCharacterWidgetComponent::OnCharacterHealthChanged);
}

void UCharacterWidgetComponent::OpenOverlayWidget()
{
	if (CharacterOverlayWidgetClass == nullptr) 
		return;

	CharacterOverlayWidget = CreateWidget<UCharacterOverlayWidget>(GetWorld(), CharacterOverlayWidgetClass);

	if (CharacterOverlayWidget == nullptr) 
		return;

	CharacterOverlayWidget->AddToViewport();
	
}

void UCharacterWidgetComponent::UseSkill(uint32 SkillIndex)
{
	if (CharacterOverlayWidget == nullptr)
		return;

	if (Skills[SkillIndex].SkillType == ESkillType::Casting || Skills[SkillIndex].SkillType == ESkillType::Charging) return;
	CharacterOverlayWidget->UseSkill(Skills[SkillIndex]);
}

void UCharacterWidgetComponent::SetCharacterStat(UCharacterStatComponent* StatComponent)
{
	if (CharacterOverlayWidget == nullptr)
		return;

	CharacterOverlayWidget->SetCharacterStat(StatComponent);
}

void UCharacterWidgetComponent::OnCharacterStaminaChanged()
{
	if (CharacterOverlayWidget == nullptr) 
		return;

	CharacterOverlayWidget->GetStatWidget()->SetStaminaBar();
}

void UCharacterWidgetComponent::OnCharacterHealthChanged()
{
	if (CharacterOverlayWidget == nullptr)
		return;

	CharacterOverlayWidget->GetStatWidget()->SetHealthBar();
}

void UCharacterWidgetComponent::OpenSkillGaugeWidget()
{
	if (SkillGaugeWidgetClass == nullptr)
		return;

	SkillGaugeWidget = Cast<UCastingSkillWidget>(CreateWidget(GetWorld(), SkillGaugeWidgetClass));

	if (SkillGaugeWidget == nullptr)
		return;

	SkillGaugeWidget->AddToViewport();
	// TODO:
	SkillGaugeWidget->StartGauge(1.f);
}

void UCharacterWidgetComponent::CloseSkillGaugeWidget()
{
	if (SkillGaugeWidget == nullptr) return;

	SkillGaugeWidget->RemoveFromParent();
}

float UCharacterWidgetComponent::GetGaugeRate()
{
	if (SkillGaugeWidget == nullptr) return 0;

	return SkillGaugeWidget->GetGaugeRate();
}
