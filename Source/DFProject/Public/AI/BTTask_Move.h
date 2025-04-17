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


private:
	void AdjustSpeed(class ADFCharacter* MyCharacter, class ADFAIController* AIController, float Distance);

	UPROPERTY(EditAnywhere, Category = "AI")
	float AttackRange;

	UPROPERTY(EditAnywhere, Category = "AI")
	float DefaultSpeed;

	UPROPERTY(EditDefaultsOnly, Category = "Blackboard")
	FName MoveLocationKey;

};
