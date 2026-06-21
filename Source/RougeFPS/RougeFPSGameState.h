// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "RougeFPSGameState.generated.h"

/**
 * Shared match state for the first person game.
 */
UCLASS()
class ROUGEFPS_API ARougeFPSGameState : public AGameStateBase
{
	GENERATED_BODY()

public:
	ARougeFPSGameState();

	UFUNCTION(BlueprintPure, Category="Victory")
	int32 GetTotalFireCount() const;

	UFUNCTION(BlueprintPure, Category="Victory")
	int32 GetFireLimit() const;

	UFUNCTION(BlueprintPure, Category="Victory")
	bool IsGameWon() const;

	int32 AddFireCount();
	void SetGameWon(bool bNewGameWon);

protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category="Victory")
	int32 TotalFireCount = 0;

	UPROPERTY(Replicated, EditDefaultsOnly, BlueprintReadOnly, Category="Victory", meta=(ClampMin="1"))
	int32 FireLimit = 30;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category="Victory")
	bool bGameWon = false;
};
