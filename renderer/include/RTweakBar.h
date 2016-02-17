#pragma once

#include <limits.h>
#include <float.h>
#include "pch.h"

#ifdef RND_D3D11
#include "RD3D11TweakBar.h"
#define RTWEAKBARBASE_API RD3D11TweakBar
#elif defined(RND_NULL)
#include "RNULLTweakbar.h"
#define RTWEAKBARBASE_API RNULLTweakbar
#elif defined(RND_GL)
#include "RGLTweakbar.h"
#define RTWEAKBARBASE_API RGLTweakbar
#endif

namespace RAPI
{
	enum ETValMode
	{
		TVM_ReadWrite,
		TVM_Read
	};


	class RTweakBar : public RTWEAKBARBASE_API
	{
	public:
		RTweakBar();

		~RTweakBar();

		/** Initializes the tweakbar */
		bool Init();

#if defined(WIN32) || defined(_WIN32)
		/** On window message */
		LRESULT OnWindowMessage(WindowHandle hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
#endif

		/** Draws the bars */
		void Draw();

		/** Adds values to the bar */
		void AddFloatRW(const char *table, float *value, const char *name, float min = FLT_MIN, float max = FLT_MAX,
						float step = 0.1f);

		void AddFloatR(const char *table, float *value, const char *name, float min = FLT_MIN, float max = FLT_MAX,
					   float step = 0.1f);

		void AddIntRW(const char *table, int *value, const char *name, float min = INT_MIN, float max = INT_MAX,
					  float step = 1);

		void AddIntR(const char *table, int *value, const char *name, float min = INT_MIN, float max = INT_MAX,
					 float step = 1);

		void AddBoolRW(const char *table, bool *value, const char *name);

		void AddBoolR(const char *table, bool *value, const char *name);

		void AddButton(const char *table, const char *name, std::function<void(void *)> onButtonPressed,
					   void *userdata = nullptr);

	protected:
		

#ifdef USE_TWEAK_BAR
		/** Helper callback for button presses */
		static void TW_CALL OnButtonPressedHelper(void *userData);

		/** Returns the bar of the given name. Creates one if not present */
		TwBar *GetBarByName(const char *name);
#else
		static void  OnButtonPressedHelper(void *userData);
#endif
		

	};

}