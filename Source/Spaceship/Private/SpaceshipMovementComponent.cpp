#include "SpaceshipMovementComponent.h"
#include "GameFramework/Actor.h"
#include "Components/StaticMeshComponent.h"

DEFINE_LOG_CATEGORY(LogSpaceshipMovement);

USpaceshipMovementComponent::USpaceshipMovementComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void USpaceshipMovementComponent::BeginPlay()
{
	Super::BeginPlay();

	PawnOwner = Cast<APawn>(GetOwner());

	if (PawnOwner)
	{
		UpdatedComponent = PawnOwner->GetRootComponent();
		SpaceshipStaticMesh = PawnOwner->FindComponentByClass<UStaticMeshComponent>();
	}
}

void USpaceshipMovementComponent::SetLookInput(float Yaw, float Pitch)
{
	LookYaw = Yaw;
	LookPitch = Pitch;
}

void USpaceshipMovementComponent::SetSteeringInput(float Value)
{
	SteeringInput = Value;
	UE_LOG(LogSpaceshipMovement, Log, TEXT("Steering Input: %f"), Value);
	if (Value > 0.f)
	{
		StrifeButtonState = EStrifeButtonState::Right;
	}
	else if (Value < 0.f)
	{
		StrifeButtonState = EStrifeButtonState::Left;
	}
	else
	{
		StrifeButtonState = EStrifeButtonState::Idle;
	}
}

void USpaceshipMovementComponent::SetThrottleInput(float Value)
{
	ThrottleInput = Value;
	UE_LOG(LogSpaceshipMovement, Log, TEXT("Throttle Input: %f"), Value);
	if (Value > 0.f)
	{
		ThrottleButtonState = EThrottleButtonState::Forward;
	}
	else if (Value < 0.f)
	{
		ThrottleButtonState = EThrottleButtonState::Backward;
	}
	else
	{
		ThrottleButtonState = EThrottleButtonState::Idle;
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
	
	if (!PawnOwner->HasAuthority() && !PawnOwner->IsLocallyControlled())
	{
		return;
	}

	UpdateVelocity(DeltaTime);
	UpdateSteering(DeltaTime);
	UpdateRotation(DeltaTime);

	const FVector ForwardVector = UpdatedComponent->GetForwardVector();
	const FVector RightVector   = UpdatedComponent->GetRightVector();
	
	Velocity =
	(ForwardVector * CurrentForwardSpeed) +
	(RightVector   * CurrentRightSpeed);

	const FVector Delta = Velocity * DeltaTime;

	FRotator RotationDelta(-LookPitch, LookYaw, 0.f);
	UpdatedComponent->AddLocalRotation(FRotator(RotationDelta));

	FHitResult Hit;

	SafeMoveUpdatedComponent(
		Delta,
		UpdatedComponent->GetComponentQuat(),
		true,
		Hit);

	if (Hit.IsValidBlockingHit())
	{
		SlideAlongSurface(Delta, 1.f - Hit.Time, Hit.Normal, Hit);
	}
}

void USpaceshipMovementComponent::UpdateVelocity(float DeltaTime)
{
	const float Accel = SpaceshipCVars::GetCruiseAccelerationRate();
	const float Decel = SpaceshipCVars::GetCruiseDecelerationRate();
	const float MaxSpeeds = SpaceshipCVars::GetCruiseMaxSpeed();

	switch (ThrottleButtonState)
	{
	case EThrottleButtonState::Forward:
		CurrentForwardSpeed += Accel * DeltaTime;
		break;

	case EThrottleButtonState::Backward:
		CurrentForwardSpeed -= Decel * DeltaTime;
		break;

	case EThrottleButtonState::Idle:
		CurrentForwardSpeed = FMath::FInterpTo(CurrentForwardSpeed, 0.f, DeltaTime, 0.5f);
		break;
	}

	CurrentForwardSpeed = FMath::Clamp(CurrentForwardSpeed, -MaxSpeeds, MaxSpeeds);
}

void USpaceshipMovementComponent::UpdateSteering(float DeltaTime)
{
	constexpr float Acceleration = 1000.f;
	constexpr float MaxSideSpeed = 900.f;

	switch (StrifeButtonState)
	{
	case EStrifeButtonState::Right:
		CurrentRightSpeed += Acceleration * DeltaTime;
		break;
		
	case EStrifeButtonState::Left:
		CurrentRightSpeed -= Acceleration * DeltaTime;
		break;
		
	case EStrifeButtonState::Idle:
		CurrentRightSpeed = 0;
		break;
	}
	CurrentRightSpeed = FMath::Clamp(CurrentRightSpeed, -MaxSideSpeed, MaxSideSpeed);
}

void USpaceshipMovementComponent::UpdateRotation(float DeltaTime) 
{
	if (!SpaceshipStaticMesh)
	{
		return;
	}
	
	float TargetRoll = LookYaw * 45.f;

	FRotator Current = SpaceshipStaticMesh->GetRelativeRotation();
	FRotator Target = FRotator(Current.Pitch, Current.Yaw, TargetRoll);

	FRotator NewRot = FMath::RInterpTo(Current, Target, DeltaTime, 5.f);

	SpaceshipStaticMesh->SetRelativeRotation(NewRot);
}