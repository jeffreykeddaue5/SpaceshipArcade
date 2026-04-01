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

public:
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* SteeringAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* ThrottleAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* BrakeAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* LookAroundAction;
	
	float ThrottleValue;
	
	float SteeringValue;
	
protected:
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputMappingContext * InputMappingContext;
	
	TObjectPtr<ASpaceshipPawn> SpaceshipPawn;
	
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;
	virtual void OnPossess(APawn* Pawn) override;
	
private:
	
	float Radius = 0;
	void Steering(const FInputActionValue& Value);
	void Throttle(const FInputActionValue& Value);
	void LookAround(const FInputActionValue& Value);
	
	
};
