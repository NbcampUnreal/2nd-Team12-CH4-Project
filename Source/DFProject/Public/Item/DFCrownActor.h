#pragma once

#include "CoreMinimal.h"
#include "Item/DFItemBaseActor.h"
#include "DFCrownActor.generated.h"

class UDFWearableItem;

UCLASS()
class DFPROJECT_API ADFCrownActor : public ADFItemBaseActor
{
	GENERATED_BODY()
	
public:
	ADFCrownActor();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SpawnCrown")
	UDFWearableItem* CrownData;
};
