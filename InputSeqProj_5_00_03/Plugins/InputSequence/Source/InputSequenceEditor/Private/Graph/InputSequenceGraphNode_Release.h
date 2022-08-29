// Copyright 2022. Pentangle Studio. All rights reserved.

#pragma once

#include "EdGraph/EdGraphNode.h"
#include "InputSequenceGraphNode_Release.generated.h"

class UInputSequenceEvent;

UCLASS()
class UInputSequenceGraphNode_Release : public UEdGraphNode
{
	GENERATED_UCLASS_BODY()

public:

	virtual void AllocateDefaultPins() override;

	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;

	virtual FLinearColor GetNodeTitleColor() const override;

	virtual FText GetTooltipText() const;

	virtual void AutowireNewNode(UEdGraphPin* FromPin) override;

	virtual bool CanUserDeleteNode() const override { return false; }

	bool CanBePassedAfterTime() const { return canBePassedAfterTime; }

	float GetPassedAfterTime() const { return PassedAfterTime; }

	float GetResetAfterTime() const { return ResetAfterTime; }

	uint8 IsOverridingRequirePreciseMatch() const { return isOverridingRequirePreciseMatch; }

	uint8 RequirePreciseMatch() const { return requirePreciseMatch; }

	uint8 IsOverridingResetAfterTime() const { return isOverridingResetAfterTime; }

	uint8 IsResetAfterTime() const { return isResetAfterTime; }

	UObject* GetStateObject() const { return StateObject; }

	const FString& GetStateContext() { return StateContext; }

	const TArray<TSubclassOf<UInputSequenceEvent>>& GetEnterEventClasses() const { return EnterEventClasses; }

	const TArray<TSubclassOf<UInputSequenceEvent>>& GetPassEventClasses() const { return PassEventClasses; }

	const TArray<TSubclassOf<UInputSequenceEvent>>& GetResetEventClasses() const { return ResetEventClasses; }

protected:

	/* If true, node can be passed only after some time and will not be reset by time in any case */
	UPROPERTY(EditAnywhere, Category = "Release node", meta = (DisplayPriority = 0))
		uint8 canBePassedAfterTime : 1;

	/* Node Time interval, after which node can be passed */
	UPROPERTY(EditAnywhere, Category = "Release node", meta = (DisplayPriority = 1, UIMin = 0.01, Min = 0.01, UIMax = 10, Max = 10, EditCondition = "canBePassedAfterTime", EditConditionHides))
		float PassedAfterTime;

	/* If true, node will override it's owning asset parameters */
	UPROPERTY(EditAnywhere, Category = "Release node", meta = (DisplayPriority = 5))
		uint8 isOverridingRequirePreciseMatch : 1;

	/* If true, any mismatched input will reset asset to initial state */
	UPROPERTY(EditAnywhere, Category = "Release node", meta = (DisplayPriority = 6, EditCondition = isOverridingRequirePreciseMatch, EditConditionHides))
		uint8 requirePreciseMatch : 1;

	/* If true, node will override it's owning asset parameters */
	UPROPERTY(EditAnywhere, Category = "Release node", meta = (DisplayPriority = 10, EditCondition = "!canBePassedAfterTime", EditConditionHides))
		uint8 isOverridingResetAfterTime : 1;

	/* If true, node will reset it's owning asset if no any successful steps are made during some time interval */
	UPROPERTY(EditAnywhere, Category = "Release node", meta = (DisplayPriority = 11, EditCondition = "!canBePassedAfterTime && isOverridingResetAfterTime", EditConditionHides))
		uint8 isResetAfterTime : 1;

	/* Node Time interval, that overrides asset parameter */
	UPROPERTY(EditAnywhere, Category = "Release node", meta = (DisplayPriority = 12, UIMin = 0.01, Min = 0.01, UIMax = 10, Max = 10, EditCondition = "!canBePassedAfterTime && isOverridingResetAfterTime && isResetAfterTime", EditConditionHides))
		float ResetAfterTime;

	/* State object for Event calls when this state is reset by ticking */
	UPROPERTY(EditAnywhere, Category = "Release node", meta = (DisplayPriority = 20))
		UObject* StateObject;

	/* State context for Event calls when this state is reset by ticking */
	UPROPERTY(EditAnywhere, Category = "Release node", meta = (DisplayPriority = 21))
		FString StateContext;

	/* Event Classes to execute Event calls when this state is entered */
	UPROPERTY(EditAnywhere, Category = "Release node", meta = (DisplayPriority = 30))
		TArray<TSubclassOf<UInputSequenceEvent>> EnterEventClasses;

	/* Event Classes to execute Event calls when this state is passed */
	UPROPERTY(EditAnywhere, Category = "Release node", meta = (DisplayPriority = 31))
		TArray<TSubclassOf<UInputSequenceEvent>> PassEventClasses;

	/* Event Classes to execute Event calls when this state is reset by ticking */
	UPROPERTY(EditAnywhere, Category = "Release node", meta = (DisplayPriority = 32))
		TArray<TSubclassOf<UInputSequenceEvent>> ResetEventClasses;
};