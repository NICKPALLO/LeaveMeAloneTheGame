// LeaveMeAlone Game by Netologiya. All RightsReserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "LMAPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class LEAVEMEALONE_API ALMAPlayerController : public APlayerController
{
	GENERATED_BODY()
	
protected:
	virtual void BeginPlay() override;
	virtual void BeginSpectatingState() override;
};
