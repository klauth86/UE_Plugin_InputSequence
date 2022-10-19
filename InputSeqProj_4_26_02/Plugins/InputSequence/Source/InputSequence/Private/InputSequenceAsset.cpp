// Copyright 2022 Pentangle Studio Licensed under the Apache License, Version 2.0 (the «License»);

#include "InputSequenceAsset.h"

UInputSequenceAsset::UInputSequenceAsset(const FObjectInitializer& objInit) :Super(objInit)
{
	requirePreciseMatch = 0;

	ResetAfterTime = 0.2f;
	isResetAfterTime = 0;

	bStepFromStatesWhenGamePaused = 0;
	bTickStatesWhenGamePaused = 0;

	ActiveIndice.Empty();
	ResetSources.Empty();
}

void UInputSequenceAsset::OnInput(const float DeltaTime, const bool bGamePaused, const TMap<FName, TEnumAsByte<EInputEvent>>& inputActionEvents, const TMap<FName, float>& inputAxisEvents, TArray<FInputSequenceEventCall>& outEventCalls, TArray<FInputSequenceResetSource>& outResetSources)
{
	if (ActiveIndice.Num() <= 0) MakeTransition(0, States[0].NextIndice, outEventCalls);

	TSet<int32> prevActiveIndice = ActiveIndice;

	if (!bGamePaused || bStepFromStatesWhenGamePaused)
	{
		for (int32 activeIndex : prevActiveIndice)
		{
			FInputSequenceState& state = States[activeIndex];

			if (!state.IsInputNode)
			{
				RequestResetWithNode(activeIndex, state);
			}
			else if (inputActionEvents.Num())
			{
				bool match = true;

				if (!state.IsAxisNode) // No need to check Precise Match for Axis Input because it comes as continual data
				{
					if (requirePreciseMatch && !state.isOverridingRequirePreciseMatch || state.isOverridingRequirePreciseMatch && state.requirePreciseMatch)
					{
						if (inputActionEvents.Num() != state.InputActions.Num())
						{
							match = false;
							RequestResetWithNode(activeIndex, state);
						}
						else
						{
							for (TPair<FName, FInputActionState>& inputActionEntry : state.InputActions)
							{
								if (!inputActionEvents.Contains(inputActionEntry.Key))
								{
									match = false;
									RequestResetWithNode(activeIndex, state);
									break;
								}
							}
						}
					}
				}

				if (match && !state.IsOpen())
				{
					if (state.canBePassedAfterTime)
					{
						float accumulatedTime = state.AccumulatedTime;

						if (state.ConsumeInput(inputActionEvents, inputAxisEvents))
						{
							match = state.IsOpen();

							if (accumulatedTime < state.TimeParam)
							{
								match = false;
								RequestResetWithNode(activeIndex, state);
							}
						}
					}
					else
					{
						match = state.ConsumeInput(inputActionEvents, inputAxisEvents) && state.IsOpen();
					}
				}

				if (match) MakeTransition(activeIndex, state.NextIndice, outEventCalls);
			}
		}
	}

	if (!bGamePaused || bTickStatesWhenGamePaused)
	{
		for (int32 activeIndex : prevActiveIndice)
		{
			if (ActiveIndice.Contains(activeIndex)) // Tick on states that already were active before this frame
			{
				FInputSequenceState& state = States[activeIndex];

				if (!state.IsInputNode) continue;

				state.AccumulatedTime += DeltaTime;

				if (state.canBePassedAfterTime) continue; // States that can be passed only after time are not reset by time at all

				if (state.isOverridingResetAfterTime ? state.isResetAfterTime : isResetAfterTime)
				{
					if (state.AccumulatedTime > (state.isOverridingResetAfterTime ? state.TimeParam : ResetAfterTime))
					{
						RequestResetWithNode(activeIndex, state);
					}
				}
			}
		}
	}

	ProcessResetSources(outEventCalls, outResetSources);
}

void UInputSequenceAsset::MakeTransition(int32 fromIndex, const TSet<int32>& nextIndice, TArray<FInputSequenceEventCall>& outEventCalls)
{
	if (nextIndice.Num() > 0)
	{
		for (int32 nextIndex : nextIndice) EnterNode(nextIndex, outEventCalls);
	}
	else // Make Transition to First Layer Parent if nextIndice is empty
	{
		FInputSequenceState& state = States[fromIndex];
		EnterNode(state.FirstLayerParentIndex, outEventCalls);
	}

	PassNode(fromIndex, outEventCalls);
}

void UInputSequenceAsset::RequestResetWithNode(int32 nodeIndex, FInputSequenceState& state)
{
	if (state.IsFirstLayer())
	{
		state.Reset();
	}
	else
	{
		FScopeLock Lock(&resetSourcesCS);

		int32 emplacedIndex = ResetSources.Emplace();
		ResetSources[emplacedIndex].SourceIndex = nodeIndex;

		ActiveIndice.Remove(nodeIndex);
	}
}

void UInputSequenceAsset::EnterNode(int32 nodeIndex, TArray<FInputSequenceEventCall>& outEventCalls)
{
	if (!ActiveIndice.Contains(nodeIndex))
	{
		FInputSequenceState& state = States[nodeIndex];

		for (const TSubclassOf<UInputSequenceEvent>& enterEventClass : state.EnterEventClasses)
		{
			int32 emplacedIndex = outEventCalls.Emplace();
			outEventCalls[emplacedIndex].EventClass = enterEventClass;
			outEventCalls[emplacedIndex].Object = state.StateObject;
			outEventCalls[emplacedIndex].Context = state.StateContext;
		}

		state.Reset();
		ActiveIndice.Add(nodeIndex);

		// Jump through empty Input nodes

		if (state.IsInputNode && state.IsEmpty()) MakeTransition(nodeIndex, state.NextIndice, outEventCalls);
	}
}

