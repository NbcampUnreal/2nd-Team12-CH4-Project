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

	UPROPERTY()
	ADFItemBaseActor* ParentActor;

	UPROPERTY()
	UDFBattleItem* ParentItemData;

	UPROPERTY()
	USkeletalMeshComponent* ParentMesh;

	UPROPERTY()
	UAnimMontage* ParentActionAnim;

	UPROPERTY()
	USoundBase* ParentActionsound;

	virtual void MainAction();

	virtual void PlayActionSound();

	UFUNCTION(Server, Reliable)
	void Server_MainAction();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	virtual void BeginPlay() override;

};
