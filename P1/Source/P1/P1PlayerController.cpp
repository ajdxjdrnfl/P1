// Copyright Epic Games, Inc. All Rights Reserved.

#include "P1PlayerController.h"
#include "GameFramework/Pawn.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "NiagaraSystem.h"
#include "NiagaraFunctionLibrary.h"
#include "P1Character.h"
#include "Engine/World.h"
#include "EnhancedInputComponent.h"
#include "InputActionValue.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"
#include "P1Character.h"
#include "P1.h"
#include "P1ObjectBase.h"
#include "InputMappingContext.h"
#include "InputAction.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

AP1PlayerController::AP1PlayerController()
{
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Default;
	CachedDestination = FVector::ZeroVector;
	FollowTime = 0.f;
}

void AP1PlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		Subsystem->AddMappingContext(DefaultMappingContext, 0);

		for (int i = 0; i < 12; i++)
		{
			//6254!
			// KeySkillMap.Add(KeyArray[i], i);
		}
	}
}

void AP1PlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	MoveByServer(DeltaTime);
}

void AP1PlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent))
	{
		EnhancedInputComponent->BindAction(SetDestinationClickAction, ETriggerEvent::Started, this, &AP1PlayerController::OnInputStarted);
		EnhancedInputComponent->BindAction(SetDestinationClickAction, ETriggerEvent::Triggered, this, &AP1PlayerController::OnSetDestinationTriggered);
		EnhancedInputComponent->BindAction(SetDestinationClickAction, ETriggerEvent::Completed, this, &AP1PlayerController::OnSetDestinationReleased);
		EnhancedInputComponent->BindAction(SetDestinationClickAction, ETriggerEvent::Canceled, this, &AP1PlayerController::OnSetDestinationReleased);

		EnhancedInputComponent->BindAction(SetDestinationTouchAction, ETriggerEvent::Started, this, &AP1PlayerController::OnInputStarted);
		EnhancedInputComponent->BindAction(SetDestinationTouchAction, ETriggerEvent::Triggered, this, &AP1PlayerController::OnTouchTriggered);
		EnhancedInputComponent->BindAction(SetDestinationTouchAction, ETriggerEvent::Completed, this, &AP1PlayerController::OnTouchReleased);
		EnhancedInputComponent->BindAction(SetDestinationTouchAction, ETriggerEvent::Canceled, this, &AP1PlayerController::OnTouchReleased);

		EnhancedInputComponent->BindAction(Skill1Action, ETriggerEvent::Started, this, &AP1PlayerController::OnSkillTriggered);
		EnhancedInputComponent->BindAction(Skill2Action, ETriggerEvent::Started, this, &AP1PlayerController::OnSkillTriggered);
		EnhancedInputComponent->BindAction(Skill3Action, ETriggerEvent::Started, this, &AP1PlayerController::OnSkillTriggered);
		EnhancedInputComponent->BindAction(Skill4Action, ETriggerEvent::Started, this, &AP1PlayerController::OnSkillTriggered);

		EnhancedInputComponent->BindAction(Skill1Action, ETriggerEvent::Completed, this, &AP1PlayerController::OnSkillReleased);
		EnhancedInputComponent->BindAction(Skill2Action, ETriggerEvent::Completed, this, &AP1PlayerController::OnSkillReleased);
		EnhancedInputComponent->BindAction(Skill3Action, ETriggerEvent::Completed, this, &AP1PlayerController::OnSkillReleased);
		EnhancedInputComponent->BindAction(Skill4Action, ETriggerEvent::Completed, this, &AP1PlayerController::OnSkillReleased);
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input Component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void AP1PlayerController::OnPossess(APawn* aPawn)
{
	Super::OnPossess(aPawn);

	OwnerCharacter = Cast<AP1Character>(aPawn);
}

void AP1PlayerController::OnInputStarted()
{
	StopMovement();
}

void AP1PlayerController::OnSetDestinationTriggered()
{
	FollowTime += GetWorld()->GetDeltaSeconds();
	
	FHitResult Hit;
	bool bHitSuccessful = false;
	if (bIsTouch)
	{
		bHitSuccessful = GetHitResultUnderFinger(ETouchIndex::Touch1, ECollisionChannel::ECC_Visibility, true, Hit);
	}
	else
	{
		bHitSuccessful = GetHitResultUnderCursor(ECollisionChannel::ECC_Visibility, true, Hit);
	}

	if (bHitSuccessful)
	{
		CachedDestination = Hit.Location;
	}
	
	APawn* ControlledPawn = GetPawn();
	if (ControlledPawn != nullptr)
	{
		FVector WorldDirection = (CachedDestination - ControlledPawn->GetActorLocation()).GetSafeNormal();
		ControlledPawn->AddMovementInput(WorldDirection, 1.0, false);
	}
}

void AP1PlayerController::OnSetDestinationReleased()
{
	if (FollowTime <= ShortPressThreshold)
	{
		UAIBlueprintHelperLibrary::SimpleMoveToLocation(this, CachedDestination);
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, FXCursor, CachedDestination, FRotator::ZeroRotator, FVector(1.f, 1.f, 1.f), true, true, ENCPoolMethod::None, true);
	}

	FollowTime = 0.f;
}

void AP1PlayerController::OnTouchTriggered()
{
	bIsTouch = true;
	OnSetDestinationTriggered();
}

void AP1PlayerController::OnTouchReleased()
{
	bIsTouch = false;
	OnSetDestinationReleased();
}

void AP1PlayerController::OnSkillTriggered()
{
	if (OwnerCharacter == nullptr) return;

	// TODO:
	OwnerCharacter->UseSkill(0);
}

void AP1PlayerController::OnSkillReleased()
{
	if (OwnerCharacter == nullptr) return;

	// OwnerCharacter->Cancel
}

void AP1PlayerController::SendMovePacketToServer()
{
	if (OwnerCharacter == nullptr) return;

	Protocol::C_MOVE Pkt;
	Protocol::ObjectInfo* info = Pkt.mutable_info();
	// TODO: State
	info->set_object_id(OwnerCharacter->ObjectInfo->object_id());
	info->set_x(OwnerCharacter->GetActorLocation().X);
	info->set_y(OwnerCharacter->GetActorLocation().Y);
	info->set_z(OwnerCharacter->GetActorLocation().Z);
	info->set_yaw(OwnerCharacter->GetActorRotation().Yaw);
	SEND_PACKET(Pkt);
}

void AP1PlayerController::MoveByServer(float DeltaTime)
{
	if (OwnerCharacter == nullptr) return;

	if (CurrentTimeToSendPacket < DefaultTimeToSendPacket)
	{
		CurrentTimeToSendPacket += DeltaTime;
	}
	else
	{
		SendMovePacketToServer();
		CurrentTimeToSendPacket = 0.f;
	}
}
