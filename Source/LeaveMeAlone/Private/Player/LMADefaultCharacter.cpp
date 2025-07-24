// LeaveMeAlone Game by Netologiya. All RightsReserved

#include "Player/LMADefaultCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/DecalComponent.h"
#include "Components/InputComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "LMAHealthComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "TimerManager.h"
#include "Engine/Engine.h"
#include "LMA_WeaponComponent.h"

// Sets default values
ALMADefaultCharacter::ALMADefaultCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>("SpringArm");
	SpringArmComponent->SetupAttachment(GetRootComponent());
	CameraComponent = CreateDefaultSubobject<UCameraComponent>("CameraComponent");
	CameraComponent->SetupAttachment(SpringArmComponent);

	SpringArmComponent->SetUsingAbsoluteRotation(true);
	SpringArmComponent->TargetArmLength = ArmLength;
	SpringArmComponent->SetRelativeRotation(FRotator(YRotation, 0.0f, 0.0f));
	SpringArmComponent->bDoCollisionTest = false;
	SpringArmComponent->bEnableCameraLag = true;
	CameraComponent->SetFieldOfView(FOV);
	CameraComponent->bUsePawnControlRotation = false;
	
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	HealthComponent = CreateDefaultSubobject<ULMAHealthComponent>("HealthComponent");

	Stamina = MaxStamina;

	//HealthComponent->OnDeath.AddUObject(this, &ALMADefaultCharacter::OnDeath);
	WeaponComponent = CreateDefaultSubobject<ULMA_WeaponComponent>("Weapon");
}

// Called when the game starts or when spawned
void ALMADefaultCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	if (CursorMaterial)
	{
		CurrentCursor = UGameplayStatics::SpawnDecalAtLocation(GetWorld(), CursorMaterial, CursorSize, FVector(0));
	}

	HealthComponent->OnDeath.AddUObject(this, &ALMADefaultCharacter::OnDeath);

	OnHealthChanged(HealthComponent->GetHealth());
	HealthComponent->OnHealthChanged.AddUObject(this, &ALMADefaultCharacter::OnHealthChanged);


}

bool ALMADefaultCharacter::getSprintStatus()
{
	return SprintState;
}

// Called every frame
void ALMADefaultCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (!(HealthComponent->IsDead()))
	{
		RotationPlayerOnCursor();
	}
	FString text1 = TEXT("Stamina: ")+ FString::FromInt(Stamina);
	//FString text2 = TEXT("Health: ") + FString::SanitizeFloat(this->HealthComponent->GetHealth());
	GEngine->AddOnScreenDebugMessage(-1, DeltaTime, FColor::Black, text1, true, FVector2D(2.0f, 2.0f));
	//GEngine->AddOnScreenDebugMessage(-1, DeltaTime, FColor::Black, text2, false, FVector2D(2.0f, 2.0f));
}

// Called to bind functionality to input
void ALMADefaultCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PlayerInputComponent->BindAxis("MoveForward", this, &ALMADefaultCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ALMADefaultCharacter::MoveRight);
	PlayerInputComponent->BindAxis("ChangeSpringArm", this, &ALMADefaultCharacter::ChangeSpringArm);
	PlayerInputComponent->BindAction("Sprint", EInputEvent::IE_Pressed, this, &ALMADefaultCharacter::SprintEnable);
	PlayerInputComponent->BindAction("Sprint", EInputEvent::IE_Released, this, &ALMADefaultCharacter::SprintDisable);
	PlayerInputComponent->BindAction("Fire", IE_Pressed, WeaponComponent, &ULMA_WeaponComponent::Fire);
	PlayerInputComponent->BindAction("Reload", IE_Pressed, WeaponComponent, &ULMA_WeaponComponent::Reload);
}

void ALMADefaultCharacter::OnHealthChanged(float NewHealth) {
	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, FString::Printf(TEXT("Health = %f"), NewHealth));
}



void ALMADefaultCharacter::SprintEnable()
{
	SprintState = true;
	GetWorldTimerManager().ClearTimer(StaminaIncreaseTimer);
	GetWorldTimerManager().SetTimer(StaminaDecreaseTimer, this, &ALMADefaultCharacter::DecreaseStamina, 1.0f, true);
}
void ALMADefaultCharacter::SprintDisable()
{
	SprintState = false;
	GetWorldTimerManager().ClearTimer(StaminaDecreaseTimer);
	GetWorldTimerManager().SetTimer(StaminaIncreaseTimer, this, &ALMADefaultCharacter::IncreaseStamina, 1.0f, true);
}

void ALMADefaultCharacter::DecreaseStamina()
{
	if (Stamina > 0)
	{
		--Stamina;
	}
	else
	{
		SprintState = false;
		GetWorldTimerManager().ClearTimer(StaminaDecreaseTimer);
		GetWorldTimerManager().SetTimer(StaminaIncreaseTimer, this, &ALMADefaultCharacter::IncreaseStamina, 1.0f, true);
	}
};
void ALMADefaultCharacter::IncreaseStamina()
{
	if (Stamina < MaxStamina)
	{
		++Stamina;
	}
	else
	{
		GetWorldTimerManager().ClearTimer(StaminaIncreaseTimer);
	}
}

void ALMADefaultCharacter::OnDeath() {
	CurrentCursor->DestroyRenderState_Concurrent();
	PlayAnimMontage(DeathMontage);
	GetCharacterMovement()->DisableMovement();
	SetLifeSpan(5.0f);
	if (Controller)
	{
		Controller->ChangeState(NAME_Spectating);
	}
}


void ALMADefaultCharacter::RotationPlayerOnCursor() {
	APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (PC)
	{
		FHitResult ResultHit;
		PC->GetHitResultUnderCursor(ECC_GameTraceChannel1, true, ResultHit);
		float FindRotatorResultYaw = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), ResultHit.Location).Yaw;
		SetActorRotation(FQuat(FRotator(0.0f, FindRotatorResultYaw, 0.0f)));
		if (CurrentCursor)
		{
			CurrentCursor->SetWorldLocation(ResultHit.Location);
		}
	}
};


void ALMADefaultCharacter::MoveForward(float Value)
{	

	if (!(SprintState && Value > 0 && Stamina>0))
	{
		Value /= 2;
	}
	AddMovementInput(GetActorForwardVector(), Value);
}

void ALMADefaultCharacter::MoveRight(float Value)
{
	if (Value != 0)
	{
		Value /= 2;
		SprintState = false;
	}
	AddMovementInput(GetActorRightVector(), Value);
}

void ALMADefaultCharacter::ChangeSpringArm(float Value)
{
	if ((Value < 0 && ArmLength < MaxArmLength) || (Value > 0 && ArmLength > MinArmLength))
	{
		ArmLength -= Value * 10;
		SpringArmComponent->TargetArmLength = ArmLength;
	}
}

bool ALMADefaultCharacter::getStaminaAvailable()
{
	if (Stamina>0)
	{
		return true;
	}
	return false;
}