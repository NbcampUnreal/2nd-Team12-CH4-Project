#pragma once

#include "CoreMinimal.h"
#include "Item/DFItemAbilityComponent.h"
#include "DFBatAttackComponent.generated.h"

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class DFPROJECT_API UDFBatAttackComponent : public UDFItemAbilityComponent
{
	GENERATED_BODY()
	
public:
	UDFBatAttackComponent();
};
