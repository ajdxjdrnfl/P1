// Fill out your copyright notice in the Description page of Project Settings.


#include "CinematicSubsystem.h"
#include "P1/P1GameInstance.h"
#include "P1/P1Character.h"
#include "Kismet/GameplayStatics.h"
#include "LevelSequence.h"
#include "LevelSequencePlayer.h"
#include "LevelSequenceActor.h"
#include "P1/SubSystem/GameInstance/SkillManagerSubsystem.h"
#include "P1/Enemy/EnemyBoss.h"

void UCinematicSubsystem::StartCinema(ULevelSequence* Sequence, AEnemyBoss* Boss)
{
	SetCharactersVisibility(false);

	CurrentBoss = Boss;
	if (Sequence && DeathSequencePlayer == nullptr)
	{
		DeathSequencePlayer = ULevelSequencePlayer::CreateLevelSequencePlayer(GWorld, Sequence, FMovieSceneSequencePlaybackSettings{}, DeathSequenceActor);
		DeathSequencePlayer->Play();
		DeathSequencePlayer->OnFinished.AddUniqueDynamic(this, &UCinematicSubsystem::OnDeathSequenceEnd);
		if (USkillManagerSubSystem* SkillSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<USkillManagerSubSystem>())
		{
			SkillSubsystem->SetCanMove(false);
		}
	}
}

void UCinematicSubsystem::SetCharactersVisibility(bool bIsVisible)
{
	UP1GameInstance* GameInstance = Cast<UP1GameInstance>(UGameplayStatics::GetGameInstance(GWorld));
	if (GameInstance)
	{
		for (auto Info : GameInstance->Characters)
		{
			if (GameInstance->IsMyCharacter(Info.Value->ObjectInfo->object_id()))
			{
				Info.Value->SetWidgetVisibility(bIsVisible);
			}
			if (Info.Value)
			{
				Info.Value->SetActorHiddenInGame(!bIsVisible);
			}
		}
	}
}

void UCinematicSubsystem::OnDeathSequenceEnd()
{
	if (IsValid(CurrentBoss))
	{
		CurrentBoss->OnDeathSequenceEnd();
	}
	SetCharactersVisibility(true);
	if (USkillManagerSubSystem* SkillSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<USkillManagerSubSystem>())
	{
		SkillSubsystem->SetCanMove(true);
	}
}
