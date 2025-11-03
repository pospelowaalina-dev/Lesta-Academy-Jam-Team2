// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Components/BoxComponent.h"
#include "GameFramework/Actor.h"
#include "TP_ChangeCameraAngle.generated.h"

UCLASS()
class ATP_ChangeCameraAngle : public AActor
{
	GENERATED_BODY()

	ATP_ChangeCameraAngle();
	
public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Components, meta = (AllowPrivateAccess = "true"))
	UBoxComponent *Collider = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera, meta = (AllowPrivateAccess = "true"))
	FRotator CameraRotator;

	virtual void BeginPlay() override;

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};
