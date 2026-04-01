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
	UE_LOG(LogSpaceshipMovement, Display, TEXT("Throttle: %f"), ThrottleInput);
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
	
	constexpr float AccelRate = 1000.f;
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

	constexpr float AccelRate     = 1200.f; // throttle = 1
	constexpr float CoastDecel    = 300.f;  // throttle = 0
	constexpr float BrakeDecel    = 1800.f; // throttle = -1
	
	if (ThrottleInput > 0.f)
	{
		CurrentForwardSpeed += AccelRate * DeltaTime;
	}
	else if (ThrottleInput < 0.f)
	{
		CurrentForwardSpeed -= BrakeDecel * DeltaTime;
	}
	else
	{
		CurrentForwardSpeed -= CoastDecel * DeltaTime;
	}
	
	CurrentForwardSpeed = FMath::Clamp(CurrentForwardSpeed, 0.f, MaxSpeed);
	
}

void USpaceshipMovementComponent::UpdateRotation(float DeltaTime)
{
	
}
