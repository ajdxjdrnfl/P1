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
}

void AP1Character::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (SkillComponent)
		SkillComponent->OwnerAnimInstance = GetMesh()->GetAnimInstance();
}

void AP1Character::Init()
{
	if (WidgetComponent == nullptr || StatComponent == nullptr)
		return;

	WidgetComponent->SetCharacterStat(StatComponent);
}

void AP1Character::UseSkill(uint16 SkillIndex)
{
	if (SkillComponent == nullptr || WidgetComponent == nullptr) return;

	SkillComponent->UseSkill(SkillIndex);
	WidgetComponent->UseSkill(SkillIndex);
}
