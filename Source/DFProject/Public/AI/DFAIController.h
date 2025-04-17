#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "DFAIController.generated.h"

class UBlackboardData;
class UBehaviorTree;

UENUM(BlueprintType)
enum class EAI_AILevels : uint8
{
	Rookie UMETA(DisplayName = "Rookie"),
	Basic UMETA(DisplayName = "Basic"),
	Expert UMETA(DisplayName = "Expert")
};

UCLASS()
class DFPROJECT_API ADFAIController : public AAIController
{
	GENERATED_BODY()

public:
	ADFAIController();

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_SetAILevel(EAI_AILevels NewLevel);

	EAI_AILevels GetAILevel() const;

protected:
	void BeginAI(APawn* InPawn);
	void EndAI();

	//void SetAITeamID(int32 InTeamID);

private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UBlackboardData> BlackboardDataAsset;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UBehaviorTree> BehaviorTree;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI", Meta = (AllowPrivateAccess = "true"))
	EAI_AILevels CurrentAILevel;
};
