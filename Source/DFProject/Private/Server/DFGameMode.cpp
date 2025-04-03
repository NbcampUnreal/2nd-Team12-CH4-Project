#include "Server/DFGameMode.h"
#include "Blueprint/UserWidget.h"
#include "Server/DFMenuController.h"
#include "Server/DFGameInstance.h"

ADFGameMode::ADFGameMode()
{
    GameInstanceClass = UDFGameInstance::StaticClass();
    PlayerControllerClass = ADFMenuController::StaticClass();
    // 메뉴 UI 위젯 클래스 설정
    MenuWidgetClass = nullptr; // 블루프린트 에디터에서 설정할 수 있도록 초기화
    CurrentMenuWidget = nullptr; // 현재 메뉴 위젯 인스턴스 초기화
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