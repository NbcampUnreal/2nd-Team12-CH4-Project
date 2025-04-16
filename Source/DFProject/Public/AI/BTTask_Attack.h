#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "Character/DFCharacter.h"
#include "BTTask_Attack.generated.h"


UCLASS()
class DFPROJECT_API UBTTask_Attack : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_Attack();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

private:
	FName AILevelKey;

	FTimerHandle PunchTimerHandle;

	void HandlePunch(ADFCharacter* MyCharacter);
	void HandleHeadbutt(ADFCharacter* MyCharacter);
};