// Fill out your copyright notice in the Description page of Project Settings.


#include "SpaceshipPawn.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "SpaceshipMovementComponent.h"
#include "SpaceshipPlayerController.h"
#include "GameFramework/PlayerState.h"
#include "Net/UnrealNetwork.h"

DEFINE_LOG_CATEGORY(LogSpaceship);

ASpaceshipPawn::ASpaceshipPawn()
{
	bReplicates = true;
	PrimaryActorTick.bCanEverTick = true;
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
	
}

void ASpaceshipPawn::SetupPlayerInputComponent(UInputComponent* InputComponent)
{
	Super::SetupPlayerInputComponent(InputComponent);
	
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent))
	{
		
		// steering 
		EnhancedInputComponent->BindAction(SteeringAction, ETriggerEvent::Triggered, this, &ASpaceshipPawn::Steering);
		EnhancedInputComponent->BindAction(SteeringAction, ETriggerEvent::Completed, this, &ASpaceshipPawn::Steering);

		// throttle 
		EnhancedInputComponent->BindAction(ThrottleAction, ETriggerEvent::Triggered, this, &ASpaceshipPawn::Throttle);
		EnhancedInputComponent->BindAction(ThrottleAction, ETriggerEvent::Completed, this, &ASpaceshipPawn::Throttle);
		
		// boost
		EnhancedInputComponent->BindAction(BoostAction, ETriggerEvent::Triggered, this, &ASpaceshipPawn::Boost);
		EnhancedInputComponent->BindAction(BoostAction, ETriggerEvent::Completed, this, &ASpaceshipPawn::Boost);
		
		// look around 
		EnhancedInputComponent->BindAction(LookAroundAction, ETriggerEvent::Triggered, this, &ASpaceshipPawn::LookAround);
	}
	else
	{
		UE_LOG(LogSpaceship, Error, TEXT("'%s' Failed to find an Enhanced Input component! "
								   "This template is built to use the Enhanced Input system. "
								   "If you intend to use the legacy system, then you will need to update this C++ file."),
								   *GetNameSafe(this));
	}
}

void ASpaceshipPawn::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(ASpaceshipPawn, DeltaYaw)
	DOREPLIFETIME(ASpaceshipPawn, DeltaPitch)
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
	
	MovementComponent->SetSteeringInput(SteeringValue);
	MovementComponent->SetThrottleInput(ThrottleValue);
	MovementComponent->SetBoostInput(BoostValue);
	
	double CurrentRoll = SpaceshipStaticMesh->GetRelativeRotation().Roll;
	float TargetRoll = 0.f;
	
	if (FMath::Abs(DeltaYaw / 2) > 0.3f)
	{
		TargetRoll = ( DeltaYaw / 2  ) * maxRoll;
	}
	
	float NewRoll = FMath::FInterpTo(
		CurrentRoll,
		TargetRoll,
		DeltaTime,
		1
	);

	NewRoll = FMath::Clamp(NewRoll, minRoll, maxRoll);
	DeltaRoll = NewRoll - CurrentRoll;
	
	AddActorLocalRotation(FRotator(DeltaPitch * -1, DeltaYaw, 0));
	SpaceshipStaticMesh->AddRelativeRotation(FRotator(0, 0, DeltaRoll));
	
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


void ASpaceshipPawn::Steering(const FInputActionValue& Value)
{
	SteeringValue = Value.Get<float>();
}

void ASpaceshipPawn::Throttle(const FInputActionValue& Value)
{
	ThrottleValue = Value.Get<float>();
}

void ASpaceshipPawn::Boost(const FInputActionValue& Value)
{
	BoostValue = Value.Get<bool>();
}

void ASpaceshipPawn::LookAround(const FInputActionValue& Value)
{
	setVirtualCursor(Value.Get<FVector2D>());
}




