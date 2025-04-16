#pragma once

#include "CoreMinimal.h"
#include "Server/DFBattleGameMode.h"
#include "DFTimeOutGameMode.generated.h"

UCLASS()
class DFPROJECT_API ADFTimeOutGameMode : public ADFBattleGameMode
{
	GENERATED_BODY()

public:
	ADFTimeOutGameMode();

    // 게임 시작 시 초기화
    virtual void BeginPlay() override;

    // 매 프레임 호출되는 Tick 함수에서 타임아웃 여부 체크
    virtual void Tick(float DeltaSeconds) override;

    virtual void EndGame() override;

    virtual void HandlePlayerOutOfBounds(APawn* Pawn) override;

    // Timeout 관련 변수
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Timeout")
    float TimeoutDuration;  // 예: 180초

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Timeout")
    float RespawnDelay;     // 예: 5초

    UFUNCTION(BlueprintCallable, Category = "Timeout")
    bool HasTimedOut() const;

    // Battle 모드 선택: 팀전/자유전
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "GameMode")
    EBattleModeType BattleMode;

    /////////////////////////////////////////////////////////
    // 팀전 모드 관련 변수
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Team")
    int32 Team1Score;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Team")
    int32 Team2Score;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Team")
    float TeamScoreUpdateInterval;

    /////////////////////////////////////////////////////////
    // 자유전 모드 관련 변수
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "FreeMode")
    float FreeScoreUpdateInterval;

    /////////////////////////////////////////////////////////
    // Sudden Death 모드 관련 변수
    // bSuddenDeath가 true이면 추가 시간이 무제한으로 주어지며, 이후 점수 차이가 발생하면 승리 결정
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GameMode")
    bool bSuddenDeath;


private:
    FTimerHandle TimeoutHandle;
    FTimerHandle TeamScoreTimerHandle;
    FTimerHandle FreeScoreTimerHandle;

    // Timeout 발생 시 호출되는 함수
    void OnTimeout();

    // 팀전 모드 관련 함수
    void UpdateTeamScores();
    void CheckTeamWinningCondition();

    // 자유전 모드 관련 함수
    void UpdateCrownScores();
    void CheckFreeWinningCondition();

    // 리스폰 함수
    void RespawnPlayer(AController* Controller, APawn* PawnToRespawn);


};
