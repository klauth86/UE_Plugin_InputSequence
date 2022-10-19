// Copyright 2022 Pentangle Studio Licensed under the Apache License, Version 2.0 (the «License»);

#pragma once

#include "SGraphPin.h"

class UEdGraphPin;

class SGraphPin_Add : public SGraphPin
{
public:
	SLATE_BEGIN_ARGS(SGraphPin_Add) {}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, UEdGraphPin* InPin);

protected:

	TSharedRef<SWidget> OnGetAddButtonMenuContent();

protected:

	TSharedPtr<SComboButton> AddButton;
};