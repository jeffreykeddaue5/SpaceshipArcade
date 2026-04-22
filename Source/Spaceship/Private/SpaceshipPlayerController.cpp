// Fill out your copyright notice in the Description page of Project Settings.


#include "SpaceshipPlayerController.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "SpaceshipPawn.h"
#include "Blueprint/UserWidget.h"
#include "HUDWidget.h"

void ASpaceshipPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
}

void ASpaceshipPlayerController::BeginPlay()
{
	Super::BeginPlay();
	CreateHUDWidget();
}

void ASpaceshipPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	
	if (UEnhancedInputLocalPlayerSubsystem * Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		Subsystem->AddMappingContext(InputMappingContext, 0);
	}
}

void ASpaceshipPlayerController::CreateHUDWidget()
{
	if (!IsLocalPlayerController())
	{
		return;
	}
	
	HUDWidget = CreateWidget<UHUDWidget>(this, HUDWidgetClass);
	
	if (IsValid(HUDWidget))
	{
		HUDWidget->AddToViewport();
	}
	
}


