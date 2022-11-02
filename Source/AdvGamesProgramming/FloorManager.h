// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FloorManager.generated.h"

UCLASS()
class ADVGAMESPROGRAMMING_API AFloorManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AFloorManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	

	UPROPERTY(EditAnywhere, Category = "Floor Properties")
		int32 NumFloor;
	UPROPERTY(VisibleAnywhere, Category = "Navigation Nodes")
		TArray<class ANavigationNode*> AllNodes;
	UPROPERTY(VisibleAnywhere, Category = "Agents")
		TArray<class AProceduralRoom*> AllAgents;
	UPROPERTY(EditAnywhere, Category = "Agents")
		TSubclassOf<AProceduralRoom> AgentToSpawn;
	UPROPERTY(EditAnywhere, Category = "Agents")
		TSubclassOf<AProceduralRoom> AgentToSpawn2;
	UPROPERTY(EditAnywhere, Category = "Agents")
		TSubclassOf<AProceduralRoom> AgentToSpawn3;

	TArray<ANavigationNode*> GeneratePath(ANavigationNode* StartNode, ANavigationNode* EndNode);
	void PopulateNodes();
	void CreateAgents();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
