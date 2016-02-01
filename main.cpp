#include "pch.h"
#include <iostream>
#include "REngine.h"
#include "Window.h"

using namespace std;
using namespace RAPI;


void Update()
{
    REngine::RenderingDevice->OnFrameStart();

    REngine::RenderingDevice->OnFrameEnd();

    REngine::RenderingDevice->Present();
}

void RunApp()
{
    bool bGotMsg;
    MSG msg;
    msg.message = WM_NULL;
    PeekMessage( &msg, NULL, 0U, 0U, PM_NOREMOVE );

    while( WM_QUIT != msg.message )
    {
        // Use PeekMessage() so we can use idle time to render the scene.
        bGotMsg = ( PeekMessage( &msg, NULL, 0U, 0U, PM_REMOVE ) != 0 );

        if( bGotMsg )
        {
            TranslateMessage( &msg );
            DispatchMessage( &msg );
        }
        else
        {
            Update();
        }
    }
}

int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow )
{
	Window wnd;
	wnd.Create("--- Test ---", 20, 20, 1280, 720, hInstance, WS_OVERLAPPEDWINDOW | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN);

	REngine::InitializeEngine();
    REngine::RenderingDevice->SetWindow(wnd.GetHandle());
	cout << "Hello, World!" << endl;

    RunApp();

	REngine::UninitializeEngine();

	return 0;
}