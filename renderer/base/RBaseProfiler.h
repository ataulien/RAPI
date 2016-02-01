#pragma once
#include "pch.h"

class RBaseProfiler
{
public:
	RBaseProfiler();
	~RBaseProfiler();

	// Constants
	static const uint64_t QueryLatency = 5;

protected:
	uint64_t currFrame;
};

