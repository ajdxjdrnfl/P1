// Copyright Epic Games, Inc. All Rights Reserved.

#include "P1Character.h"
#include "UObject/ConstructorHelpers.h"
#include "Camera/CameraComponent.h"
#include "Components/DecalComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/SpringArmComponent.h"
#include "Materials/Material.h"
#include "Engine/World.h"
#include "Component/SkillComponent/CharacterSkillComponent.h"
#include "Component/StatComponent/CharacterStatComponent.h"
#include "Component/WidgetComponent/CharacterWidgetComponent.h"
#include "Widget/Stat/CharacterStatWidget.h"
#include "Widget/CharacterOverlayWidget.h"
#include "Kismet/KismetMathLibrary.h"
#include "P1/Widget/SkillButtonWidget.h"
#include "P1/SubSystem/GameInstance/SkillManagerSubSystem.h"
#include "P1GameInstance.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "AIController.h"
#include "P1/Skill/SkillActorBase.h"

AP1Character::AP1Character()
{
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = true; // Rotate character to moving direction
	GetCharacterMovement()->RotationRate = FRotator(0.f, 640.f, 0.f);
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->bSnapToPlaneAtStart = true;
	GetCharacterMovement()->bRunPhysicsWithNoController = true;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->SetUsingAbsoluteRotation(true); // Don't want arm to rotate when character does
	CameraBoom->TargetArmLength = 800.f;
	CameraBoom->SetRelativeRotation(FRotator(-60.f, 0.f, 0.f));
	CameraBoom->bDoCollisionTest = false; // Don't want to pull camera in when it collides with level

	TopDownCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("TopDownCamera"));
	TopDownCameraComponent->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	TopDownCameraComponent->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;


	SkillComponent = CreateDefaultSubobject<UCharacterSkillComponent>(TEXT("CharacterSkillComponent"));

	StatComponent = CreateDefaultSubobject<UCharacterStatComponent>(TEXT("CharacterStatComponent"));

	WidgetComponent = CreateDefaultSubobject<UCharacterWidgetComponent>(TEXT("CharacterSWidgetComponent"));
}

void AP1Character::BeginPlay()
{
	Super::BeginPlay();

	CameraBoom->TargetArmLength = 1500;
	TargetCameraBoomLength = 1500;
}

void AP1Character::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	CameraBoom->TargetArmLength = UKismetMathLibrary::FInterpTo(CameraBoom->TargetArmLength, TargetCameraBoomLength, DeltaSeconds, 20);

	switch (ObjectInfo->state())
	{
	case Protocol::MOVE_STATE_IDLE:
		TickIdle(DeltaSeconds);
		break;
	case Protocol::MOVE_STATE_RUN:
		TickMove(DeltaSeconds);
		break;
	case Protocol::MOVE_STATE_JUMP:
		TickJump();
		break;
	case Protocol::MOVE_STATE_STOP:
		TickStop();
		break;
	case Protocol::MOVE_STATE_SKILL:
		TickSkill(DeltaSeconds);
		break;
	case Protocol::MOVE_STATE_STUN:
		break;
	default:
		break;
	}

	if (bShowMoveState)
	{
		if (UP1GameInstance* GameInstance = GetWorld()->GetGameInstance<UP1GameInstance>())
		{
			if (GameInstance->IsMyCharacter(ObjectInfo->object_id()))
			{
				FString Str = FString();

				if (ObjectInfo->state() == Protocol::MOVE_STATE_IDLE)
				{
					Str = FString("Idle");
				}
				else if (ObjectInfo->state() == Protocol::MOVE_STATE_STUN)
				{
					Str = FString("KNOCKBACK");
					GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("%s"), *Str));
				}
				else if (ObjectInfo->state() == Protocol::MOVE_STATE_RUN)
				{
					Str = FString("Run");
				}
				else if (ObjectInfo->state() == Protocol::MOVE_STATE_JUMP)
				{
					Str = FString("Jump");
				}

				//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("%s"), *Str));
			}
		}
	}
}

