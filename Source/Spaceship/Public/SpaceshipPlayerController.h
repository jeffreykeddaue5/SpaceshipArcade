// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "SpaceshipPlayerController.generated.h"

class ASpaceshipPawn;
class UInputMappingContext;

/**
 * 
 */
UCLASS(abstract)
class SPACESHIP_API ASpaceshipPlayerController : public APlayerController
{
	GENERATED_BODY()
	

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputMappingContext * InputMappingContext;
	
	TObjectPtr<ASpaceshipPawn> SpaceshipPawn;
	
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;
	virtual void OnPossess(APawn* Pawn) override;
	
};
