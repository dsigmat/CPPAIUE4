// Fill out your copyright notice in the Description page of Project Settings.

#include "FindRandomLocation.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "NavigationSystem.h"
#include "CPP_AINPCController.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Vector.h"
#include "CPP_AIBlackboardKeys.h"

UFindRandomLocation::UFindRandomLocation(FObjectInitializer const& ObjectInitializer)
{
    NodeName = TEXT("Find Random Location");
}

EBTNodeResult::Type UFindRandomLocation::ExecuteTask(UBehaviorTreeComponent& OwnerComponent, uint8* NodeMemory)
{
    auto const Controller = Cast<ACPP_AINPCController>(OwnerComponent.GetAIOwner());
    auto const NPC = Controller->GetPawn();

    FVector const OriginLocation = NPC->GetActorLocation();
    FNavLocation NavLocation;

    UNavigationSystemV1* const NavigationSystem = UNavigationSystemV1::GetCurrent(GetWorld());

    if (NavigationSystem->GetRandomPointInNavigableRadius(OriginLocation, SearchRadius, NavLocation, nullptr))
    {
        Controller->GetBlackboard()->SetValueAsVector(bb_keys::TargetLocation, NavLocation.Location);
    }

    FinishLatentTask(OwnerComponent, EBTNodeResult::Succeeded);

    return EBTNodeResult::Succeeded;
}
