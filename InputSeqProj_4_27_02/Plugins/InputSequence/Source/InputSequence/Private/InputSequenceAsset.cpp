// Copyright 2022. Pentangle Studio. All rights reserved.

#include "InputSequenceAsset.h"

UInputSequenceAsset::UInputSequenceAsset(const FObjectInitializer& objInit) :Super(objInit)
{
	requirePreciseMatch = false;

	ResetAfterTime = 0.2f;
	isResetAfterTime = 0;

	bStepFromStatesWhenGamePaused = 0;
	bTickStatesWhenGamePaused = 0;

	ActiveIndice.Empty();
	ResetSources.Empty();
}

void UInputSequenceAsset::OnInput(const float DeltaTime, const bool bGamePaused, const TMap<FName, TEnumAsByte<EInputEvent>>& inputActionEvents, TArray<FInputSequenceEventCall>& outEventCalls)
{
	TArray<FInputSequenceEventCall> eventCalls;

	if (ActiveIndice.Num() <= 0) StepToNext(States[0].NextIndice, eventCalls);

	if (!bGamePaused || bStepFromStatesWhenGamePaused)
	{
		PassedIndice.Empty();
		NextIndice.Empty();

		for (int32 activeIndex : ActiveIndice) StepFrom(activeIndex, inputActionEvents, eventCalls, true);

		for (int32 passedIndex : PassedIndice)
		{
			for (const TSubclassOf<UInputSequenceEvent>& passEventClass : States[passedIndex].PassEventClasses)
			{
				int32 emplacedIndex = eventCalls.Emplace();
				eventCalls[emplacedIndex].EventClass = passEventClass;
				eventCalls[emplacedIndex].Object = States[passedIndex].StateObject;
				eventCalls[emplacedIndex].Context = States[passedIndex].StateContext;
			}

			ActiveIndice.Remove(passedIndex);
		}
		
		StepToNext(NextIndice, eventCalls);
	}

	if (!bGamePaused || bTickStatesWhenGamePaused)
	{
		for (int32 activeIndex : ActiveIndice)
		{
			if (PassedIndice.Contains(activeIndex)) continue; // Dont tick on already passed nodes

			if (NextIndice.Contains(activeIndex)) continue; // Dont tick on just entered nodes

			FInputSequenceState& inputSequenceState = States[activeIndex];

			if (!inputSequenceState.IsInputNode) continue;

			inputSequenceState.AccumulatedTime += DeltaTime;

			if (inputSequenceState.isOverridingResetAfterTime ? inputSequenceState.isResetAfterTime : isResetAfterTime)
			{
				if (inputSequenceState.AccumulatedTime > (inputSequenceState.isOverridingResetAfterTime ? inputSequenceState.ResetAfterTime : ResetAfterTime))
				{
					RequestReset(activeIndex);
				}
			}
		}
	}

	CheckResetRequests(eventCalls);

	////// TODO Optimize

	for (FInputSequenceEventCall& eventCall : eventCalls) outEventCalls.Add(eventCall);
}

void UInputSequenceAsset::RequestReset(UObject* sourceObject, const FString& sourceContext)
{
	// TODO Sync

	int32 emplacedIndex = ResetSources.Emplace();
	ResetSources[emplacedIndex].SourceObject = sourceObject;
	ResetSources[emplacedIndex].SourceContext = sourceContext;
}

void UInputSequenceAsset::RequestReset(int32 sourceIndex)
{
	// TODO Sync

	int32 emplacedIndex = ResetSources.Emplace();
	ResetSources[emplacedIndex].SourceIndex = sourceIndex;
}

