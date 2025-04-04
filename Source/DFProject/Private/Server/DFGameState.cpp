#include "Server/DFGameState.h"
#include "Net/UnrealNetwork.h"

ADFGameState::ADFGameState()
{
    // 기본값 설정 (필요에 따라 변경)
    LobbyName = TEXT("Default Lobby");
    LobbyMaxPlayers = 4;
    CurrentPlayerCount = 0;
    StartCountdown = 10.0f; // 예시: 10초 카운트다운
    bLobbyReady = false;
    PlayerNames = TArray<FString>();
}

void ADFGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(ADFGameState, LobbyName);
    DOREPLIFETIME(ADFGameState, LobbyMaxPlayers);
    DOREPLIFETIME(ADFGameState, CurrentPlayerCount);
    DOREPLIFETIME(ADFGameState, StartCountdown);
    DOREPLIFETIME(ADFGameState, bLobbyReady);
    DOREPLIFETIME(ADFGameState, PlayerNames);
}


