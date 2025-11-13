// Copyright SomethingNotRandom. All Rights Reserved.

#include "UnicornCharacter.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputAction.h"
#include "InputMappingContext.h"

// Sets default values
AUnicornCharacter::AUnicornCharacter()
{
 	// Set this character to call Tick() every frame.  
	// You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CachedMovementComponent = GetCharacterMovement();
	if (CachedMovementComponent)
	{
		CachedMovementComponent->bOrientRotationToMovement = false;
		CachedMovementComponent->GravityScale = 1.f;
		CachedMovementComponent->JumpZVelocity = 0.f; // Disables built-in jumps
		CachedMovementComponent->AirControl = UnicornAirControl;
		CachedMovementComponent->GroundFriction = UnicornGroundFriction;
	}
}

// Called when the game starts or when spawned
void AUnicornCharacter::BeginPlay()
{
	Super::BeginPlay();
	CalculateJumpPhysics();
}

// Called every frame
void AUnicornCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	check(CachedMovementComponent);
	if (CachedMovementComponent && CachedMovementComponent->IsFalling())
	{
		TimeInAir += DeltaTime;
	}
	else
	{
		TimeInAir = 0.0f;
		bCanDoubleJump = bIsDoubleJumpEnabled;
	}

	ApplyCustomPhysics(DeltaTime);
	if (CachedMovementComponent)
	{
		bool bIsOnGround = CachedMovementComponent->IsMovingOnGround();
		if (bIsOnGround)
		{
			RemainingCoyoteTime = UnicornCoyoteTime;
		}
		else
		{
			RemainingCoyoteTime -= DeltaTime;
		}

		RemainingCoyoteTime = FMath::Max(0.f, RemainingCoyoteTime);

		if (RemainingJumpBufferTime > 0.0f)
		{
			RemainingJumpBufferTime -= DeltaTime;
			if (RemainingCoyoteTime > 0.0f)
			{
				PerformJump();
				RemainingJumpBufferTime = 0.0f;
			}
		}

		bWasOnGround = bIsOnGround;
	}
}

void AUnicornCharacter::ToggleDoubleJump(bool bEnabled)
{
	bIsDoubleJumpEnabled = bEnabled;
	bCanDoubleJump = bEnabled;
}

void AUnicornCharacter::CalculateJumpPhysics()
{
	JumpGravity = (2.f * JumpHeight) / (JumpTime * JumpTime);
	JumpVelocity = FMath::Sqrt(2.f * JumpGravity * JumpHeight);

	GEngine->AddOnScreenDebugMessage(
		-1, 5.0f, FColor::Yellow,
		FString::Printf(TEXT("Jump Velocity: %.2f, Gravity: %.2f"),	JumpVelocity, JumpGravity)
	);
}

void AUnicornCharacter::ApplyCustomPhysics(float DeltaTime)
{
	check(CachedMovementComponent);
	float CurrentGravity = JumpGravity;

	if (CachedMovementComponent->IsFalling())
	{
		if (bIsJumpHeld && CachedMovementComponent->Velocity.Z > 0.0f)
		{
			// Reduced gravity while holding jump button
			CurrentGravity *= JumpHoldGravityScale;
		}
		else if (CachedMovementComponent->Velocity.Z <= 0.0f)
		{
			// Increased gravity when falling
			CurrentGravity *= FallGravityScale;
		}

		// Apply gravity manually
		FVector NewVelocity = CachedMovementComponent->Velocity;
		NewVelocity.Z -= CurrentGravity * DeltaTime;

		// Clamp fall speed
		NewVelocity.Z = FMath::Max(NewVelocity.Z, -MaxFallSpeed);

		CachedMovementComponent->Velocity = NewVelocity;
	}
}

void AUnicornCharacter::PerformJump()
{
	if (CachedMovementComponent) 
	{
		CachedMovementComponent->Velocity.Z = JumpVelocity;
		CachedMovementComponent->SetMovementMode(MOVE_Falling);
	}
}

void AUnicornCharacter::PerformDoubleJump()
{
	if (bCanDoubleJump && TimeInAir > 0.1f) // Small delay to prevent accidental double jumps
	{
		if (CachedMovementComponent)
		{
			CachedMovementComponent->Velocity.Z = JumpVelocity * 0.9f; // Double jump is slightly weaker
		}
		bCanDoubleJump = false;
	}
}

void AUnicornCharacter::MoveRight(const float Value)
{
	if (Value != 0.0f)
	{
		AddMovementInput(FVector(0.f, 1.f, 0.f), Value);
	}
}

void AUnicornCharacter::OnJumpPressed()
{
	bIsJumpHeld = true;

	if (CachedMovementComponent)
	{
		if (CachedMovementComponent->IsMovingOnGround() || RemainingCoyoteTime > 0.0f)
		{
			PerformJump();
		}
		else if (bCanDoubleJump)
		{
			PerformDoubleJump();
		}
		else
		{
			RemainingJumpBufferTime = UnicornJumpBuffer;
		}
	}
}

void AUnicornCharacter::OnJumpReleased()
{
	bIsJumpHeld = false;
}
