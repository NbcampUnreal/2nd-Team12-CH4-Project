#include "Server/DFBattleGameMode.h"
#include "TimerManager.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"

ADFBattleGameMode::ADFBattleGameMode()
{
    PrimaryActorTick.bCanEverTick = true;

    RoundTime = 300.f;       // 전체 게임 시간 (예: 300초)
    CurrentTime = RoundTime; // 시작 시 남은 시간 = 전체 게임 시간
    CurrentGameState = EBattleGameState::Waiting;
    Score = 0;
}

void ADFBattleGameMode::BeginPlay()
{
    Super::BeginPlay();

    // 모든 초기화가 완료되면 게임 상태를 InProgress로 전환
    SetGameState(EBattleGameState::InProgress);

    // 1초 간격으로 UpdateGameState 함수를 호출하는 타이머 시작
    GetWorldTimerManager().SetTimer(GameStateTimerHandle, this, &ADFBattleGameMode::UpdateGameState, 1.0f, true);
}

void ADFBattleGameMode::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);
    // 추가적인 프레임 단위 업데이트 로직 추가 가능
}

void ADFBattleGameMode::UpdateGameState()
{
    if (CurrentGameState == EBattleGameState::InProgress)
    {
        // 매 호출마다 남은 시간 1초씩 감소
        CurrentTime -= 1.0f;

        // 남은 시간이 0 이하가 되면 시간 초과 처리
        if (CurrentTime <= 0)
        {
            // 동점 상태인지 확인하여 서든데스 모드 전환 또는 게임 종료
            if (IsTie())
            {
                SetGameState(EBattleGameState::SuddenDeath);
                HandleSuddenDeath();
            }
            else
            {
                EndGame();
            }
        }
    }

    // 추가: 게임 도중 점수 변경, 플레이어 상태 체크 등 다른 로직을 여기서 구현 가능
}

void ADFBattleGameMode::HandleSuddenDeath()
{
    // 서든데스 모드 진입 시 관련 로직을 구현
    UE_LOG(LogTemp, Warning, TEXT("서든데스 모드 시작! 첫 공격 시 승패 결정"));
    // 서든데스 전용 타이머 재설정, UI 알림 등 추가 로직 구현 가능
}

void ADFBattleGameMode::EndGame()
{
    // 게임 종료 상태로 전환하고 타이머를 정리
    SetGameState(EBattleGameState::Ended);
    GetWorldTimerManager().ClearTimer(GameStateTimerHandle);

    UE_LOG(LogTemp, Warning, TEXT("게임 종료"));
    // 승리자 결정, 결과 전송, UI 전환 등 추가 로직 구현 가능
}

void ADFBattleGameMode::SetGameState(EBattleGameState NewState)
{
    if (CurrentGameState != NewState)
    {
        CurrentGameState = NewState;
        // 상태 변경 시 블루프린트에 이벤트 브로드캐스트 (클라이언트가 직접 접근하지는 않지만, UI 업데이트 등 서버 내 로직에 활용 가능)
        OnGameStateChanged.Broadcast(CurrentGameState);
    }
}

bool ADFBattleGameMode::IsTie() const
{
    // 예제에서는 항상 동점으로 간주 (테스트 목적)
    // 실제 구현 시에는 점수 관리 로직에 따라 동점 여부를 판단하세요.
    return true;
}