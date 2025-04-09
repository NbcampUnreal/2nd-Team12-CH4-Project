// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/DFItemInstance.h"
#include "Item/DFBattleItem.h"

void UDFItemInstance::Initialize(UDFBattleItem* SpawnItemData)
{
	ItemData = SpawnItemData;
}