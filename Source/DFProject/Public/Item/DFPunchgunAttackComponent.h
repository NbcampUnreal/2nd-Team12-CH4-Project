#pragma once

#include "CoreMinimal.h"
#include "Item/DFItemAbilityComponent.h"
#include "DFPunchgunAttackComponent.generated.h"

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class DFPROJECT_API UDFPunchgunAttackComponent : public UDFItemAbilityComponent
{
	GENERATED_BODY()

public:
	UDFPunchgunAttackComponent();
	UFUNCTION(BlueprintCallable, Category = "Ability")
	void MainAction() override;

protected:
	virtual void BeginPlay() override;

	FTimerHandle SimulTimerHandle;
	
};
