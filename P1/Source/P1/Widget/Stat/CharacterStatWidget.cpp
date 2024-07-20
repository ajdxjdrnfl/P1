// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterStatWidget.h"
#include "P1/Component/StatComponent/CharacterStatComponent.h"
#include "Components/ProgressBar.h"

void UCharacterStatWidget::SetHealthBar()
{
	if (CharacterStat == nullptr) 
		return;

	float HealthRate = CharacterStat->GetCurrentHealth() / CharacterStat->GetMaxHealth();
	HealthBar->SetPercent(HealthRate);
}

void UCharacterStatWidget::SetStaminaBar()
{
	if (CharacterStat == nullptr) 
		return;

	float StaminaRate = CharacterStat->GetCurrentStamina() / CharacterStat->GetMaxStamina();
	StaminaBar->SetPercent(StaminaRate);
}

void UCharacterStatWidget::SetStat(UCharacterStatComponent* StatComponent)
{
	CharacterStat = StatComponent;
	CharacterStat->SetCurrentHealth(CharacterStat->GetMaxHealth());
	CharacterStat->SetCurrentStamina(CharacterStat->GetMaxStamina());
	SetHealthBar();
	SetStaminaBar();
}
