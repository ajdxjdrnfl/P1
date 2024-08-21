// Fill out your copyright notice in the Description page of Project Settings.


#include "GridBuilder.h"
#include "Kismet/GameplayStatics.h"
#include "Enemy/EnemyBase.h"
#include "Enemy/EnemyBoss.h"
#include "Enemy/EnemyMob.h"

// Sets default values
AGridBuilder::AGridBuilder()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
	SetRootComponent(SceneComponent);

}

// Called when the game starts or when spawned
void AGridBuilder::BeginPlay()
{
	Super::BeginPlay();

	if (_execute)
	{
		BuildGrids(GetWorld());
		BuildWorldEnemies(GetWorld());
		FString fileName;

		if (name.Len() > 0)
		{
			fileName = "/Data/" + name + ".txt";
		}
		else
		{
			fileName = "temp.txt";
		}

		SaveToFile(FPaths::ProjectDir() + fileName);
	}

}

void AGridBuilder::SaveToFile(FString path)
{
	if (true)
	{
		FString content;

		FVector pos = GetActorLocation();
		// worldsize, gridsize, node 크기 - 모든 크기는 x,y 순으로
		content += FString::SanitizeFloat(_worldSize.X) + " " + FString::SanitizeFloat(_worldSize.Y) + " " + FString::SanitizeFloat(_gridSize) + " " + FString::SanitizeFloat(_gridSize) + " "
			+ FString::SanitizeFloat(pos.X) + " " + FString::SanitizeFloat(pos.Y) + " "
			+ FString::FromInt(_nodes.Num()) +  " " + FString::FromInt(_nodes[0].Num()) + "\n";

		// x좌표, y좌표 순으로 타일종류 , 만약 움직일 수 있는 타일일 경우 주변 타일 종류도 함께 저장
		for (int32 i = 0; i < _nodes.Num(); i++)
		{
			for (int32 j = 0; j < _nodes[i].Num(); j++)
			{
				FNode node = _nodes[i][j];
				content += FString::SanitizeFloat(node.Location.X) + " " + FString::SanitizeFloat(node.Location.Y) + " " + FString::SanitizeFloat(node.Location.Z) + "\n";
				content += FString::FromInt(node.tile) + " ";

				if (node.tile == 1)
				{
					for (int32 k = 0; k < 8; k++)
					{
						content += FString::FromInt(node.around[k]) + " ";
					}
				}

				content += "\n";
				
			}
		}

		FString enemyContent;

		enemyContent += FString::FromInt(_enemies.Num()) + "\n";
		for (int32 i = 0; i < _enemies.Num(); i++)
		{
			FEnemyInfo info = _enemies[i];
			FVector Location = info.Location;
			float yaw = info.yaw;

			enemyContent += FString::SanitizeFloat(Location.X) + " " + FString::SanitizeFloat(Location.Y) + " " + FString::SanitizeFloat(Location.Z) + " "
				+ FString::SanitizeFloat(yaw) + " "
				+ FString::FromInt(info.casterType) + " " + FString::FromInt(info.phaseNumber) + "\n";
		}

		bool success = FFileHelper::SaveStringToFile(content + enemyContent, *path);
		if (success)
		{
			GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, FString::Printf(TEXT("File success!")));
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, FString::Printf(TEXT("File Failed!")));
		}
			
	}

}

// Called every frame
void AGridBuilder::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (_debug)
	{
		DrawDebugBox(GetWorld(), GetActorLocation(), _worldSize / 2, FColor::Green);
	}
}

void AGridBuilder::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	DrawDebugBox(GetWorld(), GetActorLocation(), _worldSize / 2, FColor::Green);
}

void AGridBuilder::BuildGrids(UWorld* world)
{

	FVector builderLocation = GetActorLocation();

	for (float y = builderLocation.Y - _worldSize.Y / 2; y < builderLocation.Y + _worldSize.Y / 2; y += _gridSize)
	{
		TArray<FNode> nodes;

		for (float x = builderLocation.X - _worldSize.X / 2; x < builderLocation.X + _worldSize.X / 2; x += _gridSize)
		{
			FNode node;

			FVector Location(x + _gridSize / 2, y + _gridSize / 2, builderLocation.Z - _worldSize.Z / 2);

			FHitResult hit;
			float length;


			if (IsBuildableAtLocation(world, Location, hit, length))
			{
				node.tile = 1;
			}
			else
			{
				node.tile = 0;
			}

			node.Location = Location;
			node.height = length;

			nodes.Add(node);
		}

		_nodes.Add(nodes);
	}

	CheckAroundGrid(GetWorld());
}

