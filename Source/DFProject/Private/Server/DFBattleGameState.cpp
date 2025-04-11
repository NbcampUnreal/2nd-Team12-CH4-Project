#include "Server/DFBattleGameState.h"
#include "Net/UnrealNetwork.h"

ADFBattleGameState::ADFBattleGameState()
{
    // 올바른 enum 값으로 초기화
    CurrentGameState = EDFBattleGameStateStatus::Waiting;

    // GameState는 기본적으로 복제를 활성화합니다.
    bReplicates = true;
}

void ADFBattleGameState::OnRep_GameState()
{
    // 서버에서 업데이트된 상태를 클라이언트에 알림
    OnGameStateChanged.Broadcast(CurrentGameState);
}

void ADFBattleGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(ADFBattleGameState, CurrentGameState);
	DOREPLIFETIME(ADFBattleGameState, FinalWinnerName);
}