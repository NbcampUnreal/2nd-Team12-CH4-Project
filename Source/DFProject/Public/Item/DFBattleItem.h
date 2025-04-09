#pragma once

#include "CoreMinimal.h"
#include "Item/DFBaseItem.h"
#include "DFItemAbilityComponent.h"
#include "DFBattleItem.generated.h"

class ADFItemBaseActor;

UCLASS()
class DFPROJECT_API UDFBattleItem : public UDFBaseItem
{
	GENERATED_BODY()
	

public:
	UDFBattleItem();

	UPROPERTY(EditAnywhere)
	TSubclassOf<ADFItemBaseActor> ItemActorClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
	TArray<TSubclassOf<UDFItemAbilityComponent>> Abilities;	
};
