// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnemyBase.h"
#include "EnemyBoss.generated.h"

/*UENUM(BlueprintType)
enum class EBossWSkillState : uint8
{
	Normal UMETA(Displayname = "Normal"),
	Vanish UMETA(Displayname = "Vanish"),
	Emergence UMETA(Displayname = "Emergence"),
	SpawnPillar UMETA(Displayname = "SpawnPillar"),
	SpellStart UMETA(Displayname = "SpellStart"),
	SpellLoop UMETA(Displayname = "SpellLoop" ),
	SpellStop UMETA(Displayname = "SpellStop" ),
};*/

UCLASS()
class P1_API AEnemyBoss : public AEnemyBase
{
	GENERATED_BODY()
	
public:
	virtual void BeginPlay() override;

private:
	bool bIsAttackMode;

	UPROPERTY()
	class AAIControllerEnemy* EnemyController;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UUserWidget> HealthBarWidgetClass;

public:
	void MoveToTargetLocation(FVector TargetLoc);
	void MoveToTargetActor(AActor* TargetActor);
	void UseSkill(int32 SkillIndex);
	void UsePillarSkill(FVector2D Location);
	void SetAttackMode(bool bAttackMode);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<int32> BossSkillState;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 BossSkillStateIndex;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 BossSkillID;

	virtual void PlayAnimMontageByServer(Protocol::S_MONTAGE& pkt) override;

	UFUNCTION()
	virtual void OnCollisionOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};
