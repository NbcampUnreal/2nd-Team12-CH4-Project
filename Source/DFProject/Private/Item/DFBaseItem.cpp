#include "Item/DFBaseItem.h"

UDFBaseItem::UDFBaseItem()
{

}

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