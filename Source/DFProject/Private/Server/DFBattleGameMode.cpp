#include "Server/DFBattleGameMode.h"
#include "Server/DFBattleGameState.h"
#include "TimerManager.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "Camera/CameraActor.h"
#include "GameFramework/Actor.h"
#include "Character/DFPlayerState.h"
#include "Kismet/GameplayStatics.h"

ADFBattleGameMode::ADFBattleGameMode()
{
    PrimaryActorTick.bCanEverTick = true;

    CurrentGameState = EBattleGameState::Waiting;

    // 예: 최초 3초간은 생존자 체크하지 않음
    GracePeriod = 3.f;
    GameStartTime = 0.f;  // BeginPlay()에서 세팅
}

void ADFBattleGameMode::BeginPlay()
{
    Super::BeginPlay();

    // Spectator용 카메라 찾기
    TArray<AActor*> FoundCameras;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACameraActor::StaticClass(), FoundCameras);
    for (AActor* Actor : FoundCameras)
    {
        if (Actor->Tags.Contains(TEXT("Spectator")))
        {
            SpectatorCamera = Cast<ACameraActor>(Actor);
            UE_LOG(LogTemp, Log, TEXT("Spectator camera found: %s"), *Actor->GetName());
            break;
        }
    }
    if (!SpectatorCamera)
    {
        UE_LOG(LogTemp, Warning, TEXT("Spectator camera not found."));
    }

    // 게임 시작 시간 기록
    GameStartTime = GetWorld()->GetTimeSeconds();

    // 3초 뒤 인게임 상태로 전환
    GetWorldTimerManager()
        .SetTimer(DelayTimerHandle, this, &ADFBattleGameMode::SetGameStateToInProgress, GracePeriod, false);
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

void ADFBattleGameMode::CheckRemainingPlayers()
{
    if (CurrentGameState == EBattleGameState::Ended)
        return;

    // 살아있는 컨트롤러 수 파악
    TArray<AController*> AliveControllers;
    for (FConstControllerIterator It = GetWorld()->GetControllerIterator(); It; ++It)
    {
        AController* Controller = It->Get();
        if (Controller && IsValid(Controller->GetPawn()))
        {
            AliveControllers.Add(Controller);
        }
    }

    // 살아남은 사람이 1명이하일 때만 EndGame() 호출
    if (AliveControllers.Num() <= 1)
    {
        EndGame();  // 여기서 최종 우승자 집계 및 관전 모드 전환까지 한 번에 처리
    }
}

void ADFBattleGameMode::EndGame()
{
    if (CurrentGameState == EBattleGameState::Ended)
        return;

    SetGameState(EBattleGameState::Ended);
    UE_LOG(LogTemp, Warning, TEXT("게임 종료"));

    // ② 모든 PlayerState 수집
    TArray<ADFPlayerState*> AllPS;
    for (FConstControllerIterator It = GetWorld()->GetControllerIterator(); It; ++It)
    {
        if (AController* C = It->Get())
        {
            if (ADFPlayerState* PS = Cast<ADFPlayerState>(C->PlayerState))
            {
                UE_LOG(LogTemp, Warning, TEXT("수집된 PlayerState: %s (IsAlive: %s)"),
                    *PS->GetPlayerName(), PS->IsAlive() ? TEXT("True") : TEXT("False"));
                AllPS.Add(PS);
            }
            else
            {
                UE_LOG(LogTemp, Warning, TEXT("Controller %s 는 ADFPlayerState 아님"), *C->GetName());
            }
        }
    }

    // ③ 생존 시간 계산용 구조체
    struct FSurvivalEntry { ADFPlayerState* PS; float SurvivalTime; };
    TArray<FSurvivalEntry> Results;
    Results.Reserve(AllPS.Num());

    // 종료 시점
    const float EndTime = GetWorld()->GetTimeSeconds();

    // ④ 각자의 생존 시간 계산
    for (ADFPlayerState* PS : AllPS)
    {
        // 아직 살아있다면 EndTime, 아니면 사망 시점 DeathTime
        const float death = PS->IsAlive() ? EndTime : PS->DeathTime;
        const float surv = FMath::Max(0.f, death - GameStartTime);
        Results.Add({ PS, surv });
    }

    // ⑤ 생존 시간 내림차순 정렬
    Results.Sort([](const FSurvivalEntry& A, const FSurvivalEntry& B) {
        return A.SurvivalTime > B.SurvivalTime;
        });

    // ⑥ GameState에 순위 저장
    if (ADFBattleGameState* GS = GetGameState<ADFBattleGameState>())
    {
        // 배열로 만들었다면 PlaceNames[0..2], PlaceTimes[0..2] 로 저장
        for (int32 i = 0; i < 3; ++i)
        {
            if (Results.IsValidIndex(i))
            {
                GS->PlaceNames[i] = Results[i].PS->GetPlayerName();
                GS->PlaceTimes[i] = Results[i].SurvivalTime;
            }
            else
            {
                GS->PlaceNames[i] = TEXT("None");
                GS->PlaceTimes[i] = 0.f;
            }
        }

        UE_LOG(LogTemp, Warning,
            TEXT("서바이벌 순위: 1위 %s(%.1f초), 2위 %s(%.1f초), 3위 %s(%.1f초)"),
            *GS->PlaceNames[0], GS->PlaceTimes[0],
            *GS->PlaceNames[1], GS->PlaceTimes[1],
            *GS->PlaceNames[2], GS->PlaceTimes[2]
        );
    }

    // ⑦ 남은 플레이어 전부 관전 모드로 전환
    for (FConstControllerIterator It = GetWorld()->GetControllerIterator(); It; ++It)
    {
        if (AController* C = It->Get())
        {
            if (APawn* P = C->GetPawn())
            {
                HandlePlayerOutOfBounds(P);
            }
        }
    }

    FTimerHandle AwardTimer;
    GetWorldTimerManager().SetTimer(
        AwardTimer,
        this,
        &ADFBattleGameMode::TravelToAwards,
        AwardsDelay,
        false
    );
}

