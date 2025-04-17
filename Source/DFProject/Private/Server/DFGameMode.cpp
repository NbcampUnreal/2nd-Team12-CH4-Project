#include "Server/DFGameMode.h"
#include "Server/DFMenuController.h"
#include "Server/DFGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Server/DFGameState.h"
#include "Character/DFPlayerState.h"
#include "Engine/World.h"

ADFGameMode::ADFGameMode()
{
    // 플레이어 컨트롤러를 사용자 정의 DFPlayerController로 지정
    PlayerControllerClass = ADFMenuController::StaticClass();

    // 플레이어 상태를 사용자 정의 DFPlayerState로 지정
    PlayerStateClass = ADFPlayerState::StaticClass();

    // 게임 상태를 사용자 정의 DFGameState로 지정
    GameStateClass = ADFGameState::StaticClass();

    // Listen 서버 환경에서 seamless travel을 사용하여 레벨 전환 시 접속 유지
    bUseSeamlessTravel = true;

    // 기본 세션 정보 (필요에 따라 UI에서 설정할 수 있도록)
    MaxPlayers = 4;
    RoomName = TEXT("MyLobby");

}

void ADFGameMode::BeginPlay()
{
    Super::BeginPlay();

    UE_LOG(LogTemp, Log, TEXT("Menu GameMode BeginPlay - 초기화 완료"));
}


void ADFGameMode::PostLogin(APlayerController* NewPlayer)
{
    Super::PostLogin(NewPlayer);
    UE_LOG(LogTemp, Log, TEXT("PostLogin: %s 접속"), *NewPlayer->GetName());

}

void ADFGameMode::Logout(AController* Exiting)
{
    Super::Logout(Exiting);
    UE_LOG(LogTemp, Log, TEXT("Logout: %s 접속 종료"), *Exiting->GetName());
    // 필요한 추가 처리 (예: 로비 인원 업데이트 등)를 여기서 구현
}