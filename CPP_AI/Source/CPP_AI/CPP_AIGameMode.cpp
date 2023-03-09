// Copyright Epic Games, Inc. All Rights Reserved.

#include "CPP_AIGameMode.h"
#include "CPP_AICharacter.h"
#include "UObject/ConstructorHelpers.h"

ACPP_AIGameMode::ACPP_AIGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPersonCPP/Blueprints/ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
