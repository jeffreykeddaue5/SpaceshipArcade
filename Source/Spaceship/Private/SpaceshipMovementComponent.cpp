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
	Velocity = ForwardVector * ForwardSpeed;

	// === Movement ===
	const FVector Delta = Velocity * DeltaTime;

	FHitResult Hit;
	SafeMoveUpdatedComponent(Delta, UpdatedComponent->GetComponentQuat(), true, Hit);

	if (Hit.IsValidBlockingHit())
	{
		SlideAlongSurface(Delta, 1.f - Hit.Time, Hit.Normal, Hit);
	}
}


void USpaceshipMovementComponent::SetThrottleInput(float Value)
{
	ThrottleInput = Value;
	UE_LOG(LogSpaceshipMovement, Display, TEXT("Throttle: %f"), ThrottleInput);
}