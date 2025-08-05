// LeaveMeAlone Game by Netologiya. All RightsReserved


#include "LMAAIController.h"
#include "LMAEnemyCharacter.h"

void ALMAAIController::OnPossess(APawn* InPawn) {
	Super::OnPossess(InPawn);
	const auto AICharacter = Cast<ALMAEnemyCharacter>(InPawn);
	if (AICharacter)
	{
		RunBehaviorTree(AICharacter->BehaviorTreeAsset);
	}
}
