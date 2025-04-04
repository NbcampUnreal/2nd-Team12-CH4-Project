#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "DFGameMode.generated.h"

// Enum 예시: 메뉴 모드를 구분하기 위한 사용자 정의 타입
UENUM(BlueprintType)
enum class EMenuMode : uint8
{
    None UMETA(DisplayName = "None"),
    SinglePlayer UMETA(DisplayName = "SinglePlayer"),
    MultiPlayer UMETA(DisplayName = "MultiPlayer"),
    MapSelect   UMETA(DisplayName = "Map Selection")
};

// 맵 선택을 위한 Enum 예시
UENUM(BlueprintType)
enum class EMapSelection : uint8
{
    MapOne      UMETA(DisplayName = "Map One"),
    MapTwo      UMETA(DisplayName = "Map Two"),
    MapThree    UMETA(DisplayName = "Map Three")
};

UCLASS()
class DFPROJECT_API ADFGameMode : public AGameModeBase
{
    GENERATED_BODY()

public:
    ADFGameMode();

    virtual void BeginPlay() override;

    // 메뉴 모드를 설정하는 함수
    UFUNCTION(BlueprintCallable, Category = "Menu")
    void SetMenuMode(EMenuMode NewMode);

    // 캐릭터 선택 완료 후 호출되는 함수
    UFUNCTION(BlueprintCallable, Category = "Menu")
    void OnCharacterSelectionComplete(int32 SelectedCharacterIndex, int32 SelectedCPUCount);

    // 맵 선택 완료 후 호출되는 함수
    UFUNCTION(BlueprintCallable, Category = "Menu")
    void OnMapSelectionComplete(EMapSelection SelectedMap);

protected:
    

    // 게임 인스턴스 클래스를 블루프린트 에디터에서 지정할 수 있도록 선언 (필요한 경우)
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Game")
    TSubclassOf<class UGameInstance> GameInstanceClass;

    // 블루프린트 에디터에서 설정할 메뉴 UI 위젯 클래스
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
    TSubclassOf<class UUserWidget> MenuWidgetClass;

    // 싱글플레이 전용 UI 위젯 클래스 (블루프린트 에디터에서 지정)
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
    TSubclassOf<class UUserWidget> SinglePlayerWidgetClass;

    // 멀티플레이 전용 UI 위젯 클래스 (블루프린트 에디터에서 지정)
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
    TSubclassOf<class UUserWidget> MultiPlayerWidgetClass;

    // 생성된 메뉴 UI 위젯 인스턴스
    UPROPERTY()
    UUserWidget* CurrentMenuWidget;

    // 현재 활성화된 서브 메뉴 UI (싱글/멀티플레이 전용)
    UPROPERTY()
    UUserWidget* CurrentSubMenuWidget;

    // 현재 메뉴 모드
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Menu")
    EMenuMode CurrentMenuMode;

    // 메뉴 UI 초기화를 위한 함수
    void InitializeMenu();

};