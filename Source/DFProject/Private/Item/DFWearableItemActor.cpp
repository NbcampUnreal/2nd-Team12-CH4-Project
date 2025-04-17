#include "Item/DFWearableItemActor.h"
#include "Item/DFWearableItem.h"

ADFWearableItemActor::ADFWearableItemActor()
{
	PrimaryActorTick.bCanEverTick = false;

	Scene = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	SetRootComponent(Scene);

	PreviewMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("PreviewMesh"));
	PreviewMesh->SetupAttachment(Scene);
}

void ADFWearableItemActor::SetActor(UDFWearableItem* ItemDataToShow)
{
	if (!ItemDataToShow || !ItemDataToShow->ItemMesh)
	{
		return;
	}

	WearableItemData = ItemDataToShow;
	FName SocketName = WearableItemData->EquipSocketName;

	ItemMesh->SetSkeletalMesh(WearableItemData->ItemMesh);
	ItemMesh->AttachToComponent(PreviewMesh, FAttachmentTransformRules::SnapToTargetIncludingScale, SocketName);
	ItemMesh->SetCollisionProfileName(TEXT("Item"));

	if (WearableItemData->ItemType == EItemType::Fabric)
	{
		ItemMesh->SetAllBodiesBelowSimulatePhysics(TEXT("FabricBone"), true, true);
		ItemMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		ItemMesh->SetCollisionResponseToAllChannels(ECR_Ignore);
		ItemMesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
	}
}

FText ADFWearableItemActor::GetItemName() const
{
	return WearableItemData->DisplayItemName;
}

int32 ADFWearableItemActor::GetItemPrice() const
{
	return WearableItemData->ItemPrice;
}


