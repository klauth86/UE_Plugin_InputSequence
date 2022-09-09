// Copyright 2022. Pentangle Studio. All rights reserved.

#pragma once

#include "UObject/Object.h"
#include "Templates/SubclassOf.h"
#include "InputSequenceAsset.generated.h"

enum EInputEvent;
class UEdGraph;

USTRUCT()
struct INPUTSEQUENCE_API FInputActionState
{
	GENERATED_USTRUCT_BODY()

public:

	FInputActionState(TArray<EInputEvent> inputEvents = {}, float from = 0, float to = 0) : InputEvents(inputEvents), Index(INDEX_NONE), From(from), To(to) {}

	bool IsOpen_Action() const { return !InputEvents.IsValidIndex(Index + 1); }

	bool ConsumeInput_Action(const EInputEvent inputEvent)
	{
		if (InputEvents.IsValidIndex(Index + 1) && InputEvents[Index + 1] == inputEvent) { Index++; return true; }
		return false;
	}

	bool IsOpen_Axis() const { return Index >= 0; }

	bool ConsumeInput_Axis(float axisValue)
	{
		if (From <= axisValue && axisValue <= To) { Index++; return true; }
		return false;
	}

	bool IsOpen(bool isAxis) const { return isAxis ? IsOpen_Axis() : IsOpen_Action(); }

	void Reset() { Index = INDEX_NONE; }

protected:

	UPROPERTY()
		TArray<TEnumAsByte<EInputEvent>> InputEvents;

	int32 Index;

	UPROPERTY()
		float From;

	UPROPERTY()
		float To;
};

USTRUCT()
struct INPUTSEQUENCE_API FInputSequenceState
{
	GENERATED_USTRUCT_BODY()

public:

	FInputSequenceState()
	{
		AccumulatedTime = 0;

		InputActions.Reset();
		EnterEventClasses.Reset();
		PassEventClasses.Reset();
		ResetEventClasses.Reset();
		NextIndice.Reset();
		FirstLayerParentIndex = INDEX_NONE;

		StateObject = nullptr;
		StateContext = "";

		IsInputNode = 0;
		IsAxisNode = 0;

		canBePassedAfterTime = 0;

		isOverridingResetAfterTime = 0;
		isResetAfterTime = 0;

		isOverridingRequirePreciseMatch = 0;
		requirePreciseMatch = 0;

		TimeParam = 0;
	}

	bool IsStartNode() const { return FirstLayerParentIndex == INDEX_NONE; }

	bool IsFirstLayer() const { return FirstLayerParentIndex == 0; }

	bool IsEmpty() const { return InputActions.Num() == 0; }

	bool IsOpen() const
	{
		for (const TPair<FName, FInputActionState>& inputActionEntry : InputActions)
		{
			const FName& actionName = inputActionEntry.Key;
			const FInputActionState& inputActionState = inputActionEntry.Value;

			if (!inputActionState.IsOpen(IsAxisNode)) return false;
		}

		return true;
	}

	bool ConsumeInput(const TMap<FName, TEnumAsByte<EInputEvent>> inputActionEvents, const TMap<FName, float>& inputAxisEvents)
	{
		bool result = false;

		for (TPair<FName, FInputActionState>& inputActionEntry : InputActions)
		{
			const FName& actionName = inputActionEntry.Key;
			FInputActionState& inputActionState = inputActionEntry.Value;

			if (IsAxisNode && inputAxisEvents.Contains(actionName) && !inputActionState.IsOpen(IsAxisNode) ||
				!IsAxisNode && inputActionEvents.Contains(actionName) && !inputActionState.IsOpen(IsAxisNode))
			{
				if (IsAxisNode && inputActionState.ConsumeInput_Axis(inputAxisEvents[actionName]) ||
					!IsAxisNode && inputActionState.ConsumeInput_Action(inputActionEvents[actionName]))
				{
					AccumulatedTime = 0;
					result = true;
				}
			}
		}

		return result;
	}

	void Reset()
	{
		AccumulatedTime = 0;
		for (TPair<FName, FInputActionState>& inputActionEntry : InputActions) inputActionEntry.Value.Reset();
	}

	float AccumulatedTime;

	UPROPERTY()
		TMap<FName, FInputActionState> InputActions;
	UPROPERTY()
		TArray<TSubclassOf<UInputSequenceEvent>> EnterEventClasses;
	UPROPERTY()
		TArray<TSubclassOf<UInputSequenceEvent>> PassEventClasses;
	UPROPERTY()
		TArray<TSubclassOf<UInputSequenceEvent>> ResetEventClasses;
	UPROPERTY()
		TSet<int32> NextIndice;
	UPROPERTY()
		int32 FirstLayerParentIndex;

	UPROPERTY()
		UObject* StateObject;
	UPROPERTY()
		FString StateContext;

	UPROPERTY()
		uint8 IsInputNode : 1;
	UPROPERTY()
		uint8 IsAxisNode : 1;

	UPROPERTY()
		uint8 canBePassedAfterTime : 1;

	UPROPERTY()
		uint8 isOverridingResetAfterTime : 1;
	UPROPERTY()
		uint8 isResetAfterTime : 1;

	UPROPERTY()
		uint8 isOverridingRequirePreciseMatch : 1;
	UPROPERTY()
		uint8 requirePreciseMatch : 1;

	UPROPERTY()
		float TimeParam;
};

USTRUCT(BlueprintType)
struct INPUTSEQUENCE_API FInputSequenceResetSource
{
	GENERATED_USTRUCT_BODY()

public:

	FInputSequenceResetSource() :SourceIndex(INDEX_NONE), SourceObject(nullptr), SourceContext("") {}

