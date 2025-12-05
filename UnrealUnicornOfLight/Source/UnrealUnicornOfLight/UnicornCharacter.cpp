// Copyright SomethingNotRandom. All Rights Reserved.

#include "UnicornCharacter.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputAction.h"
#include "InputMappingContext.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "PaperSprite.h"
#include "PaperSpriteComponent.h"

#include <Kismet/GameplayStatics.h>

// Sets default values
AUnicornCharacter::AUnicornCharacter()
{
	// Set this character to call Tick() every frame.  
	// You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MovementTrailComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("MovementTrailComponent"));
	MovementTrailComponent->SetupAttachment(RootComponent);
	MovementTrailComponent->SetRelativeLocation(FVector::ZeroVector);
	MovementTrailComponent->SetAutoActivate(true);

	SpriteComponent = CreateDefaultSubobject<UPaperSpriteComponent>(TEXT("SpriteComponent"));
	SpriteComponent->SetupAttachment(RootComponent);
	SpriteComponent->SetRelativeLocation(FVector(290.0f, 0.0f, -260.0f));
	SpriteComponent->SetRelativeRotation(FRotator(0.0f, 0.0f, 0.0f));
	SpriteComponent->SetRelativeScale3D(FVector(0.05f, 0.05f, 0.05f));

	CachedMovementComponent = GetCharacterMovement();
	if (CachedMovementComponent)
	{
		CachedMovementComponent->bOrientRotationToMovement = false;
		CachedMovementComponent->bUseControllerDesiredRotation = false;
		CachedMovementComponent->GravityScale = 1.f;
		CachedMovementComponent->JumpZVelocity = 0.f; // Disables built-in jumps
		CachedMovementComponent->AirControl = UnicornAirControl;
		CachedMovementComponent->GroundFriction = UnicornGroundFriction;

		// Prevent movement along X axis
		//CachedMovementComponent->bConstrainToPlane = true;
		//CachedMovementComponent->SetPlaneConstraintNormal(FVector(1.0f, 0.0f, 0.0f));
		//CachedMovementComponent->SetPlaneConstraintOrigin(GetActorLocation());
	}

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;
}

// Called when the game starts or when spawned
void AUnicornCharacter::BeginPlay()
{
	Super::BeginPlay();
	CalculateJumpPhysics();

	InitialXPosition = GetActorLocation().X;
	
	if (MovementTrailComponent && TrailParticleSystem)
	{
		MovementTrailComponent->SetAsset(TrailParticleSystem);
	}
}

// Called every frame
void AUnicornCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Prevent movement along X axis
	FVector CurrentLocation = GetActorLocation();
	CurrentLocation.X = InitialXPosition;
	SetActorLocation(CurrentLocation);

	check(CachedMovementComponent);
	if (bIsJumpHeld)
	{
		CurrentJumpHoldTime += DeltaTime;
	}

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
		FString::Printf(TEXT("Calculated Physics - Jump Velocity: %.2f, Jump Gravity: %.2f"), JumpVelocity, JumpGravity)
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
			CachedMovementComponent->Velocity.Z = JumpVelocity * DoubleJumpPotency;  // Double jump is slightly weaker
		}
		bCanDoubleJump = false;
		CurrentJumpHoldTime = 0.0f;
	}
}

void AUnicornCharacter::TakeDamage(int32 Damage)
{
	Health -= Damage;

	if (Health <= 0)
	{
		Health = 0;
		Respawn();
	}
}

void AUnicornCharacter::Respawn()
{
	SetActorEnableCollision(false);
	SetActorHiddenInGame(true);
	DisableInput(nullptr);

	if (CachedMovementComponent)
	{
		CachedMovementComponent->Velocity = FVector::ZeroVector;
		CachedMovementComponent->StopMovementImmediately();
		CachedMovementComponent->DisableMovement();
		CachedMovementComponent->ClearAccumulatedForces();
	}

	if (DeathSound)
	{
		UGameplayStatics::PlaySoundAtLocation(
			this,
			DeathSound,
			GetActorLocation()
		);
	}

	if (MovementTrailComponent)
	{
		MovementTrailComponent->Deactivate();
	}

	if (DeathParticleSystem)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			GetWorld(),
			DeathParticleSystem,
			GetActorLocation(),
			FRotator::ZeroRotator,
			FVector(1.0f)
		);
	}

	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &AUnicornCharacter::FinishRespawn, 1.0f, false);
}

void AUnicornCharacter::FinishRespawn()
{
	Health = MaxHealth;

	// Reset to spawn point
	SetActorLocation(FVector(-140, -20, 40));

	if (CachedMovementComponent)
	{
		CachedMovementComponent->Velocity = FVector::ZeroVector;
		CachedMovementComponent->ClearAccumulatedForces();
		CachedMovementComponent->SetMovementMode(MOVE_Walking);
	}

	if (MovementTrailComponent)
	{
		MovementTrailComponent->ResetSystem();
		MovementTrailComponent->Activate();
	}

	SetActorEnableCollision(true);
	SetActorHiddenInGame(false);
	EnableInput(nullptr); 
}

void AUnicornCharacter::MoveRight(const float Value)
{
	if (Value != 0.0f)
	{
		AddMovementInput(FVector(0.f, 1.f, 0.f), Value);

		FRotator Rot = GetActorRotation();
		Rot.Yaw = Value > 0 ? -90.0f : 90.f;
		SetActorRotation(Rot);
	}
}

void AUnicornCharacter::OnJumpPressed()
{
	bIsJumpHeld = true;
	CurrentJumpHoldTime = 0.0f;

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

void AUnicornCharacter::OnJumpHeld()
{
	bIsJumpHeld = true;
}

void AUnicornCharacter::OnJumpReleased()
{
	bIsJumpHeld = false;
}
