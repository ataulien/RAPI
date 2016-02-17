#pragma once

#if !defined(RND_GL) && !defined(RND_D3D11)
#define RND_GL
#endif

#ifdef RND_D3D11
// Disable warning about makro-redifinitions
// There are a lot of these about D3D11 and DXGI-Errorcodes which
// have been redefined in other files than those in the DX-SDK
#pragma warning( disable : 4005 )
#include <D3D11.h>
#include <DXGI.h>
#include <D3DX11.h>
#include <D3DX11tex.h>
#pragma warning( default : 4005 )
#elif defined(RND_GL)
#include <GL/glew.h>
#endif

#if defined(WIN32) || defined(_WIN32)
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#endif

#ifdef RND_D3D11
typedef HWND WindowHandle;
#elif defined(RND_GL)
struct GLFWwindow;
typedef GLFWwindow* WindowHandle;
#else
typedef void* WindowHandle;
#endif

#ifndef ARRAYSIZE
#define ARRAYSIZE(x)(sizeof(x) / sizeof(x[0]))
#endif

#include <vector>
#include <unordered_map>
#include <map>
#include <mutex>
#include <set>
#include <thread>
#include <future>
#include <numeric>
#include <sstream>
#include "Types.h"
#include <string.h>
#include <algorithm>
#include <array>

#ifdef _MSC_VER
#define SELECTANY __declspec(selectany)
#elif __GNUC__
#define SELECTANY __attribute__((weak))
#endif

#define R_PI 3.14159265358979f

#define SafeRelease(x) do{if(x){x->Release(); x = NULL;}}while(0)