void AGridBuilder::BuildWorldEnemies(UWorld* world)
{
	TArray<AActor*> OutActors;
	UGameplayStatics::GetAllActorsOfClass(world, AEnemyBase::StaticClass(), OutActors);

	for (int32 i = 0; i < OutActors.Num(); i++)
	{
		AEnemyBase* enemy = Cast<AEnemyBase>(OutActors[i]);

		FEnemyInfo info;

		info.Location = enemy->GetActorLocation();
		info.yaw = enemy->GetActorRotation().Yaw;
		info.phaseNumber = enemy->GetPhaseNum();
	
		if (enemy->IsA<AEnemyBoss>())
		{
			info.casterType = (int32)Protocol::CASTER_TYPE_BOSS;
		}
		else if (enemy->IsA<AEnemyMob>())
		{
			info.casterType = (int32)Protocol::CASTER_TYPE_MOB;
		}

		_enemies.Add(info);
	}
}

bool AGridBuilder::IsBuildableAtLocation(UWorld* world, FVector& Location, FHitResult& hit, float& length)
{
	FVector Start = Location + FVector(0.f, 0.f, _worldSize.Z/2 + _maxWalkableHeight);
	FVector End = Start + FVector(0.f, 0.f, -_downDirectionVectorLength);

	FCollisionQueryParams Params;

	// 아무도 없을 경우 trace 실패
	bool traced = world->LineTraceSingleByChannel(hit, Start, End, ECC_Visibility, Params);

	Location = hit.ImpactPoint;

	FVector traceStart = hit.TraceStart;
	FVector impactPoint = hit.ImpactPoint;

	if (traced)
	{
		// trace 될 경우 지나갈 수 없음
		bool isNonwalkable = world->LineTraceSingleByChannel(hit, Start, End, ECC_GameTraceChannel1, Params);
		
		// 충돌이 일어나면 지나갈 수 없는 곳
		if (isNonwalkable)
		{
			// 지나갈 수 없음 - 기둥 or 낙하방지용 콜리전
			length = -1.f;
			//DrawDebugLine(world, Start, End, FColor::Red, true);
		}
		else
		{
			// 지나갈 수 있음
			length = (traceStart - impactPoint).Length();
			if(_debug)
				DrawDebugPoint(world, FVector(Location.X, Location.Y, 0.f), 5.f, FColor::Green, true);
		}

		

		return !isNonwalkable;
	}
	else
	{
		length = -1.f;
		//DrawDebugLine(world, Start, End, FColor::Red, true);
	}

	return false;
}

void AGridBuilder::CheckAroundGrid(UWorld* world)
{
	// 북, 북동, 동
	int32 d[8][2] = { {1,0}, {1, 1}, {0, 1}, {-1, 1}, {-1,0}, {-1,-1}, {0, -1}, {1, -1} };

	for (int32 i = 0; i < _nodes.Num(); i++)
	{

		for (int32 j = 0; j < _nodes[i].Num(); j++)
		{
			FNode& node = _nodes[i][j];
			
			for (int32 k = 0; k < 8; k++)
			{
				int32 dy = i + d[k][0];
				int32 dx = j + d[k][1];

				if (node.tile == 0)
				{
					node.around[k] = 0;
					continue;
				}

				if (dx < 0 || dy < 0 || dx >= _nodes[i].Num() || dy >= _nodes.Num())
				{
					node.around[k] = 0;
					continue;
				}

				FNode& aroundNode = _nodes[dy][dx];

				bool walkable = false;
				FColor color = FColor::Blue;

				{
					float heightFrom = node.height;
					float heightTo = aroundNode.height;

					if (heightFrom <= 0.f || heightTo <= 0.f)
					{
						walkable = false;
						color = FColor::Red;
					}

					else if (heightFrom <= heightTo)
					{
						walkable = true;
					}

					else if (heightFrom <= heightTo + _maxWalkableZ)
					{
						walkable = true;
					}

					else
					{
						walkable = false;
						color = FColor::Yellow;
					}
				}

				if (walkable)
				{
					node.around[k] = 1;
					if(_debug)
						DrawDebugLine(world, FVector(node.Location.X, node.Location.Y, 0.f), node.Location + FVector((float)d[k][1] * (_gridSize / 2 - 1.f), (float)d[k][0] * (_gridSize / 2 - 1.f), -node.Location.Z), color, true);

				}
				else
				{
					node.around[k] = 0;
					if(_debug)
						DrawDebugLine(world, FVector(node.Location.X, node.Location.Y, 0.f), node.Location + FVector((float)d[k][1] * (_gridSize / 2 -1.f), (float)d[k][0] * (_gridSize / 2-1.f), -node.Location.Z), color, true);
				}
			}
		}
		
	}
}

bool AGridBuilder::IsWalkable(FVector from, FVector to)
{
	float heightFrom = from.Z;
	float heightTo = to.Z;

	if (heightFrom <= 0.f || heightTo <= 0.f)
		return false;

	if (heightFrom <= heightTo)
		return true;

	if (heightFrom <= heightTo + _maxWalkableZ)
		return true;

	return false;
}
