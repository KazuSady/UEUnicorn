// Copyright SomethingNotRandom. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "InputActionValue.h"

#include "UnicornPlayerController.generated.h"

class AUnicornCharacter;
class UInputAction;
class UInputMappingContext;

UCLASS()
class UNREALUNICORNOFLIGHT_API AUnicornPlayerController : public APlayerController
{
	GENERATED_BODY()

protected:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enhanced Input")
    TSoftObjectPtr<UInputMappingContext> InputMappingContext;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enhanced Input")
    TSoftObjectPtr<UInputAction> MoveAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enhanced Input")
    TSoftObjectPtr<UInputAction> JumpAction;

private:
    UPROPERTY(Transient)
    UInputMappingContext* LoadedMappingContext;

    UPROPERTY(Transient)
    UInputAction* LoadedMoveAction;

    UPROPERTY(Transient)
    UInputAction* LoadedJumpAction;

    UPROPERTY(Transient)
    TWeakObjectPtr<AUnicornCharacter> Unicorn;

private:
    bool bInputInitialized = false;

public:
    AUnicornPlayerController(){};

protected:
	virtual void BeginPlay() override;
    virtual void OnPossess(APawn* InPawn) override;
    virtual void OnUnPossess() override;

private:
    void OnMove(const FInputActionValue& Value);
    void OnJumpPressed();
    void OnJumpReleased();

};
