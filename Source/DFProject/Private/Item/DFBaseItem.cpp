// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/DFBaseItem.h"

//#include UE_INLINE_GENERATED_CPP_BY_NAME(DFBaseItem)


FText UDFBaseItem::GetDisplayItemName() const
{
	return DisplayItemName;
}

FPrimaryAssetId UDFBaseItem::GetPrimaryAssetId() const
{
	return FPrimaryAssetId(AssetType, GetFName());
}

FName UDFBaseItem::GetItemId() const
{
	return ItemID;
}