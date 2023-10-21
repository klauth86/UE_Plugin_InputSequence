// Copyright 2022 Pentangle Studio Licensed under the Apache License, Version 2.0 (the «License»);

#pragma once

#include "Graph/InputSequenceGraphNode_Dynamic.h"
#include "InputSequenceGraphNode_Input.generated.h"

class UInputSequenceEvent;

UCLASS()
class UInputSequenceGraphNode_Input : public UInputSequenceGraphNode_Dynamic
{
	GENERATED_UCLASS_BODY()

public:

	virtual void AllocateDefaultPins() override;

	virtual FLinearColor GetNodeTitleColor() const override;

	bool IsOverridingRequirePreciseMatch() const { return isOverridingRequirePreciseMatch; }

	bool RequirePreciseMatch() const { return requirePreciseMatch; }

	bool IsOverridingResetAfterTime() const { return isOverridingResetAfterTime; }

	bool IsResetAfterTime() const { return isResetAfterTime; }

	float GetResetAfterTime() const { return ResetAfterTime; }

	UObject* GetStateObject() const { return StateObject; }

	const FString& GetStateContext() { return StateContext; }

	const TArray<TSubclassOf<UInputSequenceEvent>>& GetEnterEventClasses() const { return EnterEventClasses; }

	const TArray<TSubclassOf<UInputSequenceEvent>>& GetPassEventClasses() const { return PassEventClasses; }

	const TArray<TSubclassOf<UInputSequenceEvent>>& GetResetEventClasses() const { return ResetEventClasses; }

	TMap<FName, TObjectPtr<UObject>>& GetPinsInputActions() { return PinsInputActions; }

protected:

	UPROPERTY()
		uint8 EditConditionIndex;

	/* If true, node can be passed only after some time and will not be reset by time in any case */
	UPROPERTY(EditAnywhere, Category = "Release node", meta = (DisplayPriority = 0, EditCondition = "EditConditionIndex > 1", EditConditionHides))
		uint8 canBePassedAfterTime : 1;

	/* If true, node will override it's owning asset parameters */
	UPROPERTY(EditAnywhere, Category = "Release node", meta = (DisplayPriority = 5, EditCondition = "EditConditionIndex > 0", EditConditionHides))
		uint8 isOverridingRequirePreciseMatch : 1;

	/* If true, any mismatched input will reset asset to initial state */
	UPROPERTY(EditAnywhere, Category = "Release node", meta = (DisplayPriority = 6, EditCondition = "EditConditionIndex > 0 && isOverridingRequirePreciseMatch", EditConditionHides))
		uint8 requirePreciseMatch : 1;

	/* If true, node will override it's owning asset parameters */
	UPROPERTY(EditAnywhere, Category = "Release node", meta = (DisplayPriority = 10, EditCondition = "EditConditionIndex <= 1 || !canBePassedAfterTime", EditConditionHides))
		uint8 isOverridingResetAfterTime : 1;

	/* If true, node will reset it's owning asset if no any successful steps are made during some time interval */
	UPROPERTY(EditAnywhere, Category = "Release node", meta = (DisplayPriority = 11, EditCondition = "isOverridingResetAfterTime && !canBePassedAfterTime || EditConditionIndex <= 1", EditConditionHides))
		uint8 isResetAfterTime : 1;

	/* Node Time interval, that overrides asset parameter */
	UPROPERTY(EditAnywhere, Category = "Release node", meta = (DisplayPriority = 12, UIMin = 0.01, Min = 0.01, UIMax = 10, Max = 10, EditCondition = "isResetAfterTime && isOverridingResetAfterTime && !canBePassedAfterTime || EditConditionIndex <= 1", EditConditionHides))
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

	UPROPERTY()
		TMap<FName, TObjectPtr<UObject>> PinsInputActions;
};