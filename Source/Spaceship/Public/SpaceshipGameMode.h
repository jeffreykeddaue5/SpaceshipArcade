// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "SpaceshipGameMode.generated.h"

/**
 * 
 */
UCLASS()
class SPACESHIP_API ASpaceshipGameMode : public AGameModeBase
{
	GENERATED_BODY()
	
	
public:
	ASpaceshipGameMode();
	
	virtual AActor* ChoosePlayerStart_Implementation(AController* Player) override;
	
};
