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

// 팀전, 개인전(자유전) 모드를 구분하는 열거형
UENUM(BlueprintType)
enum class EBattleModeType : uint8
{
    TeamBased UMETA(DisplayName = "Team Based"),
    FreeForAll UMETA(DisplayName = "Free For All")
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

    // 현재 게임 진행 상태 (서버에서만 관리됨)
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Game Settings")
    EBattleGameState CurrentGameState;

    // 게임 상태 변경 이벤트 (블루프린트에서 처리할 수 있음)
    UPROPERTY(BlueprintAssignable, Category = "Events")
    FOnGameModeStateChangedSignature OnGameStateChanged;

    // 게임 종료 시 호출되는 함수 (승리자 결정, 결과 전달 등)
    UFUNCTION(BlueprintCallable, Category = "GameMode")
    virtual void EndGame();

    // 플레이어가 장외 상태일 때 호출되는 함수
    UFUNCTION(BlueprintCallable, Category = "Combat")
    virtual void HandlePlayerOutOfBounds(APawn* Pawn);

    // 남아있는 살아있는 플레이어 수 체크 함수 (서바이벌 종료 조건)
    UFUNCTION(BlueprintCallable, Category = "GameMode")
    virtual void CheckRemainingPlayers();

    // 게임 시작 후 그레이스 피리어드 (초 단위): 이 시간 동안 체크를 건너뜁니다.
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Game Settings")
    float GracePeriod;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Game Settings")
    ACameraActor* SpectatorCamera; // 관전 카메라 액터 (사전에 레벨에 배치해둔 카메라 액터를 참조)

    void InitializePlayerScores();

protected:
    // 게임이 시작된 시간을 저장 (초 단위)
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Game Settings")
    float GameStartTime;

    // 타이머 핸들러: 추가 업데이트가 필요한 경우
    FTimerHandle GameStateTimerHandle;

    // 안전하게 게임 상태를 변경하는 내부 함수 (변경 시 이벤트도 발생)
    virtual void SetGameState(EBattleGameState NewState);

    // 게임 상태를 InProgress로 전환하는 함수
    UFUNCTION()
    void SetGameStateToInProgress();

    // 타이머 핸들 (3초 딜레이용)
    FTimerHandle DelayTimerHandle;
};