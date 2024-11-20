// Copyright Epic Games, Inc. All Rights Reserved.

#include "EurotramGameMode.h"
#include "EurotramCharacter.h"
#include "UObject/ConstructorHelpers.h"

AEurotramGameMode::AEurotramGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPerson/Blueprints/BP_FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

}
