// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PhaseDoor.h"
#include "P1/Interface/InteractInterface.h"
#include "PhaseBossDoor.generated.h"

/**
 * 
 */
UCLASS()
class P1_API APhaseBossDoor : public APhaseDoor, public IInteractInterface
{
	GENERATED_BODY()

public:
	APhaseBossDoor();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

private:
	UPROPERTY(EditAnywhere)
	bool bCanInteract;
	bool bIsAlreadyOpen;

	UPROPERTY(EditAnywhere)
	class UCapsuleComponent* CapsuleComponent;

	UPROPERTY(EditAnywhere)
	class UWidgetComponent* WidgetComponent;

public:
	virtual void ClearPhase() override;

	virtual void Interact() override;
	virtual void OnOpeningDoorEnd() override;

	UFUNCTION()
	virtual void OnCollisionOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	virtual void OnCollisionOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

};
