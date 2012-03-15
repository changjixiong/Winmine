#ifndef GAMEBOX_HEAD
#define GAMEBOX_HEAD

#include <windows.h>

class GameBox
{
public:
	GameBox()
	{

	}
	~GameBox()
	{

	}

	int Init(HWND hwnd,int nWidth, int nHeight)
	{
		g_Mainhwnd=hwnd;
		g_hdcScreen=GetDC(hwnd);

		g_hdcCanvas=CreateCompatibleDC(g_hdcScreen);		
		g_bitCanvas=CreateCompatibleBitmap(g_hdcScreen,nWidth,nHeight);
		g_bitCanvasOld=(HBITMAP)SelectObject(g_hdcCanvas,g_bitCanvas);

		return 0;
	}


	int Release()
	{
		g_bitCanvas=(HBITMAP)SelectObject(g_hdcCanvas,g_bitCanvasOld);

		DeleteDC(g_hdcCanvas);
		g_hdcCanvas=NULL;

		DeleteObject(g_bitCanvas);
		return 0;
	}

	int BitBlt()
	{
		::BitBlt(g_hdcScreen,
				0,0,
				g_ScreenWidth,g_ScreenHeigth,g_hdcCanvas,
				0,0,
				SRCCOPY);

		return 0;
	}

	HWND g_Mainhwnd;
	HINSTANCE g_hInstance;
	HDC g_hdcScreen;
	
	HDC g_hdcCanvas;
	
	HBITMAP g_bitCanvas;
	HBITMAP g_bitCanvasOld;
	
	int g_ScreenWidth;
	int g_ScreenHeigth;

protected:
private:
};
#endif