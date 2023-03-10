#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"

#include "GameFramework/Pawn.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BehaviorTreeTypes.h"
#include "UObject/UObjectGlobals.h"

#include "FindRandomLocation.generated.h"

UCLASS(Blueprintable)
class CPP_AI_API UFindRandomLocation : public UBTTask_BlackboardBase
{
    GENERATED_BODY()
public:
    UFindRandomLocation(FObjectInitializer const& ObjectInitializer);
    EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComponent, uint8* NodeMemory);

private:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Search", meta = (AllowPrivateAccess = "true"))
    float SearchRadius = 1500.0f;
};
