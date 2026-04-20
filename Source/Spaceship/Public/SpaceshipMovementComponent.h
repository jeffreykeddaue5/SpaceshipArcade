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

enum class EThrottleButtonState : uint8
{
	Idle,
	Forward,
	Backward,
};

enum class EStrifeButtonState : uint8
{
	Idle,
	Left,
	Right
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
	
	virtual void BeginPlay() override;
	
	virtual void TickComponent(float DeltaTime, 
		enum ELevelTick TickType, 
		FActorComponentTickFunction* ThisTickFunction) override;
	
	void SetSteeringInput(float Value);
	void SetThrottleInput(float Value);
	void SetBoostInput(bool Value);
	void SetLookInput(float Yaw, float Pitch);
	
protected:
	
	UStaticMeshComponent* SpaceshipStaticMesh;
	
	UPROPERTY(EditAnywhere, Category = "Movement")
	float MaxSpeed = 12000.f;
	
	float LookYaw = 0.f;
	float LookPitch = 0.f;
	float DeltaRoll = 0.f;

	UPROPERTY(EditAnywhere)
	float MaxRoll = 45.f;

	UPROPERTY(EditAnywhere)
	float MinRoll = -45.f;

private:
	ESpeedState SpeedState = ESpeedState::Idle;
	EThrottleButtonState ThrottleButtonState = EThrottleButtonState::Idle;
	EStrifeButtonState StrifeButtonState = EStrifeButtonState::Idle;
	
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
