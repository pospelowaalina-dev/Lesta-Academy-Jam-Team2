// Copyright Epic Games, Inc. All Rights Reserved.

#include "../Public/TP_KillBox.h"
#include "Kismet/GameplayStatics.h"
#include "Team2_Jam/ThirdPerson/Public/TP_ThirdPersonCharacter.h"

ATP_KillBox::ATP_KillBox()
{
	PrimaryActorTick.bCanEverTick = false;

	Collider = CreateDefaultSubobject<UBoxComponent>(TEXT("Collider"));
	RootComponent = Collider;

	Collider->SetBoxExtent(FVector(100.0f, 100.0f, 100.0f)); // Set desired size
	Collider->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Collider->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	Collider->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap); // Or ECC_WorldDynamic, etc.
	Collider->SetGenerateOverlapEvents(true);

	DisableCamera = false;
	DisableDeath = false;
}

void ATP_KillBox::BeginPlay()
{
	Super::BeginPlay();

	if (!DisableCamera)
	{
		Collider->OnComponentBeginOverlap.AddDynamic(this, &ATP_KillBox::OnOverlapBegin);
	}
	if (!DisableDeath)
	{
		Collider->OnComponentEndOverlap.AddDynamic(this, &ATP_KillBox::OnOverlapEnd);
	}
}

void ATP_KillBox::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && (OtherActor != this))
	{
		if (ATP_ThirdPersonCharacter *Character = Cast<ATP_ThirdPersonCharacter>(OtherActor))
		{
			Character->MinHeightStopCamera = Character->GetActorLocation().Z;
		}
	}
}

void ATP_KillBox::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex)
{
	if (OtherActor && (OtherActor != this))
	{
		if (ATP_ThirdPersonCharacter *Character = Cast<ATP_ThirdPersonCharacter>(OtherActor))
		{
			Character->MinHeightCharacterDead = Character->GetActorLocation().Z;
		}
	}
}