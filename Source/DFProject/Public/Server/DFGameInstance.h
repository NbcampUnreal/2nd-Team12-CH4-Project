#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "DFGameInstance.generated.h"


UCLASS()
class DFPROJECT_API UDFGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
    UDFGameInstance();

	// 1~3위 이름/시간을 임시 저장
	FString PlaceNames[3];
	float   PlaceTimes[3];


private:
	
};
