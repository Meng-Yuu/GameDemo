// Copyright Epic Games, Inc. All Rights Reserved.


#include "RougeFPSPlayerController.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"
#include "InputMappingContext.h"
#include "Kismet/GameplayStatics.h"
#include "RougeFPSGameMode.h"
#include "RougeFPSCameraManager.h"
#include "Blueprint/UserWidget.h"
#include "RougeFPS.h"
#include "Widgets/Input/SVirtualJoystick.h"

ARougeFPSPlayerController::ARougeFPSPlayerController()
{
	// set the player camera manager class
	PlayerCameraManagerClass = ARougeFPSCameraManager::StaticClass();
}

void ARougeFPSPlayerController::BeginPlay()
{
	Super::BeginPlay();

	
	// only spawn touch controls on local player controllers
	if (ShouldUseTouchControls() && IsLocalPlayerController())
	{
		// spawn the mobile controls widget
		MobileControlsWidget = CreateWidget<UUserWidget>(this, MobileControlsWidgetClass);

		if (MobileControlsWidget)
		{
			// add the controls to the player screen
			MobileControlsWidget->AddToPlayerScreen(0);

		} else {

			UE_LOG(LogRougeFPS, Error, TEXT("Could not spawn mobile controls widget."));

		}

	}
}

void ARougeFPSPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	// only add IMCs for local player controllers
	if (IsLocalPlayerController())
	{
		// Add Input Mapping Context
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
		{
			for (UInputMappingContext* CurrentContext : DefaultMappingContexts)
			{
				Subsystem->AddMappingContext(CurrentContext, 0);
			}

			// only add these IMCs if we're not using mobile touch input
			if (!ShouldUseTouchControls())
			{
				for (UInputMappingContext* CurrentContext : MobileExcludedMappingContexts)
				{
					Subsystem->AddMappingContext(CurrentContext, 0);
				}
			}
		}
	}
	
}

bool ARougeFPSPlayerController::ShouldUseTouchControls() const
{
	// are we on a mobile platform? Should we force touch?
	return SVirtualJoystick::ShouldDisplayTouchInterface() || bForceTouchControls;
}

void ARougeFPSPlayerController::ReportFireForVictoryCondition()
{
	if (HasAuthority())
	{
		RegisterFireOnServer();
		return;
	}

	ServerReportFireForVictoryCondition();
}

void ARougeFPSPlayerController::ServerReportFireForVictoryCondition_Implementation()
{
	RegisterFireOnServer();
}

void ARougeFPSPlayerController::RegisterFireOnServer()
{
	if (UWorld* World = GetWorld())
	{
		if (ARougeFPSGameMode* RougeGameMode = Cast<ARougeFPSGameMode>(World->GetAuthGameMode()))
		{
			RougeGameMode->RegisterPlayerFire(this);
		}
	}
}

void ARougeFPSPlayerController::ClientShowVictoryUI_Implementation()
{
	if (!IsLocalPlayerController() || VictoryWidget)
	{
		return;
	}

	if (!VictoryWidgetClass)
	{
		UE_LOG(LogRougeFPS, Warning, TEXT("VictoryWidgetClass is not set on %s."), *GetNameSafe(this));
		return;
	}

	VictoryWidget = CreateWidget<UUserWidget>(this, VictoryWidgetClass);
	if (!VictoryWidget)
	{
		UE_LOG(LogRougeFPS, Error, TEXT("Could not create victory widget."));
		return;
	}

	VictoryWidget->AddToPlayerScreen(100);

	bShowMouseCursor = true;
	SetIgnoreMoveInput(true);
	SetIgnoreLookInput(true);

	UWidgetBlueprintLibrary::SetInputMode_UIOnlyEx(this, VictoryWidget, EMouseLockMode::DoNotLock, false);
}