void AP1Character::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (SkillComponent)
	{
		SkillComponent->OwnerAnimInstance = GetMesh()->GetAnimInstance();
		SkillComponent->OwnerCharacter = this;
		SkillComponent->OwnerCreature = this;
	}


	if (WidgetComponent)
	{
		WidgetComponent->OwnerCharacter = this;
	}

	if (StatComponent)
	{
		StatComponent->OwnerCharacter = this;
	}
}

void AP1Character::InitOnSpawn(Protocol::ObjectInfo ObjInfo)
{
	if (WidgetComponent == nullptr || StatComponent == nullptr || SkillComponent == nullptr)
		return;

	WidgetComponent->InitOnSpawn(StatComponent);
	StatComponent->InitOnSpawn(ObjInfo);
	SkillComponent->InitOnSpawn();
}

void AP1Character::UseSkill(uint16 SkillIndex)
{
	if (SkillComponent == nullptr || WidgetComponent == nullptr) return;

	if (USkillManagerSubSystem* SkillSubSystem = GetGameInstance()->GetSubsystem<USkillManagerSubSystem>())
	{
		if (SkillSubSystem->CanUseSkill(SkillIndex))
		{
			SkillComponent->UseSkill(SkillIndex);
			WidgetComponent->UseSkill(SkillIndex);
		}
	}
}

void AP1Character::OpenSkillGaugeWidget(float CastingTime)
{
	if (WidgetComponent == nullptr) return;

	WidgetComponent->OpenSkillGaugeWidget(CastingTime);
}

void AP1Character::CloseSkillGaugeWidget()
{
	if (WidgetComponent == nullptr) return;

	WidgetComponent->CloseSkillGaugeWidget();
}

float AP1Character::GetGaugeRate()
{
	if (WidgetComponent == nullptr) return 0;

	return WidgetComponent->GetGaugeRate();
}

void AP1Character::PlayAnimMontageByServer(Protocol::S_MONTAGE& pkt)
{
	if (SkillComponent == nullptr) return;
	SkillComponent->PlayAnimMontageByServer(pkt);
}

FSkillInfo AP1Character::GetSkillInfoByIndex(int32 SkillIndex)
{
	return SkillComponent->GetSkillInfoByIndex(SkillIndex);
}

void AP1Character::TickMove(float DeltaTime)
{
	FVector TargetLocation = FVector(TargetInfo->x(), TargetInfo->y(), GetActorLocation().Z);
	FRotator TargetRotator = (TargetLocation - GetActorLocation()).Rotation();

	bool bNearX = UKismetMathLibrary::NearlyEqual_FloatFloat(TargetLocation.X, GetActorLocation().X, 5);
	bool bNearY = UKismetMathLibrary::NearlyEqual_FloatFloat(TargetLocation.Y, GetActorLocation().Y, 5);

	if (bNearX && bNearY)
	{
		ObjectInfo->set_state(Protocol::MOVE_STATE_IDLE);
		SetActorRotation(FRotator(GetActorRotation().Pitch, TargetInfo->yaw(), GetActorRotation().Roll));
		return;
	}

	AddMovementInput(TargetRotator.Vector().GetSafeNormal());
}

void AP1Character::TickIdle(float DeltaTime)
{
	FVector TargetLocation = FVector(TargetInfo->x(), TargetInfo->y(), GetActorLocation().Z);
	bool bNearX = UKismetMathLibrary::NearlyEqual_FloatFloat(TargetLocation.X, GetActorLocation().X, 5);
	bool bNearY = UKismetMathLibrary::NearlyEqual_FloatFloat(TargetLocation.Y, GetActorLocation().Y, 5);

	if (!bNearX || !bNearY || !GetMesh()->GetAnimInstance()->Montage_IsPlaying(nullptr))
	{
		SetActorLocation(TargetLocation);
	}

	FRotator TargetRotator = FRotator(GetActorRotation().Pitch, TargetInfo->yaw(), GetActorRotation().Roll);
	FRotator NextRotator = FMath::RInterpTo(GetActorRotation(), TargetRotator, DeltaTime, 10);
	SetActorRotation(NextRotator);
}

