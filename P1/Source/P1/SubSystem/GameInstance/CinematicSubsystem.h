// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "CinematicSubsystem.generated.h"

UCLASS()
class P1_API UCinematicSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
    UPROPERTY()
    class AEnemyBoss* CurrentBoss;

    UPROPERTY()
    class ULevelSequencePlayer* DeathSequencePlayer;

    UPROPERTY()
    class ALevelSequenceActor* DeathSequenceActor;

public:
    void StartCinema(class ULevelSequence* Sequence, class AEnemyBoss* Boss);
    void SetCharactersVisibility(bool bIsVisible);

    UFUNCTION()
    void OnDeathSequenceEnd();

};
