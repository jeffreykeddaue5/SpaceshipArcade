// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "SpaceshipPlayerController.generated.h"

struct FInputActionValue;
class UInputAction;
class ASpaceshipPawn;
class UInputMappingContext;

/**
 * 
 */
UCLASS(abstract)
class SPACESHIP_API ASpaceshipPlayerController : public APlayerController
{
	GENERATED_BODY()
	
	virtual void OnPossess(APawn* InPawn) override;
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;
	
protected:
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputMappingContext * InputMappingContext;
	
};
