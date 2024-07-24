// Fill out your copyright notice in the Description page of Project Settings.


#include "MobHealthBarWidgetComponent.h"

UMobHealthBarWidgetComponent::UMobHealthBarWidgetComponent()
{
	
}

void UMobHealthBarWidgetComponent::BeginPlay()
{
	Super::BeginPlay();

}

void UMobHealthBarWidgetComponent::SetStatWidgetClass(TSubclassOf<UUserWidget> StatWidget)
{
	SetWidgetClass(StatWidget);
}
