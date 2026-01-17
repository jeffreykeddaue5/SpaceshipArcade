// Fill out your copyright notice in the Description page of Project Settings.


#include "SpaceshipMovementComponent.h"

#include <ThirdParty/msdfgen/msdfgen.h>

DEFINE_LOG_CATEGORY(LogSpaceshipMovement);

USpaceshipMovementComponent::USpaceshipMovementComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
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

void USpaceshipMovementComponent::UpdateSteering(float DeltaTime)
{
	const FVector RightVector = UpdatedComponent->GetRightVector();
	
	float SideSpeed = FVector::DotProduct(Velocity, RightVector);
	
	constexpr float AccelRate = 1000.f;
	constexpr float MaxSideSpeed = 900.f;
	
	if (SteeringInput > 0.f)
	{
		SideSpeed += AccelRate * DeltaTime; 
	}
	else if (SteeringInput < 0.f)
	{
		SideSpeed -= AccelRate * DeltaTime;
	}
	else
	{
		if ((SideSpeed - (AccelRate * DeltaTime)) > 0)
		{
			SideSpeed -= AccelRate * DeltaTime;
		}
		else if ((SideSpeed + (AccelRate * DeltaTime)) < 0)
		{
			SideSpeed += AccelRate * DeltaTime;
		}
		else
		{
			SideSpeed = 0;
		}
	}
	
	SideSpeed = FMath::Clamp(SideSpeed, -MaxSideSpeed, MaxSideSpeed);
	
	Velocity.Y = SideSpeed * RightVector.Y;
	
	const FVector Delta = Velocity * DeltaTime;

	FHitResult Hit;
	SafeMoveUpdatedComponent(Delta, UpdatedComponent->GetComponentQuat(), true, Hit);
	
	if (Hit.IsValidBlockingHit())
	{
		SlideAlongSurface(Delta, 1.f - Hit.Time, Hit.Normal, Hit);
	}	
}

void USpaceshipMovementComponent::UpdateVelocity(float DeltaTime)
{
	const FVector ForwardVector = UpdatedComponent->GetForwardVector();

	// Project velocity onto forward direction (actual forward speed)
	float ForwardSpeed = FVector::DotProduct(Velocity, ForwardVector);

	constexpr float AccelRate     = 1200.f; // throttle = 1
	constexpr float CoastDecel    = 300.f;  // throttle = 0
	constexpr float BrakeDecel    = 1800.f; // throttle = -1

	// === Acceleration logic ===
	if (ThrottleInput > 0.f)
	{
		ForwardSpeed += AccelRate * DeltaTime;
	}
	else if (ThrottleInput < 0.f)
	{
		ForwardSpeed -= BrakeDecel * DeltaTime;
	}
	else
	{
		ForwardSpeed -= CoastDecel * DeltaTime;
	}

	// Never allow backing up
	ForwardSpeed = FMath::Clamp(ForwardSpeed, 0.f, MaxSpeed);

	// Rebuild velocity strictly along forward vector
	Velocity.X = ForwardVector.X * ForwardSpeed;

	// === Movement ===
	const FVector Delta = Velocity * DeltaTime;

	FHitResult Hit;
	SafeMoveUpdatedComponent(Delta, UpdatedComponent->GetComponentQuat(), true, Hit);

	if (Hit.IsValidBlockingHit())
	{
		SlideAlongSurface(Delta, 1.f - Hit.Time, Hit.Normal, Hit);
	}
	
}
