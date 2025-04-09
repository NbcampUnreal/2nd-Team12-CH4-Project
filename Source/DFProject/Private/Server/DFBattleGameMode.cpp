#include "Server/DFBattleGameMode.h"
#include "TimerManager.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "Character/DFPlayerState.h"
#include "Kismet/GameplayStatics.h"

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
            SetGameState(EBattleGameState::SuddenDeath);
            HandleSuddenDeath();
            
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
    // 게임 종료 상태 전환 및 타이머 정리
    SetGameState(EBattleGameState::Ended);
    GetWorldTimerManager().ClearTimer(GameStateTimerHandle);

    UE_LOG(LogTemp, Warning, TEXT("게임 종료 - 개인전 최종 점수 표시"));

    // 모든 플레이어의 최종 점수를 출력하여 최종 순위를 결정하는 예제
    for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
    {
        APlayerController* PC = It->Get();
        if (PC && PC->PlayerState)
        {
            ADFPlayerState* PS = Cast<ADFPlayerState>(PC->PlayerState);
            if (PS)
            {
                UE_LOG(LogTemp, Warning, TEXT("플레이어 %s 최종 점수: %d"), *PS->GetPlayerName(), PS->IndividualScore);
            }
        }
    }

    // 추가: 최종 순위에 따라 승리자 표시, 결과 화면 전환 등을 구현할 수 있습니다.
}

void ADFBattleGameMode::HandlePlayerDeath(AActor* DeadActor, AController* Killer)
{
    if (!DeadActor)
        return;

    UE_LOG(LogTemp, Log, TEXT("플레이어 %s 사망, Killer: %s"),
        *DeadActor->GetName(),
        (Killer ? *Killer->GetName() : TEXT("Unknown")));

    // 개인전에서는 팀 점수가 아니라, 각 Killer의 점수를 개별적으로 업데이트합니다.
    if (Killer && Killer->PlayerState)
    {
        ADFPlayerState* KillerPS = Cast<ADFPlayerState>(Killer->PlayerState);
        if (KillerPS)
        {
            KillerPS->IndividualScore++;  // Killer의 개인 점수 증가
            UE_LOG(LogTemp, Log, TEXT("플레이어 %s의 점수 업데이트: %d"), *KillerPS->GetPlayerName(), KillerPS->IndividualScore);
        }
    }

    // 사망한 플레이어에 대해 리스폰 로직을 추가할 수 있습니다.
    // 여기서는 간단한 예로, 사망한 플레이어를 즉시 리스폰합니다.
    AController* DeadController = nullptr;
    if (APawn* Pawn = Cast<APawn>(DeadActor))
    {
        DeadController = Pawn->GetController();
    }
    if (DeadController)
    {
        RestartPlayer(DeadController);
    }
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
