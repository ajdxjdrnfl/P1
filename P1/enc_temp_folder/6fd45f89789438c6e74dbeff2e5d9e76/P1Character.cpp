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

	UP1GameInstance* instance = Cast<UP1GameInstance>(GetGameInstance());

	if (!instance->IsMyCharacter(ObjectInfo->object_id()) && ObjectInfo->state() == Protocol::MOVE_STATE_JUMP)
	{
		Dodge();
		ObjectInfo->set_state(Protocol::MOVE_STATE_IDLE);
	}

	CameraBoom->TargetArmLength = UKismetMathLibrary::FInterpTo(CameraBoom->TargetArmLength, TargetCameraBoomLength, DeltaSeconds, 20);
}

void AP1Character::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (SkillComponent)
	{
		SkillComponent->OwnerAnimInstance = GetMesh()->GetAnimInstance();
		SkillComponent->OwnerCharacter = this;
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

void AP1Character::MoveByServer(float DeltaTime)
{
	UE_LOG(LogTemp, Log, TEXT("%s"), *(ObjectInfo->state() == Protocol::MOVE_STATE_RUN ? FString("Run") : FString("Idle")));
	if (ObjectInfo->state() == Protocol::MOVE_STATE_RUN)
	{
		FVector TargetLocation = FVector(TargetInfo->x(), TargetInfo->y(), GetActorLocation().Z);
		FVector TargetForwardVector = (TargetLocation - GetActorLocation()).GetSafeNormal();

		FRotator NextRotation = UKismetMathLibrary::RInterpTo(GetActorRotation(), TargetForwardVector.Rotation(), DeltaTime, 10.f);

		//bool NearYaw = UKismetMathLibrary::NearlyEqual_FloatFloat(TargetForwardVector.ro.Yaw, GetActorRotation().Yaw);
		//if (NearYaw) return;

		

		AddMovementInput(TargetForwardVector);
		SetObjectInfo();
		SetActorRotation(NextRotation);
	}
}

void AP1Character::SetHealthByDamage(float HealthToSet)
{
	if (StatComponent)
	{
		StatComponent->SetHealth(HealthToSet);
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

