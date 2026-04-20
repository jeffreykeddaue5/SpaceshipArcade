#include "SpaceshipPawn.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "SpaceshipMovementComponent.h"

DEFINE_LOG_CATEGORY(LogSpaceship);

ASpaceshipPawn::ASpaceshipPawn()
{
	PrimaryActorTick.bCanEverTick = true;
	
	bReplicates = true;
	AActor::SetReplicateMovement(true);

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	BackSpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("Back Spring Arm"));
	BackSpringArm->SetupAttachment(RootComponent);
	BackSpringArm->TargetArmLength = 650.0f;
	BackSpringArm->SocketOffset.Z = 150.0f;

	BackSpringArm->CameraRotationLagSpeed = 2.0f;
	BackSpringArm->CameraLagMaxDistance = 50.0f;
	BackSpringArm->bUsePawnControlRotation = true;

	BackCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("Back Camera"));
	BackCamera->SetupAttachment(BackSpringArm);
	BackCamera->bUsePawnControlRotation = false;

	SpaceshipStaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Spaceship"));
	SpaceshipStaticMesh->SetupAttachment(RootComponent);

	MovementComponent = CreateDefaultSubobject<USpaceshipMovementComponent>(TEXT("MovementComponent"));
	MovementComponent->UpdatedComponent = RootComponent;
	MovementComponent->SetIsReplicated(true);
}

void ASpaceshipPawn::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

void ASpaceshipPawn::SetupPlayerInputComponent(UInputComponent* InputComponent)
{
	Super::SetupPlayerInputComponent(InputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent))
	{
		EnhancedInputComponent->BindAction(SteeringAction, ETriggerEvent::Triggered, this, &ASpaceshipPawn::Steering);
		EnhancedInputComponent->BindAction(SteeringAction, ETriggerEvent::Completed, this, &ASpaceshipPawn::Steering);

		EnhancedInputComponent->BindAction(ThrottleAction, ETriggerEvent::Triggered, this, &ASpaceshipPawn::Throttle);
		EnhancedInputComponent->BindAction(ThrottleAction, ETriggerEvent::Completed, this, &ASpaceshipPawn::Throttle);

		EnhancedInputComponent->BindAction(BoostAction, ETriggerEvent::Triggered, this, &ASpaceshipPawn::Boost);
		EnhancedInputComponent->BindAction(BoostAction, ETriggerEvent::Completed, this, &ASpaceshipPawn::Boost);

		EnhancedInputComponent->BindAction(LookAroundAction, ETriggerEvent::Triggered, this, &ASpaceshipPawn::LookAround);
	}
}
void ASpaceshipPawn::Steering(const FInputActionValue& Value)
{
	SteeringValue = Value.Get<float>();
	if (IsLocallyControlled())
	{
		Server_UpdateSteering(SteeringValue);
	}
}

void ASpaceshipPawn::Throttle(const FInputActionValue& Value)
{
	ThrottleValue = Value.Get<float>();
	if (IsLocallyControlled())
	{
		Server_UpdateThrottle(ThrottleValue);
	}
}

void ASpaceshipPawn::Boost(const FInputActionValue& Value)
{
	BoostValue = Value.Get<bool>();
	if (IsLocallyControlled())
	{
		Server_UpdateBoost(BoostValue);
	}
}

void ASpaceshipPawn::LookAround(const FInputActionValue& Value)
{
	LookAroundValue = Value.Get<FVector2D>();
	setVirtualCursor(LookAroundValue);
	if (IsLocallyControlled())
	{
		Server_UpdateLookAround(LookAroundValue);
	}
}

void ASpaceshipPawn::Server_UpdateBoost_Implementation(bool Value)
{
	BoostValue = Value;
}

void ASpaceshipPawn::Server_UpdateSteering_Implementation(float Value)
{
	SteeringValue = Value;
}

void ASpaceshipPawn::Server_UpdateThrottle_Implementation(float Value)
{
	ThrottleValue = Value;
}

void ASpaceshipPawn::Server_UpdateLookAround_Implementation(FVector2D Value)
{
	setVirtualCursor(Value);
}

void ASpaceshipPawn::setVirtualCursor(FVector2D Value)
{
	FVector2D ViewportSize;
	if (GEngine && GEngine->GameViewport)
	{
		GEngine->GameViewport->GetViewportSize(ViewportSize);
	}

	const float CursorX = (Value.X * 5.0f) + VirtualCursor.X;
	const float CursorY = (Value.Y * 5.0f) + VirtualCursor.Y;

	Radius = ViewportSize.Y / 4.0f;

	FVector2D Cursor(CursorX, CursorY);
	FVector2D Center(0.f, 0.f);
	FVector2D Offset = Cursor - Center;

	if (const float Distance = Offset.Size(); Distance > Radius)
	{
		Offset = Offset.GetSafeNormal() * Radius;
	}

	const FVector2D ClampedCursor = Center + Offset;

	VirtualCursor.X = ClampedCursor.X;
	VirtualCursor.Y = ClampedCursor.Y;

	const float NormalizedX = FMath::Clamp(VirtualCursor.X / Radius, -1.0f, 1.0f);
	const float NormalizedY = FMath::Clamp(VirtualCursor.Y / Radius, -1.0f, 1.0f);

	DeltaYaw = NormalizedX;
	DeltaPitch = NormalizedY;
}



void ASpaceshipPawn::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(LogTemp, Warning, TEXT("Pawn Spawned: %s NetMode: %d"),
		*GetName(),
		GetNetMode());
}

void ASpaceshipPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (IsLocallyControlled() || HasAuthority())
	{
		MovementComponent->SetSteeringInput(SteeringValue);
		MovementComponent->SetThrottleInput(ThrottleValue);
		MovementComponent->SetBoostInput(BoostValue);
		MovementComponent->SetLookInput(DeltaYaw, DeltaPitch);
		
		DrawDebugDirectionalArrow(
			GetWorld(),
			GetActorLocation(),
			GetActorLocation() + GetActorForwardVector() * 400.f,
			50.f,
			FColor::Red,
			true,
			-1.0,
			0,
			50.f
		);
	}
	
	/*
	float CurrentRoll =
		SpaceshipStaticMesh->GetRelativeRotation().Roll;
	
	float TargetRoll = 0.f;
	
	if (FMath::Abs(DeltaYaw / 2.f) > 0.3f)
	{
		TargetRoll = (DeltaYaw / 2.f) * maxRoll;
	}
	
	float NewRoll = FMath::FInterpTo(
		CurrentRoll,
		TargetRoll,
		DeltaTime,
		1.f);
	
	NewRoll = FMath::Clamp(NewRoll,
		(float)minRoll,
		(float)maxRoll);
	
	AddActorLocalRotation(
		FRotator(DeltaPitch * -1.f, DeltaYaw, 0.f));
	
	SpaceshipStaticMesh->SetRelativeRotation(
		FRotator(0.f, 0.f, NewRoll));
		
		*/
}