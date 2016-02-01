#include "pch.h"
#include "RBaseDevice.h"

using namespace RAPI;

RBaseDevice::RBaseDevice()
{
    FrameCounter = 0;
    QueuedDrawCallCounter = 0;
    QueueCounter = 0;
    DoDrawcalls = true;

    SetMainClearValues(float4(0, 0, 0, 0), 1.0f);
}


RBaseDevice::~RBaseDevice()
{
}

