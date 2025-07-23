#include "Data/TrinityFlowTagData.h"
#include "Engine/DataTable.h"

FCharacterTagData UTrinityFlowTagManager::GetTagData(UDataTable* TagDataTable, FName TagName)
{
    if (!TagDataTable)
    {
        return FCharacterTagData();
    }

    FCharacterTagData* FoundData = TagDataTable->FindRow<FCharacterTagData>(TagName, TEXT("GetTagData"));
    if (FoundData)
    {
        return *FoundData;
    }

    return FCharacterTagData();
}

TArray<FName> UTrinityFlowTagManager::GetAllTagNames(UDataTable* TagDataTable)
{
    TArray<FName> TagNames;
    
    if (!TagDataTable)
    {
        return TagNames;
    }

    TArray<FCharacterTagData*> AllRows;
    TagDataTable->GetAllRows<FCharacterTagData>(TEXT("GetAllTagNames"), AllRows);
    
    for (const FCharacterTagData* Row : AllRows)
    {
        if (Row)
        {
            TagNames.Add(Row->TagName);
        }
    }

    return TagNames;
}

bool UTrinityFlowTagManager::ValidateTagCombination(UDataTable* TagDataTable, const TArray<FName>& Tags)
{
    if (!TagDataTable)
    {
        return true; // No validation without data
    }

    // Check for conflicts
    for (const FName& Tag : Tags)
    {
        FCharacterTagData* TagData = TagDataTable->FindRow<FCharacterTagData>(Tag, TEXT("ValidateTag"));
        if (TagData)
        {
            // Check conflicts
            for (const FName& ConflictTag : TagData->ConflictingTags)
            {
                if (Tags.Contains(ConflictTag))
                {
                    UE_LOG(LogTemp, Warning, TEXT("Tag conflict: %s conflicts with %s"), 
                        *Tag.ToString(), *ConflictTag.ToString());
                    return false;
                }
            }

            // Check requirements
            for (const FName& RequiredTag : TagData->RequiredTags)
            {
                if (!Tags.Contains(RequiredTag))
                {
                    UE_LOG(LogTemp, Warning, TEXT("Tag requirement not met: %s requires %s"), 
                        *Tag.ToString(), *RequiredTag.ToString());
                    return false;
                }
            }
        }
    }

    return true;
}

FString UTrinityFlowTagManager::GetTagDisplayString(UDataTable* TagDataTable, const TArray<FName>& Tags)
{
    TArray<FString> DisplayNames;
    
    if (!TagDataTable)
    {
        // Convert FName array to FString array
        for (const FName& Tag : Tags)
        {
            DisplayNames.Add(Tag.ToString());
        }
        return FString::Join(DisplayNames, TEXT(", "));
    }
    
    for (const FName& Tag : Tags)
    {
        FCharacterTagData* TagData = TagDataTable->FindRow<FCharacterTagData>(Tag, TEXT("GetDisplayString"));
        if (TagData)
        {
            DisplayNames.Add(TagData->DisplayName);
        }
        else
        {
            DisplayNames.Add(Tag.ToString());
        }
    }

    return FString::Join(DisplayNames, TEXT(", "));
}