void ADFBattleGameMode::HandlePlayerOutOfBounds(APawn* Pawn)
{
    if (!Pawn || !IsValid(Pawn)) return;

    // ① 죽은 시각 기록
    if (ADFPlayerState* PS = Cast<ADFPlayerState>(Pawn->GetPlayerState()))
    {
        PS->DeathTime = GetWorld()->GetTimeSeconds();
    }

    AController* C = Pawn->GetController();
    if (C)
    {
        if (APlayerController* PC = Cast<APlayerController>(C))
        {
            PC->UnPossess();
            PC->StartSpectatingOnly();

            FTimerHandle Tmp;
            GetWorldTimerManager().SetTimer(Tmp, FTimerDelegate::CreateLambda([PC, this]()
                {
                    if (SpectatorCamera)
                    {
                        PC->SetViewTargetWithBlend(SpectatorCamera, 0.5f);
                    }
                }), 0.1f, false);
        }
        else
        {
            C->UnPossess();
        }
    }

    Pawn->Destroy();
}

void ADFBattleGameMode::InitializePlayerScores()
{
    for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
    {
        if (AController* Controller = It->Get())
        {
            if (APawn* Pawn = Controller->GetPawn())
            {
                if (ADFPlayerState* PS = Cast<ADFPlayerState>(Pawn->GetPlayerState()))
                {
                    PS->ResetPlayerScore();
                }
            }
        }
    }
}

void ADFBattleGameMode::SetGameState(EBattleGameState NewState)
{
    if (CurrentGameState != NewState)
    {
        CurrentGameState = NewState;
        OnGameStateChanged.Broadcast(CurrentGameState);
    }
}

void ADFBattleGameMode::SetGameStateToInProgress()
{
    SetGameState(EBattleGameState::InProgress);
    UE_LOG(LogTemp, Log, TEXT("GameState → InProgress"));
}


void ADFBattleGameMode::TravelToAwards()
{
    if (!HasAuthority())
        return;                                    // 서버에서만 실행

    const FString MapName = AwardsMap.ToString();  // "Awards" 또는 "Maps/Awards"

    // 서버 자신(호스트)은 OpenLevel
    UGameplayStatics::OpenLevel(this, AwardsMap);

    // 접속해 있는 원격 클라이언트들에게는 ClientTravel
    const FString URL = MapName;                   // 필요하면 "?listen" 등 옵션 추가
    for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
    {
        if (APlayerController* PC = It->Get())
        {
            if (!PC->IsLocalController())          // 원격 클라이언트만
            {
                PC->ClientTravel(URL, TRAVEL_Absolute);
            }
        }
    }

    UE_LOG(LogTemp, Log, TEXT("▶ Awards 맵으로 OpenLevel/ClientTravel: %s"), *URL);
}