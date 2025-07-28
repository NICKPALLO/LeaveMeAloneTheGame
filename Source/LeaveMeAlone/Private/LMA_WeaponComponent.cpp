// LeaveMeAlone Game by Netologiya. All RightsReserved


#include "LMA_WeaponComponent.h"
#include "Weapon/LMABaseWeapon.h"
#include "Player/LMADefaultCharacter.h"
#include "Animations/LMAReloadFinishedAnimNotify.h"
#include "Engine/World.h"


// Sets default values for this component's properties
ULMA_WeaponComponent::ULMA_WeaponComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

void ULMA_WeaponComponent::StartFire() {
	if (Weapon && !AnimReloading)
	{
		GetWorld()->GetTimerManager().SetTimer(ShotTimer, Weapon, &ALMABaseWeapon::Fire, 0.1f, true, 0.0f);
	}
}

void ULMA_WeaponComponent::StopFire() {
	GetWorld()->GetTimerManager().ClearTimer(ShotTimer);
}

void ULMA_WeaponComponent::SpawnWeapon() {
	Weapon = GetWorld()->SpawnActor<ALMABaseWeapon>(WeaponClass);
	if (Weapon)
	{
		const auto Character = Cast<ACharacter>(GetOwner());
		if (Character)
		{
			FAttachmentTransformRules AttachmentRules(EAttachmentRule::SnapToTarget, false);
			Weapon->AttachToComponent(Character->GetMesh(), AttachmentRules, "r_Weapon_Socket");
		}
	}
}


// Called when the game starts
void ULMA_WeaponComponent::BeginPlay()
{
	Super::BeginPlay();
	SpawnWeapon();
	InitAnimNotify();

	Weapon->OnReload.AddUObject(this, &ULMA_WeaponComponent::StopFire);
	Weapon->OnReload.AddUObject(this, &ULMA_WeaponComponent::Reload);

	// ...
	
}


// Called every frame
void ULMA_WeaponComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

bool ULMA_WeaponComponent::GetCurrentWeaponAmmo(FAmmoWeapon& AmmoWeapon) const
{
	if (Weapon)
	{
		AmmoWeapon = Weapon->GetCurrentAmmoWeapon();
		return true;
	}
	return false;
}

void ULMA_WeaponComponent::InitAnimNotify()
{
	if (!ReloadMontage)
	{
		return;
	}

	const auto NotifiesEvents = ReloadMontage->Notifies;
	for (auto NotifyEvent : NotifiesEvents)
	{
		auto ReloadFinish = Cast<ULMAReloadFinishedAnimNotify>(NotifyEvent.Notify);
		if (ReloadFinish)
		{
			ReloadFinish->OnNotifyReloadFinished.AddUObject(this, &ULMA_WeaponComponent::OnNotifyReloadFinished);
			break;
		}
	}
}

void ULMA_WeaponComponent::OnNotifyReloadFinished(USkeletalMeshComponent* SkeletalMesh) {
	const auto Character = Cast<ACharacter>(GetOwner());
	if (Character->GetMesh() == SkeletalMesh)
	{
		AnimReloading = false;
	}
}

bool ULMA_WeaponComponent::CanReload() const
{
	return (!AnimReloading && !Weapon->ClipIsFull());
}

void ULMA_WeaponComponent::Reload()
{
	if (!CanReload())
		{
			return;
		}
	Weapon->ChangeClip();
	AnimReloading = true;
	ACharacter* Character = Cast<ACharacter>(GetOwner());
	Character->PlayAnimMontage(ReloadMontage);
}