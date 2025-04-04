#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "DFGameState.generated.h"

UCLASS()
class DFPROJECT_API ADFGameState : public AGameStateBase
{
	GENERATED_BODY()

public:
	ADFGameState();

    // �κ� �̸�
    UPROPERTY(Replicated, BlueprintReadOnly, Category = "Lobby")
    FString LobbyName;

    // �κ� �ִ� �÷��̾� ��
    UPROPERTY(Replicated, BlueprintReadOnly, Category = "Lobby")
    int32 LobbyMaxPlayers;

    // ���� �κ� ������ �÷��̾� ��
    UPROPERTY(Replicated, BlueprintReadOnly, Category = "Lobby")
    int32 CurrentPlayerCount;

    // ���� ���� �� ī��Ʈ�ٿ� �ð� (��)
    UPROPERTY(Replicated, BlueprintReadOnly, Category = "Game")
    float StartCountdown;

    // �κ� �غ�Ǿ����� ����
    UPROPERTY(Replicated, BlueprintReadOnly, Category = "Lobby")
    bool bLobbyReady;

    // �κ� ������ �÷��̾���� �̸� (�ɼ�)
    UPROPERTY(Replicated, BlueprintReadOnly, Category = "Lobby")
    TArray<FString> PlayerNames;

    // ������ �������� ����
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
};
