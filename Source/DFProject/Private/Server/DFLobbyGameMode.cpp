#include "Server/DFLobbyGameMode.h"
#include "Server/DFMenuController.h"
#include "Server/DFGameInstance.h"
#include "Character/DFPlayerState.h"

ADFLobbyGameMode::ADFLobbyGameMode()
	: NextSlotIndex(0)
{
	// PlayerStateClass, PlayerControllerClass 등을 여기서 지정해도 됩니다.
	PlayerStateClass = ADFPlayerState::StaticClass();
}
void ADFLobbyGameMode::PostLogin(APlayerController* NewPlayer)
{

	Super::PostLogin(NewPlayer);

    if (ADFPlayerState* PS = Cast<ADFPlayerState>(NewPlayer->PlayerState))
    {
        // 아직 슬롯이 안 정해져 있다면 (UI에서 안 고른 경우에만)
        if (PS->SlotIndex == INDEX_NONE)
        {
            PS->SlotIndex = NextSlotIndex++;
            UE_LOG(LogTemp, Log, TEXT("Assigned Slot %d to %s"), PS->SlotIndex, *PS->GetPlayerName());
        }
    }
}
