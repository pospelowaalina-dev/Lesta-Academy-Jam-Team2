// Copyright Epic Games, Inc. All Rights Reserved.

#include "../Public/TP_ThirdPersonGameMode.h"
#include "../Public/TP_ThirdPersonCharacter.h"
#include "UObject/ConstructorHelpers.h"

ATP_ThirdPersonGameMode::ATP_ThirdPersonGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/Content/ThirdPerson/Blueprints/BP_MainCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
