// Copyright 2022. Pentangle Studio. All rights reserved.

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

void UInputSequenceAsset::OnInput(const float DeltaTime, const bool bGamePaused, const TMap<FName, TEnumAsByte<EInputEvent>>& inputActionEvents, const TMap<FName, float>& inputAxisEvents, TArray<FInputSequenceEventCall>& outEventCalls)
{
	if (ActiveIndice.IsEmpty()) MakeTransition(0, States[0].NextIndice, outEventCalls);

	TSet<int32> prevActiveIndice = ActiveIndice;

	if (!bGamePaused || bStepFromStatesWhenGamePaused)
	{
		for (int32 activeIndex : prevActiveIndice)
		{
			FInputSequenceState& state = States[activeIndex];

			if (!state.IsInputNode)
			{
				RequestReset(activeIndex);
				ActiveIndice.Remove(activeIndex);
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

							RequestReset(activeIndex);
							ActiveIndice.Remove(activeIndex);
						}
						else
						{
							for (TPair<FName, FInputActionState>& inputActionEntry : state.InputActions)
							{
								if (!inputActionEvents.Contains(inputActionEntry.Key))
								{
									match = false;

									RequestReset(activeIndex);
									ActiveIndice.Remove(activeIndex);

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

								RequestReset(activeIndex);
								ActiveIndice.Remove(activeIndex);
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
						RequestReset(activeIndex);
						ActiveIndice.Remove(activeIndex);
					}
				}
			}
		}
	}

	ProcessResetSources(outEventCalls);
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

void UInputSequenceAsset::EnterNode(int32 nextIndex, TArray<FInputSequenceEventCall>& outEventCalls)
{
	if (!ActiveIndice.Contains(nextIndex))
	{
		FInputSequenceState& state = States[nextIndex];

		for (const TSubclassOf<UInputSequenceEvent>& enterEventClass : state.EnterEventClasses)
		{
			int32 emplacedIndex = outEventCalls.Emplace();
			outEventCalls[emplacedIndex].EventClass = enterEventClass;
			outEventCalls[emplacedIndex].Object = state.StateObject;
			outEventCalls[emplacedIndex].Context = state.StateContext;
		}

		state.Reset();
		ActiveIndice.Add(nextIndex);

		// Jump through empty Input nodes

		if (state.IsInputNode && state.IsEmpty()) MakeTransition(nextIndex, state.NextIndice, outEventCalls);
	}
}

void UInputSequenceAsset::PassNode(int32 fromIndex, TArray<FInputSequenceEventCall>& outEventCalls)
{
	if (ActiveIndice.Contains(fromIndex))
	{
		FInputSequenceState& state = States[fromIndex];

		for (const TSubclassOf<UInputSequenceEvent>& passEventClass : state.PassEventClasses)
		{
			int32 emplacedIndex = outEventCalls.Emplace();
			outEventCalls[emplacedIndex].EventClass = passEventClass;
			outEventCalls[emplacedIndex].Object = state.StateObject;
			outEventCalls[emplacedIndex].Context = state.StateContext;
		}

		ActiveIndice.Remove(fromIndex);
	}
}

void UInputSequenceAsset::RequestReset(UObject* sourceObject, const FString& sourceContext)
{
	FScopeLock Lock(&resetSourcesCS);

	int32 emplacedIndex = ResetSources.Emplace();
	ResetSources[emplacedIndex].SourceObject = sourceObject;
	ResetSources[emplacedIndex].SourceContext = sourceContext;
}

void UInputSequenceAsset::RequestReset(int32 sourceIndex)
{
	FScopeLock Lock(&resetSourcesCS);

	int32 emplacedIndex = ResetSources.Emplace();
	ResetSources[emplacedIndex].SourceIndex = sourceIndex;
}

void UInputSequenceAsset::ProcessResetSources(TArray<FInputSequenceEventCall>& outEventCalls)
{
	FScopeLock Lock(&resetSourcesCS);

	if (ResetSources.Num() > 0)
	{
		bool bResetAll = false;

		TSet<int32> firstLayerParentsToReset;

		for (const FInputSequenceResetSource& resetSource : ResetSources)
		{
			bResetAll |= resetSource.SourceIndex == INDEX_NONE;

			if (States.IsValidIndex(resetSource.SourceIndex))
			{
				const FInputSequenceState& state = States[resetSource.SourceIndex];

				// GoToStartNode is reseting all Active nodes that have the same FirstLayerParentIndex

				if (!state.IsInputNode && !firstLayerParentsToReset.Contains(state.FirstLayerParentIndex))
				{
					firstLayerParentsToReset.Add(state.FirstLayerParentIndex);
				}

				for (const TSubclassOf<UInputSequenceEvent>& resetEventClass : state.ResetEventClasses)
				{
					int32 emplacedIndex = outEventCalls.Emplace();
					outEventCalls[emplacedIndex].EventClass = resetEventClass;
					outEventCalls[emplacedIndex].Object = state.StateObject;
					outEventCalls[emplacedIndex].Context = state.StateContext;
				}
			}
		}

		for (TSet<int32>::TIterator It(ActiveIndice); It; ++It)
		{
			const FInputSequenceState& state = States[*It];

			if (bResetAll || firstLayerParentsToReset.Contains(state.FirstLayerParentIndex))
			{
				for (const TSubclassOf<UInputSequenceEvent>& resetEventClass : state.ResetEventClasses)
				{
					int32 emplacedIndex = outEventCalls.Emplace();
					outEventCalls[emplacedIndex].EventClass = resetEventClass;
					outEventCalls[emplacedIndex].Object = state.StateObject;
					outEventCalls[emplacedIndex].Context = state.StateContext;
				}

				It.RemoveCurrent();
			}
		}

		////// TODO Think if we can avoid copying reset sources to each call

		for (FInputSequenceEventCall& eventCall : outEventCalls) eventCall.ResetSources = ResetSources;

		ResetSources.Empty();
	}
}