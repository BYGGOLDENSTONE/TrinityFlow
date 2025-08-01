#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "TrinityFlowPlayerController.generated.h"

UCLASS()
class TRINITYFLOW_API ATrinityFlowPlayerController : public APlayerController
{
    GENERATED_BODY()

public:
    ATrinityFlowPlayerController();

    virtual void BeginPlay() override;
    virtual void SetupInputComponent() override;
};