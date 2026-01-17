// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PawnMovementComponent.h"
#include "SpaceshipMovementComponent.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogSpaceshipMovement, Log, All);

/**
 * 
 */
UCLASS(ClassGroup=(Movement), meta=(BlueprintSpawnableComponent))
class SPACESHIP_API USpaceshipMovementComponent : public UPawnMovementComponent
{
	GENERATED_BODY()
	
public:
	USpaceshipMovementComponent();
	
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	void SetSteeringInput(float Value);
	
	void SetThrottleInput(float Value);
	
protected:
	UPROPERTY(EditAnywhere, Category = "Movement")
	float MaxSpeed = 1200.f;
	
	UPROPERTY(EditAnywhere, Category = "Movement")
	float MaxAcceleration = 400.f;
	
	UPROPERTY(EditAnywhere, Category = "Movement")
	float Deceleration = 50.f;
	
private:
	float ThrottleInput = 0.f;
	
	float SteeringInput = 0.f;
	
	float CurrentAcceleration = 0.0f;
	
	void UpdateSteering(float DeltaTime);
	
	void UpdateVelocity(float DeltaTime);
};
