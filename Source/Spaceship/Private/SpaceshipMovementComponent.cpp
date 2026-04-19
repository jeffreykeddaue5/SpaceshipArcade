// Fill out your copyright notice in the Description page of Project Settings.


#include "SpaceshipMovementComponent.h"

#include <ThirdParty/msdfgen/msdfgen.h>

DEFINE_LOG_CATEGORY(LogSpaceshipMovement);

USpaceshipMovementComponent::USpaceshipMovementComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void USpaceshipMovementComponent::SetSteeringInput(float Value)
{
	SteeringInput = Value;
}

void USpaceshipMovementComponent::SetThrottleInput(float Value)
{
	ThrottleInput = Value;
	if (Value == 0.f)
	{
		ThrottleButtonState = EButtonState::Idle;
	}else if (Value == 1.f)
	{
		ThrottleButtonState = EButtonState::Forward;
	}else if (Value == -1.f)
	{
		ThrottleButtonState = EButtonState::Backward;
	}
}

void USpaceshipMovementComponent::SetBoostInput(bool Value)
{
	BoostInput = Value;
}

void USpaceshipMovementComponent::TickComponent(
	float DeltaTime,
	ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!PawnOwner || !UpdatedComponent)
	{
		return;
	}
	
	UpdateVelocity(DeltaTime);
	UpdateSteering(DeltaTime);
	
	const FVector ForwardVector = UpdatedComponent->GetForwardVector();
	const FVector RightVector   = UpdatedComponent->GetRightVector();
	
	Velocity =
			(ForwardVector * CurrentForwardSpeed) +
			(RightVector   * CurrentRightSpeed);
	
	const FVector Delta = Velocity * DeltaTime;
	FHitResult Hit;
	SafeMoveUpdatedComponent(Delta, UpdatedComponent->GetComponentQuat(), true, Hit);

	if (Hit.IsValidBlockingHit())
	{
		SlideAlongSurface(Delta, 1.f - Hit.Time, Hit.Normal, Hit);
	}
}

void USpaceshipMovementComponent::UpdateSteering(float DeltaTime)
{
	const FVector RightVector = UpdatedComponent->GetRightVector();
	
	CurrentRightSpeed = FVector::DotProduct(Velocity, RightVector);
	
	AccelRate = 1000.f;
	constexpr float MaxSideSpeed = 900.f;
	
	if (SteeringInput > 0.f)
	{
		CurrentRightSpeed += AccelRate * DeltaTime; 
	}
	else if (SteeringInput < 0.f)
	{
		CurrentRightSpeed -= AccelRate * DeltaTime;
	}
	else
	{
		if ((CurrentRightSpeed - (AccelRate * DeltaTime)) > 0)
		{
			CurrentRightSpeed -= AccelRate * DeltaTime;
		}
		else if ((CurrentRightSpeed + (AccelRate * DeltaTime)) < 0)
		{
			CurrentRightSpeed += AccelRate * DeltaTime;
		}
		else
		{
			CurrentRightSpeed = 0;
		}
	}
	
	CurrentRightSpeed = FMath::Clamp(CurrentRightSpeed, -MaxSideSpeed, MaxSideSpeed);
}

void USpaceshipMovementComponent::UpdateVelocity(float DeltaTime)
{
	const FVector ForwardVector = UpdatedComponent->GetForwardVector();
	
	CurrentForwardSpeed = FVector::DotProduct(Velocity, ForwardVector);
	
	//BoostInput
	if (ThrottleButtonState == EButtonState::Forward)
	{
		CurrentForwardSpeed += SpaceshipCVars::GetCruiseAccelerationRate() * DeltaTime;
	}
	else if (ThrottleButtonState == EButtonState::Backward)
	{
		CurrentForwardSpeed -= SpaceshipCVars::GetCruiseDecelerationRate() * DeltaTime;
	}
	else if (ThrottleButtonState == EButtonState::Idle)
	{
		//there is no coasting when nothing is pressed, and the ship continues to go forward
	}
	
	CurrentForwardSpeed = FMath::Clamp(CurrentForwardSpeed, 0.f, SpaceshipCVars::GetCruiseMaxSpeed());
}

void USpaceshipMovementComponent::UpdateRotation(float DeltaTime)
{
	
}
