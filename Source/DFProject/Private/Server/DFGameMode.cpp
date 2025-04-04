#include "Server/DFGameMode.h"
#include "Blueprint/UserWidget.h"
#include "Server/DFMenuController.h"
#include "Server/DFGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Server/DFGameState.h"

ADFGameMode::ADFGameMode()
{
	GameInstanceClass = UDFGameInstance::StaticClass(); // 게임 인스턴스 클래스 설정
	PlayerControllerClass = ADFMenuController::StaticClass(); // 플레이어 컨트롤러 클래스 설정
	GameStateClass = ADFGameState::StaticClass(); // 게임 상태 클래스 설정
    // 메뉴 UI 위젯 클래스 설정
    MenuWidgetClass = nullptr; // 블루프린트 에디터에서 설정할 수 있도록 초기화
    CurrentMenuWidget = nullptr; // 현재 메뉴 위젯 인스턴스 초기화
	CurrentSubMenuWidget = nullptr; // 현재 서브 메뉴 위젯 인스턴스 초기화
	SinglePlayerWidgetClass = nullptr; // 싱글 플레이어 전용 UI 위젯 클래스 초기화
	MultiPlayerWidgetClass = nullptr; // 멀티 플레이어 전용 UI 위젯 클래스 초기화
	CurrentMenuMode = EMenuMode::None; // 초기 메뉴 모드 설정
	// 게임 모드의 기본 클래스 설정
}

void ADFGameMode::SetMenuMode(EMenuMode NewMode)
{
    // 기존에 활성화된 서브 메뉴 위젯이 있다면 제거
    if (CurrentSubMenuWidget)
    {
        CurrentSubMenuWidget->RemoveFromParent();
        CurrentSubMenuWidget = nullptr;
    }

    CurrentMenuMode = NewMode;

    // 플레이어 컨트롤러 가져오기
    APlayerController* PC = GetWorld()->GetFirstPlayerController();
    if (!PC)
    {
        UE_LOG(LogTemp, Warning, TEXT("PlayerController not found"));
        return;
    }

    // 새 모드에 따라 적절한 UI 위젯 생성 및 뷰포트에 추가
    switch (CurrentMenuMode)
    {
    case EMenuMode::SinglePlayer:
    {
        UE_LOG(LogTemp, Log, TEXT("Switching to Single Player UI"));
        if (SinglePlayerWidgetClass)
        {
            CurrentSubMenuWidget = CreateWidget<UUserWidget>(PC, SinglePlayerWidgetClass);
        }
        break;
    }
    case EMenuMode::MultiPlayer:
    {
        UE_LOG(LogTemp, Log, TEXT("Switching to Multi Player UI"));
        if (MultiPlayerWidgetClass)
        {
            CurrentSubMenuWidget = CreateWidget<UUserWidget>(PC, MultiPlayerWidgetClass);
        }
        break;
    }
    case EMenuMode::None:
    default:
    {
        UE_LOG(LogTemp, Log, TEXT("No valid menu mode selected"));
        break;
    }
    }

    if (CurrentSubMenuWidget)
    {
        CurrentSubMenuWidget->AddToViewport();
        // UI 전용 입력 모드 재설정 (필요 시)
        FInputModeUIOnly InputMode;
        InputMode.SetWidgetToFocus(CurrentSubMenuWidget->TakeWidget());
        PC->SetInputMode(InputMode);
    }
}

void ADFGameMode::OnCharacterSelectionComplete(int32 SelectedCharacterIndex, int32 SelectedCPUCount)
{
    UE_LOG(LogTemp, Log, TEXT("Character selected: %d, CPU count: %d"), SelectedCharacterIndex, SelectedCPUCount);
    // 여기서 선택된 캐릭터와 CPU 수를 저장하거나 GameInstance에 전달할 수 있습니다.

    // 캐릭터 선택이 완료되면 맵 선택 UI로 전환
    SetMenuMode(EMenuMode::MapSelect);
}

void ADFGameMode::OnMapSelectionComplete(EMapSelection SelectedMap)
{
    FString LevelName;

    // Enum 값에 따라 전환할 레벨 이름 결정
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

void ADFGameMode::BeginPlay()
{
    Super::BeginPlay();


    // 메뉴 UI 초기화
    InitializeMenu();
}

void ADFGameMode::InitializeMenu()
{
    // 메뉴 위젯 클래스가 설정되어 있는지 확인
    if (MenuWidgetClass)
    {
        // 첫 번째 플레이어 컨트롤러 가져오기
        APlayerController* PC = GetWorld()->GetFirstPlayerController();
        if (PC)
        {
            // 플레이어 컨트롤러를 컨텍스트로 하여 위젯 생성
            CurrentMenuWidget = CreateWidget<UUserWidget>(PC, MenuWidgetClass);
            if (CurrentMenuWidget)
            {
                // 생성된 위젯을 뷰포트에 추가해 화면에 표시
                CurrentMenuWidget->AddToViewport();

                // 플레이어 컨트롤러에 마우스 커서를 보이도록 설정
                PC->bShowMouseCursor = true;

                // UI 전용 입력 모드 설정: 메뉴 위젯에 포커스를 지정하여 UI 조작이 가능하게 함
                FInputModeUIOnly InputMode;
                InputMode.SetWidgetToFocus(CurrentMenuWidget->TakeWidget());
                PC->SetInputMode(InputMode);
            }
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("MenuWidgetClass is not set in MenuGameMode"));
    }
}