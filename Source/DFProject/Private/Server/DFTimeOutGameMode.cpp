#include "Server/DFTimeOutGameMode.h"
#include "Server/DFBattleGameState.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Actor.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "Engine/World.h"
#include "Character/DFPlayerState.h"
#include "Camera/CameraActor.h"
#include "GameFramework/PlayerStart.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Character/DFCharacter.h"
#include "Server/DFTimeOutGameMode.h"
#include "Server/DFBattleGameState.h"
#include "AIController.h"

ADFTimeOutGameMode::ADFTimeOutGameMode()
{
    PrimaryActorTick.bCanEverTick = true;

    // Timeout 기본값 설정
    TimeoutDuration = 180.f;
    RespawnDelay = 5.f;

    // 기본 Battle Mode 설정: 팀전 또는 자유전 (여기서는 팀전으로 설정)
    BattleMode = EBattleModeType::TeamBased;

    // 팀전 기본 설정
    Team1Score = 0;
    Team2Score = 0;
    TeamScoreUpdateInterval = 5.f;  // 필요에 따라 조정

    // 자유전 기본 설정
    FreeScoreUpdateInterval = 2.f;

    // 게임 상태 초기화 (InProgress)
    CurrentGameState = EBattleGameState::InProgress;

    // Sudden Death 모드는 기본적으로 false
    bSuddenDeath = false;
}

void ADFTimeOutGameMode::BeginPlay()
{
    Super::BeginPlay();

    // Timeout 타이머 설정: TimeoutDuration이 지나면 OnTimeout() 호출
    GetWorld()->GetTimerManager().SetTimer(TimeoutHandle, this, &ADFTimeOutGameMode::OnTimeout, TimeoutDuration, false);

    // 플레이어 점수 초기화
    InitializePlayerScores();

    if (BattleMode == EBattleModeType::TeamBased)
    {
        GetWorld()->GetTimerManager().SetTimer(TeamScoreTimerHandle, this, &ADFTimeOutGameMode::UpdateTeamScores, TeamScoreUpdateInterval, true);
        UE_LOG(LogTemp, Log, TEXT("팀전 모드: 팀 점수 업데이트 타이머 시작"));
    }
    else if (BattleMode == EBattleModeType::FreeForAll)
    {
        GetWorld()->GetTimerManager().SetTimer(FreeScoreTimerHandle, this, &ADFTimeOutGameMode::UpdateCrownScores, FreeScoreUpdateInterval, true);
        UE_LOG(LogTemp, Log, TEXT("자유전 모드: 점수 업데이트 타이머 시작"));
    }
}

void ADFTimeOutGameMode::Tick(float DeltaSeconds)
{
    // Timeout 조건 확인: TimeoutDuration 시간 도달 시
    if (HasTimedOut() && CurrentGameState == EBattleGameState::InProgress)
    {
        if (BattleMode == EBattleModeType::TeamBased)
        {
            // 팀전 모드: Timeout 도달 시 두 팀의 점수가 같다면 Sudden Death 모드로 전환
            if (Team1Score == Team2Score)
            {
                if (!bSuddenDeath)
                {
                    bSuddenDeath = true;
                    UE_LOG(LogTemp, Warning, TEXT("Timeout 도달 (팀전) - 두 팀 점수 동점. Sudden Death 모드 진입 (시간 무제한)."));
                }
            }
            else
            {
                // 동점이 아니라면 Timeout 시 바로 게임 종료
                EndGame();
                return;
            }
            // Sudden Death 모드일 경우, 매 프레임 점수 차이가 발생하면 EndGame() 호출
            CheckTeamWinningCondition();
        }
        else if (BattleMode == EBattleModeType::FreeForAll)
        {
            // 자유전 모드: 최고 점수를 가진 플레이어가 여러 명이면 Sudden Death 모드 진입
            TArray<AController*> Controllers;
            for (FConstControllerIterator It = GetWorld()->GetControllerIterator(); It; ++It)
            {
                AController* Controller = It->Get();
                if (Controller && IsValid(Controller->GetPawn()))
                {
                    Controllers.Add(Controller);
                }
            }

            float TopScore = -1.f;
            int32 TopScoreCount = 0;
            for (AController* Controller : Controllers)
            {
                ADFPlayerState* PS = Cast<ADFPlayerState>(Controller->PlayerState);
                if (PS)
                {
                    float CurrentScore = PS->GetScore();
                    if (CurrentScore > TopScore)
                    {
                        TopScore = CurrentScore;
                        TopScoreCount = 1;
                    }
                    else if (FMath::IsNearlyEqual(CurrentScore, TopScore, 0.01f))
                    {
                        TopScoreCount++;
                    }
                }
            }
            if (TopScoreCount > 1)
            {
                if (!bSuddenDeath)
                {
                    bSuddenDeath = true;
                    UE_LOG(LogTemp, Warning, TEXT("Timeout 도달 (자유전) - 최고 점수 동점. Sudden Death 모드 진입 (시간 무제한)."));
                }
            }
            else
            {
                EndGame();
                return;
            }
            CheckFreeWinningCondition();
        }
    }
    else
    {
        // Timeout이 아직 안되었거나 Sudden Death 모드가 아닌 상태에서는 모드별 승리 조건 체크
        if (BattleMode == EBattleModeType::TeamBased)
        {
            CheckTeamWinningCondition();
        }
        else if (BattleMode == EBattleModeType::FreeForAll)
        {
            CheckFreeWinningCondition();
        }
    }
}

