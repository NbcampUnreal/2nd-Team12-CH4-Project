#include "Item/DFItemBaseActor.h"
#include "Item/DFItemInstance.h"
#include "Item/DFBattleItem.h"
#include "Item/DFItemAbilityComponent.h"
#include "Character/BodyPart/AttachInfoComponent.h"
#include "Components/SphereComponent.h"
#include "PhysicsEngine/PhysicalAnimationComponent.h"

ADFItemBaseActor::ADFItemBaseActor()
{
	PrimaryActorTick.bCanEverTick = false;

	ItemMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("RootItemMesh"));
	SetRootComponent(ItemMesh);
	ItemMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	ItemMesh->SetCollisionObjectType(ECC_PhysicsBody);	

	PhysicalAnimComp = CreateDefaultSubobject<UPhysicalAnimationComponent>(TEXT("PhysicsAnimComp"));

	GripArea = CreateDefaultSubobject<USphereComponent>(TEXT("GripArea"));
	GripArea->InitSphereRadius(50.0f);
	GripArea->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	GripArea->SetCollisionObjectType(ECC_WorldDynamic);
	GripArea->SetCollisionResponseToAllChannels(ECR_Ignore);
	GripArea->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	GripArea->OnComponentBeginOverlap.AddDynamic(this, &ADFItemBaseActor::OnGripAreaBeginOverlap);
	GripArea->OnComponentEndOverlap.AddDynamic(this, &ADFItemBaseActor::OnGripAreaEndOverlap);

	ItemInstance = nullptr;

	bCanBeGrabbed = false;
}

void ADFItemBaseActor::BeginPlay()
{
	Super::BeginPlay();	
}

void ADFItemBaseActor::SetupItem(UDFItemInstance* NewInstance)
{
	if (NewInstance && NewInstance->ItemData->ItemMesh)
	{		
		ItemInstance = NewInstance;
		ItemMesh->SetSkeletalMesh(NewInstance->ItemData->ItemMesh);
		ItemMesh->SetSimulatePhysics(true);
		ItemMesh->SetAnimationMode(EAnimationMode::AnimationBlueprint);
		ItemMesh->SetAnimInstanceClass(NewInstance->ItemData->AnimBP);

		if (NewInstance->ItemData->AssetType != FPrimaryAssetType("BattleItem"))
		{
			PhysicalAnimComp->DestroyComponent();
			GripArea->DestroyComponent();
			return;
		}

		PhysicalAnimComp->SetSkeletalMeshComponent(ItemMesh);

		TArray<FName> SocketNames = ItemMesh->GetAllSocketNames();
		if (SocketNames.Find(FName("HandGripSocket")))
		{
			GripArea->AttachToComponent(ItemMesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("HandGripSocket"));
		}

		UAttachInfoComponent* AttachInfo = NewObject<UAttachInfoComponent>(this);
		AttachInfo->RegisterComponent();


		if (SocketNames.Find(FName("ColliderBoneSocket")))
		{
			AttachInfo->AttachToComponent(ItemMesh, FAttachmentTransformRules::SnapToTargetIncludingScale, FName("ColliderBoneSocket"));
		}

		AttachAbilities();
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
	bCanBeGrabbed = true;
}

void ADFItemBaseActor::OnGripAreaEndOverlap(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex)
{
	bCanBeGrabbed = false;
}

void ADFItemBaseActor::AttachAbilities()
{
	if (ItemInstance)
	{
		for (TSubclassOf<UDFItemAbilityComponent> Ability : ItemInstance->ItemData->Abilities)
		{
			if (!Ability)
			{
				continue;
			}

			UDFItemAbilityComponent* NewAbility = NewObject<UDFItemAbilityComponent>(this, Ability);

			if (NewAbility)
			{
				NewAbility->RegisterComponent();
				AddInstanceComponent(NewAbility);
				NewAbility->Activate(true);
				ItemAbilities.Add(NewAbility);
			}
		}
	}
}

void ADFItemBaseActor::AbilitiesMainAction()
{
	if (ItemAbilities.Num() == 0)
	{
		return;
	}

	for (UDFItemAbilityComponent* Ability : ItemAbilities)
	{
		if (!IsValid(Ability))
		{
			continue;
		}

		Ability->MainAction();
	}
}

FName ADFItemBaseActor::GetCurrentItemId() const
{
	return ItemInstance->ItemData->GetItemId();
}