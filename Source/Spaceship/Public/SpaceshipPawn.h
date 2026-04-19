// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "SpaceshipPawn.generated.h"

class ASpaceshipPlayerController;
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
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* BackSpringArm;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* BackCamera;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* SpaceshipStaticMesh;	
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Movement, meta = (AllowPrivateAccess = "true"))
	USpaceshipMovementComponent* MovementComponent;

public:
	ASpaceshipPawn();
	
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;
	
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	
	virtual void BeginPlay() override;
	
	virtual void Tick(float DeltaTime) override;

protected:
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* SteeringAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* ThrottleAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* BoostAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* BrakeAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* LookAroundAction;
	
	UPROPERTY(BlueprintReadOnly, Category = Input)
	float ThrottleValue;
	UPROPERTY(BlueprintReadOnly, Category = Input)
	bool BoostValue;
	UPROPERTY(BlueprintReadOnly, Category = Input)
	float SteeringValue;
	
	void Steering(const FInputActionValue& Value);
	void Throttle(const FInputActionValue& Value);
	void Boost(const FInputActionValue& Value);
	void LookAround(const FInputActionValue& Value);
	
	
private:
	
	int maxRoll = 45;
	int minRoll = -45;
	float Radius = 0;
	
	UPROPERTY()
	ASpaceshipPlayerController* SpaceshipPlayerController;
	
public:
	
	UPROPERTY(EditAnywhere, Replicated, BlueprintReadOnly)
	float DeltaYaw;
	
	UPROPERTY(EditAnywhere, Replicated, BlueprintReadOnly)
	float DeltaPitch;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float DeltaRoll;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float DeltaMovement;
	
	UPROPERTY(BlueprintReadOnly)
	FVector2D VirtualCursor{0,0};
	
	FORCEINLINE USpringArmComponent* GetBackSpringArm() const { return BackSpringArm; }

	FORCEINLINE UCameraComponent* GetBackCamera() const { return BackCamera; }
	
	void setVirtualCursor( FVector2D Value);
	
};
