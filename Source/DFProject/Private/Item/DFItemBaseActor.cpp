#include "Item/DFItemBaseActor.h"
#include "Item/DFItemInstance.h"
#include "Item/DFBattleItem.h"
#include "Item/DFItemAbilityComponent.h"
#include "Character/DFCharacter.h"
#include "Ability/Strategy/AbilityStrategy.h"
#include "Ability/Strategy/AbilityStrategyManager.h"
#include "Character/BodyPart/AttachInfoComponent.h"
#include "Components/SphereComponent.h"
#include "PhysicsEngine/PhysicalAnimationComponent.h"
#include "Net/UnrealNetwork.h"
#include "Engine/AssetManager.h"

ADFItemBaseActor::ADFItemBaseActor()
{
	bReplicates = true;
	SetReplicateMovement(true);
	PrimaryActorTick.bCanEverTick = false;


	ItemMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("RootItemMesh"));
	SetRootComponent(ItemMesh);

	PhysicalAnimComp = CreateDefaultSubobject<UPhysicalAnimationComponent>(TEXT("PhysicsAnimComp"));

	GripArea = CreateDefaultSubobject<USphereComponent>(TEXT("GripArea"));
	GripArea->InitSphereRadius(50.0f);
	GripArea->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	GripArea->SetCollisionObjectType(ECC_WorldDynamic);
	GripArea->SetCollisionResponseToAllChannels(ECR_Ignore);
	GripArea->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	bCanBeGrabbed = false;
}

void ADFItemBaseActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ADFItemBaseActor, ItemData);
}

void ADFItemBaseActor::BeginPlay()
{
	Super::BeginPlay();	

	if (HasAuthority())
	{
		GripArea->OnComponentBeginOverlap.AddDynamic(this, &ADFItemBaseActor::OnGripAreaBeginOverlap);
		GripArea->OnComponentEndOverlap.AddDynamic(this, &ADFItemBaseActor::OnGripAreaEndOverlap);
	}
}

void ADFItemBaseActor::OnRep_ItemData()
{
	SetupItem(ItemData);
}

void ADFItemBaseActor::SetupItem(const FItemInstanceData& InData)
{

	if (!InData.ItemId.IsValid())
	{
		return;
	}

	UDFBattleItem* LoadedItem = Cast<UDFBattleItem>(UAssetManager::Get().GetPrimaryAssetObject(InData.ItemId));

	if (!LoadedItem || !LoadedItem->ItemMesh)
	{
		return;
	}
	
	DataAssetInfo = LoadedItem;

	ItemMesh->SetSkeletalMesh(LoadedItem->ItemMesh);	
	ItemMesh->SetAnimationMode(EAnimationMode::AnimationBlueprint);
	ItemMesh->SetAnimInstanceClass(LoadedItem->AnimBP);
	ItemMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	ItemMesh->SetCollisionResponseToChannel(ECC_Visibility, ECollisionResponse::ECR_Overlap);
	ItemMesh->SetCollisionResponseToChannel(ECC_PhysicsBody, ECollisionResponse::ECR_Overlap);
	ItemMesh->SetGenerateOverlapEvents(true);
	ItemMesh->SetIsReplicated(true);
	ItemMesh->SetSimulatePhysics(true);

	if (LoadedItem->AssetType != FPrimaryAssetType("BattleItem"))
	{
		PhysicalAnimComp->DestroyComponent();
		GripArea->DestroyComponent();
		return;
	}

	PhysicalAnimComp->SetSkeletalMeshComponent(ItemMesh);

	TArray<FName> SocketNames = ItemMesh->GetAllSocketNames();
	if (SocketNames.Contains(FName("HandGripSocket")))
	{
		GripArea->AttachToComponent(ItemMesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("HandGripSocket"));
	}

	UAttachInfoComponent* AttachInfo = NewObject<UAttachInfoComponent>(this);
	AttachInfo->RegisterComponent();


	if (SocketNames.Contains(FName("ColliderBoneSocket")))
	{
		AttachInfo->AttachToComponent(ItemMesh, FAttachmentTransformRules::SnapToTargetIncludingScale, FName("ColliderBoneSocket"));
	}

	for (TSubclassOf<UDFItemAbilityComponent> Ability : LoadedItem->Abilities)
	{
		if (!Ability)
		{
			continue;
		}

		UDFItemAbilityComponent* NewAbility = NewObject<UDFItemAbilityComponent>(this, Ability);
		NewAbility->RegisterComponent();
		AddInstanceComponent(NewAbility);
		NewAbility->Activate(true);
		ItemAbilities.Add(NewAbility);
		
	}
	
	if (LoadedItem->CharacterAbility)
	{
		OwnerCharacterAbility = LoadedItem->CharacterAbility;
	}
}

void ADFItemBaseActor::OnGripAreaBeginOverlap(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult)
{
	if (OtherActor->IsA(ADFCharacter::StaticClass()))
	{
		bCanBeGrabbed = true;
	}
}

void ADFItemBaseActor::OnGripAreaEndOverlap(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex)
{
	if (OtherActor->IsA(ADFCharacter::StaticClass()))
	{
		bCanBeGrabbed = false;
	}
}

void ADFItemBaseActor::AbilitiesMainAction()
{
	for (UDFItemAbilityComponent* Ability : ItemAbilities)
	{
		if (IsValid(Ability))
		{
			Ability->MainAction();
		}

	}
}

TSubclassOf<UAbilityStrategy> ADFItemBaseActor::GetCharacterAbility() const
{
	return OwnerCharacterAbility;
}

FText ADFItemBaseActor::GetItemName() const
{
	return DataAssetInfo->DisplayItemName;
}

int32 ADFItemBaseActor::GetItemPrice() const
{
	return DataAssetInfo->ItemPrice;
}

void ADFItemBaseActor::OnGrabbedBy_Implementation(AActor* Grabber, UPhysicsConstraintComponent* GrabbersConstraint)  
{
	if (!Grabber) return;
	// 캐릭터고, 제대로 잡았다면 어빌리티 부여  
	ADFCharacter* Character = Cast<ADFCharacter>(Grabber);  
	if (Character && bCanBeGrabbed)  
	{  
		Character->AbilityManager->RegisterAbility("UseItem", NewObject<UAbilityStrategy>(Character, OwnerCharacterAbility));  
	}  
}  
  
void ADFItemBaseActor::OnGrabReleasedBy_Implementation(AActor* Grabber)  
{  
	if (!Grabber) return;
	// 캐릭터고, 제대로 잡았다면 어빌리티 제거  
	ADFCharacter* Character = Cast<ADFCharacter>(Grabber);  
	if (Character && bCanBeGrabbed)  
	{  
		Character->AbilityManager->RemoveAbility("UseItem");  
	}  
}  
  
UPrimitiveComponent* ADFItemBaseActor::GetRoot_Implementation()  
{  
	return ItemMesh;  
}  
  
TArray<FName> ADFItemBaseActor::GetGrabSocketNames_Implementation() const  
{  
	return ItemMesh->GetAllSocketNames();  
}