#include "pch.h"
#include "RBaseDevice.h"

using namespace RAPI;

RBaseDevice::RBaseDevice()
{
    FrameCounter = 0;
    QueuedDrawCallCounter = 0;
    QueueCounter = 0;
    DoDrawcalls = true;

    SetMainClearValues(RFloat4(0.2f, 0.2f, 0.2f, 0), 1.0f);
}


RBaseDevice::~RBaseDevice()
{
}

