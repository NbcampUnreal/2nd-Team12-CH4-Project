#include "Item/DFPunchgunAttackComponent.h"
#include "Item/DFBattleItem.h"
#include "Item/DFItemBaseActor.h"
#include "Item/DFItemInstance.h"
#include "PhysicsEngine/PhysicalAnimationComponent.h"

UDFPunchgunAttackComponent::UDFPunchgunAttackComponent()
{

}

void UDFPunchgunAttackComponent::BeginPlay()
{
	Super::BeginPlay();

	ParentActor->PhysicalAnimComp->ApplyPhysicalAnimationProfileBelow(FName("HandGripR"), FName("ItemPhysicAnim"), true);
	ParentActor->PhysicalAnimComp->ApplyPhysicalAnimationProfileBelow(FName("HandGripL"), FName("ItemPhysicAnim"), true);
	ParentActor->PhysicalAnimComp->SetStrengthMultiplyer(2.0f);

	ParentMesh->SetAllBodiesBelowPhysicsBlendWeight(FName("Dummy"), 1.0f, true);
}

void UDFPunchgunAttackComponent::MainAction()
{
	ParentMesh->SetSimulatePhysics(false);

	Super::MainAction();

	GetWorld()->GetTimerManager().SetTimer(SimulTimerHandle, [this](){	ParentMesh->SetSimulatePhysics(true);}, 0.3f, false);

}