// LeaveMeAlone Game by Netologiya. All RightsReserved


#include "Weapon/LMABaseWeapon.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"

// Sets default values
ALMABaseWeapon::ALMABaseWeapon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	WeaponComponent = CreateDefaultSubobject<USkeletalMeshComponent>("Weapon");
	SetRootComponent(WeaponComponent);
}

// Called when the game starts or when spawned
void ALMABaseWeapon::BeginPlay()
{
	Super::BeginPlay();
	CurrentAmmoWeapon = AmmoWeapon;
}

void ALMABaseWeapon::Fire() {
	Shoot();
	DecrementBullets();
}

void ALMABaseWeapon::Shoot() {
	const FTransform SocketTransform = WeaponComponent->GetSocketTransform("Muzzle");
	const FVector TraceStart = SocketTransform.GetLocation();
	const FVector ShootDirection = SocketTransform.GetRotation().GetForwardVector();
	const FVector TraceEnd = TraceStart + ShootDirection * TraceDistance;
	//DrawDebugLine(GetWorld(), TraceStart, TraceEnd, FColor::Black, false, 1.0f, 0, 2.0f);
	SpawnTrace(TraceStart, TraceEnd);
	UGameplayStatics::PlaySoundAtLocation(GetWorld(), ShootWave, TraceStart);

	FHitResult HitResult;
	GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, ECollisionChannel::ECC_Visibility);
	//if (HitResult.bBlockingHit)
	//{
	//	DrawDebugSphere(GetWorld(), HitResult.ImpactPoint, 5.0f, 24, FColor::Red, false, 1.0f);
	//}
}

// Called every frame
void ALMABaseWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

bool ALMABaseWeapon::ClipIsFull() const
{
	return CurrentAmmoWeapon.Bullets == AmmoWeapon.Bullets;
}

FAmmoWeapon ALMABaseWeapon::GetCurrentAmmoWeapon() const
{
	return CurrentAmmoWeapon;
}

void ALMABaseWeapon::DecrementBullets() {
	--CurrentAmmoWeapon.Bullets;
	UE_LOG(LogWeapon, Display, TEXT("Bullets = %s"), *FString::FromInt(CurrentAmmoWeapon.Bullets));
	if (IsCurrentClipEmpty())
	{
		OnReload.Broadcast();
	}
}

bool ALMABaseWeapon::IsCurrentClipEmpty() const
{
	return CurrentAmmoWeapon.Bullets == 0;
}

void ALMABaseWeapon::ChangeClip() {
	CurrentAmmoWeapon.Bullets = AmmoWeapon.Bullets;
}

 void ALMABaseWeapon::SpawnTrace(const FVector& TraceStart, const FVector& TraceEnd)
{
	const auto TraceFX = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), TraceEffect, TraceStart);
	if (TraceFX)
	{
		TraceFX->SetNiagaraVariableVec3(TraceName, TraceEnd);
	}
}