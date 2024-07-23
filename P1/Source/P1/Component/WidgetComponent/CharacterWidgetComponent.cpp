// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterWidgetComponent.h"
#include "P1/Widget/CharacterOverlayWidget.h"
#include "P1/Widget/Stat/CharacterStatWidget.h"
#include "P1/P1Character.h"
#include "P1/Component/StatComponent/CharacterStatComponent.h"

void UCharacterWidgetComponent::BeginPlay()
{
	Super::BeginPlay();

	OpenOverlayWidget();

	// TODO: 
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

	CharacterOverlayWidget->UseSkill(Skills[SkillIndex]);
}

void UCharacterWidgetComponent::SetCharacterStat(UCharacterStatComponent* StatComponent)
{
	if (CharacterOverlayWidget == nullptr)
		return;

	CharacterOverlayWidget->SetCharacterStat(StatComponent);
}

void UCharacterWidgetComponent::OnCharacterHealthChanged()
{
	if (CharacterOverlayWidget == nullptr) 
		return;

	CharacterOverlayWidget->GetStatWidget()->SetHealthBar();
}
