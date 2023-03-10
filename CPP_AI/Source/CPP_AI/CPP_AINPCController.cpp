#include "CPP_AINPCController.h"

#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "CPP_AICharacter.h"

#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AIPerceptionStimuliSourceComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "GameFramework/Character.h"
#include "CPP_AIBlackboardKeys.h"
#include "Perception/AIPerceptionComponent.h"

ACPP_AINPCController::ACPP_AINPCController(FObjectInitializer const& ObjectInitializer)
{
    static ConstructorHelpers::FObjectFinder<UBehaviorTree> obj(TEXT("BehaviorTree'/Game/AI/NPC_BT.NPC_BT'"));
    if (obj.Succeeded())
    {
        BehaviorTree = obj.Object;
    }
    BehaviorTreeComponent = ObjectInitializer.CreateDefaultSubobject<UBehaviorTreeComponent>(this, TEXT("UBehaviorComponent"));
    Blackboard = ObjectInitializer.CreateDefaultSubobject<UBlackboardComponent>(this, TEXT("BlackboardComponent"));

    SetupPerceptionSystem();
}

void ACPP_AINPCController::BeginPlay()
{
    Super::BeginPlay();
    RunBehaviorTree(BehaviorTree);
    BehaviorTreeComponent->StartTree(*BehaviorTree);
}

void ACPP_AINPCController::OnPossess(APawn* const pawn)
{
    Super::OnPossess(pawn);

    if (Blackboard)
    {
        Blackboard->InitializeBlackboard(*BehaviorTree->BlackboardAsset);
    }
}

UBlackboardComponent* ACPP_AINPCController::GetBlackboard() const
{
    return Blackboard;
}

void ACPP_AINPCController::OnUpdated(TArray<AActor*> const& UpdatedActor) {}

void ACPP_AINPCController::OnTargetDetected(AActor* Actor, FAIStimulus const Stimulus)
{
    if (auto const character = Cast<ACPP_AICharacter>(Actor))
    {
        GetBlackboard()->SetValueAsBool(bb_keys::CanSeePlayer, Stimulus.WasSuccessfullySensed());
    }
}

void ACPP_AINPCController::SetupPerceptionSystem()
{
    SenseConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("Sense Config"));
    SetPerceptionComponent(*CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("Perception Component")));

    SenseConfig->SightRadius = 500.0f;
    SenseConfig->LoseSightRadius = SenseConfig->SightRadius + 50.0f;
    SenseConfig->PeripheralVisionAngleDegrees = 90.0f;
    SenseConfig->SetMaxAge(5.0f);
    SenseConfig->AutoSuccessRangeFromLastSeenLocation = 900.0f;
    SenseConfig->DetectionByAffiliation.bDetectEnemies = true;
    SenseConfig->DetectionByAffiliation.bDetectFriendlies = true;
    SenseConfig->DetectionByAffiliation.bDetectNeutrals = true;

    GetPerceptionComponent()->SetDominantSense(*SenseConfig->GetSenseImplementation());
    GetPerceptionComponent()->OnTargetPerceptionUpdated.AddDynamic(this, &ACPP_AINPCController::OnTargetDetected);
    GetPerceptionComponent()->ConfigureSense(*SenseConfig);
}
