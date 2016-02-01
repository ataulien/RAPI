#pragma once
#include <Windows.h>

class Window
{
public:
	Window();
	virtual ~Window(void);

	/** Creates the window */
	bool Create(LPCSTR WindowName,
				int PosX, int PosY, 
				int SizeX, int SizeY, 
				HINSTANCE hInstance, 
				long WindowStyle = WS_OVERLAPPEDWINDOW | WS_VISIBLE, 
				DWORD ExWindowStyle = 0,
				HWND Parent = NULL);

	/** Returns the windows handle */
	HWND GetHandle(){return Handle;}

	/** Sets an array of bools for the keys */
	void SetKeyArray(bool* keys){Keys = keys;}

protected:

	/** Non-static callback */
	virtual LRESULT OnWindowMessage(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

	/** Static callback */
	static LRESULT CALLBACK WndProc(
        HWND hWnd,
        UINT msg,
        WPARAM wParam,
        LPARAM lParam
		);

	HWND Handle;

	bool* Keys;
};
