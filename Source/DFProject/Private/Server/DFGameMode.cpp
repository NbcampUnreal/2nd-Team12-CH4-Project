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

void ADFGameMode::OnCharacterSelectionComplete(int32 SelectedCharacterIndex, int32 SelectedCPUCount)
{
    UE_LOG(LogTemp, Log, TEXT("캐릭터 선택 완료: 인덱스=%d, CPU Count=%d"), SelectedCharacterIndex, SelectedCPUCount);

    // 모든 플레이어에게 팀을 할당 (예: 접속한 순서대로 번갈아 할당)
    int32 TeamCounter = 0;
    for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
    {
        APlayerController* PC = It->Get();
        if (PC && PC->PlayerState)
        {
            int32 AssignedTeam = TeamCounter % 2;
            AssignTeam(PC, AssignedTeam);
            UE_LOG(LogTemp, Log, TEXT("플레이어 %s에게 팀 %d 할당됨"), *PC->PlayerState->GetPlayerName(), AssignedTeam);
            TeamCounter++;
        }
    }

    // 메뉴 단계에서 세션 생성(리슨 서버) 호출: GameInstance를 통해 HostSession() 호출
    UDFGameInstance* GI = Cast<UDFGameInstance>(GetGameInstance());
    if (GI)
    {
        // 예시: RoomName, LAN 여부 false, 최대 플레이어 수 MaxPlayers
        GI->HostSession(FName(*RoomName), false, MaxPlayers);
        UE_LOG(LogTemp, Log, TEXT("HostSession() 호출됨 - Listen 서버로 세션 생성 시도 (RoomName=%s)"), *RoomName);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("GameInstance 캐스팅 실패"));
    }

    // 맵 전환은 별도 UI 위젯 블루프린트에서 처리하도록 설계합니다.
    UE_LOG(LogTemp, Log, TEXT("캐릭터 선택 완료 후, 맵 선택 UI로 전환 예정"));
}

void ADFGameMode::AssignTeam(APlayerController* PlayerController, int32 TeamID)
{
    if (PlayerController && PlayerController->PlayerState)
    {
        ADFPlayerState* MyPlayerState = Cast<ADFPlayerState>(PlayerController->PlayerState);
        if (MyPlayerState)
        {
            MyPlayerState->TeamID = TeamID;
            UE_LOG(LogTemp, Log, TEXT("플레이어 %s에게 팀 %d 할당"), *MyPlayerState->GetPlayerName(), TeamID);
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("AssignTeam: ADFPlayerState 캐스팅 실패"));
        }
    }
}

void ADFGameMode::PostLogin(APlayerController* NewPlayer)
{
    Super::PostLogin(NewPlayer);
    UE_LOG(LogTemp, Log, TEXT("PostLogin: %s 접속"), *NewPlayer->GetName());

    // 새 플레이어 접속 시 자동으로 팀 할당 (간단한 예제: 접속 순서에 따라 번갈아 할당)
    int32 AssignedTeam = GetNumPlayers() % 2;
    AssignTeam(NewPlayer, AssignedTeam);
}

void ADFGameMode::Logout(AController* Exiting)
{
    Super::Logout(Exiting);
    UE_LOG(LogTemp, Log, TEXT("Logout: %s 접속 종료"), *Exiting->GetName());
    // 필요한 추가 처리 (예: 로비 인원 업데이트 등)를 여기서 구현
}