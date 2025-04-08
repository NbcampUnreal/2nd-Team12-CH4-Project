#include "Server/DFGameState.h"
#include "Net/UnrealNetwork.h"

ADFGameState::ADFGameState()
{
    // �⺻�� ���� (�ʿ信 ���� ����)
    LobbyName = TEXT("Default Lobby");
    LobbyMaxPlayers = 4;
    CurrentPlayerCount = 0;
    StartCountdown = 10.0f; // ����: 10�� ī��Ʈ�ٿ�
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


