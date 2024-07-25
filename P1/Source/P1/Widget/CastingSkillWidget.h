// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CastingSkillWidget.generated.h"

/**
 * 
 */
UCLASS()
class P1_API UCastingSkillWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

private:
	bool bIsActivated;
	float CurrentValue;
	float MaxValue;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), meta = (AllowPrivateAccess = "true"))
	class UProgressBar* CastingBar;

public:
	void StartCasting(float MaxValueToSet);
	void SetCastingBar(float ValueToAdd);
	bool IsCastingEnd();

};
