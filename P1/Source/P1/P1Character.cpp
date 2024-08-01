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

	Init();


	//if (StatComponent == nullptr) return;
	//StatComponent->OnCharacterHealthChangedDelegate.AddDynamic(WidgetComponent->GetCharacterOverlayWidget()->GetStatWidget(), &UCharacterStatWidget::OnCharacterHealthChanged);
}

void AP1Character::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

	MoveByServer(DeltaSeconds);
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

void AP1Character::Init()
{
	if (WidgetComponent == nullptr || StatComponent == nullptr)
		return;

	WidgetComponent->SetCharacterStat(StatComponent);
	StatComponent->InitStat();
}

void AP1Character::InitOnSpawn(float HealthToSet, float StaminaToSet)
{
	if (WidgetComponent == nullptr || StatComponent == nullptr)
		return;

	WidgetComponent->SetCharacterStat(StatComponent);
	StatComponent->InitStat(HealthToSet, StaminaToSet);
}

void AP1Character::OnSpawn(float HealthToSet, float StaminaToSet)
{
	InitOnSpawn(HealthToSet, StaminaToSet);
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

void AP1Character::MoveByServer(float DeltaTime)
{
	if (CreatureState == ECreatureState::Move)
	{
		FVector TargetLocation = FVector(ObjectInfo->x(), ObjectInfo->y(), GetActorLocation().Z);
		FRotator TargetRotation = FRotator(GetActorRotation().Pitch, ObjectInfo->yaw(), GetActorRotation().Roll);
		FRotator NextRotation = UKismetMathLibrary::RInterpTo(GetActorRotation(), TargetRotation, DeltaTime, 10.f);

		AddMovementInput((TargetLocation - GetActorLocation()).GetSafeNormal());
		SetActorRotation(NextRotation);
	}
}

void AP1Character::SetMoveValueByServer(Protocol::S_MOVE Pkt)
{
	FTransform TargetTransform;
	FVector TargetLocation = FVector(Pkt.info().x(), Pkt.info().y(), Pkt.info().z());
	FRotator TargetRotation = FRotator(GetActorRotation().Pitch, Pkt.info().yaw(), GetActorRotation().Roll);
	TargetTransform.SetLocation(TargetLocation);
	TargetTransform.SetRotation(TargetRotation.Quaternion());
	
	bool NearX = UKismetMathLibrary::NearlyEqual_FloatFloat(TargetLocation.X, GetActorLocation().X, 15);
	bool NearY = UKismetMathLibrary::NearlyEqual_FloatFloat(TargetLocation.Y, GetActorLocation().Y, 15);
	bool NearZ = UKismetMathLibrary::NearlyEqual_FloatFloat(TargetLocation.Z, GetActorLocation().Z, 15);
	bool bIsNear = NearX && NearY && NearZ;

	if (bIsNear)
	{
		CreatureState = ECreatureState::Idle;
	}
	else
	{
		CreatureState = ECreatureState::Move;
		ObjectInfo->set_x(TargetLocation.X);
		ObjectInfo->set_y(TargetLocation.Y);
		ObjectInfo->set_z(TargetLocation.Z);
		ObjectInfo->set_yaw(TargetRotation.Yaw);
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

void AP1Character::PlayAnimMontageByServer(bool bIsStop, int32 SkillIndexLocal, int32 SectionIndex)
{
	if (SkillComponent == nullptr) return;
	SkillComponent->PlayAnimMontageByServer(bIsStop, SkillIndexLocal, SectionIndex);
}