void ADFTimeOutGameMode::EndGame()
{
    if (CurrentGameState == EBattleGameState::Ended)
    {
        return;
    }

    UE_LOG(LogTemp, Warning, TEXT("게임 종료 - 최종 점수: 팀1=%d, 팀2=%d"), Team1Score, Team2Score);

    if (BattleMode == EBattleModeType::TeamBased)
    {
        if (Team1Score == Team2Score)
        {
            if (bSuddenDeath)
            {
                UE_LOG(LogTemp, Warning, TEXT("Sudden Death 종료: 최종 동점 상태."));
            }
            else
            {
                UE_LOG(LogTemp, Warning, TEXT("팀 점수 동점. 게임은 계속 진행됨 (Sudden Death 미발동)."));
                return;
            }
        }
        else if (Team1Score > Team2Score)
        {
            UE_LOG(LogTemp, Warning, TEXT("팀 1 승리."));
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("팀 2 승리."));
        }
    }
    else
    {
        // 자유전 모드의 경우, CheckFreeWinningCondition()에서 승리 조건이 결정됨
    }

    // 추가 결과 처리 및 UI 전환 로직 구현 가능

    InitializePlayerScores();

    CurrentGameState = EBattleGameState::Ended;
}

bool ADFTimeOutGameMode::HasTimedOut() const
{
    // 게임 시작 후 경과한 시간이 TimeoutDuration보다 크거나 같으면 타임아웃
    return (GetWorld()->GetTimeSeconds() - GameStartTime) >= TimeoutDuration;
}

void ADFTimeOutGameMode::HandlePlayerOutOfBounds(APawn* Pawn)
{
    if (!Pawn || !IsValid(Pawn))
        return;

    UE_LOG(LogTemp, Log, TEXT("HandlePlayerOutOfBounds: 플레이어 %s 장외 감지"), *Pawn->GetName());

    AController* Controller = Pawn->GetController();
    if (Controller && IsValid(Controller))
    {
        ADFPlayerState* PS = Cast<ADFPlayerState>(Controller->PlayerState);
        // 공격자 정보가 있을 경우
        if (PS && PS->LastDamageDealer)
        {
            // 플레이어 컨트롤러 처리
            if (APlayerController* PC = Cast<APlayerController>(Controller))
            {
                APawn* CachedPawn = Pawn;
                PC->UnPossess();
                PC->StartSpectatingOnly();
                FTimerHandle TempHandle;
                GetWorldTimerManager().SetTimer(TempHandle, FTimerDelegate::CreateLambda([PC, this]()
                    {
                        if (SpectatorCamera)
                        {
                            PC->SetViewTargetWithBlend(SpectatorCamera, 0.5f); // 0.5초 블렌드 효과 적용
                        }
                    }), 0.1f, false);

                ADFPlayerState* AttackerPS = Cast<ADFPlayerState>(PS->LastDamageDealer);
                if (AttackerPS)
                {
                    if (BattleMode == EBattleModeType::TeamBased)
                    {
                        if (AttackerPS->TeamID == 1)
                        {
                            Team1Score++;
                        }
                        else if (AttackerPS->TeamID == 2)
                        {
                            Team2Score++;
                        }
                    }
                    else // FreeForAll 모드
                    {
                        AttackerPS->AddIndividualScore(1);
                    }
                    UE_LOG(LogTemp, Log, TEXT("공격자 %s 에게 점수 추가"), *AttackerPS->GetPlayerName());
                }
                RespawnPlayer(Controller, CachedPawn);
                return;
            }
            // AI 컨트롤러 처리: AAIController 사용
            else if (AAIController* AIController = Cast<AAIController>(Controller))
            {
                APawn* CachedPawn = Pawn;
                AIController->UnPossess();
                FTimerHandle TempHandle;
                GetWorldTimerManager().SetTimer(TempHandle, FTimerDelegate::CreateLambda([this]()
                    {
                        // AI의 경우 별도 카메라 전환은 생략합니다.
                    }), 0.1f, false);

                ADFPlayerState* AttackerPS = Cast<ADFPlayerState>(PS->LastDamageDealer);
                if (AttackerPS)
                {
                    if (BattleMode == EBattleModeType::TeamBased)
                    {
                        if (AttackerPS->TeamID == 1)
                        {
                            Team1Score++;
                        }
                        else if (AttackerPS->TeamID == 2)
                        {
                            Team2Score++;
                        }
                    }
                    else // FreeForAll
                    {
                        AttackerPS->AddIndividualScore(1);
                    }
                    UE_LOG(LogTemp, Log, TEXT("공격자 %s 에게 점수 추가"), *AttackerPS->GetPlayerName());
                }
                RespawnPlayer(Controller, CachedPawn);
                return;
            }
            else
            {
                Controller->UnPossess();
            }
        }
        // 공격자 정보가 없을 경우
        else
        {
            UE_LOG(LogTemp, Log, TEXT("공격자 정보 없음"));
            if (APlayerController* PC = Cast<APlayerController>(Controller))
            {
                APawn* CachedPawn = Pawn;
                PC->UnPossess();
                PC->StartSpectatingOnly();
                FTimerHandle TempHandle;
                GetWorldTimerManager().SetTimer(TempHandle, FTimerDelegate::CreateLambda([PC, this]()
                    {
                        if (SpectatorCamera)
                        {
                            PC->SetViewTargetWithBlend(SpectatorCamera, 0.5f);
                        }
                    }), 0.1f, false);
                RespawnPlayer(Controller, CachedPawn);
                return;
            }
            else if (AAIController* AIController = Cast<AAIController>(Controller))
            {
                APawn* CachedPawn = Pawn;
                AIController->UnPossess();
                FTimerHandle TempHandle;
                GetWorldTimerManager().SetTimer(TempHandle, FTimerDelegate::CreateLambda([this]()
                    {
                        // AI 카메라 처리 생략
                    }), 0.1f, false);
                RespawnPlayer(Controller, CachedPawn);
                return;
            }
            else
            {
                Controller->UnPossess();
            }
        }
    }
    // 컨트롤러가 없으면 Pawn Destroy 처리
    Pawn->Destroy();
}


