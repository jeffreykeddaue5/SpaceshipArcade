// Fill out your copyright notice in the Description page of Project Settings.


#include "SpaceshipPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "SpaceshipPawn.h"

void ASpaceshipPlayerController::BeginPlay()
{
	Super::BeginPlay();
}

void ASpaceshipPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	
	if (UEnhancedInputLocalPlayerSubsystem * Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		Subsystem->AddMappingContext(InputMappingContext, 0);
	}
}

void ASpaceshipPlayerController::OnPossess(APawn* Pawn)
{
	Super::OnPossess(Pawn);
	SpaceshipPawn = CastChecked<ASpaceshipPawn>(Pawn);
}
