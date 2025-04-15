#pragma once

#include "CoreMinimal.h"
#include "Server/DFBattleGameMode.h"
#include "DFScrambleBattleGameMode.generated.h"


// 전방 선언: ADFCrownActor 클래스는 맵에 스폰되는 왕관 액터
class ADFCrownActor;

UCLASS()
class DFPROJECT_API ADFScrambleBattleGameMode : public ADFBattleGameMode
{
    GENERATED_BODY()

public:
    ADFScrambleBattleGameMode();

    // 어떤 모드를 사용할지 결정 (기본값은 팀전)
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "BattleMode")
    EBattleModeType BattleMode;

    //////////////////////////////////////////////////////////
    // 팀전 모드 관련 변수
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "TeamMode")
    int32 Team1Score;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "TeamMode")
    int32 Team2Score;

    // 승리 조건: 팀별 점수가 도달해야 할 점수
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "TeamMode")
    int32 TeamScoreToWin;

    // 팀 점수 업데이트 간격 (초)
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "TeamMode")
    float TeamScoreUpdateInterval;

    //////////////////////////////////////////////////////////
    // 개인전 (자유전) 모드 관련 변수
    // 왕관 액터 클래스를 지정 (맵에 스폰할 액터)
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "FreeMode")
    TSubclassOf<ADFCrownActor> CrownClass;

    // 스폰된 왕관 액터 목록
    UPROPERTY()
    TArray<ADFCrownActor*> SpawnedCrowns;

    // 승리 조건: 개인(플레이어) 점수가 도달해야 할 점수
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "FreeMode")
    int32 FreeScoreToWin;

    // 개인전 점수 업데이트 간격 (초)
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "FreeMode")
    float FreeScoreUpdateInterval;

protected:
    virtual void BeginPlay() override;

    virtual void Tick(float DeltaSeconds) override;

    virtual void EndGame() override;

private:
    // 타이머 핸들들
    FTimerHandle TeamScoreTimerHandle;
    FTimerHandle FreeScoreTimerHandle;

    // 팀전 모드용: 모든 컨트롤러를 순회하며 ADFPlayerState의 TeamID에 따라 팀 점수를 업데이트
    void UpdateTeamScores();

    // 팀전 모드용: 승리 조건을 체크
    void CheckTeamWinningCondition();

    // 개인전 모드용: 맵에 스폰된 왕관 액터를 순회하며, 왕관 소지 중인 경우 해당 플레이어의 점수를 업데이트
    void UpdateCrownScores();

    // 개인전 모드용: 승리 조건을 체크 (각 플레이어의 점수를 확인)
    void CheckFreeWinningCondition();

};
