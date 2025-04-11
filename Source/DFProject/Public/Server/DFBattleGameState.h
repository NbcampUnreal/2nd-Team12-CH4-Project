#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "Net/UnrealNetwork.h"
#include "DFBattleGameState.generated.h"

/**
 * 게임 진행 상태를 나타내는 Enum
 * Waiting: 플레이어 준비 또는 초기화 상태
 * InProgress: 게임 진행 중
 * SuddenDeath: 시간 초과 후 동점일 때 서든데스 모드
 * Ended: 게임 종료 상태
 */
UENUM(BlueprintType)
enum class EDFBattleGameStateStatus : uint8
{
    Waiting     UMETA(DisplayName = "Waiting"),
    InProgress  UMETA(DisplayName = "InProgress"),
    SuddenDeath UMETA(DisplayName = "SuddenDeath"),
    Ended       UMETA(DisplayName = "Ended")
};

/**
 * 게임 상태 변경 델리게이트
 * 게임 상태가 변경될 때 블루프린트와 연동하여 UI 업데이트 등 추가 처리를 할 수 있습니다.
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGameStateChangedSignature, EDFBattleGameStateStatus, NewState);

UCLASS()
class DFPROJECT_API ADFBattleGameState : public AGameStateBase
{
    GENERATED_BODY()

public:
    ADFBattleGameState();

    // 현재 게임 진행 상태 (서버에서 계산한 값을 클라이언트로 전달)
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, ReplicatedUsing = OnRep_GameState, Category = "Game State")
    EDFBattleGameStateStatus CurrentGameState;

    // 최종 생존자 또는 승자의 이름
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated, Category = "Game Results")
    FString FinalWinnerName;

    // 게임 상태 변경 이벤트 (블루프린트에서 바인딩 가능)
    UPROPERTY(BlueprintAssignable, Category = "Events")
    FOnGameStateChangedSignature OnGameStateChanged;
    // 네트워크 복제 설정 함수
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
    // 복제로 인해 클라이언트에서 게임 상태가 업데이트될 때 호출되는 함수
    UFUNCTION()
    virtual void OnRep_GameState();
};