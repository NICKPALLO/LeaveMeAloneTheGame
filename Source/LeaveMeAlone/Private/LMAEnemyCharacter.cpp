// LeaveMeAlone Game by Netologiya. All RightsReserved


#include "LMAEnemyCharacter.h"
#include "LMAHealthComponent.h"

// Sets default values
ALMAEnemyCharacter::ALMAEnemyCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	HealthComponent = CreateDefaultSubobject<ULMAHealthComponent>("HealthComponent");

}

// Called when the game starts or when spawned
void ALMAEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ALMAEnemyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
//void ALMAEnemyCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
//{
//	Super::SetupPlayerInputComponent(PlayerInputComponent);
//
//}

