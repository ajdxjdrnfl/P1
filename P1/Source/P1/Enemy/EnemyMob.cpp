// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyMob.h"
#include "Components/WidgetComponent.h"
#include "P1/Component/WidgetComponent/EnemyWidgetComponent.h"
#include "P1/Widget/Stat/EnemyStatWidget.h"
#include "P1/Component/WidgetComponent/MobHealthBarWidgetComponent.h"

AEnemyMob::AEnemyMob()
{
	HealthBarWidgetComponent = CreateDefaultSubobject<UMobHealthBarWidgetComponent>(TEXT("HealthBarWidget"));
	HealthBarWidgetComponent->SetupAttachment(GetMesh());
}

void AEnemyMob::BeginPlay()
{
	if (HealthBarWidgetClass)
	{
		HealthBarWidgetComponent->SetStatWidgetClass(HealthBarWidgetClass);
		HealthBarWidgetComponent->InitWidget();
		UUserWidget* UserWidget = HealthBarWidgetComponent->GetUserWidgetObject();
		UEnemyStatWidget* EnemyStatWidget = Cast<UEnemyStatWidget>(UserWidget);
		GetWidgetComponent()->SetStatWidget(EnemyStatWidget);
	}

	// Set statcomponent, before setting enemy stat: statcomponent setting (this::BeginPlay()) -> stat setting (Super::BeginPlay())
	Super::BeginPlay();
}
