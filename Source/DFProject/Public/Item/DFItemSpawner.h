#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DFItemSpawner.generated.h"

class UBoxComponent;
class ADFItemBaseActor;
class UDFBattleItem;

UCLASS()
class DFPROJECT_API ADFItemSpawner : public AActor
{
	GENERATED_BODY()
	
public:	
	ADFItemSpawner();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USceneComponent* Scene;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UBoxComponent* SpawnArea;

	UFUNCTION(BlueprintCallable)
	void SpawnItem();
	UFUNCTION(BlueprintCallable)
	FTransform SetSpawnTransform();

protected:
	virtual void BeginPlay() override;

};
