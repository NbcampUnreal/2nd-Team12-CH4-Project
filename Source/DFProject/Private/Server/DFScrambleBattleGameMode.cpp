#include "Server/DFScrambleBattleGameMode.h"
#include "TimerManager.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerState.h"
#include "GameFramework/Controller.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/PlayerStart.h"
#include "Character/DFPlayerState.h"
#include "Item/DFCrownActor.h"

ADFScrambleBattleGameMode::ADFScrambleBattleGameMode()
{
    // 기본 모드: 팀전 또는 개인전 선택 (여기서는 기본값 팀전)
    BattleMode = EBattleModeType::TeamBased;

    // 팀전 모드 초기화
    Team1Score = 0;
    Team2Score = 0;
    TeamScoreToWin = 10;
    TeamScoreUpdateInterval = 5.f;

    // 개인전 모드 초기화
    FreeScoreToWin = 30;
    FreeScoreUpdateInterval = 2.f;

    PrimaryActorTick.bCanEverTick = true;
}

void ADFScrambleBattleGameMode::BeginPlay()
{
    // 게임 시작 시간 기록 (필요 시)
    GameStartTime = GetWorld()->GetTimeSeconds();

    // 모드에 따라 초기화
    if (BattleMode == EBattleModeType::TeamBased)
    {
        // 팀전 모드: 팀 점수 업데이트 타이머 시작
        GetWorldTimerManager().SetTimer(TeamScoreTimerHandle, this, &ADFScrambleBattleGameMode::UpdateTeamScores, TeamScoreUpdateInterval, true);
        UE_LOG(LogTemp, Log, TEXT("팀전 모드 시작됨: 팀 점수 업데이트 타이머 시작"));
    }
    else if (BattleMode == EBattleModeType::FreeForAll)
    {
        // 개인전 모드: 왕관 액터 스폰 및 점수 업데이트 타이머 시작
        GetWorldTimerManager().SetTimer(FreeScoreTimerHandle, this, &ADFScrambleBattleGameMode::UpdateCrownScores, FreeScoreUpdateInterval, true);
        UE_LOG(LogTemp, Log, TEXT("개인전 모드 시작됨: 왕관 스폰 및 점수 업데이트 타이머 시작"));
    }
}

void ADFScrambleBattleGameMode::Tick(float DeltaSeconds)
{
    if (BattleMode == EBattleModeType::TeamBased)
    {
        //CheckTeamWinningCondition();
    }
    else if (BattleMode == EBattleModeType::FreeForAll)
    {
        CheckFreeWinningCondition();
    }
}



///////////////////////////////////////////////////////////
// 팀전 모드 관련 함수

void ADFScrambleBattleGameMode::UpdateTeamScores()
{
    // 모든 컨트롤러를 순회하여, ADFPlayerState의 TeamID를 기준으로 팀 점수를 업데이트
    for (FConstControllerIterator It = GetWorld()->GetControllerIterator(); It; ++It)
    {
        AController* Controller = It->Get();
        if (Controller && IsValid(Controller->GetPawn()))
        {
            // ADFPlayerState에 TeamID라는 정수형 변수가 있다고 가정 (1 또는 2)
            ADFPlayerState* PS = Cast<ADFPlayerState>(Controller->PlayerState);
            if (PS)
            {
                if (PS->TeamID == 1)
                {
                    Team1Score++;
                }
                else if (PS->TeamID == 2)
                {
                    Team2Score++;
                }
            }
        }
    }
    UE_LOG(LogTemp, Log, TEXT("팀 점수 업데이트됨 - 팀1: %d, 팀2: %d"), Team1Score, Team2Score);
}

void ADFScrambleBattleGameMode::CheckTeamWinningCondition()
{
    //// Spawn된 각 왕관에 대해, 왕관이 소지되어 있다면 해당 플레이어의 점수를 +1
    //for (ADFCrownActor* Crown : SpawnedCrowns)
    //{
    //    if (Crown && Crown->IsHeld()) // IsHeld() 함수가 왕관이 누군가에게 소지되었는지 판별
    //    {
    //        APawn* Holder = Crown->GetHolder(); // GetHolder()가 현재 왕관을 들고 있는 Pawn을 반환한다고 가정
    //        if (Holder)
    //        {
    //            ADFPlayerState* PS = Cast<ADFPlayerState>(Holder->GetPlayerState());
    //            if (PS)
    //            {
    //                // 플레이어에게 1점 추가 (내부적으로 AddIndividualScore() 또는 점수 직접 업데이트)
    //                PS->AddIndividualScore(1);
    //                UE_LOG(LogTemp, Log, TEXT("UpdateCrownScores: 플레이어 %s에게 왕관 점수 +1 (총 점수: %d)"),
    //                    *PS->GetPlayerName(), PS->GetScore());
    //            }
    //        }
    //    }
    //}
}

///////////////////////////////////////////////////////////
// 개인전 모드 관련 함수

void ADFScrambleBattleGameMode::UpdateCrownScores()
{
    //// SpawnedCrowns 배열은 별도의 Crown 스포너(또는 레벨에 배치된 Crown 액터)가 채워둔 상태
    //for (ADFCrownActor* Crown : SpawnedCrowns)
    //{
    //    if (Crown && Crown->IsHeld()) // 왕관이 소지 중인지 확인
    //    {
    //        APawn* Holder = Crown->GetHolder(); // 소지한 Pawn 반환 (Crown 내부 변수)
    //        if (Holder)
    //        {
    //            ADFPlayerState* PS = Cast<ADFPlayerState>(Holder->GetPlayerState());
    //            if (PS)
    //            {
    //                PS->AddIndividualScore(1);
    //                UE_LOG(LogTemp, Log, TEXT("UpdateCrownScores: 플레이어 %s에게 왕관 점수 +1 (총 점수: %d)"),
    //                    *PS->GetPlayerName(), PS->GetScore());
    //            }
    //        }
    //    }
    //}
}

void ADFScrambleBattleGameMode::CheckFreeWinningCondition()
{
    for (FConstControllerIterator It = GetWorld()->GetControllerIterator(); It; ++It)
    {
        AController* Controller = It->Get();
        if (Controller && IsValid(Controller->GetPawn()))
        {
            ADFPlayerState* PS = Cast<ADFPlayerState>(Controller->PlayerState);
            if (PS && PS->GetScore() >= FreeScoreToWin)
            {
                UE_LOG(LogTemp, Warning, TEXT("게임 종료 (자유전): 플레이어 %s 승리 (점수: %f)"),
                    *PS->GetPlayerName(), PS->GetScore());
                EndGame();
                return;
            }
        }
    }
}


void ADFScrambleBattleGameMode::EndGame()
{
    UE_LOG(LogTemp, Warning, TEXT("점수에 도달하여 게임 종료"));
    InitializePlayerScores();
    if (CurrentGameState == EBattleGameState::Ended)
    {
        return;
    }
}