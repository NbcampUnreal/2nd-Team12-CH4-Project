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

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "ItemSpawner")
	USceneComponent* Scene;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemSpawner")
	UBoxComponent* SpawnArea;

	UFUNCTION(BlueprintCallable, Category = "ItemSpawner")
	void SpawnItem();
	UFUNCTION(BlueprintCallable, Category = "ItemSpawner")
	FTransform SetSpawnTransform();
	UFUNCTION(BlueprintCallable, Category = "ItemSpawner")
	int32 GetSpawnCount() const;

protected:
	//virtual void BeginPlay() override;
	bool bIsValidArea;
	int32 SpawnCount;
};
