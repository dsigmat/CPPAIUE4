#pragma once

#include "CoreMinimal.h"
#include "AIController.h"

#include "Perception/AIPerceptionTypes.h"

#include "CPP_AINPCController.generated.h"

class UBlackboardComponent;
class UBehaviorTree;
class UBehaviorTreeComponent;
class UAISenseConfig_Sight;

UCLASS()
class CPP_AI_API ACPP_AINPCController : public AAIController
{
    GENERATED_BODY()
public:
    ACPP_AINPCController(FObjectInitializer const& ObjectInitializer = FObjectInitializer::Get());
    void BeginPlay() override;
    void OnPossess(APawn* const pawn) override;
    UBlackboardComponent* GetBlackboard() const;

private:
    UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "AI", meta = (AllowPrivateAccess = "true"))
    UBehaviorTreeComponent* BehaviorTreeComponent;

    UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "AI", meta = (AllowPrivateAccess = "true"))
    UBehaviorTree* BehaviorTree;
    UBlackboardComponent* Blackboard;

    UAISenseConfig_Sight* SenseConfig;

    UFUNCTION()
    void OnUpdated(TArray<AActor*> const& UpdatedActor);

    void SetupPerceptionSystem();

    UFUNCTION()
    void OnTargetDetected(AActor* Actor, FAIStimulus const Stimulus);
};
