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
    // 기본 타임아웃 시간 (예: 300초)
    TimeoutDuration = 300.f;
    // 기본 리스폰 딜레이 (예: 5초)
    RespawnDelay = 5.f;
    PrimaryActorTick.bCanEverTick = true;

    // 기본 모드는 자유전(개인전)으로 설정 (프로젝트 요구에 따라 TeamBased로 변경 가능)
    BattleMode = EBattleModeType::FreeForAll;

    Team1Score = 0;
    Team2Score = 0;

    // 기본 게임 상태
    CurrentGameState = EBattleGameState::InProgress;

    PrimaryActorTick.bCanEverTick = true;
}

void ADFTimeOutGameMode::BeginPlay()
{

    Super::BeginPlay();

    // 게임 시작 시간 기록
    GameStartTime = GetWorld()->GetTimeSeconds();

    // 타임아웃 발생 시 호출되는 함수 등록
    GetWorld()->GetTimerManager().SetTimer(TimeoutHandle, this, &ADFTimeOutGameMode::OnTimeout, TimeoutDuration, false);

    // 게임 시작 시 플레이어 점수를 초기화 (모드에 상관없이)
    InitializePlayerScores();
}

void ADFTimeOutGameMode::Tick(float DeltaSeconds)
{
    // Tick에서 타임아웃 여부를 다시 체크하여 안전하게 게임을 종료
    if (HasTimedOut() && CurrentGameState == EBattleGameState::InProgress)
    {
        EndGame();
    }
}

void ADFTimeOutGameMode::EndGame()
{
    UE_LOG(LogTemp, Warning, TEXT("타임아웃에 도달하여 게임 종료"));

    // 새 게임 시작이나 종료 전에 플레이어 점수 초기화
    InitializePlayerScores();

    if (CurrentGameState == EBattleGameState::Ended)
        return;

    CurrentGameState = EBattleGameState::Ended;
    // 추가적으로 최종 승자 결정 등 처리를 여기에 구현할 수 있음.
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
    UE_LOG(LogTemp, Warning, TEXT("타임아웃 발생: %f초가 경과."), TimeoutDuration);
    if (CurrentGameState == EBattleGameState::InProgress)
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
                // Possess() 후 입력은 자동 활성화되므로 EnableInput() 호출은 생략
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