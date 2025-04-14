#pragma once

#include "CoreMinimal.h"
#include "Item/DFBaseItem.h"
#include "DFItemAbilityComponent.h"
#include "DFBattleItem.generated.h"

UCLASS()
class DFPROJECT_API UDFBattleItem : public UDFBaseItem
{
	GENERATED_BODY()
	

public:
	UDFBattleItem();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
	TArray<TSubclassOf<UDFItemAbilityComponent>> Abilities;	

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	USoundBase* ActionSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	UAnimMontage* ActionAnim;
};
