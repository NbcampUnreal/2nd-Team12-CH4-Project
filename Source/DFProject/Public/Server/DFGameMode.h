#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "DFGameMode.generated.h"

UCLASS()
class DFPROJECT_API ADFGameMode : public AGameModeBase
{
    GENERATED_BODY()

public:
    ADFGameMode();

protected:
    virtual void BeginPlay() override;

    // 게임 인스턴스 클래스를 블루프린트 에디터에서 지정할 수 있도록 선언 (필요한 경우)
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Game")
    TSubclassOf<class UGameInstance> GameInstanceClass;

    // 블루프린트 에디터에서 설정할 메뉴 UI 위젯 클래스
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
    TSubclassOf<class UUserWidget> MenuWidgetClass;

    // 생성된 메뉴 UI 위젯 인스턴스
    UPROPERTY()
    UUserWidget* CurrentMenuWidget;

    // 메뉴 UI 초기화를 위한 함수
    void InitializeMenu();

};