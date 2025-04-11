#include "Item/DFPunchgunAttackComponent.h"

UDFPunchgunAttackComponent::UDFPunchgunAttackComponent()
{

}

void UDFPunchgunAttackComponent::MainAction()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, FString::Printf(TEXT("띠용")));
}