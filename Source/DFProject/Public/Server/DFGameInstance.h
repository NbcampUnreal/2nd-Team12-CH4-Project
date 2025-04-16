#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "OnlineSubsystem.h"
#include "DFGameInstance.generated.h"


UCLASS()
class DFPROJECT_API UDFGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void HostSession(FName SessionName, bool bIsLAN, int32 MaxPlayers);

private:
	void OnCreateSessionComplete(FName InSessionName, bool bWasSuccessful);

	IOnlineSessionPtr SessionInterface; // IOnlineSession 포인터
	
};
