#pragma once

#include "CoreMinimal.h"
#include "Components/StaticMeshComponent.h"
#include "DFDeadZoneComponent.generated.h"

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class DFPROJECT_API UDFDeadZoneComponent : public UStaticMeshComponent
{
	GENERATED_BODY()
	
public:
	UDFDeadZoneComponent();

protected:
    virtual void BeginPlay() override;

    // 오버랩 이벤트 핸들러 (UBoxComponent의 OnComponentBeginOverlap과 연결됨)
    UFUNCTION()
    void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
        bool bFromSweep, const FHitResult& SweepResult);
};
