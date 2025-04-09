#include "Character/DFPlayerState.h"
#include "Net/UnrealNetwork.h"

ADFPlayerState::ADFPlayerState()
{
    // 기본적으로 팀 번호를 -1로 초기화(할당되지 않음을 의미)
    TeamID = -1;
    IndividualScore = 0; // 점수 초기화
}

void ADFPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(ADFPlayerState, TeamID);
    DOREPLIFETIME(ADFPlayerState, IndividualScore);
}