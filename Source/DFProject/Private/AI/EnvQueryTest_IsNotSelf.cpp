#include "AI/EnvQueryTest_IsNotSelf.h"
#include "EnvironmentQuery/Items/EnvQueryItemType_Actor.h"
#include "GameFramework/Actor.h"

UEnvQueryTest_IsNotSelf::UEnvQueryTest_IsNotSelf()
{
	ValidItemType = UEnvQueryItemType_Actor::StaticClass();
	TestPurpose = EEnvTestPurpose::Filter;
	Cost = EEnvTestCost::Low;
	FilterType = EEnvTestFilterType::Match; 
}

void UEnvQueryTest_IsNotSelf::RunTest(FEnvQueryInstance& QueryInstance) const
{
	UObject* QueryOwner = QueryInstance.Owner.Get();
	AActor* Querier = Cast<AActor>(QueryOwner);
	if (!Querier) return;

	for (FEnvQueryInstance::ItemIterator It(this, QueryInstance); It; ++It)
	{
		AActor* ItemActor = GetItemActor(QueryInstance, It.GetIndex());
		bool bPass = ItemActor != nullptr && ItemActor != Querier;

		It.SetScore(TestPurpose, FilterType, bPass, 1.0f);
	}
}