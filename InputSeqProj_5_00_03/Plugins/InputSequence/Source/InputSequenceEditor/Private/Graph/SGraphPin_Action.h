// Copyright 2022 Pentangle Studio Licensed under the Apache License, Version 2.0 (the «License»);

#pragma once

#include "SGraphPin.h"

class UEdGraphPin;

class SGraphPin_Action : public SGraphPin
{
public:
	SLATE_BEGIN_ARGS(SGraphPin_Action) {}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, UEdGraphPin* InPin);

	virtual FSlateColor GetPinTextColor() const override;

protected:

	FText ToolTipText_Raw_Label() const;

	EVisibility Visibility_Raw_SelfPin() const;

	EVisibility Visibility_Raw_ArrowUp() const;

	FText ToolTipText_Raw_RemovePin() const;

	FReply OnClicked_Raw_RemovePin() const;

	FText ToolTipText_Raw_TogglePin() const;

	FReply OnClicked_Raw_TogglePin() const;
};