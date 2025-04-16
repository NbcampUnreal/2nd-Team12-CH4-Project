#pragma once

#include "CoreMinimal.h"
#include "Item/DFBaseItem.h"
#include "DFWearableItem.generated.h"


UCLASS()
class DFPROJECT_API UDFWearableItem : public UDFBaseItem
{
	GENERATED_BODY()
	
public:
	UDFWearableItem();

	UPROPERTY(EditAnywhere, Category = "Item")
	bool bIsUnlocked = false;
};
