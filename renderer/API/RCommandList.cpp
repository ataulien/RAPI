#include "pch.h"
#include "RCommandList.h"

using namespace RAPI;

RCommandList::RCommandList()
{
}


RCommandList::~RCommandList()
{
}

/** Initializes a commandlist for the given Thread ID */
bool RCommandList::Init()
{
    return InitAPI();
}

/** Creates the commandlist and makes it ready to be played back */
bool RCommandList::FinalizeCommandList()
{
    return FinalizeCommandListAPI();
}

/** Plays the generated commandlist back on the main-thread */
bool RCommandList::ExecuteCommandList()
{
    return ExecuteCommandListAPI();
}
