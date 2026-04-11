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

DEFINE_LOG_CATEGORY(LogSpaceship);

void ASpaceshipPawn::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	
	SpaceshipPlayerController = Cast<ASpaceshipPlayerController>(NewController);
}

ASpaceshipPawn::ASpaceshipPawn()
{
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

void ASpaceshipPawn::BeginPlay()
{
	Super::BeginPlay();
	
}

void ASpaceshipPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	MovementComponent->SetSteeringInput(SpaceshipPlayerController->SteeringValue);
	MovementComponent->SetThrottleInput(SpaceshipPlayerController->ThrottleValue);
	MovementComponent->SetBoostInput(SpaceshipPlayerController->BoostValue);
	
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




