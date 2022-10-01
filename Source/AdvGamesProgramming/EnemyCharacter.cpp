// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyCharacter.h"
#include "AIManager.h"
#include "NavigationNode.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "HealthComponent.h"
#include <Runtime/Engine/Classes/Kismet/KismetMathLibrary.h>
#include "Kismet/GameplayStatics.h"

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
	bCanSeePlayer = false;
	isItemExist = true;
	isItemChecked = false;
	isEnemyDead = false;
	GetCharacterMovement()->MaxWalkSpeed = 400.0f;
}

// Called every frame
void AEnemyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	//get all enemycharacterblueprint
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AEnemyCharacter::StaticClass(), FoundActors);

	if (HealthComponent->CurrentHealth <= 0) {
		isEnemyDead = true;
	}
	if (!isEnemyDead) {
		if (CurrentAgentState == AgentState::PATROL)
		{
			AgentPatrol();
			MoveAlongPath();
			if (bCanSeePlayer) {
				CurrentAgentState = AgentState::CHASE;
			}
		}
		else if (CurrentAgentState == AgentState::CHASE)
		{
			AgentChase();
			if (!DetectedActor) {
				CurrentAgentState = AgentState::PATROL;
			}
			else {
				if ((GetActorLocation() - DetectedActor->GetActorLocation()).Size() <= 300) {
					CurrentAgentState = AgentState::AIM;
				}
			}
		}
		else if (CurrentAgentState == AgentState::CHECK)
		{
			AgentCheck();
			if (!isItemExist) {
				CurrentAgentState = AgentState::SEARCH;
			}
		}
		else if (CurrentAgentState == AgentState::AIM)
		{
			AgentAim();
			if (!DetectedActor) {
				CurrentAgentState = AgentState::PATROL;
			}
			else {
				if ((GetActorLocation() - DetectedActor->GetActorLocation()).Size() >= 700) {
					CurrentAgentState = AgentState::CHASE;
				}
			}

		}
		else if (CurrentAgentState == AgentState::SEARCH)
		{
			AgentSearch();
			if (bCanSeePlayer) {
				CurrentAgentState = AgentState::CHASE;
			}
		}


		if (DetectedActor) {
			//Rebuild Blueprint Lookat Function in C++
			//look at this actor
			FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), DetectedActor->GetActorLocation());
			//set the rotator to the actor
			SetActorRotation(LookAtRotation);
		}
	}
	
}

// Called to bind functionality to input
void AEnemyCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AEnemyCharacter::AgentPatrol()
{
	UE_LOG(LogTemp, Warning, TEXT("I'm Patrolling"));
	if (Path.Num() == 0)
	{
		if (Manager)
		{
			Path = Manager->GeneratePath(CurrentNode, Manager->AllNodes[FMath::RandRange(0, Manager->AllNodes.Num() - 1)]);
			isItemChecked = false;
		}
	}
}
void AEnemyCharacter::AgentChase()
{
	UE_LOG(LogTemp, Warning, TEXT("I'm Chasing"));
	//move to the detected actor
	if (DetectedActor != nullptr) {
		AddMovementInput(DetectedActor->GetActorLocation() - GetActorLocation());
		//slow down when close to the actor
		if ((GetActorLocation() - DetectedActor->GetActorLocation()).Size() < 300.0f) {
			GetCharacterMovement()->MaxWalkSpeed = 100.0f;
		}
		else {
			GetCharacterMovement()->MaxWalkSpeed = 400.0f;
		}
	}
}
void AEnemyCharacter::AgentCheck()
{
	UE_LOG(LogTemp, Warning, TEXT("I'm Checking"));
	CheckItem();
}
void AEnemyCharacter::AgentAim()
{
	UE_LOG(LogTemp, Warning, TEXT("I'm Aiming"));
	if (DetectedActor) {
		FVector FireDirection = DetectedActor->GetActorLocation() - GetActorLocation();
		Fire(FireDirection);
	}
}
void AEnemyCharacter::AgentSearch()
{
	UE_LOG(LogTemp, Warning, TEXT("I'm Searching"));
	//search for player location
	FVector SearchLocation = GetWorld()->GetFirstPlayerController()->GetPawn()->GetActorLocation();
	//create a random location around the player
	SearchLocation.X += FMath::RandRange(-1000.0f, 1000.0f);
	SearchLocation.Z += FMath::RandRange(-1000.0f, 1000.0f);
	//move to the random location
	AddMovementInput(SearchLocation - GetActorLocation());
	
}

void AEnemyCharacter::SensePlayer(AActor* ActorSensed, FAIStimulus Stimulus)
{
	if (Stimulus.WasSuccessfullySensed())
	{
		UE_LOG(LogTemp, Warning, TEXT("Player Detected"))
		
		//check if actor is player
		if (ActorSensed->ActorHasTag("Player")) {
			DetectedActor = ActorSensed;
			bCanSeePlayer = true;
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Player Lost"))
		DetectedActor = nullptr;
		bCanSeePlayer = false;
	}
}


void AEnemyCharacter::CheckItem()
{
	//get all the actor with tag
	TArray<AActor*> FoundItems;
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), "Item", FoundItems);
	//for each actor
	for (AActor* Item : FoundItems) {
		//check if the actor is in the range
		if ((GetActorLocation() - Item->GetActorLocation()).Size() <= 300.0f) {
			isItemExist = true;
			return;
		}
		else{
			isItemExist = false;
		}
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

