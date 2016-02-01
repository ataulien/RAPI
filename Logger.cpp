#include "pch.h"
#include "Logger.h"

#ifdef USE_LOG

// Definition of the static log callback
std::function<void(const std::string&)> Log::s_LogCallback;
std::string Log::s_LogFile;
#endif