	/* If reset is requested by one of the states */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input Sequence Reset State")
		int32 SourceIndex;

	/* If reset is requested by external call */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input Sequence Reset State")
		UObject* SourceObject;

	/* If reset is requested by external call */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input Sequence Reset State")
		FString SourceContext;
};

USTRUCT(BlueprintType)
struct INPUTSEQUENCE_API FInputSequenceEventCall
{
	GENERATED_USTRUCT_BODY()

public:

	FInputSequenceEventCall() : EventClass(nullptr), Object(nullptr), Context("") {}
	
	/* Event Class for this Event call */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input Sequence Event Call")
		TSubclassOf<UInputSequenceEvent> EventClass;

	/* State index for this Event call */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input Sequence Event Call")
		int32 Index;

	/* State object for this Event call */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input Sequence Event Call")
		UObject* Object;

	/* State context for this Event call */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input Sequence Event Call")
		FString Context;

	/* Reset sources for this Event call */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input Sequence Event Call")
		TArray<FInputSequenceResetSource> ResetSources;
};

UCLASS(Abstract, editinlinenew, BlueprintType, Blueprintable)
class INPUTSEQUENCE_API UInputSequenceEvent : public UObject
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, Category = "Input Sequence Event")
		static void OnExecuteByClass(const TSubclassOf<UInputSequenceEvent>& eventClass, int32 index, UObject* callingObject, const FString& callingContext, UObject* stateObject, const FString& stateContext, const TArray<FInputSequenceResetSource>& resetSources)
	{
		if (eventClass)
		{
			if (UInputSequenceEvent* eventObject = eventClass->GetDefaultObject<UInputSequenceEvent>())
			{
				eventObject->NativeOnExecute(index, callingObject, callingContext, stateObject, stateContext, resetSources);
			}
		}
	}

	UFUNCTION(BlueprintImplementableEvent, BlueprintCosmetic, Category = "Input Sequence Event")
		void OnExecute(int32 index, UObject* callingObject, const FString& callingContext, UObject* stateObject, const FString& stateContext, const TArray<FInputSequenceResetSource>& resetSources);

	virtual void NativeOnExecute(int32 index, UObject* callingObject, const FString& callingContext, UObject* stateObject, const FString& stateContext, const TArray<FInputSequenceResetSource>& resetSources)
	{
		OnExecute(index, callingObject, callingContext, stateObject, stateContext, resetSources);
	}
};

UCLASS(BlueprintType)
class INPUTSEQUENCE_API UInputSequenceAsset : public UObject
{
	GENERATED_UCLASS_BODY()

public:

	UFUNCTION(BlueprintCallable, Category = "Input Sequence Asset")
		void OnInput(const float DeltaTime, const bool bGamePaused, const TMap<FName, TEnumAsByte<EInputEvent>>& inputActionEvents, const TMap<FName, float>& inputAxisEvents, TArray<FInputSequenceEventCall>& outEventCalls);

	UFUNCTION(BlueprintCallable, Category = "Input Sequence Asset")
		void RequestReset(UObject* sourceObject, const FString& sourceContext);

protected:

	void MakeTransition(int32 fromIndex, const TSet<int32>& nextIndice, TArray<FInputSequenceEventCall>& outEventCalls);

	void EnterNode(int32 nextIndex, TArray<FInputSequenceEventCall>& outEventCalls);

	void PassNode(int32 fromIndex, TArray<FInputSequenceEventCall>& outEventCalls);

	void RequestReset(int32 sourceIndex);

	void ProcessResetSources(TArray<FInputSequenceEventCall>& outEventCalls);

public:

#if WITH_EDITORONLY_DATA

	UPROPERTY()
		UEdGraph* EdGraph;

#endif

	UPROPERTY()
		TArray<FInputSequenceState> States;

protected:

	mutable FCriticalSection resetSourcesCS;

	TSet<int32> ActiveIndice;

	/* Reset sources for this Event call */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Input Sequence Asset", meta = (DisplayPriority = 20))
		TArray<FInputSequenceResetSource> ResetSources;

	/* Asset Time interval, after which asset will be reset to initial state if no any successful steps will be made during that period */
	UPROPERTY(EditAnywhere, BlueprintReadWRite, Category = "Input Sequence Asset", meta = (DisplayPriority = 2, UIMin = 0.01, Min = 0.01, UIMax = 10, Max = 10, EditCondition = isResetAfterTime, EditConditionHides))
		float ResetAfterTime;

	/* If true, any mismatched input will reset asset to initial state */
	UPROPERTY(EditAnywhere, BlueprintReadWRite, Category = "Input Sequence Asset", meta = (DisplayPriority = 0))
		uint8 requirePreciseMatch : 1;

	/* If true, asset will be reset if no any successful steps are made during some time interval */
	UPROPERTY(EditAnywhere, BlueprintReadWRite, Category = "Input Sequence Asset", meta = (DisplayPriority = 1))
		uint8 isResetAfterTime : 1;

	/* If true, active states will continue to try stepping further even if Game is paused (Input Sequence Asset is stepping by OnInput method call) */
	UPROPERTY(EditAnywhere, BlueprintReadWRite, Category = "Input Sequence Asset", meta = (DisplayPriority = 10))
		uint8 bStepFromStatesWhenGamePaused : 1;

	/* If true, active states will continue to tick even if Game is paused (Input Sequence Asset is ticking by OnInput method call) */
	UPROPERTY(EditAnywhere, BlueprintReadWRite, Category = "Input Sequence Asset", meta = (DisplayPriority = 11))
		uint8 bTickStatesWhenGamePaused : 1;
};