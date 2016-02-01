#include "pch.h"
#include "RTweakBar.h"

namespace RAPI
{


    RTweakBar::RTweakBar()
    {
        IsActive = true;
    }


    RTweakBar::~RTweakBar()
    {
    }

#ifdef USE_TWEAK_BAR

#ifdef WIN32
#pragma comment(lib, "AntTweakBar.lib")
#endif

    /** Initializes the tweakbar */
        bool RTweakBar::Init()
        {
            return InitAPI();
        }

    /** Draws the bars */
        void RTweakBar::Draw()
        {
            if (IsActive)
                TwDraw();
        }

#ifdef WIN32
        /** On window message */
        LRESULT RTweakBar::OnWindowMessage(WindowHandle hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
        {
            if(IsActive)
                return TwEventWin(hWnd, msg, wParam, lParam);

            return true;
        }
#endif

        void RTweakBar::AddFloatRW(const char *table, float *value, const char *name, float min, float max, float step)
        {
            if (!GetBarByName(table))
                return;

            TwAddVarRW(GetBarByName(table), name, TwType::TW_TYPE_FLOAT, value, nullptr);
            TwDefine((std::string(table) + "/" + std::string(name) + " min=" + std::to_string(min) + " max=" +
                      std::to_string(max) + " step=" + std::to_string(step)).c_str());
        }

        void RTweakBar::AddFloatR(const char *table, float *value, const char *name, float min, float max, float step)
        {
            if (!GetBarByName(table))
                return;

            TwAddVarRO(GetBarByName(table), name, TwType::TW_TYPE_FLOAT, value, nullptr);
        }

        void RTweakBar::AddIntRW(const char *table, int *value, const char *name, float min, float max, float step)
        {
            if (!GetBarByName(table))
                return;

            TwAddVarRW(GetBarByName(table), name, TwType::TW_TYPE_INT32, value, nullptr);
            TwDefine((std::string(table) + "/" + std::string(name) + " min=" + std::to_string(min) + " max=" +
                      std::to_string(max) + " step=" + std::to_string(step)).c_str());
        }

        void RTweakBar::AddIntR(const char *table, int *value, const char *name, float min, float max, float step)
        {
            if (!GetBarByName(table))
                return;

            TwAddVarRO(GetBarByName(table), name, TwType::TW_TYPE_INT32, value, nullptr);
        }

        void RTweakBar::AddBoolRW(const char *table, bool *value, const char *name)
        {
            if (!GetBarByName(table))
                return;

            TwAddVarRW(GetBarByName(table), name, TwType::TW_TYPE_BOOLCPP, value, nullptr);
        }

        void RTweakBar::AddBoolR(const char *table, bool *value, const char *name)
        {
            if (!GetBarByName(table))
                return;

            TwAddVarRO(GetBarByName(table), name, TwType::TW_TYPE_BOOLCPP, value, nullptr);
        }

    /** Helper callback for button presses */
        void RTweakBar::OnButtonPressedHelper(void *userData)
        {
            cbStruct *data = (cbStruct *) userData;

            // Proxy to the given std::function
            data->onButtonPressed(data->userdata);
        }

        void RTweakBar::AddButton(const char *table, const char *name, std::function<void(void *)> onButtonPressed,
                                  void *userdata)
        {
            if (!GetBarByName(table))
                return;

            // Place the std::function into the map so we can call lambdas later
            ButtonCallbackMap[std::string(table) + "|" + std::string(name)] = cbStruct(userdata, onButtonPressed);

            TwAddButton(GetBarByName(table), name, OnButtonPressedHelper,
                        &ButtonCallbackMap[std::string(table) + "|" + std::string(name)], "");
        }

    /** Returns the bar of the given name. Creates one if not present */
        TwBar *RTweakBar::GetBarByName(const char *name)
        {
            if (BarMap[std::string(name)])
                return BarMap[std::string(name)];

            BarMap[std::string(name)] = TwNewBar(name);

            return BarMap[std::string(name)];
        }
#else
    /** Initializes the tweakbar */
    bool RTweakBar::Init()
    {
        return true;
    }

    /** Draws the bars */
    void RTweakBar::Draw()
    {
    }

#ifdef WIN32
    /** On window message */
        LRESULT RTweakBar::OnWindowMessage(WindowHandle hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
        {
            return 0;
        }
#endif

    void RTweakBar::AddFloatRW(const char *table, float *value, const char *name, float min, float max, float step)
    {
    }

    void RTweakBar::AddFloatR(const char *table, float *value, const char *name, float min, float max, float step)
    {
    }

    void RTweakBar::AddIntRW(const char *table, int *value, const char *name, float min, float max, float step)
    {
    }

    void RTweakBar::AddIntR(const char *table, int *value, const char *name, float min, float max, float step)
    {
    }

    void RTweakBar::AddBoolRW(const char *table, bool *value, const char *name)
    {
    }

    void RTweakBar::AddBoolR(const char *table, bool *value, const char *name)
    {
    }

/** Helper callback for button presses */
    void RTweakBar::OnButtonPressedHelper(void *userData)
    {
    }

    void RTweakBar::AddButton(const char *table, const char *name, std::function<void(void *)> onButtonPressed,
                              void *userdata)
    {
    }

#endif
}