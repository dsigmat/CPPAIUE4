#include "FindPlayerLocation.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "NavigationSystem.h"
#include "CPP_AINPCController.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Vector.h"
#include "CPP_AIBlackboardKeys.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"

UFindPlayerLocation::UFindPlayerLocation(FObjectInitializer const& ObjectInitializer)
{
    NodeName = TEXT("Find Player Location");
}

EBTNodeResult::Type UFindPlayerLocation::ExecuteTask(UBehaviorTreeComponent& OwnerComponent, uint8* NodeMemory)
{
    ACharacter* const Player = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
    auto const Controller = Cast<ACPP_AINPCController>(OwnerComponent.GetAIOwner());
    FVector const PlayerLocation = Player->GetActorLocation();

    if (SearchRandom)
    {
        FNavLocation NavLocation;
        UNavigationSystemV1* const NavigationSystem = UNavigationSystemV1::GetCurrent(GetWorld());

        if (NavigationSystem->GetRandomPointInNavigableRadius(PlayerLocation, SearchRadius, NavLocation, nullptr))
        {
            Controller->GetBlackboard()->SetValueAsVector(bb_keys::TargetLocation, NavLocation.Location);
        }
    }
    else
    {
        Controller->GetBlackboard()->SetValueAsVector(bb_keys::TargetLocation, PlayerLocation);
    }

    FinishLatentTask(OwnerComponent, EBTNodeResult::Succeeded);
    return EBTNodeResult::Succeeded;
}
