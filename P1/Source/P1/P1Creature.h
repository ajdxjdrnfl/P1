// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "P1Creature.generated.h"

UCLASS()
class P1_API AP1Creature : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AP1Creature();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
	UPROPERTY()
	class UP1ObjectBase* ObjectBase;

public:
	class UP1ObjectBase* GetObjectBase() { return ObjectBase; }

};
