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

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

protected:
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FName SelectedAttackTypeKey;

private:
	int32 CurrentPunchCount;
	float PunchInterval;
	float ElapsedTime;
	bool bIsPunching;
};