void ADFTimeOutGameMode::OnTimeout()
{
    UE_LOG(LogTemp, Warning, TEXT("Timeout 발생: %f초 경과"), TimeoutDuration);
}

void ADFTimeOutGameMode::UpdateTeamScores()
{
    // 팀전 모드일 경우, 각 컨트롤러의 PlayerState를 통해 팀 점수를 업데이트합니다.
    for (FConstControllerIterator It = GetWorld()->GetControllerIterator(); It; ++It)
    {
        AController* Controller = It->Get();
        if (Controller && IsValid(Controller->GetPawn()))
        {
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
    UE_LOG(LogTemp, Log, TEXT("Team scores updated - Team1: %d, Team2: %d"), Team1Score, Team2Score);
}

void ADFTimeOutGameMode::CheckTeamWinningCondition()
{
    if (bSuddenDeath)
    {
        // Sudden Death 모드에서는 점수 차이가 발생하면 승리 결정
        if (Team1Score != Team2Score)
        {
            UE_LOG(LogTemp, Warning, TEXT("Sudden Death: 팀 점수 차이 감지. 게임 종료."));
            EndGame();
        }
    }
    else
    {
        // 일반 상태에서는 Timeout 시 EndGame()가 호출됨 (이 경우 이미 동점이 아니어야 함)
        if (Team1Score > Team2Score)
        {
            UE_LOG(LogTemp, Warning, TEXT("팀 1 승리 (점수: %d vs %d)"), Team1Score, Team2Score);
            EndGame();
        }
        else if (Team2Score > Team1Score)
        {
            UE_LOG(LogTemp, Warning, TEXT("팀 2 승리 (점수: %d vs %d)"), Team2Score, Team1Score);
            EndGame();
        }
    }
}

void ADFTimeOutGameMode::UpdateCrownScores()
{

}

void ADFTimeOutGameMode::CheckFreeWinningCondition()
{
    TArray<AController*> Controllers;
    for (FConstControllerIterator It = GetWorld()->GetControllerIterator(); It; ++It)
    {
        AController* Controller = It->Get();
        if (Controller && IsValid(Controller->GetPawn()))
        {
            Controllers.Add(Controller);
        }
    }

    float TopScore = -1.f;
    int32 TopScoreCount = 0;
    ADFPlayerState* Winner = nullptr;
    for (AController* Controller : Controllers)
    {
        ADFPlayerState* PS = Cast<ADFPlayerState>(Controller->PlayerState);
        if (PS)
        {
            float CurrentScore = PS->GetScore();
            if (CurrentScore > TopScore)
            {
                TopScore = CurrentScore;
                TopScoreCount = 1;
                Winner = PS;
            }
            else if (FMath::IsNearlyEqual(CurrentScore, TopScore, 0.01f))
            {
                TopScoreCount++;
            }
        }
    }
    if (TopScoreCount > 1)
    {
        if (!bSuddenDeath)
        {
            bSuddenDeath = true;
            UE_LOG(LogTemp, Warning, TEXT("자유전: 최고 점수 동점. Sudden Death 모드 진입 (시간 무제한)."));
        }
    }
    else if (TopScoreCount == 1 && bSuddenDeath)
    {
        UE_LOG(LogTemp, Warning, TEXT("Sudden Death: 단독 우위 플레이어 감지."));
        EndGame();
    }
    else if (TopScoreCount == 1 && !bSuddenDeath)
    {
        EndGame();
    }
}

void ADFTimeOutGameMode::RespawnPlayer(AController* Controller, APawn* PawnToRespawn)
{
    if (!Controller)
    {
        UE_LOG(LogTemp, Warning, TEXT("RespawnPlayer: 유효하지 않은 컨트롤러"));
        return;
    }
    if (!PawnToRespawn)
    {
        UE_LOG(LogTemp, Warning, TEXT("RespawnPlayer: 리스폰할 Pawn이 없음!"));
        return;
    }
    UE_LOG(LogTemp, Log, TEXT("RespawnPlayer: 컨트롤러 %s 리스폰 시작"), *Controller->GetName());

    // Pawn을 즉시 숨기고 충돌 비활성화
    PawnToRespawn->SetActorHiddenInGame(true);
    PawnToRespawn->SetActorEnableCollision(false);
    if (UCharacterMovementComponent* MoveComp = Cast<UCharacterMovementComponent>(PawnToRespawn->GetMovementComponent()))
    {
        MoveComp->StopMovementImmediately();
    }

    // 플레이어 시작(Spawn Point) 액터를 찾음.
    AActor* PlayerStart = FindPlayerStart(Controller);
    if (!PlayerStart)
    {
        UE_LOG(LogTemp, Error, TEXT("RespawnPlayer: 플레이어 시작 지점을 찾을 수 없습니다!"));
        return;
    }
    UE_LOG(LogTemp, Log, TEXT("RespawnPlayer: 플레이어 시작 위치 %s"), *PlayerStart->GetActorLocation().ToString());

    // Pawn의 위치와 회전을 스폰 포인트로 재설정.
    PawnToRespawn->SetActorLocation(PlayerStart->GetActorLocation());
    PawnToRespawn->SetActorRotation(PlayerStart->GetActorRotation());

    // 타이머 설정 직후 로그
    FTimerHandle RespawnTimerHandle;
    UE_LOG(LogTemp, Log, TEXT("RespawnPlayer: RespawnDelay = %f, 타이머 설정 완료"), RespawnDelay);

    // 타이머 딜레이 후에 Pawn 재활성화 및 Possess 실행 (수정된 lambda 사용)
    FTimerDelegate RespawnDelegate = FTimerDelegate::CreateLambda([Controller, PawnToRespawn]()
        {
            UE_LOG(LogTemp, Log, TEXT("RespawnDelegate 람다 실행됨"));

            PawnToRespawn->SetActorHiddenInGame(false);
            PawnToRespawn->SetActorEnableCollision(true);

            Controller->Possess(PawnToRespawn);

            APawn* PossessedPawn = Controller->GetPawn();
            if (PossessedPawn == PawnToRespawn)
            {
                UE_LOG(LogTemp, Log, TEXT("Controller %s가 Pawn %s를 성공적으로 소유함."), *Controller->GetName(), *PawnToRespawn->GetName());
            }
            else
            {
                UE_LOG(LogTemp, Error, TEXT("Controller %s가 기대한 Pawn을 소유하지 않음!"), *Controller->GetName());
            }

            if (APlayerController* PC = Cast<APlayerController>(Controller))
            {
                PC->SetViewTargetWithBlend(PawnToRespawn, 0.0f);
                if (ADFCharacter* DFCharacter = Cast<ADFCharacter>(PawnToRespawn))
                {
                    DFCharacter->RecoverStart();
                }
            }
            else
            {
                UE_LOG(LogTemp, Log, TEXT("AI Pawn %s 부활"), *PawnToRespawn->GetName());
            }

            UE_LOG(LogTemp, Log, TEXT("RespawnPlayer: Pawn %s가 다시 소유됨."), *PawnToRespawn->GetName());
        });

    GetWorld()->GetTimerManager().SetTimer(RespawnTimerHandle, RespawnDelegate, RespawnDelay, false);
}