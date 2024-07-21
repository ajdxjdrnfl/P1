// Fill out your copyright notice in the Description page of Project Settings.


#include "P1ObjectBase.h"

// Sets default values
AP1ObjectBase::AP1ObjectBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AP1ObjectBase::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AP1ObjectBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

