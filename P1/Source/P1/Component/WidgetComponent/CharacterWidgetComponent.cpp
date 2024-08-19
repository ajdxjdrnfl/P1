// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterWidgetComponent.h"
#include "P1/Widget/CharacterOverlayWidget.h"
#include "P1/Widget/Stat/CharacterStatWidget.h"
#include "P1/P1Character.h"
#include "P1/P1GameInstance.h"
#include "P1/Component/StatComponent/CharacterStatComponent.h"
#include "P1/Widget/CastingSkillWidget.h"
#include "P1/DamageIndicator/DamageIndicatorActor.h"

void UCharacterWidgetComponent::BeginPlay()
{
	Super::BeginPlay();

}

void UCharacterWidgetComponent::OpenOverlayWidget()
{
	UP1GameInstance* GameInstance = GetWorld()->GetGameInstance<UP1GameInstance>();
	if (GameInstance == nullptr || !GameInstance->IsMyCharacter(OwnerCharacter->ObjectInfo->object_id()) || CharacterOverlayWidgetClass == nullptr) return;

	CharacterOverlayWidget = CreateWidget<UCharacterOverlayWidget>(GetWorld(), CharacterOverlayWidgetClass);

	if (CharacterOverlayWidget == nullptr) 
		return;

	CharacterOverlayWidget->AddToViewport();
}

void UCharacterWidgetComponent::InitOnSpawn(UStatComponentBase* StatComponent)
{
	Super::InitOnSpawn(StatComponent);

	OpenOverlayWidget();

	UP1GameInstance* GameInstance = GetWorld()->GetGameInstance<UP1GameInstance>();
	if (GameInstance && GameInstance->IsMyCharacter(OwnerCharacter->ObjectInfo->object_id()))
	{
		OwnerCharacter->GetStatComponent()->OnCharacterStaminaChangedDelegate.AddDynamic(this, &UCharacterWidgetComponent::OnCharacterStaminaChanged);
		OwnerCharacter->GetStatComponent()->OnCharacterHealthChangedDelegate.AddDynamic(this, &UCharacterWidgetComponent::OnCharacterHealthChanged);
	}

	SetCharacterStat(Cast<UCharacterStatComponent>(StatComponent));
	SetSkillButton();
}

void UCharacterWidgetComponent::UseSkill(uint32 SkillIndex)
{
	if (CharacterOverlayWidget == nullptr || Skills.IsValidIndex(SkillIndex))
		return;

	if (Skills[SkillIndex].SkillType == ESkillType::Casting || Skills[SkillIndex].SkillType == ESkillType::Charging || Skills[SkillIndex].SkillType == ESkillType::Hold) return;
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

void UCharacterWidgetComponent::OpenSkillGaugeWidget(float CastingTime)
{
	if (SkillGaugeWidgetClass == nullptr)
		return;

	SkillGaugeWidget = Cast<UCastingSkillWidget>(CreateWidget(GetWorld(), SkillGaugeWidgetClass));

	if (SkillGaugeWidget == nullptr)
		return;

	SkillGaugeWidget->AddToViewport();
	// TODO:
	SkillGaugeWidget->StartGauge(CastingTime);
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

void UCharacterWidgetComponent::SetSkillButton()
{
	if (CharacterOverlayWidget == nullptr) return;
	CharacterOverlayWidget->SetSkillButton(Skills);
}

void UCharacterWidgetComponent::SpawnDamageIndicator(float Damage)
{
	FVector SpawnLocation = OwnerCharacter->GetActorLocation();
	ADamageIndicatorActor* DamageIndicator = Cast<ADamageIndicatorActor>(GetWorld()->SpawnActor(DamageIndicatorActorClass, &SpawnLocation));
	if (DamageIndicator)
	{
		DamageIndicator->ActivateDamageIndicator(Damage, false);
	}
}
