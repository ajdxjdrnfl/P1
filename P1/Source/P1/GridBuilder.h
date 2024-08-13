// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GridBuilder.generated.h"

struct FNode
{
	FVector Location;
	float height;
	uint64 tile; // 0 - ������������, 1 - ������ �� ����
	uint64 around[8]; // ������� ��, �ϵ�, ��, ����..
};

UCLASS()
class P1_API AGridBuilder : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AGridBuilder();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	virtual void SaveToFile(FString path);
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;

public:
	void BuildGrids(UWorld* world);

	bool IsBuildableAtLocation(UWorld* world, FVector& Location, FHitResult& hit, float& length);

	void CheckAroundGrid(UWorld* world);

	bool IsWalkable(FVector from, FVector to);

public:
	UPROPERTY(EditAnywhere)
	bool _debug = true;

	UPROPERTY(EditAnywhere)
	bool _execute = true;

	UPROPERTY(EditAnywhere)
	float _gridSize = 10.f;

	UPROPERTY(EditAnywhere)
	FVector _worldSize;

	UPROPERTY(EditAnywhere)
	FString name;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class USceneComponent* SceneComponent;

public:
	// �����̰� ������ ���� �� �ִ� �ְ� ����
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float _maxWalkableZ = 45.f;

	// 
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float _maxWalkableHeight = 200.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float _downDirectionVectorLength = 1000.f;

private:
	TArray<TArray<FNode>> _nodes;
};