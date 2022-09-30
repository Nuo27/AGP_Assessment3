// Fill out your copyright notice in the Description page of Project Settings.


#include "FloorManager.h"
#include "EngineUtils.h"
#include "NavigationNode.h"
#include "ProceduralRoom.h"

// Sets default values
AFloorManager::AFloorManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AFloorManager::BeginPlay()
{
	Super::BeginPlay();

	PopulateNodes();
	CreateAgents();
	
}

// Called every frame
void AFloorManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

TArray<ANavigationNode*> AFloorManager::GeneratePath(ANavigationNode* StartNode, ANavigationNode* EndNode)
{
	return TArray<ANavigationNode*>();
}

void AFloorManager::PopulateNodes()
{
	for (TActorIterator<ANavigationNode> It(GetWorld()); It; ++It)
	{
		AllNodes.Add(*It);
	}
}

void AFloorManager::CreateAgents()
{
	if (AllNodes.Num() > 0)
	{
		for (int32 i = 0; i < NumFloor; i++)
		{
			// Get a random node index
			int32 NodeIndex = FMath::RandRange(0, AllNodes.Num() - 1);
			AProceduralRoom* SpawnedFloor = GetWorld()->SpawnActor<AProceduralRoom>(AgentToSpawn, AllNodes[NodeIndex]->GetActorLocation(), AllNodes[NodeIndex]->GetActorRotation());

		}
	}
}

