#include "Server/DFGameMode.h"
#include "Blueprint/UserWidget.h"
#include "Server/DFMenuController.h"
#include "Server/DFGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Server/DFGameState.h"
#include "Character/DFPlayerState.h"

ADFGameMode::ADFGameMode()
{
    // 플레이어 컨트롤러를 사용자 정의 DFPlayerController로 지정
    PlayerControllerClass = ADFMenuController::StaticClass();

    // 플레이어 상태를 사용자 정의 DFPlayerState로 지정
    PlayerStateClass = ADFPlayerState::StaticClass();

    // 게임 상태를 사용자 정의 DFGameState로 지정
    GameStateClass = ADFGameState::StaticClass();

}

void ADFGameMode::BeginPlay()
{
    Super::BeginPlay();
    // 게임 시작 시 추가 로직이 필요하다면 여기서 처리합니다.
}

void ADFGameMode::OnCharacterSelectionComplete(int32 SelectedCharacterIndex, int32 SelectedCPUCount)
{
    UE_LOG(LogTemp, Log, TEXT("Character selection complete: %d, CPU Count: %d"), SelectedCharacterIndex, SelectedCPUCount);

    // 예시로, 현재 접속한 플레이어에게 자동으로 팀 할당
    APlayerController* PC = GetWorld()->GetFirstPlayerController();
    if (PC)
    {
        // 간단한 예시: 접속한 플레이어 수를 기준으로 팀 0 또는 팀 1로 번갈아 할당
        int32 AssignedTeam = GetNumPlayers() % 2;
        AssignTeam(PC, AssignedTeam);
    }

    // 캐릭터 선택 완료 후 맵 선택 단계로 전환하는 로직(또는 이벤트)를 실행합니다.
    UE_LOG(LogTemp, Log, TEXT("Switching to Map Selection mode"));
    // 실제로는 UI에서 맵 선택이 이루어지도록 블루프린트와 연동하면 됩니다.
}

void ADFGameMode::OnMapSelectionComplete(EMapSelection SelectedMap)
{
    FString LevelName;

    // 선택한 맵에 따라 전환할 레벨 이름 결정
    switch (SelectedMap)
    {
    case EMapSelection::MapOne:
        LevelName = TEXT("MapOneLevel");
        break;
    case EMapSelection::MapTwo:
        LevelName = TEXT("MapTwoLevel");
        break;
    case EMapSelection::MapThree:
        LevelName = TEXT("MapThreeLevel");
        break;
    default:
        LevelName = TEXT("DefaultMap");
        break;
    }

    UE_LOG(LogTemp, Log, TEXT("Map selected: %s"), *LevelName);

    // 선택한 레벨로 전환
    UGameplayStatics::OpenLevel(GetWorld(), FName(*LevelName));
}

void ADFGameMode::AssignTeam(APlayerController* PlayerController, int32 TeamID)
{
    if (PlayerController && PlayerController->PlayerState)
    {
        // 사용자 정의 PlayerState (ADFPlayerState)에 TeamID 변수가 정의되어 있다고 가정합니다.
        ADFPlayerState* MyPlayerState = Cast<ADFPlayerState>(PlayerController->PlayerState);
        if (MyPlayerState)
        {
            MyPlayerState->TeamID = TeamID;
            UE_LOG(LogTemp, Log, TEXT("Assigned team %d to player %s"), TeamID, *MyPlayerState->GetPlayerName());
        }
    }
}