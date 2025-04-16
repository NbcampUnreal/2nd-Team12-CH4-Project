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

	void OnGripAreaBeginOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult) override;

	void DestroySelf();
};
