// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "SpaceshipPawn.generated.h"

class USpaceshipMovementComponent;
class UCameraComponent;
class USpringArmComponent;
class UInputAction;
struct FInputActionValue;

DECLARE_LOG_CATEGORY_EXTERN(LogSpaceship, Log, All);

UCLASS(abstract)
class SPACESHIP_API ASpaceshipPawn : public APawn
{
	GENERATED_BODY()

protected:
	
	/** Spring Arm for the back camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* BackSpringArm;

	/** Back Camera component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* BackCamera;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* Spaceship;	
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Movement, meta = (AllowPrivateAccess = "true"))
	USpaceshipMovementComponent* MovementComponent;
	
protected:
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* SteeringAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* ThrottleAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* BrakeAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* LookAroundAction;
	
protected:
	void Steering(const FInputActionValue& Value);
	
	void Throttle(const FInputActionValue& Value);
	
	void Brake(const FInputActionValue& Value);
	
	void StartBrake(const FInputActionValue& Value);
	
	void StopBrake(const FInputActionValue& Value);
	
	void LookAround(const FInputActionValue& Value);
	
	void setVirtualCursor(const FVector2D& Value);
	
public:
	/** Returns the back spring arm subobject */
	FORCEINLINE USpringArmComponent* GetBackSpringArm() const { return BackSpringArm; }
	/** Returns the back camera subobject */
	FORCEINLINE UCameraComponent* GetBackCamera() const { return BackCamera; }
	
	UPROPERTY(BlueprintReadOnly)
	FVector2D VirtualCursor;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float DeltaYaw;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float DeltaPitch;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float DeltaRoll;
	
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float DeltaMovement;
	
private:
	
	int maxRoll = 45;
	int minRoll = -45;

public:
	ASpaceshipPawn();
	
	virtual void BeginPlay() override;
	
	virtual void Tick(float DeltaTime) override;
	
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	

};
