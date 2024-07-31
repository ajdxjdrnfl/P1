// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "P1Creature.h"
#include "P1.h"
#include "P1Character.generated.h"

UCLASS(Blueprintable)
class AP1Character : public AP1Creature
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

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCharacterSkillComponent* SkillComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCharacterStatComponent* StatComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCharacterWidgetComponent* WidgetComponent;

public:
	void OnSpawn(float HealthToSet, float StaminaToSet);
	void InitOnSpawn(float HealthToSet, float StaminaToSet);
	void Init();
	void UseSkill(uint16 SkillIndex);
	void MoveByServer(float DeltaTime);
	void SetMoveValueByServer(Protocol::S_MOVE Pkt);
	void OpenSkillGaugeWidget();
	void CloseSkillGaugeWidget();
	float GetGaugeRate();
	void PlayAnimMontageByServer(bool bIsStop, int32 SkillIndexLocal, int32 SectionIndex);

	FORCEINLINE class UCharacterStatComponent* GetStatComponent() const { return StatComponent; }
	FORCEINLINE class UCharacterSkillComponent* GetSkillComponent() const { return SkillComponent; }
	FORCEINLINE class UCharacterWidgetComponent* GetWidgetComponent() const { return WidgetComponent; }
};

