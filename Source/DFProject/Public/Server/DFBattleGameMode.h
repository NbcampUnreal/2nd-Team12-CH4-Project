#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "DFBattleGameMode.generated.h"

/**
 * 게임 진행 상태를 나타내는 Enum
 * Waiting: 모든 플레이어가 준비되기 전 (또는 초기화 상태)
 * InProgress: 게임 진행 중
 * SuddenDeath: 시간 종료 후 동점일 때 서든데스 모드
 * Ended: 게임 종료 상태
 */

UENUM(BlueprintType)
enum class EBattleGameState : uint8
{
    Waiting     UMETA(DisplayName = "Waiting"),
    InProgress  UMETA(DisplayName = "InProgress"),
    SuddenDeath UMETA(DisplayName = "SuddenDeath"),
    Ended       UMETA(DisplayName = "Ended")
};

// 게임 상태 변경 시 발생시킬 이벤트 (블루프린트와 연동 가능)
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGameModeStateChangedSignature, EBattleGameState, NewState);

UCLASS()
class DFPROJECT_API ADFBattleGameMode : public AGameModeBase
{
    GENERATED_BODY()

public:
    ADFBattleGameMode();

    virtual void BeginPlay() override;
    virtual void Tick(float DeltaSeconds) override;

    // 라운드 전체 시간을 초 단위로 설정 (기본값 300초; 예: 5분)
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Game Settings")
    float RoundTime;

    // 현재 남은 시간을 나타냄
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Game Settings")
    float CurrentTime;

    // 현재 게임 진행 상태 (서버에서만 관리됨)
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Game Settings")
    EBattleGameState CurrentGameState;

    // 게임 상태 변경 이벤트 (블루프린트에서 처리할 수 있음)
    UPROPERTY(BlueprintAssignable, Category = "Events")
    FOnGameModeStateChangedSignature OnGameStateChanged;

    // 예제용: 게임 내 점수를 관리할 변수 (실제 구현시 플레이어별 혹은 팀별로 관리)
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Game Settings")
    int32 Score;

    // 1초마다 호출되어 게임의 진행 시간을 업데이트하고 상태를 체크함
    UFUNCTION(BlueprintCallable, Category = "GameMode")
    virtual void UpdateGameState();

    // 서든데스 모드로 전환 시 호출되는 함수 (특수 룰, 효과, UI 업데이트 등 추가)
    UFUNCTION(BlueprintCallable, Category = "GameMode")
    virtual void HandleSuddenDeath();

    // 게임 종료 시 호출되는 함수 (승리자 결정, 결과 전달 등)
    UFUNCTION(BlueprintCallable, Category = "GameMode")
    virtual void EndGame();

protected:
    // 타이머 핸들러: 일정 주기마다 UpdateGameState 호출
    FTimerHandle GameStateTimerHandle;

    // 동점 여부를 판단하는 예제 함수 (실제 점수/플레이어 정보를 이용하여 구현)
    virtual bool IsTie() const;

    // 안전하게 게임 상태를 변경하는 내부 함수 (변경 시 이벤트도 발생)
    virtual void SetGameState(EBattleGameState NewState);
};