#include "Item/DFItemAbilityComponent.h"
#include "Item/DFBattleItem.h"
#include "Item/DFItemBaseActor.h"
#include "Item/DFItemInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/AssetManager.h"

UDFItemAbilityComponent::UDFItemAbilityComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	ParentItemData = nullptr;
	ParentActor = nullptr;
	ParentMesh = nullptr;
	ParentActionAnim = nullptr;
}

void UDFItemAbilityComponent::BeginPlay()
{
	Super::BeginPlay();

	if (AActor* OwnerActor = GetOwner())
	{
		ParentActor = Cast<ADFItemBaseActor>(OwnerActor);

		if (ParentActor)
		{
			FPrimaryAssetId ItemId = ParentActor->ItemData.ItemId;

			if (ItemId.IsValid())
			{
				UDFBattleItem* LoadedItem = Cast<UDFBattleItem>(UAssetManager::Get().GetPrimaryAssetObject(ItemId));

				if (LoadedItem)
				{
					ParentItemData = LoadedItem;
					ParentActionAnim = LoadedItem->ActionAnim;
					ParentActionsound = LoadedItem->ActionSound;
				}
			}

			if (ParentActor->ItemMesh)
			{
				ParentMesh = ParentActor->ItemMesh;
			}

		}
	}		
}

void UDFItemAbilityComponent::MainAction()
{	
	if (GetOwner()->HasAuthority())
	{
		Server_MainAction();
	}
}

void UDFItemAbilityComponent::Server_MainAction_Implementation()
{
	if (ParentMesh && ParentActionAnim)
	{
		if (UAnimInstance* AnimInstance = ParentMesh->GetAnimInstance())
		{

			float Played = AnimInstance->Montage_Play(ParentActionAnim);

		}
	}

	PlayActionSound();
}


void UDFItemAbilityComponent::PlayActionSound()
{
	if (!ParentActionsound)
	{
		return;
	}

	if (ParentActionsound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, ParentActionsound, ParentActor->GetActorLocation());
	}
}

void UDFItemAbilityComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}