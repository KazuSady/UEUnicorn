// Copyright SomethingNotRandom. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"

#include "UnicornCharacter.generated.h"

class UInputAction;
class UInputMappingContext;
class UNiagaraSystem;
class UNiagaraComponent;
class UPaperSprite;
class UPaperSpriteComponent;

UCLASS()
class UNREALUNICORNOFLIGHT_API AUnicornCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
	int32 Health = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
	int32 MaxHealth = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Controls")
	float UnicornAirControl = 0.8f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Controls")
	float UnicornJumpVelocity = 500.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Controls")
	float UnicornGroundFriction = 0.8f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Controls")
	float UnicornCoyoteTime = 0.15f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Controls")
	float UnicornJumpBuffer = 0.15f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Physics")
	float JumpHoldGravityScale = 0.2f; // Reduces gravity while holding jump

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Physics")
	float FallGravityScale = 2.0f; // Increases gravity when falling

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Physics")
	float MaxFallSpeed = 2000.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Jumps")
	float MinJumpHeight = 120.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Jumps")
	float JumpHeight = 240.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Jumps")
	float JumpHoldTimeForMaxHeight = 0.15f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Jumps")
	float JumpTime = 0.75f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Jumps")
	float DoubleJumpPotency = 0.9f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement|Jumps")
	float CurrentJumpHoldTime = 0.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement|Jumps")
	float JumpVelocity;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement|Jumps")
	float JumpGravity;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement|State")
	float TimeInAir = 0.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement|State")
	bool bIsJumpHeld = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement|State")
	bool bCanDoubleJump = true;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement|State")
	bool bIsDoubleJumpEnabled = true;

private:
	bool bWasOnGround = false;

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Effects|Movement")
	TObjectPtr<UNiagaraComponent> MovementTrailComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Effects|Movement")
	TObjectPtr<UNiagaraSystem> TrailParticleSystem;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Effects|Death")
	TObjectPtr<UNiagaraSystem> DeathParticleSystem;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Effects|Death")
	TObjectPtr<USoundBase> DeathSound;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Sprite", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UPaperSpriteComponent> SpriteComponent;

	TObjectPtr<UCharacterMovementComponent> CachedMovementComponent;

	float RemainingCoyoteTime = 0.0f;

	float RemainingJumpBufferTime = 0.0f;

	float TargetMaxVelocity = 0.0f;

	float InitialXPosition = 0.0f;

public:
	// Sets default values for this character's properties
	AUnicornCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	UFUNCTION(BlueprintCallable, Category = "Movement")
	void ToggleDoubleJump(bool bEnabled);

protected:
	UFUNCTION(BlueprintCallable, Category = "Movement")
	void CalculateJumpPhysics();

	UFUNCTION(BlueprintCallable, Category = "Movement")
	void ApplyCustomPhysics(float DeltaTime);

	UFUNCTION(BlueprintCallable, Category = "Movement")
	void PerformJump();

	UFUNCTION(BlueprintCallable, Category = "Movement")
	void PerformDoubleJump();

	UFUNCTION(BlueprintCallable, Category = "Health")
	void TakeDamage(int32 Damage = 1);

	UFUNCTION(BlueprintCallable, Category = "Health")
	void Respawn();

private:
	void FinishRespawn();

public:
	void MoveRight(const float Value);

	void OnJumpPressed();

	void OnJumpHeld();

	void OnJumpReleased();
};
