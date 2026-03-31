// Fill out your copyright notice in the Description page of Project Settings.


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
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;
		
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	// construct the back camera boom
	BackSpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("Back Spring Arm"));
	BackSpringArm->SetupAttachment(RootComponent);
	BackSpringArm->TargetArmLength = 650.0f;
	BackSpringArm->SocketOffset.Z = 150.0f;
	// BackSpringArm->bDoCollisionTest = false;
	// BackSpringArm->bInheritPitch = false;
	// BackSpringArm->bInheritRoll = false;
	//BackSpringArm->bEnableCameraRotationLag = true;
	BackSpringArm->CameraRotationLagSpeed = 2.0f;
	BackSpringArm->CameraLagMaxDistance = 50.0f;
	BackSpringArm->bUsePawnControlRotation = true;

	BackCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("Back Camera"));
	BackCamera->SetupAttachment(BackSpringArm);
	BackCamera->bUsePawnControlRotation = false;
	
	Spaceship = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Spaceship"));
	Spaceship->SetupAttachment(RootComponent);
	
	MovementComponent = CreateDefaultSubobject<USpaceshipMovementComponent>(TEXT("MovementComponent"));
	MovementComponent->UpdatedComponent = RootComponent;
	
	VirtualCursor = FVector2D(0.0f, 0.0f);
}

void ASpaceshipPawn::BeginPlay()
{
	Super::BeginPlay();
	
}

void ASpaceshipPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		
		// steering 
		EnhancedInputComponent->BindAction(SteeringAction, ETriggerEvent::Triggered, this, &ASpaceshipPawn::Steering);
		EnhancedInputComponent->BindAction(SteeringAction, ETriggerEvent::Completed, this, &ASpaceshipPawn::Steering);

		// throttle 
		EnhancedInputComponent->BindAction(ThrottleAction, ETriggerEvent::Triggered, this, &ASpaceshipPawn::Throttle);
		EnhancedInputComponent->BindAction(ThrottleAction, ETriggerEvent::Completed, this, &ASpaceshipPawn::Throttle);

		// break 
		//EnhancedInputComponent->BindAction(BrakeAction, ETriggerEvent::Triggered, this, &ASpaceshipPawn::Brake);
		//EnhancedInputComponent->BindAction(BrakeAction, ETriggerEvent::Started, this, &ASpaceshipPawn::StartBrake);
		//EnhancedInputComponent->BindAction(BrakeAction, ETriggerEvent::Completed, this, &ASpaceshipPawn::StopBrake);
		
		// look around 
		EnhancedInputComponent->BindAction(LookAroundAction, ETriggerEvent::Triggered, this, &ASpaceshipPawn::LookAround);
		
	}
	else
	{
		UE_LOG(LogSpaceship, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}

}

void ASpaceshipPawn::Steering(const FInputActionValue& Value)
{
	float SteeringValue = Value.Get<float>();
	MovementComponent->SetSteeringInput(SteeringValue);
}

void ASpaceshipPawn::Throttle(const FInputActionValue& Value)
{
	float ThrottleValue = Value.Get<float>();
	MovementComponent->SetThrottleInput(ThrottleValue);
}

void ASpaceshipPawn::LookAround(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		const FRotator rotator = GetActorRotation();
		setVirtualCursor(LookAxisVector);
	}
}

void ASpaceshipPawn::setVirtualCursor(const FVector2D& Value)
{
	
	FVector2D ViewportSize;
	if (GEngine && GEngine->GameViewport)
	{
		GEngine->GameViewport->GetViewportSize(ViewportSize);
	}
	
	const float CursorX = (Value.X * 10.0f) + VirtualCursor.X;
	const float CursorY = (Value.Y * 10.0f) + VirtualCursor.Y;
	float Radius = ViewportSize.Y / 4.0f;
	
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

	constexpr float Expo = 9.0f;

	const float NormalizedX = FMath::Clamp(VirtualCursor.X / Radius, -1.0f, 1.0f);
	const float NormalizedY = FMath::Clamp(VirtualCursor.Y / Radius, -1.0f, 1.0f);

	// DeltaX = FMath::Sign(NormalizedX) * FMath::Pow(FMath::Abs(NormalizedX), Expo);
	// DeltaY = FMath::Sign(NormalizedY) * FMath::Pow(FMath::Abs(NormalizedY), Expo);
	
	DeltaYaw = NormalizedX * 2;
	DeltaPitch = NormalizedY * 2;
	
	UE_LOG(LogSpaceshipMovement, Display, TEXT("DeltaX: %f"), DeltaYaw);
	UE_LOG(LogSpaceshipMovement, Display, TEXT("DeltaY: %f"), DeltaPitch);
	
}

void ASpaceshipPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	double CurrentRoll = Spaceship->GetRelativeRotation().Pitch * -1;
	float TargetRoll = 0.f;

	// Player turning → bank ship
	if (FMath::Abs(DeltaYaw / 2) > 0.1f)
	{
		TargetRoll = ( DeltaYaw /2  ) * maxRoll;
	}

	// Smoothly move toward target roll
	float NewRoll = FMath::FInterpTo(
		CurrentRoll,
		TargetRoll,
		DeltaTime,
		1
	);

	// Clamp safety
	NewRoll = FMath::Clamp(NewRoll, minRoll, maxRoll);
	
	DeltaRoll = NewRoll - CurrentRoll;
	
	
	AddActorLocalRotation(FRotator(DeltaPitch * -1, DeltaYaw, 0));
	Spaceship->AddRelativeRotation(FRotator(-1 * DeltaRoll, 0, 0));
	
	
}

