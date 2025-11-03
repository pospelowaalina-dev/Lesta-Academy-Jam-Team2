// Copyright Epic Games, Inc. All Rights Reserved.

#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "../Public/TP_ChangeCameraAngle.h"

#include "GameFramework/SpringArmComponent.h"
#include "Team2_Jam/ThirdPerson/Public/TP_ThirdPersonCharacter.h"

ATP_ChangeCameraAngle::ATP_ChangeCameraAngle()
{
	CameraRotator = FRotator(0, 0, 0);
	
	PrimaryActorTick.bCanEverTick = false;

	Collider = CreateDefaultSubobject<UBoxComponent>(TEXT("Collider"));
	RootComponent = Collider;

	Collider->SetBoxExtent(FVector(100.0f, 100.0f, 1000.0f)); // Set desired size
	Collider->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Collider->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	Collider->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap); // Or ECC_WorldDynamic, etc.
	Collider->SetGenerateOverlapEvents(true);
}

void ATP_ChangeCameraAngle::BeginPlay()
{
	Super::BeginPlay();

	Collider->OnComponentBeginOverlap.AddDynamic(this, &ATP_ChangeCameraAngle::OnOverlapBegin);
}

void ATP_ChangeCameraAngle::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && (OtherActor != this))
	{
		if (TObjectPtr<ATP_ThirdPersonCharacter> Character = Cast<ATP_ThirdPersonCharacter>(OtherActor))
		{
			Character->GetCameraBoom()->SetRelativeRotation(CameraRotator);
		}
	}
}