void AP1Character::TickStop()
{
	UP1GameInstance* GameInstance = GetWorld()->GetGameInstance<UP1GameInstance>();
	if (GameInstance == nullptr) return;

	if (GameInstance->IsMyCharacter(ObjectInfo->object_id()))
	{
		Protocol::C_MOVE Pkt;
		Protocol::ObjectInfo* _TargetInfo = Pkt.mutable_targetinfo();
		Protocol::ObjectInfo* ObjInfo = Pkt.mutable_info();
		ObjInfo->CopyFrom(*ObjectInfo);
		_TargetInfo->CopyFrom(*ObjectInfo);
		_TargetInfo->set_state(Protocol::MOVE_STATE_IDLE);

		SEND_PACKET(Pkt);
	}

	ObjectInfo->set_state(Protocol::MOVE_STATE_IDLE);
}

void AP1Character::TickJump()
{
	if (SkillComponent)
	{
		if (bDoOnceDodge) return;
		bDoOnceDodge = true;
		SkillComponent->DodgeByServer();
	}
}

void AP1Character::TickSkill(float DeltaTime)
{
	FRotator TargetRotation = FRotator(GetActorRotation().Pitch, TargetInfo->yaw(), GetActorRotation().Roll);
	FRotator NextRotation = UKismetMathLibrary::RInterpTo(GetActorRotation(), TargetRotation, DeltaTime, 10);
	SetActorRotation(NextRotation);
}

void AP1Character::SetHealthByDamage(float HealthToSet, ASkillActorBase* HitSkill)
{
	if (WidgetComponent)
	{
		WidgetComponent->SpawnDamageIndicator(ObjectInfo->hp() - HealthToSet);
	}

	if (StatComponent)
	{
		StatComponent->SetHealth(HealthToSet);

		SetHitMaterial();
	}

	if (SkillComponent)
	{
		SkillComponent->SetCC(HitSkill);
	}
}

void AP1Character::SetSpawnedSkill(int32 SkillID, ASkillActorBase* SkillActor)
{
	if (SkillComponent)
	{
		SkillComponent->SetSpawnedSkill(SkillID, SkillActor);
	}
}

void AP1Character::Dodge()
{
	if (SkillComponent)
	{
		SkillComponent->Dodge();
	}
}

void AP1Character::AddCameraBoomLength(float Value)
{
	TargetCameraBoomLength = FMath::Clamp(CameraBoom->TargetArmLength - Value * 100, 500, 2500);
}

void AP1Character::StopMoving()
{
	Protocol::C_MOVE Pkt;
	Protocol::ObjectInfo* _TargetInfo = Pkt.mutable_targetinfo();
	Protocol::ObjectInfo* ObjInfo = Pkt.mutable_info();
	ObjInfo->CopyFrom(*ObjectInfo);
	_TargetInfo->CopyFrom(*TargetInfo);
	_TargetInfo->set_state(Protocol::MOVE_STATE_STOP);

	SEND_PACKET(Pkt);
}

void AP1Character::Interact()
{
	if (InteractInterfaceActor == nullptr) return;
	InteractInterfaceActor->Interact();
}

void AP1Character::SetHealthByDamageByDot(float HealthToSet)
{
	if (WidgetComponent)
	{
		WidgetComponent->SpawnDamageIndicator(ObjectInfo->hp() - HealthToSet);
	}

	if (StatComponent)
	{
		StatComponent->SetHealth(HealthToSet);

		SetHitMaterial();
	}
}