void UInputSequenceAsset::StepFrom(const int32 nodeIndex, const TMap<FName, TEnumAsByte<EInputEvent>>& inputActionEvents, TArray<FInputSequenceEventCall>& outEventCalls, const bool stepForward)
{
	FInputSequenceState& inputSequenceState = States[nodeIndex];
	
	if (inputSequenceState.IsGoToStartNode)
	{
		RequestReset(nodeIndex);
	}
	else if (inputSequenceState.IsInputNode)
	{
		if (stepForward)
		{
			bool match = true;
			bool consumedInput = false;

			bool usePreciseMatch = requirePreciseMatch && !inputSequenceState.isOverridingRequirePreciseMatch ||
				inputSequenceState.isOverridingRequirePreciseMatch && inputSequenceState.requirePreciseMatch;

			if (usePreciseMatch && inputActionEvents.Num() > 0)
			{
				if (inputActionEvents.Num() != inputSequenceState.InputActions.Num())
				{
					match = false;
					RequestReset(nodeIndex);
				}
				else
				{
					for (TPair<FName, FInputActionState>& inputActionEntry : inputSequenceState.InputActions)
					{
						if (!inputActionEvents.Contains(inputActionEntry.Key))
						{
							match = false;
							RequestReset(nodeIndex);
							break;
						}
					}
				}
			}

			if (match)
			{
				for (TPair<FName, FInputActionState>& inputActionEntry : inputSequenceState.InputActions)
				{
					const FName& actionName = inputActionEntry.Key;
					FInputActionState& inputActionState = inputActionEntry.Value;

					if (!inputActionState.IsOpen())
					{
						consumedInput = inputActionEvents.Contains(actionName) && inputActionState.ConsumeInput(inputActionEvents[actionName]);

						if (consumedInput) inputSequenceState.AccumulatedTime = 0;

						if (!inputActionState.IsOpen()) match = false;
					}
				}
			}

			if (match)
			{
				if (!PassedIndice.Contains(nodeIndex)) PassedIndice.Add(nodeIndex);
				for (int32 nextIndex : inputSequenceState.NextIndice) StepFrom(nextIndex, inputActionEvents, outEventCalls, !consumedInput);
			}
		}
		else
		{
			if (!NextIndice.Contains(nodeIndex)) NextIndice.Add(nodeIndex);
		}
	}
}

void UInputSequenceAsset::StepToNext(const TSet<int32>& nextIndice, TArray<FInputSequenceEventCall>& outEventCalls)
{
	for (int32 nextIndex : nextIndice)
	{
		bool bIsAlreadyInSet = false;
		if (!ActiveIndice.Contains(nextIndex)) ActiveIndice.Add(nextIndex, &bIsAlreadyInSet);

		if (!bIsAlreadyInSet)
		{
			States[nextIndex].Reset();

			for (const TSubclassOf<UInputSequenceEvent>& enterEventClass : States[nextIndex].EnterEventClasses)
			{
				int32 emplacedIndex = outEventCalls.Emplace();
				outEventCalls[emplacedIndex].EventClass = enterEventClass;
				outEventCalls[emplacedIndex].Object = States[nextIndex].StateObject;
				outEventCalls[emplacedIndex].Context = States[nextIndex].StateContext;
			}
		}
	}
}

void UInputSequenceAsset::CheckResetRequests(TArray<FInputSequenceEventCall>& outEventCalls)
{
	// TODO Sync

	if (ResetSources.Num() > 0)
	{
		for (const FInputSequenceResetSource& resetSource : ResetSources)
		{
			if (resetSource.SourceIndex == INDEX_NONE) continue;

			if (States.IsValidIndex(resetSource.SourceIndex))
			{
				for (const TSubclassOf<UInputSequenceEvent>& resetEventClass : States[resetSource.SourceIndex].ResetEventClasses)
				{
					int32 emplacedIndex = outEventCalls.Emplace();
					outEventCalls[emplacedIndex].EventClass = resetEventClass;
					outEventCalls[emplacedIndex].Object = States[resetSource.SourceIndex].StateObject;
					outEventCalls[emplacedIndex].Context = States[resetSource.SourceIndex].StateContext;
				}
			}
		}

		for (FInputSequenceEventCall& eventCall : outEventCalls) eventCall.ResetSources = ResetSources;

		ResetSources.Empty();

		ActiveIndice.Empty();
	}
}