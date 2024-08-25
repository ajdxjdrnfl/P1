// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "P1Creature.h"
#include "P1/Interface/InteractInterface.h"
#include "P1Character.generated.h"

UCLASS(Blueprintable)
class AP1Character : public AP1Creature, public IInteractInterface
{
	GENERATED_BODY()

public:
	AP1Character();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void PostInitializeComponents() override;

	FORCEINLINE class UCameraComponent* GetTopDownCameraComponent() const { return TopDownCameraComponent; }
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* TopDownCameraComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UCharacterSkillComponent* SkillComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UCharacterStatComponent* StatComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UCharacterWidgetComponent* WidgetComponent;

	float TargetCameraBoomLength;

	bool bDoOnceDodge;

	UPROPERTY()
	TScriptInterface<IInteractInterface> InteractInterfaceActor;

public:
	void InitOnSpawn(Protocol::ObjectInfo ObjInfo);
	void UseSkill(uint16 SkillIndex);
	void OpenSkillGaugeWidget(float CastingTime);
	void CloseSkillGaugeWidget();
	float GetGaugeRate();
	virtual void PlayAnimMontageByServer(Protocol::S_MONTAGE& pkt) override;
	virtual FSkillInfo GetSkillInfoByIndex(int32 SkillIndex) override;
	virtual void TickMove(float DeltaTime) override;
	void TickIdle(float DeltaTime);
	void TickStop();
	void TickJump();
	void TickSkill(float DeltaTime);
	virtual void SetHealthByDamage(float HealthToSet) override;
	virtual void SetSpawnedSkill(int32 SkillID, class ASkillActorBase* SkillActor) override;
	void Dodge();
	void AddCameraBoomLength(float Value);
	void StopMoving();
	void Interact();

	FORCEINLINE class UCharacterStatComponent* GetStatComponent() const { return StatComponent; }
	FORCEINLINE class UCharacterSkillComponent* GetSkillComponent() const { return SkillComponent; }
	FORCEINLINE class UCharacterWidgetComponent* GetWidgetComponent() const { return WidgetComponent; }
	FORCEINLINE void SetOnceDodge(bool bOnce) { bDoOnceDodge = bOnce; }
	FORCEINLINE void SetInteractInterfaceActor(TScriptInterface<IInteractInterface> ActorToSet) { InteractInterfaceActor = ActorToSet; }
	FORCEINLINE TScriptInterface<IInteractInterface> GetInteractInterfaceActor() const { return InteractInterfaceActor; }
};

