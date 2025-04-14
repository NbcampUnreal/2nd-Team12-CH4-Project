#include "Server/DFTimeOutGameMode.h"
#include "TimerManager.h"
#include "Engine/World.h"
#include "Character/DFPlayerState.h"

ADFTimeOutGameMode::ADFTimeOutGameMode()
{
	// 기본 타임아웃 시간 (예: 300초)
	TimeoutDuration = 300.f;
}

void ADFTimeOutGameMode::BeginPlay()
{
	Super::BeginPlay();
	// 타임아웃 발생 시 호출되는 함수 등록
	GetWorld()->GetTimerManager().SetTimer(TimeoutHandle, this, &ADFTimeOutGameMode::OnTimeout, TimeoutDuration, false);
}

void ADFTimeOutGameMode::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	// Tick에서 타임아웃 여부를 다시 체크하여 안전하게 게임을 종료
	if (HasTimedOut() && CurrentGameState == EBattleGameState::InProgress)
	{
		UE_LOG(LogTemp, Warning, TEXT("타임아웃에 도달하여 게임 종료"));
		EndGame();
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
			// 공격자의 PlayerState에 점수를 추가
			ADFPlayerState* AttackerPS = Cast<ADFPlayerState>(PS->LastDamageDealer);
			if (AttackerPS)
			{
				// 예: 1점 추가 (필요에 따라 점수 값을 조정)
				AttackerPS->AddIndividualScore(1);
				UE_LOG(LogTemp, Log, TEXT("공격자 %s 에게 점수 추가"), *AttackerPS->GetPlayerName());
			}
		}

		// 컨트롤러를 언포제스하고 관전 모드로 전환 (원하는 경우)
		if (APlayerController* PC = Cast<APlayerController>(Controller))
		{
			PC->UnPossess();
			PC->StartSpectatingOnly();
			// 관전자 카메라 전환 기능이 있다면 여기에 추가할 수 있음.
		}
	}

	// 플레이어 Pawn 제거
	Pawn->Destroy();

	// 일정 딜레이 후 플레이어 리스폰 (컨트롤러를 기준으로 리스폰)
	FTimerHandle RespawnTimerHandle;
	FTimerDelegate RespawnDelegate = FTimerDelegate::CreateUObject(this, &ADFTimeOutGameMode::RespawnPlayer, Controller);
	GetWorld()->GetTimerManager().SetTimer(RespawnTimerHandle, RespawnDelegate, RespawnDelay, false);
}


void ADFTimeOutGameMode::OnTimeout()
{
	UE_LOG(LogTemp, Warning, TEXT("타임아웃 발생: %f초가 경과했습니다."), TimeoutDuration);
	if (CurrentGameState == EBattleGameState::InProgress)
	{
		EndGame();
	}
}

void ADFTimeOutGameMode::RespawnPlayer(AController* Controller)
{
	// 컨트롤러가 유효하면 플레이어 리스폰 (GameMode의 RestartPlayer 사용)
	if (Controller)
	{
		UE_LOG(LogTemp, Log, TEXT("플레이어 리스폰: %s"), *Controller->GetName());
		RestartPlayer(Controller);
	}
}

