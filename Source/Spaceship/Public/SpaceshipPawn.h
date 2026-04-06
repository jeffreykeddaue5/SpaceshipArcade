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

public:
	ASpaceshipPawn();
	
	virtual void BeginPlay() override;
	
	virtual void Tick(float DeltaTime) override;
	
public:
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float DeltaYaw;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
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

protected:
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* BackSpringArm;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* BackCamera;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* SpaceshipStaticMesh;	
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Movement, meta = (AllowPrivateAccess = "true"))
	USpaceshipMovementComponent* MovementComponent;
	
private:
	int maxRoll = 45;
	
	int minRoll = -45;
	
	float Radius = 0;
	
	UPROPERTY()
	ASpaceshipPlayerController* SpaceshipPlayerController;
	
	virtual void PossessedBy(AController* NewController) override;
	
};
