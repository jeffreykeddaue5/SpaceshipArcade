#include "SpaceshipPawn.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "SpaceshipMovementComponent.h"
#include "Net/UnrealNetwork.h"

DEFINE_LOG_CATEGORY(LogSpaceship);

ASpaceshipPawn::ASpaceshipPawn()
{
	PrimaryActorTick.bCanEverTick = true;
	
	bReplicates = true;

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
	SpaceshipStaticMesh->SetIsReplicated(true);

	MovementComponent = CreateDefaultSubobject<USpaceshipMovementComponent>(TEXT("MovementComponent"));
	MovementComponent->UpdatedComponent = RootComponent;
	MovementComponent->SetIsReplicated(true);
}

void ASpaceshipPawn::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(ASpaceshipPawn, ThrottleValue);
	DOREPLIFETIME(ASpaceshipPawn, BoostValue);
	DOREPLIFETIME(ASpaceshipPawn, SteeringValue);
	DOREPLIFETIME(ASpaceshipPawn, LookAroundValue);
	DOREPLIFETIME(ASpaceshipPawn, ServerState);
	
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

void ASpaceshipPawn::OnRep_ServerState()
{
	if (IsLocallyControlled())
	{
		return; // prediction already correct
	}
	
	UE_LOG(LogSpaceship, Warning, TEXT("CLIENT received state"));
	
	SnapshotBuffer.Add(ServerState);
	DrawDebugDirectionalArrow(
		GetWorld(),
		ServerState.Location,
		ServerState.Location + GetActorForwardVector() * 200,
		40,
		FColor::Red,
		true,
		-1,
		0,
		50.0
	);
	
	if (SnapshotBuffer.Num() > 20)
	{
		SnapshotBuffer.RemoveAt(0);
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
	if (!IsLocallyControlled())
		return;

	FVector2D Input = Value.Get<FVector2D>();

	setVirtualCursor(Input);
	Server_UpdateLookAround(DeltaYaw, DeltaPitch);
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

void ASpaceshipPawn::Server_UpdateLookAround_Implementation(float Yaw, float Pitch)
{
	DeltaYaw = Yaw;
	DeltaPitch = Pitch;
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
}

void ASpaceshipPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	MovementComponent->SetSteeringInput(SteeringValue);
	MovementComponent->SetThrottleInput(ThrottleValue);
	MovementComponent->SetBoostInput(BoostValue);
	MovementComponent->SetLookInput(DeltaYaw, DeltaPitch);
	
	if (HasAuthority())
	{
		UE_LOG(LogSpaceship, Log, TEXT("SERVER updating state"));
		FShipState NewState;

		NewState.Location = GetActorLocation();
		NewState.Rotation = GetActorRotation();
		NewState.Velocity = MovementComponent->Velocity;
		NewState.TimeStamp = GetWorld()->GetTimeSeconds();

		ServerState = NewState; 
	}
	
	if (!HasAuthority() && !IsLocallyControlled() && SnapshotBuffer.Num() > 0)
	{
		UE_LOG(LogSpaceship, Log,
			TEXT("ServerState Location %s"),
			*SnapshotBuffer.Last().Location.ToString());
	}
}