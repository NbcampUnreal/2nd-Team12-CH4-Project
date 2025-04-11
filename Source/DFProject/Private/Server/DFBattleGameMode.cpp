#include "Server/DFBattleGameMode.h"
#include "Server/DFBattleGameState.h"
#include "TimerManager.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "Character/DFPlayerState.h"
#include "Kismet/GameplayStatics.h"

ADFBattleGameMode::ADFBattleGameMode()
{
    PrimaryActorTick.bCanEverTick = true;

    CurrentGameState = EBattleGameState::Waiting;

    // 그레이스 피리어드 (예: 10초 동안 체크하지 않음)
    GracePeriod = 10.f;
    GameStartTime = 0.f;  // BeginPlay()에서 설정할 예정
}

void ADFBattleGameMode::BeginPlay()
{
    Super::BeginPlay();

    // 게임 시작 시 현재 시간을 기록
    GameStartTime = GetWorld()->GetTimeSeconds();

    // 모든 초기화가 완료되면 게임 상태를 InProgress로 전환
    SetGameState(EBattleGameState::InProgress);
}

void ADFBattleGameMode::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    // 서바이벌 모드에서는 매 프레임마다 플레이어들의 Pawn 위치를 확인합니다.
    for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
    {
        APlayerController* PC = It->Get();
        if (!PC)
        {
            UE_LOG(LogTemp, Warning, TEXT("Tick(): 유효하지 않은 PlayerController"));
            continue;
        }

        APawn* Pawn = PC->GetPawn();
        if (!Pawn || !IsValid(Pawn)) // Pawn의 유효성을 검사
        {
            continue;
        }
    }

    // 그레이스 피리어드 이후에 살아있는 플레이어 수 체크
    float ElapsedTime = GetWorld()->GetTimeSeconds() - GameStartTime;
    if (ElapsedTime > GracePeriod)
    {
        CheckRemainingPlayers();
    }

}

void ADFBattleGameMode::EndGame()
{
    // 이미 종료 상태이면 중복 호출 방지
    if (CurrentGameState == EBattleGameState::Ended)
    {
        return;
    }

    SetGameState(EBattleGameState::Ended);

    UE_LOG(LogTemp, Warning, TEXT("게임 종료"));

    // 최종 승자(생존자) 정보를 GameState에 업데이트합니다.
    // GameState는 서버와 클라이언트에 모두 존재하므로,
    // 복제된 FinalWinnerName 변수를 HUD 등에서 읽어 최종 결과를 표시할 수 있습니다.
    ADFBattleGameState* MyGS = GetGameState<ADFBattleGameState>();
    if (MyGS)
    {
        TArray<APlayerController*> AliveControllers;
        for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
        {
            APlayerController* PC = It->Get();
            if (PC && IsValid(PC->GetPawn()))
            {
                AliveControllers.Add(PC);
            }
        }
        if (AliveControllers.Num() == 1)
        {
            FString WinnerName = AliveControllers[0]->PlayerState ? AliveControllers[0]->PlayerState->GetPlayerName() : TEXT("Unknown");
            MyGS->FinalWinnerName = WinnerName;
        }
        else
        {
            MyGS->FinalWinnerName = TEXT("No Winner");
        }
        UE_LOG(LogTemp, Warning, TEXT("최종 승자: %s"), *MyGS->FinalWinnerName);
    }

    // 이후 클라이언트의 HUD/UMG 위젯이 GameState의 복제된 FinalWinnerName를 통해 결과를 표시하게 하면 됩니다.
}

void ADFBattleGameMode::HandlePlayerOutOfBounds(APawn* Pawn)
{
    // Pawn이 유효하지 않으면 바로 반환
    if (!Pawn || !IsValid(Pawn))
        return;

    UE_LOG(LogTemp, Log, TEXT("HandlePlayerOutOfBounds: Eliminating player %s"), *Pawn->GetName());

    AController* Controller = Pawn->GetController();
    if (Controller && IsValid(Controller))
    {
        Controller->UnPossess();
    }

    // 추가 정리(예: PlayerState 업데이트)가 필요한 경우 여기에 구현 가능
    Pawn->Destroy();
}

void ADFBattleGameMode::CheckRemainingPlayers()
{
    int32 AliveCount = 0;
    for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
    {
        APlayerController* PC = It->Get();
        if (PC && IsValid(PC->GetPawn()))
        {
            AliveCount++;
        }
    }

    UE_LOG(LogTemp, Verbose, TEXT("남은 플레이어 수: %d"), AliveCount);

    // 만약 살아있는 플레이어가 1명 이하이고 게임 상태가 진행 중이면 게임 종료
    if (AliveCount <= 1 && CurrentGameState == EBattleGameState::InProgress)
    {
        UE_LOG(LogTemp, Warning, TEXT("마지막 플레이어 남음. 게임 종료."));
        EndGame();
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
