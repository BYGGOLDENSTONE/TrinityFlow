#include "Core/TagComponent.h"

UTagComponent::UTagComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void UTagComponent::AddTag(ECharacterTag Tag)
{
    Tags = Tags | Tag;
}

void UTagComponent::RemoveTag(ECharacterTag Tag)
{
    Tags = Tags & ~Tag;
}

bool UTagComponent::HasTag(ECharacterTag Tag) const
{
    return EnumHasAnyFlags(Tags, Tag);
}