// Copyright Epic Games, Inc. All Rights Reserved.

#include "RougeFPSGameMode.h"

#include "RougeFPSGameState.h"
#include "RougeFPSPlayerController.h"
#include "Engine/World.h"

ARougeFPSGameMode::ARougeFPSGameMode()
{
	GameStateClass = ARougeFPSGameState::StaticClass();
}

void ARougeFPSGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	const ARougeFPSGameState* RougeGameState = GetGameState<ARougeFPSGameState>();
	if (RougeGameState && RougeGameState->IsGameWon())
	{
		if (ARougeFPSPlayerController* RougePlayerController = Cast<ARougeFPSPlayerController>(NewPlayer))
		{
			RougePlayerController->ClientShowVictoryUI();
		}
	}
}

void ARougeFPSGameMode::RegisterPlayerFire(AController* FireInstigator)
{
	if (!HasAuthority())
	{
		return;
	}

	ARougeFPSGameState* RougeGameState = GetGameState<ARougeFPSGameState>();
	if (!RougeGameState || RougeGameState->IsGameWon())
	{
		return;
	}

	const int32 NewFireCount = RougeGameState->AddFireCount();
	if (NewFireCount >= RougeGameState->GetFireLimit())
	{
		HandleGameWon();
	}
}

void ARougeFPSGameMode::HandleGameWon()
{
	ARougeFPSGameState* RougeGameState = GetGameState<ARougeFPSGameState>();
	if (!RougeGameState || RougeGameState->IsGameWon())
	{
		return;
	}

	RougeGameState->SetGameWon(true);

	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	for (FConstPlayerControllerIterator It = World->GetPlayerControllerIterator(); It; ++It)
	{
		if (ARougeFPSPlayerController* RougePlayerController = Cast<ARougeFPSPlayerController>(It->Get()))
		{
			RougePlayerController->ClientShowVictoryUI();
		}
	}
}
