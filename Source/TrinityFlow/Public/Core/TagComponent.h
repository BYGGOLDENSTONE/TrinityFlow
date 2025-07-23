#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TrinityFlowTypes.h"
#include "TagComponent.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class TRINITYFLOW_API UTagComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UTagComponent();

    UFUNCTION()
    void AddTag(ECharacterTag Tag);

    UFUNCTION()
    void RemoveTag(ECharacterTag Tag);

    UFUNCTION()
    bool HasTag(ECharacterTag Tag) const;

    UFUNCTION()
    ECharacterTag GetTags() const { return Tags; }

    UFUNCTION()
    void SetTags(ECharacterTag NewTags) { Tags = NewTags; }

protected:
    UPROPERTY()
    ECharacterTag Tags = ECharacterTag::None;
};