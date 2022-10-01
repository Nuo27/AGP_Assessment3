// Fill out your copyright notice in the Description page of Project Settings.


#include "ProceduralRoom.h"
#include "DrawDebugHelpers.h"
// Sets default values
AProceduralRoom::AProceduralRoom()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Floor = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("FloorComponent"));
	SetRootComponent(Floor);

	GridSizeX = 5;
	GridSizeY = 5;
	SqareWidth = 600.f;

	TopLeft = FVector(0.f);
	BottomRighe = FVector(3000.f, 3000.f, 0.f);
	RoomLength = 3000.f;
	RoomWidth = 3000.f;
	Radius = 50.f;
}

// Called when the game starts or when spawned
void AProceduralRoom::BeginPlay()
{
	Super::BeginPlay();
	
	
	PlacePointsOnGrid();
}

// Called every frame
void AProceduralRoom::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AProceduralRoom::SpawnItem(UClass* ItemToSpawn)
{
	float XCoordinate = FMath::FRandRange(-500.f, 500.f);
	float YCoordinate = FMath::FRandRange(-500.f, 500.f);

	float Yaw = FMath::FRandRange(0.f, 360.f);

	FVector Location(XCoordinate, YCoordinate, 0.f);
	FRotator Rotation(0.f, Yaw, 0.f);

	GetWorld()->SpawnActor<AActor>(ItemToSpawn, Location, Rotation);
}

void AProceduralRoom::CreateGrid()
{
	for (int32 i = 0; i < GridSizeX + 1; i++)
	{
		FVector Start = TopLeft + FVector(i * SqareWidth, 0.f, 1.f);
		FVector End = Start + FVector(0.f, RoomLength, 1.f);
		DrawDebugLine(GetWorld(), Start, End, FColor::Blue, true);

	}

	for (int32 i = 0; i < GridSizeY + 1; i++)
	{
		FVector Start = TopLeft + FVector(0.f, i * SqareWidth, 1.f);
		FVector End = Start + FVector(RoomLength, 0.f, 1.f);
		DrawDebugLine(GetWorld(), Start, End, FColor::Blue, true);
	}
}

FVector AProceduralRoom::GetRandomPointInSquare(const FVector & UpperLeft, const FVector & LowerRight)
{
	float RandomX = FMath::FRandRange(UpperLeft.X, LowerRight.X);
	float RandomY = FMath::FRandRange(UpperLeft.Y, LowerRight.Y);

	return FVector(RandomX, RandomY, 0.f);
}

void AProceduralRoom::PlacePointsOnGrid()
{
	for (int32 i = 0; i < GridSizeX; i++)
	{
		for (int32 j = 0; j < GridSizeY; j++)
		{
			FVector UpperLeft(i * SqareWidth + Radius, j * SqareWidth + Radius, 1.f);
			FVector LowerRight(i * SqareWidth + SqareWidth - Radius, j * SqareWidth + SqareWidth - Radius, 1.f);
			FVector RandomPointInSquare = GetRandomPointInSquare(UpperLeft, LowerRight);

			float RandomYaw = FMath::FRandRange(0.f, 360.f);
			GetWorld()->SpawnActor<AActor>(GunClass, RandomPointInSquare, FRotator(0.f, RandomYaw, 0.f));
		}
	}
}
