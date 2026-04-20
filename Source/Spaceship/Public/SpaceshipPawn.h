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

	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	// Camera
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* BackSpringArm;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* BackCamera;

	// Mesh
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Mesh, meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* SpaceshipStaticMesh;

	// Movement
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Movement, meta = (AllowPrivateAccess = "true"))
	USpaceshipMovementComponent* MovementComponent;

protected:

	// Input Actions
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
	float ThrottleValue = 0.f;

	UPROPERTY(BlueprintReadOnly, Category = Input)
	bool BoostValue = false;

	UPROPERTY(BlueprintReadOnly, Category = Input)
	float SteeringValue = 0.f;
	
	UPROPERTY(BlueprintReadOnly, Category = Input)
	FVector2D LookAroundValue;

	// Input handlers
	void Steering(const FInputActionValue& Value);
	void Throttle(const FInputActionValue& Value);
	void Boost(const FInputActionValue& Value);
	void LookAround(const FInputActionValue& Value);

	// Server RPCs
	UFUNCTION(Server, Reliable)
	void Server_UpdateLookAround(float Yaw, float Pitch);
	
	UFUNCTION(Server, Unreliable)
	void Server_UpdateThrottle(float Value);
	
	UFUNCTION(Server, Unreliable)
	void Server_UpdateSteering(float Value);
	
	UFUNCTION(Server, Unreliable)
	void Server_UpdateBoost(bool Value);

private:

	int32 maxRoll = 45;
	int32 minRoll = -45;
	float Radius = 0.f;

public:
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float DeltaYaw = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float DeltaPitch = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float DeltaRoll = 0.f;
	
	UPROPERTY(BlueprintReadOnly, Replicated)
	FVector2D VirtualCursor = FVector2D::ZeroVector;
	
	FORCEINLINE USpringArmComponent* GetBackSpringArm() const { return BackSpringArm; }
	FORCEINLINE UCameraComponent* GetBackCamera() const { return BackCamera; }

	void setVirtualCursor(FVector2D Value);
};