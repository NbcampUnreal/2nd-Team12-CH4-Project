#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DFItemAbilityComponent.generated.h"

class UDFBattleItem;
class ADFItemBaseActor;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DFPROJECT_API UDFItemAbilityComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UDFItemAbilityComponent();

	virtual void MainAction();

protected:
	virtual void BeginPlay() override;
	ADFItemBaseActor* ParentActor;
	UDFBattleItem* ParentItemData;
	USkeletalMeshComponent* ParentMesh;
	UAnimMontage* ParentActionAnim;
};
