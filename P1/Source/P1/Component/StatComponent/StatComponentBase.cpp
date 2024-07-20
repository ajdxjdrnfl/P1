// Fill out your copyright notice in the Description page of Project Settings.


#include "StatComponentBase.h"

UStatComponentBase::UStatComponentBase()
{
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


void UStatComponentBase::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


void UStatComponentBase::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

