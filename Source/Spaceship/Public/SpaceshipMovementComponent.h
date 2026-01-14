// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PawnMovementComponent.h"
#include "SpaceshipMovementComponent.generated.h"

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
	
	void setThrottleInput(float Value);
	
protected:
	UPROPERTY(EditAnywhere, Category = "Movement")
	float MaxSpeed = 600.f;
	
	UPROPERTY(EditAnywhere, Category = "Movement")
	float Acceleration = 800.f;
	
	UPROPERTY(EditAnywhere, Category = "Movement")
	float Deceleration = 100.f;
	
private:
	float ThrottleInput = 0.f;
	FVector Velocity = FVector::ZeroVector;
	
};
