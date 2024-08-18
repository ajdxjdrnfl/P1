// Fill out your copyright notice in the Description page of Project Settings.


#include "DamageIndicatorWidget.h"
#include "Components/TextBlock.h"
#include "P1/DamageIndicator/DamageIndicatorActor.h"

void UDamageIndicatorWidget::NativeConstruct()
{ 
	Super::NativeConstruct();

	OnDamageIndicatorAnimationEndDelegate.BindDynamic(this, &UDamageIndicatorWidget::OnDamageIndicatorAnimationEnd);
	BindToAnimationFinished(DamageIndicatorAnimation, OnDamageIndicatorAnimationEndDelegate);
}

void UDamageIndicatorWidget::ActivateDamageIndicatorWidget(ADamageIndicatorActor* _DamageIndicatorActor, float Damage, bool bIsEnemy)
{
	DamageIndicatorActor = _DamageIndicatorActor;

	FString DamageString = FString::SanitizeFloat(Damage);
	FText DamageText = FText::FromString(DamageString);
	DamageIndicatorText->SetText(DamageText);
	//DamageIndicatorText->SetColorAndOpacity(bIsEnemy ? EnemyDamageColor : CharacterDamageColor);

	PlayAnimation(DamageIndicatorAnimation);
}

void UDamageIndicatorWidget::OnDamageIndicatorAnimationEnd()
{
	if (IsValid(DamageIndicatorActor))
	{
		DamageIndicatorActor->Destroy();
	}
}
