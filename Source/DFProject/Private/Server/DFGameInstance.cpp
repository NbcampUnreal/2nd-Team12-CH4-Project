#include "Server/DFGameInstance.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "Kismet/GameplayStatics.h"

void UDFGameInstance::HostSession(FName SessionName, bool bIsLAN, int32 MaxPlayers)
{
    // OnlineSubsystem 가져오기
    IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get();
    if (Subsystem)
    {
        SessionInterface = Subsystem->GetSessionInterface();
        if (SessionInterface.IsValid())
        {
            // 이미 세션이 존재한다면 제거
            FNamedOnlineSession* ExistingSession = SessionInterface->GetNamedSession(SessionName);
            if (ExistingSession)
            {
                SessionInterface->DestroySession(SessionName);
            }

            // 세션 설정
            TSharedPtr<FOnlineSessionSettings> SessionSettings = MakeShareable(new FOnlineSessionSettings());
            SessionSettings->bIsLANMatch = bIsLAN;
            SessionSettings->NumPublicConnections = MaxPlayers;
            SessionSettings->bShouldAdvertise = true;
            SessionSettings->bUsesPresence = true;

            // 세션 생성 완료 델리게이트 바인딩
            SessionInterface->OnCreateSessionCompleteDelegates.AddUObject(this, &UDFGameInstance::OnCreateSessionComplete);

            // 세션 생성
            SessionInterface->CreateSession(0, SessionName, *SessionSettings);
        }
    }
}

void UDFGameInstance::OnCreateSessionComplete(FName InSessionName, bool bWasSuccessful)
{
    UE_LOG(LogTemp, Log, TEXT("OnCreateSessionComplete: %s, success: %d"), *InSessionName.ToString(), bWasSuccessful);

    // 델리게이트 해제
    if (SessionInterface.IsValid())
    {
        SessionInterface->OnCreateSessionCompleteDelegates.RemoveAll(this);
    }

    if (bWasSuccessful)
    {
        // 맵 오픈 (listen 서버)
        UGameplayStatics::OpenLevel(GetWorld(), TEXT("LobbyMap"), true, TEXT("listen"));
    }
}
