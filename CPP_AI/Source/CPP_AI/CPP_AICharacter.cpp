// Copyright Epic Games, Inc. All Rights Reserved.

#include "CPP_AICharacter.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"

#include "Components/SkeletalMeshComponent.h"
#include "Runtime/Engine/Classes/Engine/World.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "GameFramework/PlayerController.h"
#include "Perception/AIPerceptionStimuliSourceComponent.h"
#include "Perception/AISense_Sight.h"

ACPP_AICharacter::ACPP_AICharacter()
{
    GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

    BaseTurnRate = 45.f;
    BaseLookUpRate = 45.f;

    bUseControllerRotationPitch = false;
    bUseControllerRotationYaw = false;
    bUseControllerRotationRoll = false;

    GetCharacterMovement()->bOrientRotationToMovement = true;            // Character moves in the direction of input...
    GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate
    GetCharacterMovement()->JumpZVelocity = 600.f;
    GetCharacterMovement()->AirControl = 0.2f;

    CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
    CameraBoom->SetupAttachment(RootComponent);
    CameraBoom->TargetArmLength = 300.0f;       // The camera follows at this distance behind the character
    CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

    FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
    FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom
                                                                                // adjust to match the controller orientation
    FollowCamera->bUsePawnControlRotation = false;                              // Camera does not rotate relative to arm

    SetupStimulus();
}

//////////////////////////////////////////////////////////////////////////
// Input

void ACPP_AICharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
    // Set up gameplay key bindings
    check(PlayerInputComponent);
    PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
    PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

    PlayerInputComponent->BindAxis("MoveForward", this, &ACPP_AICharacter::MoveForward);
    PlayerInputComponent->BindAxis("MoveRight", this, &ACPP_AICharacter::MoveRight);

    // We have 2 versions of the rotation bindings to handle different kinds of devices differently
    // "turn" handles devices that provide an absolute delta, such as a mouse.
    // "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
    PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
    PlayerInputComponent->BindAxis("TurnRate", this, &ACPP_AICharacter::TurnAtRate);
    PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
    PlayerInputComponent->BindAxis("LookUpRate", this, &ACPP_AICharacter::LookUpAtRate);

    // handle touch devices
    PlayerInputComponent->BindTouch(IE_Pressed, this, &ACPP_AICharacter::TouchStarted);
    PlayerInputComponent->BindTouch(IE_Released, this, &ACPP_AICharacter::TouchStopped);

    // VR headset functionality
    PlayerInputComponent->BindAction("ResetVR", IE_Pressed, this, &ACPP_AICharacter::OnResetVR);
}

void ACPP_AICharacter::SetupStimulus() 
{
    Stimulus = CreateDefaultSubobject<UAIPerceptionStimuliSourceComponent>(TEXT("Stimulus"));
    Stimulus->RegisterForSense(TSubclassOf<UAISense_Sight>());
    Stimulus->RegisterWithPerceptionSystem();
}

void ACPP_AICharacter::BeginPlay()
{
    Super::BeginPlay();
    APlayerCameraManager* const CameraManager = GetWorld()->GetFirstPlayerController()->PlayerCameraManager;
    CameraManager->ViewPitchMin = -50.0f;
    CameraManager->ViewPitchMax = 10.0f;

    UMaterialInstanceDynamic* const MaterialInstance = UMaterialInstanceDynamic::Create(GetMesh()->GetMaterial(0), this);
    if (MaterialInstance)
    {
        MaterialInstance->SetVectorParameterValue("BodyColor", FLinearColor::Red);
        GetMesh()->SetMaterial(0, MaterialInstance);
    }
}

void ACPP_AICharacter::OnResetVR()
{
    // If CPP_AI is added to a project via 'Add Feature' in the Unreal Editor the dependency on HeadMountedDisplay in CPP_AI.Build.cs is not
    // automatically propagated and a linker error will result. You will need to either:
    //		Add "HeadMountedDisplay" to [YourProject].Build.cs PublicDependencyModuleNames in order to build successfully (appropriate
    //if supporting VR).
    // or:
    //		Comment or delete the call to ResetOrientationAndPosition below (appropriate if not supporting VR)
    UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition();
}

void ACPP_AICharacter::TouchStarted(ETouchIndex::Type FingerIndex, FVector Location)
{
    Jump();
}

void ACPP_AICharacter::TouchStopped(ETouchIndex::Type FingerIndex, FVector Location)
{
    StopJumping();
}

void ACPP_AICharacter::TurnAtRate(float Rate)
{
    // calculate delta for this frame from the rate information
    AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void ACPP_AICharacter::LookUpAtRate(float Rate)
{
    // calculate delta for this frame from the rate information
    AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void ACPP_AICharacter::MoveForward(float Value)
{
    if ((Controller != nullptr) && (Value != 0.0f))
    {
        // find out which way is forward
        const FRotator Rotation = Controller->GetControlRotation();
        const FRotator YawRotation(0, Rotation.Yaw, 0);

        // get forward vector
        const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
        AddMovementInput(Direction, Value);
    }
}

void ACPP_AICharacter::MoveRight(float Value)
{
    if ((Controller != nullptr) && (Value != 0.0f))
    {
        // find out which way is right
        const FRotator Rotation = Controller->GetControlRotation();
        const FRotator YawRotation(0, Rotation.Yaw, 0);

        // get right vector
        const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
        // add movement in that direction
        AddMovementInput(Direction, Value);
    }
}
