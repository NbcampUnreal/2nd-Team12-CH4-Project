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
#include "AI/DFAIController.h"
#include "Character/DFCharacter.h"


ADFTimeOutGameMode::ADFTimeOutGameMode()
{
	// 기본 타임아웃 시간 (예: 300초)
	TimeoutDuration = 300.f;
	// 기본 리스폰 딜레이 (예: 5초)
	RespawnDelay = 5.f;
}

void ADFTimeOutGameMode::BeginPlay()
{

	Super::BeginPlay();

	// 타임아웃 발생 시 호출되는 함수 등록
	GetWorld()->GetTimerManager().SetTimer(TimeoutHandle, this, &ADFTimeOutGameMode::OnTimeout, TimeoutDuration, false);
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

	if (CurrentGameState == EBattleGameState::Ended)
	{
		return;
	}
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

    // 플레이어의 컨트롤러 및 PlayerState 확인
    AController* Controller = Pawn->GetController();
    if (Controller && IsValid(Controller))
    {
        // 플레이어의 PlayerState를 통해 마지막으로 데미지를 준 공격자 확인 (ADFPlayerState 사용 가정)
        ADFPlayerState* PS = Cast<ADFPlayerState>(Controller->PlayerState);
        if (PS && PS->LastDamageDealer)
        {
            if (APlayerController* PC = Cast<APlayerController>(Controller))
            {
                // Pawn 포인터를 미리 저장 (UnPossess 후에도 사용하기 위해)
                APawn* CachedPawn = Pawn;

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
                    }), 0.1f, false);

                // 공격자의 PlayerState에 점수를 추가
                ADFPlayerState* AttackerPS = Cast<ADFPlayerState>(PS->LastDamageDealer);
                if (AttackerPS)
                {
                    AttackerPS->AddIndividualScore(1);
                    UE_LOG(LogTemp, Log, TEXT("공격자 %s 에게 점수 추가"), *AttackerPS->GetPlayerName());
                }

                // Pawn이 UnPossess() 호출 후에도 존재하므로, RespawnPlayer에서 CachedPawn을 활용하도록 수정 필요
                RespawnPlayer(Controller, CachedPawn);
                return;
            }
            else
            {
                Controller->UnPossess();
            }
        }
        else
        {
            UE_LOG(LogTemp, Log, TEXT("공격자 정보 없음"));

            if (APlayerController* PC = Cast<APlayerController>(Controller))
            {
                // Pawn 포인터 미리 저장
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
            else
            {
                Controller->UnPossess();
            }
        }
    }

    // 만약 Controller가 유효하지 않다면 기본 RespawnPlayer() 호출 (여기서는 Pawn 포인터가 없으므로 별도 처리가 필요)
    RespawnPlayer(Controller, Pawn);
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

    // 타이머 딜레이 후에 Pawn을 재활성화하고 컨트롤러가 다시 Possess 하도록 설정
    FTimerDelegate RespawnDelegate = FTimerDelegate::CreateLambda([Controller, PawnToRespawn]()
        {
            UE_LOG(LogTemp, Log, TEXT("RespawnDelegate 람다 실행됨"));

            // Pawn을 다시 보이게 하고 충돌 활성화
            PawnToRespawn->SetActorHiddenInGame(false);
            PawnToRespawn->SetActorEnableCollision(true);

            // 컨트롤러가 Pawn을 다시 소유
            Controller->Possess(PawnToRespawn);

            // DFPlayerController라면 전용 초기화 로직을 수행.
            if (APlayerController* PC = Cast<APlayerController>(Controller))
            {
                // 만약 DFPlayerController에 커스텀 초기화 함수가 있다면 호출
                // 예: DFPC->InitializeAfterPossess();
                // 카메라 전환 (블렌드 시간 0.0f로 즉시 전환)
                PC->SetViewTargetWithBlend(PawnToRespawn, 0.0f);
                // 입력 활성화 - DFPlayerController에서 입력 바인딩이 제대로 설정되어 있어야 함
                PawnToRespawn->EnableInput(PC);
                
                if (ADFCharacter* DFCharacter = Cast<ADFCharacter>(PawnToRespawn))
                {
                    DFCharacter->RecoverStart();
                }
            }
            else if (ADFAIController* AIController = Cast<ADFAIController>(Controller))
            {
                UE_LOG(LogTemp, Log, TEXT("AI Pawn %s 부활"), *PawnToRespawn->GetName());
                // AI 컨트롤러의 경우 추가 초기화가 필요하면 수행
            }

            UE_LOG(LogTemp, Log, TEXT("RespawnPlayer: Pawn %s가 다시 소유됨."), *PawnToRespawn->GetName());
        });

    // 타이머를 RespawnDelay 후에 실행
    GetWorld()->GetTimerManager().SetTimer(RespawnTimerHandle, RespawnDelegate, RespawnDelay, false);
}