#ifndef MYBITMAP_HEAD
#define MYBITMAP_HEAD
#include <windows.h>

class MyBitMap
{
public:
	MyBitMap(){};
	int Init(HINSTANCE hInst, HDC hdc, const char * lpBMPPath, int nWidth, int nHeight);
	int Destory();
	int BitBlt(HDC hdcDest, int x, int y);
	int _Width,_Height;
	~MyBitMap(){};
protected:
	HDC _hdc;
	HBITMAP _bitmap;
	HBITMAP _bitmapOld;
	
private:
};
#endif