// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/EnvQueryTest_IsEnemy.h"
#include "EnvironmentQuery/EnvQueryTypes.h"
#include "EnvironmentQuery/Items/EnvQueryItemType_Actor.h"
#include "Character/DFCharacter.h"
#include "DFProject.h"

UEnvQueryTest_IsEnemy::UEnvQueryTest_IsEnemy()
{
	Cost = EEnvTestCost::Low;
	ValidItemType = UEnvQueryItemType_Actor::StaticClass();
	TestPurpose = EEnvTestPurpose::Filter;
}

//void UEnvQueryTest_IsEnemy::RunTest(FEnvQueryInstance& QueryInstance) const
//{
//	UObject* QuerierObject = QueryInstance.Owner.Get();
//	AActor* QuerierActor = Cast<AActor>(QuerierObject);
//
//	if (!QuerierActor)
//	{
//		return;
//	}
//
//	const ADFCharacter* QuerierChar = Cast<ADFCharacter>(QuerierActor);
//	if (!QuerierChar)
//	{
//		return;
//	}
//
//	const int32 MyTeamID = QuerierChar->TeamID;
//
//	for (FEnvQueryInstance::ItemIterator It(this, QueryInstance); It; ++It)
//	{
//		AActor* ItemActor = GetItemActor(QueryInstance, It.GetIndex());
//		const ADFCharacter* TargetChar = Cast<ADFCharacter>(ItemActor);
//
//		if (!TargetChar)
//		{
//			It.ForceFailed();
//			continue;
//		}
//
//		const bool bIsEnemy = (TargetChar->TeamID != MyTeamID);
//		It.SetScore(TestPurpose, FilterType, bIsEnemy, true);
//
//		LOG_WARNING(TEXT("IsEnemyTest: Querier Team %d vs Target Team %d → %s"),
//			MyTeamID, TargetChar->TeamID,
//			bIsEnemy ? TEXT("Enemy") : TEXT("Team"));
//	}
//}