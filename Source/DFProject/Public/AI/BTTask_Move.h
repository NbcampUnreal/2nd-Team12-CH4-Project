#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_MoveTo.h"
#include "BTTask_Move.generated.h"


UCLASS()
class DFPROJECT_API UBTTask_Move : public UBTTask_MoveTo
{
	GENERATED_BODY()

public:
	UBTTask_Move();

protected:

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	bool ShouldJump(class ADFCharacter* MyCharacter) const;

private:
	bool bWasFalling;
};
