// Fill out your copyright notice in the Description page of Project Settings.


#include "P1Creature.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "P1/P1.h"

// Sets default values
AP1Creature::AP1Creature()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	GetCharacterMovement()->bRunPhysicsWithNoController = true;

	ObjectInfo = new Protocol::ObjectInfo();
}

// Called when the game starts or when spawned
void AP1Creature::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AP1Creature::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AP1Creature::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

