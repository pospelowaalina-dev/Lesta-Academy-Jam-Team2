#pragma once

#include "Components/BoxComponent.h"
#include "GameFramework/Actor.h"
#include "TP_KillBox.generated.h"

UCLASS()
class ATP_KillBox : public AActor
{
	GENERATED_BODY()

	ATP_KillBox();
	
public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Components, meta = (AllowPrivateAccess = "true"))
	UBoxComponent *Collider = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = DisableComponents, meta = (AllowPrivateAccess = "true"))
	bool DisableCamera = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = DisableComponents, meta = (AllowPrivateAccess = "true"))
	bool DisableDeath = false;

	virtual void BeginPlay() override;

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	
	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};
