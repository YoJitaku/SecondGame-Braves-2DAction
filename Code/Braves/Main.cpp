//=============================================================================
//
// [Main.cpp]
// Author : Yo Jitaku
//
//=============================================================================
#include "Main.h"
#include "DirectX.h"

extern int g_nCountFPS;//fps保存する変数
extern LPD3DXFONT g_pFont;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR LpCmdLine, int nCmdShow)
{
	//FPS
	DWORD dwExecLastTime = timeGetTime(); //draw処理実行の最終時刻
	DWORD dwFPSLastTime = timeGetTime(); //FPS計測の最終時刻
	DWORD dwCurrentTime = timeGetTime(); //現在の時刻
	DWORD dwFrameCount = 0; //FPS Count

	//window class set
	WNDCLASSEX WCEX;
	WCEX.cbClsExtra = 0;
	WCEX.cbSize = sizeof(WCEX);
	WCEX.cbWndExtra = 0;
	WCEX.hbrBackground = (HBRUSH)COLOR_WINDOW;
	WCEX.hCursor = NULL;
	WCEX.hIcon = NULL;
	WCEX.hIconSm = NULL;
	WCEX.hInstance = hInstance;
	WCEX.lpfnWndProc = WindowProc;
	WCEX.lpszClassName = "WCEX1";
	WCEX.lpszMenuName = NULL;
	WCEX.style = CS_CLASSDC;

	//window class register
	RegisterClassEx(&WCEX);

	//window size auto-set
	RECT rect = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };//left top right bottom
	AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, false);

	//window create
	HWND hWnd = CreateWindowEx
	(
		NULL,
		"WCEX1",
		"WND1",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		SCREEN_WIDTH,
		SCREEN_HEIGHT,
		NULL,
		NULL,
		hInstance,
		NULL
	);

	//DirectX window create
	if (FAILED(Init(hInstance, hWnd, TRUE))) return -1;//D3Dの定義はshowwindowの前にする

	//show window (Param2 : 0 is hide, 1 is show = SW_SHOWNORMAL = nCmdShow)
	ShowWindow(hWnd, nCmdShow);

	//update window 
	UpdateWindow(hWnd);

	//messageの定義とmessage loop
	MSG msg;
	while (true)
	{
		//Param5 : peekmessageはみるだけ、(getmessageは見たら、messageを消す)　ここでPM_REMOVE使用してGetMessageと同じのようにする
		//Param2 :NULLは確保全部の状態でmessageを取るできる。Nonexclusiveのような 。hWndの場合 means the message which come from the hWnd window, could been used.
		//if successed will return any number except 0, when 0 has been returned it's means false
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) != 0)
		{
			//脱出するかどうかの判断
			if (msg.message == WM_QUIT) break;
			else
			{
				TranslateMessage(&msg);//自動翻訳
				DispatchMessage(&msg);//自動配る
			}
		}
		else
		{
			dwCurrentTime = timeGetTime();//今のPC timeをget
			if ((dwCurrentTime - dwFPSLastTime) >= 500)//0.5秒一回実行する
			{
				g_nCountFPS = (dwFrameCount * 1000) / (dwCurrentTime - dwFPSLastTime);
				dwFPSLastTime = dwCurrentTime;//一応計算したら、今回の最終時間を保存する
				dwFrameCount = 0;//FPS計数reset
			}
			//if ((dwCurrentTime - dwExecLastTime) > (1000 / 60))
			else
			{
				dwExecLastTime = dwCurrentTime;
				Update();
				Draw();
				dwFrameCount++;
			}
		}
	}
	Uninit();

	UnregisterClass("WCEX1", WCEX.hInstance);

	return (int)msg.wParam;
}

LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	int nMsg;

	switch (uMsg)
	{
	case WM_CLOSE:
		nMsg = MessageBox(NULL, "Sure To Close?", "Confirm", MB_YESNO | MB_ICONQUESTION);
		if (nMsg == IDYES)	DestroyWindow(hWnd);//manual close and send a destroy message to system (WM_DESTROY)
		else return 0;
		break;

	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_ESCAPE:
			nMsg = MessageBox(NULL, "Sure To Close?", "Confirm", MB_YESNO | MB_ICONQUESTION);
			if (nMsg == IDYES)	DestroyWindow(hWnd);
			else return 0;
			break;
		}
		break;

	case WM_DESTROY:
		PostQuitMessage(0);//insert a WM_QUIT message into the message loop
		break;

	default:
		break;
	}

	//default window procedure
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}
