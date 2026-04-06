// Fill out your copyright notice in the Description page of Project Settings.


#include "SpaceshipPlayerController.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "SpaceshipPawn.h"

void ASpaceshipPlayerController::BeginPlay()
{
	Super::BeginPlay();
}

void ASpaceshipPlayerController::OnPossess(APawn* Pawn)
{
	Super::OnPossess(Pawn);
	SpaceshipPawn = CastChecked<ASpaceshipPawn>(Pawn);
}

void ASpaceshipPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	
	if (UEnhancedInputLocalPlayerSubsystem * Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		Subsystem->AddMappingContext(InputMappingContext, 0);
	}
	
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent))
	{
		
		// steering 
		EnhancedInputComponent->BindAction(SteeringAction, ETriggerEvent::Triggered, this, &ASpaceshipPlayerController::Steering);
		EnhancedInputComponent->BindAction(SteeringAction, ETriggerEvent::Completed, this, &ASpaceshipPlayerController::Steering);

		// throttle 
		EnhancedInputComponent->BindAction(ThrottleAction, ETriggerEvent::Triggered, this, &ASpaceshipPlayerController::Throttle);
		EnhancedInputComponent->BindAction(ThrottleAction, ETriggerEvent::Completed, this, &ASpaceshipPlayerController::Throttle);
		
		// boost
		EnhancedInputComponent->BindAction(BoostAction, ETriggerEvent::Triggered, this, &ASpaceshipPlayerController::Boost);
		EnhancedInputComponent->BindAction(BoostAction, ETriggerEvent::Completed, this, &ASpaceshipPlayerController::Boost);
		
		// look around 
		EnhancedInputComponent->BindAction(LookAroundAction, ETriggerEvent::Triggered, this, &ASpaceshipPlayerController::LookAround);
		
	}
	else
	{
		UE_LOG(LogSpaceship, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void ASpaceshipPlayerController::Steering(const FInputActionValue& Value)
{
	SteeringValue = Value.Get<float>();
}

void ASpaceshipPlayerController::Throttle(const FInputActionValue& Value)
{
	ThrottleValue = Value.Get<float>();
}

void ASpaceshipPlayerController::Boost(const FInputActionValue& Value)
{
	BoostValue = Value.Get<bool>();
}

void ASpaceshipPlayerController::LookAround(const FInputActionValue& Value)
{
	SpaceshipPawn->setVirtualCursor(Value.Get<FVector2D>());
}
