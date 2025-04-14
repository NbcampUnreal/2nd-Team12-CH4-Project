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

    // 그레이스 피리어드 (예: 10초 동안 체크하지 않음)
    GracePeriod = 3.f;
    GameStartTime = 0.f;  // BeginPlay()에서 설정할 예정
}

void ADFBattleGameMode::BeginPlay()
{
    Super::BeginPlay();

    // 레벨에 배치된 모든 카메라 액터를 가져온다.
    TArray<AActor*> FoundCameras;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACameraActor::StaticClass(), FoundCameras);

    // "Spectator" 라는 액터 태크를 가진 카메라 액터를 찾는다.
    for (AActor* Actor : FoundCameras)
    {
        if (Actor->Tags.Contains(TEXT("Spectator")))
        {
            SpectatorCamera = Cast<ACameraActor>(Actor);
            UE_LOG(LogTemp, Log, TEXT("Spectator camera found by tag: %s"), *Actor->GetName());
            break;
        }
    }

    // 찾지 못한 경우 경고 로그 출력
    if (!SpectatorCamera)
    {
        UE_LOG(LogTemp, Warning, TEXT("Spectator 이름의 카메라 액터를 찾지 못했습니다."));
    }

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
    if (CurrentGameState == EBattleGameState::Ended)
    {
        return;
    }

    SetGameState(EBattleGameState::Ended);
    UE_LOG(LogTemp, Warning, TEXT("게임 종료"));

    // 모든 컨트롤러를 순회하여 살아있는 Pawn이 있는 컨트롤러를 찾습니다.
    TArray<AController*> AliveControllers;
    for (FConstControllerIterator It = GetWorld()->GetControllerIterator(); It; ++It)
    {
        AController* Controller = It->Get();
        if (Controller && IsValid(Controller->GetPawn()))
        {
            AliveControllers.Add(Controller);
        }
    }

    // 최종 생존자가 한 명이라면 그 Controller의 이름을, 
    // 그렇지 않으면 "No Winner"로 설정
    ADFBattleGameState* MyGS = GetGameState<ADFBattleGameState>();
    if (MyGS)
    {
        if (AliveControllers.Num() == 1)
        {
            FString WinnerName;
            // AI Controller는 APlayerController가 아닐 수 있으므로, 기본적으로 Pawn의 이름을 사용
            if (AliveControllers[0]->PlayerState)
            {
                WinnerName = AliveControllers[0]->PlayerState->GetPlayerName();
            }
            else if (AliveControllers[0]->GetPawn())
            {
                WinnerName = AliveControllers[0]->GetPawn()->GetName();
            }
            else
            {
                WinnerName = TEXT("Unknown");
            }
            MyGS->FinalWinnerName = WinnerName;
        }
        else
        {
            MyGS->FinalWinnerName = TEXT("No Winner");
        }
        UE_LOG(LogTemp, Warning, TEXT("최종 승자: %s"), *MyGS->FinalWinnerName);
    }

    // 이후 클라이언트 HUD/UMG 위젯이 GameState의 복제된 FinalWinnerName를 통해 결과를 표시
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
        if (APlayerController* PC = Cast<APlayerController>(Controller))
        {
            PC->UnPossess();
            PC->StartSpectatingOnly();

            // 즉시 전환되지 않을 경우, 타이머를 사용하여 딜레이 후에 카메라 전환
            FTimerHandle TempHandle;
            GetWorldTimerManager().SetTimer(TempHandle, FTimerDelegate::CreateLambda([PC, this]()
                {
                    if (SpectatorCamera)
                    {
                        PC->SetViewTargetWithBlend(SpectatorCamera, 0.5f); // 0.5초 블렌드 효과 적용
                    }
                }), 0.1f, false); // 0.1초 후에 실행
        }
        // AI 컨트롤러라면, 관전 모드 지원이 없다면 다른 방식으로 처리
        else
        {
			Controller->UnPossess();
        }
    }

    // 추가 정리(예: PlayerState 업데이트)가 필요한 경우 여기에 구현 가능
    Pawn->Destroy();
}

void ADFBattleGameMode::CheckRemainingPlayers()
{
    int32 AliveCount = 0;
    for (FConstControllerIterator It = GetWorld()->GetControllerIterator(); It; ++It)
    {
        AController* Controller = It->Get();
        if (Controller && IsValid(Controller->GetPawn()))
        {
            AliveCount++;
        }
    }

    UE_LOG(LogTemp, Verbose, TEXT("남은 플레이어 수: %d"), AliveCount);

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
