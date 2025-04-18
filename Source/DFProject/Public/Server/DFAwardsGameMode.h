#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "DFAwardsGameMode.generated.h"


UCLASS()
class DFPROJECT_API ADFAwardsGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;

	/** Tag 이름으로 TargetPoint 찾아주는 헬퍼 */
	AActor* FindRankPoint(const FName& Tag) const;
};
