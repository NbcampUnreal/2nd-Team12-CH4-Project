#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "DFPlayerState.generated.h"

UCLASS()
class DFPROJECT_API ADFPlayerState : public APlayerState
{
	GENERATED_BODY()

public:
    ADFPlayerState();

    // 플레이어가 속한 팀 번호 (예: 0, 1, 등)
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated, Category = "Team")
    int32 TeamID;

    // 플레이어의 점수. 개인전 모드에서 개별 점수를 관리하기 위한 변수.
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated, Category = "Score")
    int32 IndividualScore;

    // 네트워크 복제를 위한 설정 함수
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
};
