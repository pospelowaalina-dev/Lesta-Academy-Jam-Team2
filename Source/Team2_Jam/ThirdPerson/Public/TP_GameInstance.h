// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Engine/GameInstance.h"
#include "TP_GameInstance.generated.h"

UCLASS()
class UTP_GameInstance : public UGameInstance
{

	GENERATED_BODY()

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess))
	int Level = 1;
	
};
