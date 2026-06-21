// Copyright Epic Games, Inc. All Rights Reserved.


#include "RougeFPSGameState.h"

#include "Net/UnrealNetwork.h"

ARougeFPSGameState::ARougeFPSGameState()
{
}

void ARougeFPSGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ARougeFPSGameState, TotalFireCount);
	DOREPLIFETIME(ARougeFPSGameState, FireLimit);
	DOREPLIFETIME(ARougeFPSGameState, bGameWon);
}

int32 ARougeFPSGameState::GetTotalFireCount() const
{
	return TotalFireCount;
}

int32 ARougeFPSGameState::GetFireLimit() const
{
	return FireLimit;
}

bool ARougeFPSGameState::IsGameWon() const
{
	return bGameWon;
}

int32 ARougeFPSGameState::AddFireCount()
{
	++TotalFireCount;
	return TotalFireCount;
}

void ARougeFPSGameState::SetGameWon(bool bNewGameWon)
{
	bGameWon = bNewGameWon;
}
