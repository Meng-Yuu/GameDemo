// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "RougeFPSGameMode.generated.h"

/**
 *  Simple GameMode for a first person game
 */
UCLASS(abstract)
class ARougeFPSGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ARougeFPSGameMode();

	UFUNCTION(BlueprintCallable, Category="Victory")
	void RegisterPlayerFire(AController* FireInstigator);

protected:
	virtual void PostLogin(APlayerController* NewPlayer) override;

	void HandleGameWon();
};