void UInputSequenceAsset::PassNode(int32 nodeIndex, TArray<FInputSequenceEventCall>& outEventCalls)
{
	if (ActiveIndice.Contains(nodeIndex))
	{
		FInputSequenceState& state = States[nodeIndex];

		for (const TSubclassOf<UInputSequenceEvent>& passEventClass : state.PassEventClasses)
		{
			int32 emplacedIndex = outEventCalls.Emplace();
			outEventCalls[emplacedIndex].EventClass = passEventClass;
			outEventCalls[emplacedIndex].Object = state.StateObject;
			outEventCalls[emplacedIndex].Context = state.StateContext;
		}

		ActiveIndice.Remove(nodeIndex);
	}
}

void UInputSequenceAsset::RequestReset(UObject* sourceObject, const FString& sourceContext)
{
	FScopeLock Lock(&resetSourcesCS);

	int32 emplacedIndex = ResetSources.Emplace();
	ResetSources[emplacedIndex].SourceObject = sourceObject;
	ResetSources[emplacedIndex].SourceContext = sourceContext;
}

void UInputSequenceAsset::ProcessResetSources(TArray<FInputSequenceEventCall>& outEventCalls, TArray<FInputSequenceResetSource>& outResetSources)
{
	bool bResetAll = false;

	TSet<int32> nodeSources;
	TSet<int32> resetFLParents;
	TSet<int32> checkFLParents;

	ProcessResetSources(bResetAll, nodeSources, resetFLParents, checkFLParents, outResetSources);

	for (int32 nodeIndex : nodeSources)
	{
		const FInputSequenceState& state = States[nodeIndex];

		for (const TSubclassOf<UInputSequenceEvent>& resetEventClass : state.ResetEventClasses)
		{
			int32 emplacedIndex = outEventCalls.Emplace();
			outEventCalls[emplacedIndex].EventClass = resetEventClass;
			outEventCalls[emplacedIndex].Object = state.StateObject;
			outEventCalls[emplacedIndex].Context = state.StateContext;
		}
	}

	if (bResetAll)
	{
		for (int32 activeIndex : ActiveIndice)
		{
			const FInputSequenceState& state = States[activeIndex];

			for (const TSubclassOf<UInputSequenceEvent>& resetEventClass : state.ResetEventClasses)
			{
				int32 emplacedIndex = outEventCalls.Emplace();
				outEventCalls[emplacedIndex].EventClass = resetEventClass;
				outEventCalls[emplacedIndex].Object = state.StateObject;
				outEventCalls[emplacedIndex].Context = state.StateContext;
			}
		}

		ActiveIndice.Empty();
	}
	else
	{
		for (TSet<int32>::TIterator It(ActiveIndice); It; ++It)
		{
			const FInputSequenceState& state = States[*It];

			if (resetFLParents.Contains(state.FirstLayerParentIndex))
			{
				for (const TSubclassOf<UInputSequenceEvent>& resetEventClass : state.ResetEventClasses)
				{
					int32 emplacedIndex = outEventCalls.Emplace();
					outEventCalls[emplacedIndex].EventClass = resetEventClass;
					outEventCalls[emplacedIndex].Object = state.StateObject;
					outEventCalls[emplacedIndex].Context = state.StateContext;
				}

				It.RemoveCurrent();

				if (checkFLParents.Contains(state.FirstLayerParentIndex))
				{
					checkFLParents.Remove(state.FirstLayerParentIndex);
				}
			}
			else if (checkFLParents.Contains(state.FirstLayerParentIndex))
			{
				checkFLParents.Remove(state.FirstLayerParentIndex);
			}
		}

		if (resetFLParents.Num() > 0) MakeTransition(0, resetFLParents, outEventCalls);

		if (checkFLParents.Num() > 0) MakeTransition(0, checkFLParents, outEventCalls);
	}
}

void UInputSequenceAsset::ProcessResetSources(bool& bResetAll, TSet<int32>& nodeSources, TSet<int32>& resetFLParents, TSet<int32>& checkFLParents, TArray<FInputSequenceResetSource>& outResetSources)
{
	FScopeLock Lock(&resetSourcesCS);

	outResetSources = ResetSources;

	for (const FInputSequenceResetSource& resetSource : ResetSources)
	{
		bResetAll |= resetSource.SourceIndex == INDEX_NONE;

		if (States.IsValidIndex(resetSource.SourceIndex))
		{
			nodeSources.Add(resetSource.SourceIndex);

			const FInputSequenceState& state = States[resetSource.SourceIndex];

			if (!state.IsInputNode) // GoToStartNode is reseting all Active nodes that have the same FirstLayerParentIndex
			{
				if (!resetFLParents.Contains(state.FirstLayerParentIndex)) resetFLParents.Add(state.FirstLayerParentIndex);
			}
			else
			{
				if (!checkFLParents.Contains(state.FirstLayerParentIndex)) checkFLParents.Add(state.FirstLayerParentIndex);
			}
		}
	}

	ResetSources.Empty();
}