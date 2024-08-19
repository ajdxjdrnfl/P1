// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Templates/SubclassOf.h"
#include "GameFramework/PlayerController.h"
#include "InputActionValue.h"
#include "P1PlayerController.generated.h"

/** Forward declaration to improve compiling times */
class UNiagaraSystem;
class UInputMappingContext;
class UInputAction;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnRMBClickedDelegate);

UCLASS()
class AP1PlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AP1PlayerController();

	/** Time Threshold to know if it was a short press */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	float ShortPressThreshold;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UNiagaraSystem* FXCursor;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	UInputAction* SetDestinationClickAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* RMBClickAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	UInputAction* SetDestinationTouchAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* QSkillAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* WSkillAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* ESkillAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* RSkillAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* DodgeAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* FOVAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* StopMovingAction;

	FOnRMBClickedDelegate OnRMBClickedDelegate;

protected:
	/** True if the controlled character should navigate to the mouse cursor. */
	uint32 bMoveToMouseCursor : 1;

	virtual void SetupInputComponent() override;

	virtual void OnPossess(APawn* aPawn) override;
	
	// To add mapping context
	virtual void BeginPlay();
	virtual void Tick(float DeltaTime) override;

	/** Input handlers for SetDestination action. */
	void OnInputStarted();
	void OnSetDestinationTriggered();
	void OnSetDestinationReleased();
	void OnTouchTriggered();
	void OnTouchReleased();
	void OnQSkillTriggered();
	void OnQSkillReleased();
	void OnWSkillTriggered();
	void OnWSkillReleased();
	void OnESkillTriggered();
	void OnESkillReleased();
	void OnRSkillTriggered();
	void OnRSkillReleased();
	void OnDodgeTriggered();
	void OnRMBTriggered();
	void OnStopMovingTriggered();
	void OnFOVTriggered(const FInputActionValue& Value);

private:
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	TArray<FKey> KeyArray;

	float CurrentTimeToSendPacket;
	float DefaultTimeToSendPacket = 0.1f;

	FVector CachedDestination;

	bool bIsTouch; // Is it a touch device
	float FollowTime; // For how long it has been pressed

	UPROPERTY()
	class AP1Character* OwnerCharacter;

	FVector MouseLocation;

	//TMap<EKeys, uint16> KeySkillMap;

	void MoveByServer(float DeltaTime);
	void SendMovePacketToServer();

public:
	FORCEINLINE FVector GetMouseLocation() const { return MouseLocation; }
	//FORCEINLINE float GetRoundTripTime() const { return RoundTripTime; }
	//FORCEINLINE void SetRoundTripTime(float TimeToSet) { RoundTripTime = TimeToSet; }
};


