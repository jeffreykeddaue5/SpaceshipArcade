#include "SpaceshipGameMode.h"
#include "SpaceshipPawn.h"
#include "GameFramework/PlayerStart.h"
#include "GameFramework/PlayerState.h"
#include "Kismet/GameplayStatics.h"

ASpaceshipGameMode::ASpaceshipGameMode()
{
	DefaultPawnClass = ASpaceshipPawn::StaticClass();
}

AActor* ASpaceshipGameMode::ChoosePlayerStart_Implementation(AController* Player)
{
	TArray<AActor*> Starts;
	UGameplayStatics::GetAllActorsOfClass(this, APlayerStart::StaticClass(), Starts);

	if (Starts.Num() == 0)
	{
		return Super::ChoosePlayerStart_Implementation(Player);
	}
	
	Starts.Sort([](const AActor& A, const AActor& B)
	{
		return A.GetName() < B.GetName();
	});

	int32 PlayerIndex = 0;

	if (Player && Player->PlayerState)
	{
		PlayerIndex = Player->PlayerState->GetPlayerId();
	}

	PlayerIndex = FMath::Abs(PlayerIndex);
	const int32 StartIndex = PlayerIndex % Starts.Num();

	return Starts[StartIndex];
}