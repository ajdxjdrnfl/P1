// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterStatWidget.h"
#include "P1/Component/StatComponent/CharacterStatComponent.h"
#include "Components/ProgressBar.h"

void UCharacterStatWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// TODO:
	//if (!CharacterStatComponent->OnCharacterHealthChangedDelegate.IsBound())
	//	CharacterStatComponent->OnCharacterHealthChangedDelegate.AddDynamic(this, &UCharacterStatWidget::OnCharacterHealthChanged);
}

void UCharacterStatWidget::SetHealthBar()
{
	if (CharacterStatComponent == nullptr)
		return;

	float HealthRate = CharacterStatComponent->GetCurrentHealth() / CharacterStatComponent->GetMaxHealth();
	HealthBar->SetPercent(HealthRate);
}

void UCharacterStatWidget::SetStaminaBar()
{
	if (CharacterStatComponent == nullptr)
		return;

	float StaminaRate = CharacterStatComponent->GetCurrentStamina() / CharacterStatComponent->GetMaxStamina();
	StaminaBar->SetPercent(StaminaRate);
}

void UCharacterStatWidget::OnCharacterHealthChanged()
{
	SetHealthBar();
}

void UCharacterStatWidget::SetCharacterStat(UCharacterStatComponent* StatComponent)
{
	CharacterStatComponent = StatComponent;
	CharacterStatComponent->SetCurrentHealth(CharacterStatComponent->GetMaxHealth());
	CharacterStatComponent->SetCurrentStamina(CharacterStatComponent->GetMaxStamina());
	SetHealthBar();
	SetStaminaBar();
}
