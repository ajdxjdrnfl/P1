// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "P1/Interface/InteractInterface.h"
#include "P1/P1.h"
#include "Teleport.generated.h"

UCLASS()
class P1_API ATeleport : public AActor, public IInteractInterface
{
	GENERATED_BODY()
	
public:	
	ATeleport();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

private:
	bool bCanInteract;

	UPROPERTY(EditAnywhere)
	class USphereComponent* SphereComponent;

	UPROPERTY(EditAnywhere, Category = Editable)
	int32 LevelID;

	UPROPERTY(EditAnywhere)
	class UWidgetComponent* WidgetComponent;

public:
	virtual void Interact() override;

	UFUNCTION()
	void OnCollisionOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnCollisionOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
};
