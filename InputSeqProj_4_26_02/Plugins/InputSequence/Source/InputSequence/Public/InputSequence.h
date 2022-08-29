// Copyright 2022. Pentangle Studio. All rights reserved.

#pragma once

#include "Modules/ModuleManager.h"

class FInputSequenceModule : public IModuleInterface
{
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};