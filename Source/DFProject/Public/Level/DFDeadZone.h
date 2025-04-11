#pragma once

#include "CoreMinimal.h"
#include "Engine/TriggerVolume.h"
#include "DFDeadZone.generated.h"

UCLASS()
class DFPROJECT_API ADFDeadZone : public ATriggerVolume
{
	GENERATED_BODY()

public:
	ADFDeadZone();

protected:
	virtual void BeginPlay() override;

	// 오버랩 시작 이벤트 (Actor가 데드존에 들어올 때 호출)
	UFUNCTION()
	void OnOverlapBegin(AActor* OverlappedActor, AActor* OtherActor);
};
