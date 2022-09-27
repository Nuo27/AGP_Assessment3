// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyCharacter.h"
#include "AIManager.h"
#include "NavigationNode.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "HealthComponent.h"
#include <Runtime/Engine/Classes/Kismet/KismetMathLibrary.h>

// Sets default values
AEnemyCharacter::AEnemyCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CurrentAgentState = AgentState::PATROL;
	PathfindingNodeAccuracy = 100.0f;
}

// Called when the game starts or when spawned
void AEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	Cast<UCharacterMovementComponent>(GetMovementComponent())->bOrientRotationToMovement = true;

	HealthComponent = FindComponentByClass<UHealthComponent>();

	PerceptionComponent = FindComponentByClass<UAIPerceptionComponent>();
	if (PerceptionComponent)
	{
		PerceptionComponent->OnTargetPerceptionUpdated.AddDynamic(this, &AEnemyCharacter::SensePlayer);
	}

	DetectedActor = nullptr;
	bCanSeeActor = false;

}

// Called every frame
void AEnemyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (CurrentAgentState == AgentState::PATROL)
	{
		AgentPatrol();
		MoveAlongPath();
		if (bCanSeeActor) {
			CurrentAgentState = AgentState::CHASE;
		}
	}
	else if (CurrentAgentState == AgentState::CHASE)
	{
		AgentChase();
		if (!bCanSeeActor)
		{
			CurrentAgentState = AgentState::PATROL;
		}
	}

	
	if (DetectedActor != nullptr) {

		//UE_LOG(LogTemp, Warning, TEXT("Detected Actor Location: %s"), *DetectedActor->GetActorLocation().ToString());

		//Rebuild Blueprint Lookat Function in C++
		//look at this actor
		FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), DetectedActor->GetActorLocation());
		//set the rotator to the actor
		SetActorRotation(LookAtRotation);
	}
}

// Called to bind functionality to input
void AEnemyCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AEnemyCharacter::AgentPatrol()
{
	if (Path.Num() == 0)
	{
		if (Manager)
		{
			Path = Manager->GeneratePath(CurrentNode, Manager->AllNodes[FMath::RandRange(0, Manager->AllNodes.Num() - 1)]);
		}
	}
}

void AEnemyCharacter::AgentChase()
{
	//move to the detected actor
	if (DetectedActor != nullptr) {
		AddMovementInput(DetectedActor->GetActorLocation() - GetActorLocation());
		//slow down when close to the actor
		if ((GetActorLocation() - DetectedActor->GetActorLocation()).Size() < 100.0f) {
			GetCharacterMovement()->MaxWalkSpeed = 100.0f;
		}
		else {
			GetCharacterMovement()->MaxWalkSpeed = 300.0f;
			UE_LOG(LogTemp, Warning, TEXT("Distance between enemy and player: %f"), (GetActorLocation() - DetectedActor->GetActorLocation()).Size());
		}
	}
}

void AEnemyCharacter::SensePlayer(AActor* ActorSensed, FAIStimulus Stimulus)
{
	if (Stimulus.WasSuccessfullySensed())
	{
		UE_LOG(LogTemp, Warning, TEXT("Player Detected"))
		DetectedActor = ActorSensed;
		bCanSeeActor = true;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Player Lost"))
		bCanSeeActor = false;
		DetectedActor = nullptr;
	}
}

void AEnemyCharacter::MoveAlongPath()
{
	if ((GetActorLocation() - CurrentNode->GetActorLocation()).IsNearlyZero(PathfindingNodeAccuracy)
		&& Path.Num() > 0)
	{
		CurrentNode = Path.Pop();
	}
	else if (!(GetActorLocation() - CurrentNode->GetActorLocation()).IsNearlyZero(PathfindingNodeAccuracy))
	{
		AddMovementInput(CurrentNode->GetActorLocation() - GetActorLocation());
	}
}

