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
	bUseControllerRotationPitch = true;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = true;
		
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
}

void ASpaceshipPawn::BeginPlay()
{
	Super::BeginPlay();
	
	VirtualCursor = FVector2D(0.0f, 0.0f);
	
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
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
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
	
	const float CursorX = (Value.X * 20.0f) + VirtualCursor.X;
	const float CursorY = (Value.Y * 20.0f) + VirtualCursor.Y;
	
	const float ClampedValueX = FMath::Clamp(CursorX, -(ViewportSize.X/2), ViewportSize.X/2);
	const float ClampedValueY = FMath::Clamp(CursorY, -(ViewportSize.Y/2), ViewportSize.Y/2);
	
	VirtualCursor.X = ClampedValueX;
	VirtualCursor.Y = ClampedValueY;
}

void ASpaceshipPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

