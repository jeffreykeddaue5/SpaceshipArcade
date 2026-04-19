// Fill out your copyright notice in the Description page of Project Settings.


#include "SpaceshipGameMode.h"
#include "SpaceshipPawn.h"
#include "GameFramework/PlayerStart.h"
#include "GameFramework/PlayerState.h"
#include "Kismet/GameplayStatics.h"

ASpaceshipGameMode::ASpaceshipGameMode()
{
	DefaultPawnClass = ASpaceshipPawn::StaticClass();
}

AActor* ASpaceshipGameMode::ChoosePlayerStart_Implementation(AController* Player)
{
	TArray<AActor*> Starts;
	UGameplayStatics::GetAllActorsOfClass(this, APlayerStart::StaticClass(), Starts);

	if (Starts.Num() == 0)
		return Super::ChoosePlayerStart(Player);
	
	Starts.Sort([](const AActor& A, const AActor& B)
	{
		return A.GetName() < B.GetName();
	});

	const int32 PlayerIndex = Player->PlayerState->GetPlayerId();

	return Starts[PlayerIndex % Starts.Num()];
}
