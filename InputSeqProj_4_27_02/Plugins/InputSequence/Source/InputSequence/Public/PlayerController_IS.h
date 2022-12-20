// Copyright 2022 Pentangle Studio Licensed under the Apache License, Version 2.0 (the «License»);

#pragma once

#include "GameFramework/PlayerController.h"
#include "PlayerController_IS.generated.h"

UCLASS()
class INPUTSEQUENCE_API APlayerController_IS : public APlayerController
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintImplementableEvent, Category = "Player Controller IS")
		void OnPreProcessInput(const float DeltaTime, const bool bGamePaused);

	virtual void PreProcessInput(const float DeltaTime, const bool bGamePaused) override
	{
		Super::PreProcessInput(DeltaTime, bGamePaused);
		OnPreProcessInput(DeltaTime, bGamePaused);
	}

	UFUNCTION(BlueprintImplementableEvent, Category = "Player Controller IS")
		void OnPostProcessInput(const float DeltaTime, const bool bGamePaused);

    virtual void PostProcessInput(const float DeltaTime, const bool bGamePaused) override
    {
        Super::PostProcessInput(DeltaTime, bGamePaused);
        OnPostProcessInput(DeltaTime, bGamePaused);
    }

    UFUNCTION(BlueprintCallable, Category = "Player Controller IS")
        void RegisterInputActionEvent(FName inputActionName, EInputEvent inputEvent) { InputActionEvents.Add(inputActionName, inputEvent); }

    UFUNCTION(BlueprintCallable, Category = "Player Controller IS")
        void RegisterInputAxisEvent(FName inputAxisName, float axisValue) { InputAxisEvents.Add(inputAxisName, axisValue); }

protected:

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Controller IS")
        TMap<FName, TEnumAsByte<EInputEvent>> InputActionEvents;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Controller IS")
        TMap<FName, float> InputAxisEvents;
};