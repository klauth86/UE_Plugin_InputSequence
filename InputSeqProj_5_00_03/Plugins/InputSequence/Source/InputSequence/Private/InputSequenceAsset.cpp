// Copyright 2022 Pentangle Studio Licensed under the Apache License, Version 2.0 (the «License»);

#include "InputSequenceAsset.h"
#include "Engine/EngineBaseTypes.h"

FInputSequenceState::FInputSequenceState()
{
	AccumulatedTime = 0;

	InputActions.Reset();
	PressedActions.Reset();
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

bool FInputSequenceState::IsOpen() const
{
	for (const TPair<FName, FInputActionState>& inputActionEntry : InputActions)
	{
		const FName& actionName = inputActionEntry.Key;
		const FInputActionState& inputActionState = inputActionEntry.Value;

		if (IsAxisNode && !inputActionState.IsOpen_Axis() || !inputActionState.IsOpen_Action()) return false;
	}

	return true;
}

bool FInputSequenceState::ConsumeInput(const TMap<FName, TEnumAsByte<EInputEvent>> inputActionEvents, const TSet<FName>& pressedActions, const TMap<FName, float>& inputAxisEvents)
{
	bool result = false;

	for (TPair<FName, FInputActionState>& inputActionEntry : InputActions)
	{
		const FName& actionName = inputActionEntry.Key;
		FInputActionState& inputActionState = inputActionEntry.Value;

		if (IsAxisNode)
		{
			if (inputActionState.Is2DAxis())
			{
				if (inputAxisEvents.Contains(inputActionState.GetSubNameA()) && inputAxisEvents.Contains(inputActionState.GetSubNameB()))
				{
					if (inputActionState.ConsumeInput_2DAxis(inputAxisEvents[inputActionState.GetSubNameA()], inputAxisEvents[inputActionState.GetSubNameB()]))
					{
						AccumulatedTime = 0;
						result = true;
					}
				}
			}
			else
			{
				if (inputAxisEvents.Contains(actionName) && !inputActionState.IsOpen_Axis())
				{
					if (inputActionState.ConsumeInput_Axis(inputAxisEvents[actionName]))
					{
						AccumulatedTime = 0;
						result = true;
					}
				}
			}
		}
		else
		{
			if (inputActionEvents.Contains(actionName) && !inputActionState.IsOpen_Action())
			{
				if (inputActionState.ConsumeInput_Action(inputActionEvents[actionName]))
				{
					AccumulatedTime = 0;
					result = true;
				}
			}

			if (pressedActions.Contains(actionName) && !inputActionState.IsOpen_Action())
			{
				if (inputActionState.ConsumeInput_Action(EInputEvent::IE_Pressed))
				{
					AccumulatedTime = 0;
					result = true;
				}
			}
		}
	}

	return result;
}



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
	for (const TPair<FName, TEnumAsByte<EInputEvent>>& inputActionEvent : inputActionEvents)
	{
		if (inputActionEvent.Value == EInputEvent::IE_Released) PressedActions.Remove(inputActionEvent.Key);
		if (inputActionEvent.Value == EInputEvent::IE_Pressed) PressedActions.FindOrAdd(inputActionEvent.Key);
	}

	int32 inputActionEventsNum = inputActionEvents.Num();
	int32 pressedActionsNum = PressedActions.Num();

	if (ActiveIndice.IsEmpty()) MakeTransition(0, States[0].NextIndice, outEventCalls);

	TSet<int32> prevActiveIndice = ActiveIndice;

	if (!bGamePaused || bStepFromStatesWhenGamePaused)
	{
		for (int32 activeIndex : prevActiveIndice)
		{
			FInputSequenceState& state = States[activeIndex];
			int32 stateInputActionsNum = state.InputActions.Num();

			if (!state.IsInputNode)
			{
				RequestResetWithNode(activeIndex, state);
			}
			else
			{
				bool match = true;

				if (!state.IsAxisNode && (inputActionEventsNum + pressedActionsNum) > 0) // No need to check Precise Match for Axis Input because it comes as continual data
				{
					if (requirePreciseMatch && !state.isOverridingRequirePreciseMatch || state.isOverridingRequirePreciseMatch && state.requirePreciseMatch)
					{
						// Match with Input Action Events

						for (const TPair<FName, TEnumAsByte<EInputEvent>>& inputActionEvent : inputActionEvents)
						{
							if (!state.InputActions.Contains(inputActionEvent.Key))
							{
								match = false;
								RequestResetWithNode(activeIndex, state);

								break;
							}
						}

						// Match with Pressed Actions

						for (const FName& pressedAction : PressedActions)
						{
							if (!state.PressedActions.Contains(pressedAction) && !state.InputActions.Contains(pressedAction))
							{
								match = false;
								RequestResetWithNode(activeIndex, state);

								break;
							}
						}
					}

					// Match Press-Release logic

					for (const FName& pressedAction : state.PressedActions)
					{
						if (!PressedActions.Contains(pressedAction))
						{
							match = false;
							RequestResetWithNode(activeIndex, state);

							break;
						}
					}
				}

				// Process if match

				if (match && !state.IsOpen())
				{
					if (state.canBePassedAfterTime)
					{
						float accumulatedTime = state.AccumulatedTime;

						if (state.ConsumeInput(inputActionEvents, PressedActions, inputAxisEvents))
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
						match = state.ConsumeInput(inputActionEvents, PressedActions, inputAxisEvents) && state.IsOpen();
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