#include "MyBitMap.h"

int MyBitMap::Init(HINSTANCE hInst, HDC hdc, const char * lpBMPPath, int nWidth, int nHeight)
{
	_hdc=CreateCompatibleDC(hdc);
	_bitmap=(HBITMAP)LoadImage(hInst,
								lpBMPPath,
								IMAGE_BITMAP,
								0,0,
								LR_LOADFROMFILE|LR_CREATEDIBSECTION);
	_bitmapOld=(HBITMAP)SelectObject(_hdc,_bitmap);	

	_Width=nWidth;
	_Height=nHeight;

	return 0;
}

int MyBitMap::Destory()
{
	SelectObject(_hdc,_bitmapOld);
	DeleteObject(_bitmapOld);
	DeleteDC(_hdc);

	return 0;
}

int MyBitMap::BitBlt(HDC hdcDest, int x, int y)
{
	::BitBlt(hdcDest,
		x,y,
		_Width,_Height,_hdc,
		0,0,
		SRCCOPY);

	return 0;
}

