// Copyright SomethingNotRandom. All Rights Reserved.

#include "UnicornPlayerController.h"
#include "Engine/Engine.h"

#include "UnicornCharacter.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "InputAction.h"

void AUnicornPlayerController::BeginPlay()
{
	Super::BeginPlay();
}

void AUnicornPlayerController::OnPossess(APawn* InPawn)
{
    Super::OnPossess(InPawn);

    Unicorn = Cast<AUnicornCharacter>(InPawn);
    if (!Unicorn.IsValid())
    {
        GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, TEXT("No Unicorn Character possessed!"));
    }

    if (!bInputInitialized)
    {
        // Load Enhanced Input assets
        if (!InputMappingContext.IsNull())
        {
            LoadedMappingContext = InputMappingContext.LoadSynchronous();
        }
        if (!MoveAction.IsNull())
        {
            LoadedMoveAction = MoveAction.LoadSynchronous();
        }
        if (!JumpAction.IsNull())
        {
            LoadedJumpAction = JumpAction.LoadSynchronous();
        }

        // Add to Enhanced Input Subsystem
        ULocalPlayer* LocalPlayer = GetLocalPlayer();
        if (LocalPlayer)
        {
            if (UEnhancedInputLocalPlayerSubsystem* Subsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
            {
                if (LoadedMappingContext)
                {
                    Subsystem->AddMappingContext(LoadedMappingContext, 0);
                }
                else
                {
                    GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, TEXT("Failed to access LoadedMappingContext!"));
                }

                if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent))
                {
                    if (LoadedMoveAction)
                    {
                        EnhancedInputComponent->BindAction(LoadedMoveAction, ETriggerEvent::Triggered, this, &AUnicornPlayerController::OnMove);
                    }
                    if (LoadedJumpAction)
                    {
                        EnhancedInputComponent->BindAction(LoadedJumpAction, ETriggerEvent::Started, this, &AUnicornPlayerController::OnJumpPressed);
                        EnhancedInputComponent->BindAction(LoadedJumpAction, ETriggerEvent::Ongoing, this, &AUnicornPlayerController::OnJumpHeld);
                        EnhancedInputComponent->BindAction(LoadedJumpAction, ETriggerEvent::Completed, this, &AUnicornPlayerController::OnJumpReleased);
                    }
                    bInputInitialized = true;
                }
                else
                {
                    GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, TEXT("Failed to access EnhancedInputComponent!"));
                }
            }
            else
            {
                GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, TEXT("Failed to access EnhancedInputSubsystem"));
            }

            
        }
        else
        {
            GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, TEXT("Failed to access LocalPlayer"));
        }
    }
}

void AUnicornPlayerController::OnUnPossess()
{
    Super::OnUnPossess();
    Unicorn = nullptr;
}

void AUnicornPlayerController::OnMove(const FInputActionValue& Value)
{
    FVector2D MovementVector = Value.Get<FVector2D>();

    if (Unicorn.IsValid())
    {
        Unicorn->MoveRight(MovementVector.X);
    }
    else
    {
        GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Red, TEXT("No cached character for movement!"));
    }
}

void AUnicornPlayerController::OnJumpPressed()
{
    if (Unicorn.IsValid())
    {
        Unicorn->OnJumpPressed();
    }
    else
    {
        GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Red, TEXT("No Unicorn Character possessed!"));
    }
}

void AUnicornPlayerController::OnJumpHeld()
{
    if (Unicorn.IsValid())
    {
        Unicorn->OnJumpHeld();
    }
    else
    {
        GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Red, TEXT("No Unicorn Character possessed!"));
    }
}

void AUnicornPlayerController::OnJumpReleased()
{
    if (Unicorn.IsValid())
    {
        Unicorn->OnJumpReleased();
    }
    else
    {
        GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Red, TEXT("No Unicorn Character possessed!"));
    }
}
