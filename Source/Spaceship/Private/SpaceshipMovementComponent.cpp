// Fill out your copyright notice in the Description page of Project Settings.


#include "SpaceshipMovementComponent.h"

USpaceshipMovementComponent::USpaceshipMovementComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void USpaceshipMovementComponent::TickComponent(float DeltaTime, enum ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!PawnOwner || !UpdatedComponent)
	{
		return;
	}
	
	const FVector ForwardVector = UpdatedComponent->GetForwardVector();
	
	const FVector TargetVelocity = ForwardVector * (ThrottleInput * MaxSpeed);
	
	const float CurrentAcceleration = 
			(FMath::Abs(ThrottleInput) > KINDA_SMALL_NUMBER)
			? Acceleration
			: Deceleration;
	
	Velocity = FMath::VInterpConstantTo(Velocity, TargetVelocity, DeltaTime, CurrentAcceleration);
	
	const FVector Delta = Velocity * DeltaTime;
	
	if (!Velocity.IsNearlyZero())
	{
		FHitResult Hit;
		SafeMoveUpdatedComponent(Delta, UpdatedComponent->GetComponentQuat(), true, Hit);	
		
		if (Hit.IsValidBlockingHit())
		{
			SlideAlongSurface(Delta, 1.f - Hit.Time, Hit.Normal, Hit);
		}
	}


}

void USpaceshipMovementComponent::setThrottleInput(float Value)
{
	ThrottleInput = FMath::Clamp(Value, -1.0f, 1.0f);
}



