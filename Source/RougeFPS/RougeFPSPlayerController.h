// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "RougeFPSPlayerController.generated.h"

class UInputMappingContext;
class UUserWidget;

/**
 *  Simple first person Player Controller
 *  Manages the input mapping context.
 *  Overrides the Player Camera Manager class.
 */
UCLASS(abstract, config="Game")
class ROUGEFPS_API ARougeFPSPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:

	/** Constructor */
	ARougeFPSPlayerController();

protected:

	/** Input Mapping Contexts */
	UPROPERTY(EditAnywhere, Category="Input|Input Mappings")
	TArray<UInputMappingContext*> DefaultMappingContexts;

	/** Input Mapping Contexts */
	UPROPERTY(EditAnywhere, Category="Input|Input Mappings")
	TArray<UInputMappingContext*> MobileExcludedMappingContexts;

	/** Mobile controls widget to spawn */
	UPROPERTY(EditAnywhere, Category="Input|Touch Controls")
	TSubclassOf<UUserWidget> MobileControlsWidgetClass;

	/** Victory widget to show when the shared fire-count objective is complete */
	UPROPERTY(EditAnywhere, Category="Victory")
	TSubclassOf<UUserWidget> VictoryWidgetClass;

	/** Pointer to the mobile controls widget */
	UPROPERTY()
	TObjectPtr<UUserWidget> MobileControlsWidget;

	/** Pointer to the victory widget */
	UPROPERTY()
	TObjectPtr<UUserWidget> VictoryWidget;

	/** If true, the player will use UMG touch controls even if not playing on mobile platforms */
	UPROPERTY(EditAnywhere, Config, Category = "Input|Touch Controls")
	bool bForceTouchControls = false;

	/** Gameplay initialization */
	virtual void BeginPlay() override;

	/** Input mapping context setup */
	virtual void SetupInputComponent() override;

	/** Returns true if the player should use UMG touch controls */
	bool ShouldUseTouchControls() const;

public:
	UFUNCTION(BlueprintCallable, Category="Victory")
	void ReportFireForVictoryCondition();

	UFUNCTION(Client, Reliable, BlueprintCallable, Category="Victory")
	void ClientShowVictoryUI();

protected:
	UFUNCTION(Server, Reliable)
	void ServerReportFireForVictoryCondition();

	void RegisterFireOnServer();
};
