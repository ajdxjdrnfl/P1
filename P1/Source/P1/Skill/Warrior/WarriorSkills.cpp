// Fill out your copyright notice in the Description page of Project Settings.


#include "WarriorSkills.h"
#include "Components/BoxComponent.h"
#include "P1/P1Character.h"
#include "P1/P1PlayerController.h"
#include "P1/Enemy/EnemyBase.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/SphereComponent.h"

AWarriorQSkill::AWarriorQSkill()
{
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	BoxCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollision"));
	BoxCollision->SetupAttachment(RootComponent);
	BoxCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	BoxCollision->OnComponentBeginOverlap.AddDynamic(this, &AWarriorQSkill::OnCollisionOverlapBegin);
}

void AWarriorQSkill::OnCollisionOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AEnemyBase* Enemy = Cast<AEnemyBase>(OtherActor);
	if (Enemy == nullptr) return;

	//SendCollisionPacketToServer(Enemy);
	// TODO:
}

AWarriorWSkill::AWarriorWSkill()
{
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	SkillMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SkillMesh"));
	SkillMeshComponent->SetupAttachment(RootComponent);

	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereCollision"));
	SphereComponent->SetupAttachment(RootComponent);
}

void AWarriorWSkill::BeginPlay()
{
	Super::BeginPlay();

	SphereComponent->OnComponentBeginOverlap.AddDynamic(this, &AWarriorWSkill::OnCollisionOverlapBegin);
}

void AWarriorWSkill::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!bIsActivated)
		return;

	if (MoveDistance <= 0)
		Destroy();

	AddActorWorldOffset(GetActorRotation().Vector() * MoveSpeed);
	MoveDistance -= DeltaTime;
}

void AWarriorWSkill::ActivateSkill()
{
	Super::ActivateSkill();

	MoveDirection.Normalize();
	bIsActivated = true;
}


void AWarriorWSkill::SendCollisionPacketToServer(AEnemyBase* Enemy)
{
	Protocol::C_ATTACK Pkt;
	Protocol::ObjectInfo* CasterInfo = Pkt.mutable_caster();
	Protocol::ObjectInfo* SkillActorInfoLocal = Pkt.mutable_skillactor();
	Protocol::ObjectInfo* VictimInfo = Pkt.mutable_victim();

	AP1Creature* InstigatorLocal = Cast<AP1Creature>(GetOwner());

	if (InstigatorLocal == nullptr)
		return;

	UP1GameInstance* P1GameInstance = Cast<UP1GameInstance>(GetWorld()->GetGameInstance());
	if (P1GameInstance == nullptr)
		return;

	if (!P1GameInstance->IsMyCharacter(InstigatorLocal->ObjectInfo->object_id()))
		return;

	if (InstigatorLocal->GetClassType() == FName("Warrior"))
	{
		CasterInfo->set_castertype(Protocol::CASTER_TYPE_WARRIOR);
	}
	else
	{
		// TODO:
	}

	if (Enemy->GetClassType() == FName("Boss"))
	{
		VictimInfo->set_castertype(Protocol::CASTER_TYPE_BOSS);
	}
	else
	{
		// TODO:
	}
	CasterInfo->set_object_id(InstigatorLocal->ObjectInfo->object_id());
	VictimInfo->set_object_id(Enemy->ObjectInfo->object_id());
	SkillActorInfoLocal->set_object_id(ObjectInfo->object_id());

	SEND_PACKET(Pkt);
}

void AWarriorWSkill::OnCollisionOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (AEnemyBase* Enemy = Cast<AEnemyBase>(OtherActor))
	{
		SendCollisionPacketToServer(Enemy);
	}
}

AWarriorESkill::AWarriorESkill()
{
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	SkillMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SkillMesh"));
	SkillMeshComponent->SetupAttachment(RootComponent);

	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereCollision"));
	SphereComponent->SetupAttachment(RootComponent);
}

void AWarriorESkill::BeginPlay()
{
	Super::BeginPlay();

	SphereComponent->OnComponentBeginOverlap.AddDynamic(this, &AWarriorESkill::OnCollisionOverlapBegin);
}

void AWarriorESkill::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!bIsActivated)
		return;

	if (MoveDistance <= 0)
		Destroy();

	AddActorWorldOffset(GetActorRotation().Vector() * MoveSpeed);
	MoveDistance -= DeltaTime;
}

void AWarriorESkill::ActivateSkill()
{
	Super::ActivateSkill();

	MoveDirection.Normalize();
	bIsActivated = true;
}

void AWarriorESkill::SendCollisionPacketToServer(AEnemyBase* Enemy)
{
	Protocol::C_ATTACK Pkt;
	Protocol::ObjectInfo* CasterInfo = Pkt.mutable_caster();
	Protocol::ObjectInfo* SkillActorInfoLocal = Pkt.mutable_skillactor();
	Protocol::ObjectInfo* VictimInfo = Pkt.mutable_victim();

	AP1Creature* InstigatorLocal = Cast<AP1Creature>(GetOwner());

	if (InstigatorLocal == nullptr)
		return;

	UP1GameInstance* P1GameInstance = Cast<UP1GameInstance>(GetWorld()->GetGameInstance());
	if (P1GameInstance == nullptr)
		return;

	if (!P1GameInstance->IsMyCharacter(InstigatorLocal->ObjectInfo->object_id()))
		return;

	if (InstigatorLocal->GetClassType() == FName("Warrior"))
	{
		CasterInfo->set_castertype(Protocol::CASTER_TYPE_WARRIOR);
	}
	else
	{
		// TODO:
	}

	if (Enemy->GetClassType() == FName("Boss"))
	{
		VictimInfo->set_castertype(Protocol::CASTER_TYPE_BOSS);
	}
	else
	{
		// TODO:
	}
	CasterInfo->set_object_id(InstigatorLocal->ObjectInfo->object_id());
	VictimInfo->set_object_id(Enemy->ObjectInfo->object_id());
	SkillActorInfoLocal->set_object_id(ObjectInfo->object_id());

	SEND_PACKET(Pkt);
}

void AWarriorESkill::OnCollisionOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (AEnemyBase* Enemy = Cast<AEnemyBase>(OtherActor))
	{
		SendCollisionPacketToServer(Enemy);
	}
}

AWarriorRSkill::AWarriorRSkill()
{
}

void AWarriorRSkill::ActivateSkill()
{
}
