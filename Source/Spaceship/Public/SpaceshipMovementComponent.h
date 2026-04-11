// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "SpaceshipCvars.h"
#include "CoreMinimal.h"
#include "GameFramework/PawnMovementComponent.h"
#include "SpaceshipMovementComponent.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogSpaceshipMovement, Log, All);

/**
 * 
 */

enum class EButtonState : uint8
{
	Idle,
	Forward,
	Backward,
};

enum class ESpeedState : uint8
{
	Idle,
	Cruise,
	Boost
};


UCLASS(ClassGroup=(Movement), meta=(BlueprintSpawnableComponent))
class SPACESHIP_API USpaceshipMovementComponent : public UPawnMovementComponent
{
	GENERATED_BODY()
	
public:
	USpaceshipMovementComponent();
	
	virtual void TickComponent(float DeltaTime, 
		enum ELevelTick TickType, 
		FActorComponentTickFunction* ThisTickFunction) override;
	
	void SetSteeringInput(float Value);
	void SetThrottleInput(float Value);
	void SetBoostInput(bool Value);
	
protected:
	UPROPERTY(EditAnywhere, Category = "Movement")
	float MaxSpeed = 12000.f;

private:
	ESpeedState SpeedState = ESpeedState::Idle;
	EButtonState ThrottleButtonState = EButtonState::Idle;
	
	float ThrottleInput = 0.f;
	float SteeringInput = 0.f;
	bool  BoostInput    = false;
	float CurrentAcceleration = 0.0f;
	float CurrentForwardSpeed = 0.0f;
	float CurrentRightSpeed = 0.0f;
	
	float AccelRate     = 1200.f; // throttle = 1
	float CoastDecel    = 300.f;  // throttle = 0
	float BrakeDecel    = 1800.f; // throttle = -1
	
	void UpdateSteering(float DeltaTime);
	
	void UpdateVelocity(float DeltaTime);
	
	void UpdateRotation(float DeltaTime);
};
