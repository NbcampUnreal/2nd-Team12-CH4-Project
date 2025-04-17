#include "Server/DFGameInstance.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "Kismet/GameplayStatics.h"

void UDFGameInstance::HostSession(FName SessionName, bool bIsLAN, int32 MaxPlayers)
{
    UE_LOG(LogTemp, Log, TEXT("HostSession() 시작: SessionName=%s, IsLAN=%s, MaxPlayers=%d"),
        *SessionName.ToString(),
        bIsLAN ? TEXT("true") : TEXT("false"),
        MaxPlayers);

    // OnlineSubsystem 가져오기
    IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get();
    if (!Subsystem)
    {
        UE_LOG(LogTemp, Error, TEXT("HostSession: OnlineSubsystem 을 찾을 수 없습니다!"));
        return;
    }
    UE_LOG(LogTemp, Log, TEXT("HostSession: OnlineSubsystem = %s"), *Subsystem->GetSubsystemName().ToString());

    // SessionInterface 얻기
    SessionInterface = Subsystem->GetSessionInterface();
    if (!SessionInterface.IsValid())
    {
        UE_LOG(LogTemp, Error, TEXT("HostSession: SessionInterface 가 유효하지 않습니다!"));
        return;
    }
    UE_LOG(LogTemp, Log, TEXT("HostSession: SessionInterface 유효"));

    // 이미 세션이 존재한다면 파괴
    if (FNamedOnlineSession* ExistingSession = SessionInterface->GetNamedSession(SessionName))
    {
        UE_LOG(LogTemp, Warning, TEXT("HostSession: 기존 세션 \"%s\" 를 파괴합니다."), *SessionName.ToString());
        SessionInterface->DestroySession(SessionName);
    }

    // 세션 설정
    TSharedPtr<FOnlineSessionSettings> SessionSettings = MakeShareable(new FOnlineSessionSettings());
    SessionSettings->bIsLANMatch = bIsLAN;
    SessionSettings->NumPublicConnections = MaxPlayers;
    SessionSettings->bShouldAdvertise = true;
    SessionSettings->bUsesPresence = true;
    UE_LOG(LogTemp, Log, TEXT("HostSession: FOnlineSessionSettings 구성 완료"));

    // 델리게이트 바인딩
    SessionInterface->OnCreateSessionCompleteDelegates.AddUObject(this, &UDFGameInstance::OnCreateSessionComplete);
    UE_LOG(LogTemp, Log, TEXT("HostSession: OnCreateSessionComplete 델리게이트 바인딩 완료"));

    // 세션 생성
    const ULocalPlayer* LocalPlayer = GetFirstGamePlayer();
    UE_LOG(LogTemp, Log, TEXT("HostSession: CreateSession 호출 (Player=%d)"), LocalPlayer ? LocalPlayer->GetControllerId() : -1);
    SessionInterface->CreateSession(
        LocalPlayer ? LocalPlayer->GetControllerId() : 0,
        SessionName,
        *SessionSettings
    );
}

void UDFGameInstance::OnCreateSessionComplete(FName InSessionName, bool bWasSuccessful)
{
    UE_LOG(LogTemp, Log, TEXT("OnCreateSessionComplete: %s, Success=%s"),
        *InSessionName.ToString(),
        bWasSuccessful ? TEXT("true") : TEXT("false"));

    // 델리게이트 해제
    if (SessionInterface.IsValid())
    {
        SessionInterface->OnCreateSessionCompleteDelegates.RemoveAll(this);
        UE_LOG(LogTemp, Log, TEXT("OnCreateSessionComplete: 델리게이트 해제"));
    }

    if (bWasSuccessful)
    {
        // 맵 오픈 (listen 서버)
        UE_LOG(LogTemp, Log, TEXT("OnCreateSessionComplete: LobbyMap 로 이동합니다 (listen)"));
        UGameplayStatics::OpenLevel(GetWorld(), TEXT("Lobby"), true, TEXT("listen"));
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("OnCreateSessionComplete: 세션 생성에 실패했습니다."));
    }
}
