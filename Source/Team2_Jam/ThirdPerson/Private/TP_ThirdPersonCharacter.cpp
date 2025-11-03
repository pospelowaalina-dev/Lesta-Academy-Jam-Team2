// Copyright Epic Games, Inc. All Rights Reserved.

#include "../Public/TP_ThirdPersonCharacter.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "Components/SkeletalMeshComponent.h"
#include "UObject/UnrealTypePrivate.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

//////////////////////////////////////////////////////////////////////////
// ATP_ThirdPersonCharacter

ATP_ThirdPersonCharacter::ATP_ThirdPersonCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
		
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	//GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	//GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character
	CameraBoom->bDoCollisionTest = false;

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)
}

void ATP_ThirdPersonCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//Stop camera after falling
	if (!bIsCameraStop && GetActorLocation().Z < MinHeightStopCamera)
	{
		bIsCameraStop = true;
		FDetachmentTransformRules rules = FDetachmentTransformRules(EDetachmentRule::KeepWorld, true);
		CameraBoom->DetachFromComponent(rules);
	}

	//Jumping
	if (bIsJumping)
	{
		GetCharacterMovement()->Velocity.Z += GetCharacterMovement()->JumpZVelocity * DeltaTime;
	}
	if (GetCharacterMovement() && GetCharacterMovement()->IsFalling())
	{
		StartFalling = true;
	}
	else
	{
		if (StartFalling)
		{
			StartFalling = true;
			StopFalling = true;
		}
	}
	if (StopFalling)
	{
		StopFalling = false;
		GetCharacterMovement()->Velocity.Z = JumpHeight;
	}

	// Rotate in movement
	if (bIsChangeRotation && Controller)
	{
		RotationTickTime += DeltaTime;

		if (USkeletalMeshComponent *CharacterMesh = GetMesh())
		{
			FRotator CurrentRotate = CharacterMesh->GetRelativeRotation();
			FRotator FinishRotate;// = FRotator(0, -MovementDirection.X * (180 + FirstRotation.Yaw), 0);
			if (MovementDirection.X == 1)
			{
				FinishRotate = FRotator(0, -90, 0);
			}
			else
			{
				FinishRotate = FRotator(0, 90, 0);
			}
			if (CurrentRotate == FinishRotate)
			{
				RotationTickTime = 0.0f;
				bIsChangeRotation = false;
			}
			else
			{
				CharacterMesh->SetRelativeRotation(FMath::RInterpTo(FirstRotation,
					FinishRotate, RotationTickTime / RotationTime, 1));
			}
		}
	}

	//Dead after falling
	if (!bIsCharacterDead && GetActorLocation().Z < MinHeightCharacterDead)
	{
		bIsCharacterDead = true;
		Dead();
	}
}

//////////////////////////////////////////////////////////////////////////
// Input

void ATP_ThirdPersonCharacter::NotifyControllerChanged()
{
	Super::NotifyControllerChanged();

	// Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
}

void ATP_ThirdPersonCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {

		// add character's rotation for movement
		if (USkeletalMeshComponent *CharacterMesh = GetMesh())
		{
			CharacterMesh->SetMobility(EComponentMobility::Movable);
		}

		// Moving
		MovementDirection = FVector(1, 0, 0);
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered,
			this, &ATP_ThirdPersonCharacter::Move);
		
		if (GetMovementComponent())
		{
			GetMovementComponent()->GetNavAgentPropertiesRef().bCanJump = true;
			GetMovementComponent()->GetNavAgentPropertiesRef().bCanCrouch = true;
		}

		// Jumping
		//JumpHeight = JumpMaxHoldTime;
		JumpHeight = GetCharacterMovement()->Velocity.Z;
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started,
			this, &ATP_ThirdPersonCharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed,
			this, &ACharacter::StopJumping);

		//Crouching
		EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Started,
			this, &ATP_ThirdPersonCharacter::CrouchToggle);
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void ATP_ThirdPersonCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector
	FVector MovementVector = Value.Get<FVector>();

	if (Controller != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		if (MovementDirection != MovementVector)
		{
			if (USkeletalMeshComponent *CharacterMesh = GetMesh())
			{
				FirstRotation = CharacterMesh->GetRelativeRotation();
			}
			bIsChangeRotation = true;
			RotationTickTime = 0.0f;
			MovementDirection = MovementVector;
		}

		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = YawRotation.Vector();

		// add movement 
		AddMovementInput(ForwardDirection, MovementVector.X);
	}
}
	
void ATP_ThirdPersonCharacter::CrouchToggle(const FInputActionValue& Value)
{
	if (!IsElspressed)
	{
		if (!bIsJumping)
		{
			if (GetMovementComponent() && !GetMovementComponent()->IsFalling())
			{
				if (bIsCrouched)
				{
					Super::UnCrouch();
				}
				else
				{
					Super::Crouch();
				}
			}
		}
	}
}

void ATP_ThirdPersonCharacter::Jump()
{
	if (!IsElspressed)
	{
		if (!bIsJumping)
		{
			JumpHeight = GetCharacterMovement()->Velocity.Z;
			bIsJumping = true;
			Super::Jump();
		}
	}
}

void ATP_ThirdPersonCharacter::StopJumping()
{
	if (!IsElspressed)
	{
		StartFalling = true;
		bIsJumping = false;
		//	JumpMaxHoldTime = JumpHeight;
		Super::StopJumping();
	}
}

void ATP_ThirdPersonCharacter::Dead()
{
	UFunction *RestartFunction = GetClass()->FindFunctionByName(FName("Restart"));
	if (RestartFunction)
	{
		ProcessEvent(RestartFunction, this);
	}
}