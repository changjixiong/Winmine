// WindowsModel.cpp : Defines the entry point for the application.

//say no to MFC
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#define INITGUID            // include all GUIDs 

#include <windows.h>
#include <windowsx.h>
#include <mmsystem.h>
#include <stdio.h>
#include <ddraw.h>
#include <string>

using namespace std;

#include "WinmineRC.h"

#include "MyBitMap.h"
#include "GameBox.h"

#define		WINDOW_CLASS_NAME "WindowsModel"
#define		WINDOW_SUB_CLASS_NAME "Record"
#define		WINDOW_TITLE "Winmine"
#define		WINDOW_SUB_TITLE "Winmine"

#define KEYDOWN(vk_code) (GetAsyncKeyState(vk_code) & 0x8000 ? 1 : 0 )
#define KEYUP(vk_code) (GetAsyncKeyState(vk_code) & 0x8000 ? 0 : 1 )

HWND g_subhwnd;

GameBox		gameBox;
MyBitMap	bitmapGrid;
MyBitMap	bitmapOpen;
MyBitMap	bitmapFlag;
MyBitMap	bitmapQuest;
MyBitMap	bitmapMine;
MyBitMap	bitmapBomb;
MyBitMap	bitButton;
MyBitMap	bitButton_c;

MyBitMap	bitmapBottom;
MyBitMap	bitmapBottomL;
MyBitMap	bitmapBottomR;
MyBitMap	bitmapTop;
MyBitMap	bitmapTopL;
MyBitMap	bitmapTopR;
MyBitMap	bitmapFrameL;
MyBitMap	bitmapFrameR;

MyBitMap	bitmapChr0;
MyBitMap	bitmapChr1;
MyBitMap	bitmapChr2;
MyBitMap	bitmapChr3;
MyBitMap	bitmapChr4;
MyBitMap	bitmapChr5;
MyBitMap	bitmapChr6;
MyBitMap	bitmapChr7;
MyBitMap	bitmapChr8;
MyBitMap	bitmapChr9;
MyBitMap	bitmapChr10;

MyBitMap	bitmapNumFrame1;
MyBitMap	bitmapNumFrame2;

int WidthNum=12;
int HeightNum=12;
int MineNum=20;
int MarkedNum=0;
int OpenedGrid=0;
int GameOver=0;
int NeedRefresh=0;
int * pMine=NULL;
int nYOffset=52;
int nXOffset=9;
int nYExtra=8;
int nXExtra=8;
int nWordXOffset=5;
int nCurrentLevel=1;

int nRecordLevel1=999;
int nRecordLevel2=999;
int nRecordLevel3=999;

string strNameLevel1="";
string strNameLevel2="";
string strNameLevel3="";
string strPicFolder="pic/";

int nButtonXOffset=80;
int nButtonYOffset=10;
int nXGridNumStep=12;
int nYGridNumStep=12;

int nNumFrameXOffset=14;
int nNumFrameYOffset=12;
int nNumFrameXOffset2=14;
int nWin=0;
long nTimeElapse=-1;
long nTimeStamp=0;

#define MASK_MINE		0x100
#define MASK_OPEN		0x80
#define MASK_STATU		0x70
#define MASK_FLAG		0x20
#define MASK_MINE_NUM	0xF

#define GRID_OPEN		0x80
#define GRID_GRID		0x10
#define GRID_FLAG		0x20
#define GRID_QUEST		0x40

MyBitMap * bitchrs[11];

POINT around[8]={
	{-1,-1},
	{0,-1},
	{1,-1},
	{-1,0},	
	{1,0},
	{-1,1},
	{0,1},
	{1,1}
};

LRESULT CALLBACK WndProcSub(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

int DealGrid(int xNum, int yNum);
int ShowAllMine();

int BuildGrid(int xnum, int ynum)
{
	for (int i=0;i<xnum;i++)
	{
		for (int j=0;j<ynum;j++)
		{
			bitmapGrid.BitBlt(gameBox.g_hdcCanvas,bitmapGrid._Width*i+nXOffset,bitmapGrid._Height*j+nYOffset);
		}
	}
	return 0;
}

int MarkGrid(int x, int y)
{
	if (GameOver==1 || nWin==1)
	{
		return 0;
	}

	if (nTimeElapse==-1)
	{
		nTimeElapse=0;
		nTimeStamp=GetTickCount();
	}

	int xnum=(x - nXOffset)/bitmapOpen._Width;
	int ynum=(y - nYOffset)/bitmapOpen._Height;

	if (pMine[xnum+ynum*WidthNum] & MASK_OPEN)
	{
		return 0;
	}

	switch (pMine[xnum+ynum*WidthNum]& MASK_STATU)
	{
	case GRID_GRID:
		bitmapFlag.BitBlt(gameBox.g_hdcCanvas,
							xnum*bitmapFlag._Width+nXOffset,
							ynum*bitmapFlag._Height+nYOffset);
		pMine[xnum+ynum*WidthNum]&=~GRID_GRID;
		pMine[xnum+ynum*WidthNum]|=GRID_FLAG;
		MarkedNum++;
		break;
	case GRID_FLAG:
		bitmapQuest.BitBlt(gameBox.g_hdcCanvas,
							xnum*bitmapQuest._Width+nXOffset,
							ynum*bitmapQuest._Height+nYOffset);
		pMine[xnum+ynum*WidthNum]&=~GRID_FLAG;
		pMine[xnum+ynum*WidthNum]|=GRID_QUEST;
		MarkedNum--;
		break;
	case GRID_QUEST:
		bitmapGrid.BitBlt(gameBox.g_hdcCanvas,
							xnum*bitmapGrid._Width+nXOffset,
							ynum*bitmapGrid._Height+nYOffset);
		pMine[xnum+ynum*WidthNum]&=~GRID_QUEST;
		pMine[xnum+ynum*WidthNum]|=GRID_GRID;
		break;
	}

	return 0;	
}

int GetSurroundMineNum(int xNum, int yNum)
{
	int nResult=0;
	int xPos;
	int yPos;
	for (int i=0;i<8;i++)
	{
		xPos=xNum+around[i].x;
		yPos=yNum+around[i].y;
		if ( xPos < 0  || xPos >= WidthNum 
			|| yPos < 0 || yPos >= HeightNum)

		{
			continue;
		}

		if ((pMine[xPos+yPos*WidthNum] & MASK_MINE) == MASK_MINE)
		{
			nResult++;
		}
	}
	return nResult;
}

int GetSurroundFlag(int xNum, int yNum)
{
	int nResult=0;
	int xPos;
	int yPos;
	for (int i=0;i<8;i++)
	{
		xPos=xNum+around[i].x;
		yPos=yNum+around[i].y;
		if ( xPos < 0  || xPos >= WidthNum 
			|| yPos < 0 || yPos >= HeightNum)
			
		{
			continue;
		}
		
		if ((pMine[xPos+yPos*WidthNum] & MASK_OPEN) != MASK_OPEN 
			&& (pMine[xPos+yPos*WidthNum] & MASK_STATU) == GRID_FLAG)
		{
			nResult++;
		}
	}
	return nResult;
}

int DealSurround(int xNum, int yNum)
{
	int xPos;
	int yPos;
	for (int i=0;i<8;i++)
	{
		xPos=xNum+around[i].x;
		yPos=yNum+around[i].y;
		if ( xPos < 0  || xPos >= WidthNum 
			|| yPos < 0 || yPos >= HeightNum)
			
		{
			continue;
		}

		DealGrid(xPos,yPos);
	}

	return 0;
}

int DealSurroundWithFlag(int xNum, int yNum)
{
	int xPos;
	int yPos;
	int nMineNum=pMine[xNum+yNum*WidthNum] & MASK_MINE_NUM;
	if ( nMineNum > 0 && GetSurroundFlag(xNum, yNum) == nMineNum)
	{		
		for (int i=0;i<8;i++)
		{
			xPos=xNum+around[i].x;
			yPos=yNum+around[i].y;
			if ( xPos < 0  || xPos >= WidthNum 
				|| yPos < 0 || yPos >= HeightNum)
				
			{
				continue;
			}
			
			if ((pMine[xPos+ yPos*WidthNum] & MASK_STATU) == GRID_GRID)
			{
				DealGrid(xPos,yPos);
			}			
		}
	}

	return 0;
}

int OpenSurroundGrid(int x, int y)
{
	if (GameOver==1)
	{
		return 0;
	}
	
	int xnum=(x - nXOffset)/bitmapOpen._Width;
	int ynum=(y - nYOffset)/bitmapOpen._Height;
	
	if ((pMine[xnum+ynum*WidthNum] & MASK_OPEN) == MASK_OPEN)
	{
		return DealSurroundWithFlag(xnum, ynum);	
	}

	return 0;
}

int DealGrid(int xNum, int yNum)
{
	if ( ( pMine[xNum+yNum*WidthNum] & MASK_OPEN ) == GRID_OPEN
		|| ( pMine[xNum+yNum*WidthNum] & MASK_FLAG ) == GRID_FLAG ) 
	{
		return 0;
	}

	if (pMine[xNum+yNum*WidthNum] & MASK_MINE)
	{
		GameOver=1;
		nTimeElapse=-1;

		bitmapBomb.BitBlt(gameBox.g_hdcCanvas,
			xNum*bitmapBomb._Width+nXOffset,
			yNum*bitmapBomb._Height+nYOffset);	
		pMine[xNum+yNum*WidthNum]&=~MASK_MINE;
		return ShowAllMine();		
	}

	bitmapOpen.BitBlt(gameBox.g_hdcCanvas,
						xNum*bitmapOpen._Width+nXOffset,
						yNum*bitmapOpen._Height+nYOffset);

	gameBox.BitBlt();	

	pMine[xNum+yNum*WidthNum] |= MASK_OPEN;
	OpenedGrid++;

	if (OpenedGrid+MineNum>=WidthNum*HeightNum)
	{
		nWin=1;
		return ShowAllMine();		
	}

	int nMineNum=GetSurroundMineNum(xNum,yNum);

	if (nMineNum == 0)
	{
		return DealSurround(xNum,yNum);
	}
	else
	{
		if ((pMine[xNum+yNum*WidthNum] & MASK_MINE_NUM) == 0)
		{
			char szNum[2]={0};
			sprintf(szNum,"%d",nMineNum);
			TextOut(gameBox.g_hdcCanvas,xNum*bitmapOpen._Width+nWordXOffset+nXOffset,yNum*bitmapOpen._Height+nYOffset,szNum,strlen("1"));
			pMine[xNum+yNum*WidthNum]+=nMineNum;
		}

		return 0;		
	}
}

int ShowAllMine()
{
	for (int x=0;x<WidthNum;x++)
	{
		for (int y=0; y<HeightNum;y++)
		{
			if ((pMine[x+y*WidthNum] & MASK_STATU) == GRID_GRID
				&& (pMine[x+y*WidthNum] & MASK_MINE) > 0)
			{
				bitmapMine.BitBlt(gameBox.g_hdcCanvas,
								x*bitmapMine._Width+nXOffset,
								y*bitmapMine._Height+nYOffset);	
			}
		}
	}

	return 0;
}

int ClickGrid(int x, int y)
{
	if (GameOver==1 || nWin==1)
	{
		return 0;
	}

	int xnum=(x - nXOffset)/bitmapOpen._Width;
	int ynum=(y - nYOffset)/bitmapOpen._Height;

	if (nTimeElapse==-1)
	{
		nTimeElapse=0;
		nTimeStamp=GetTickCount();
	}

	return DealGrid(xnum,ynum);
}

int BltChr(MyBitMap * pbitmapChr, int x, int y)
{
	pbitmapChr->BitBlt(gameBox.g_hdcCanvas,x,y);
	
	return 0;
}


int ReInit()
{
	GameOver=0;
	NeedRefresh=0;

	MarkedNum=0;
	OpenedGrid=0;

	delete[] pMine;
	pMine=NULL;
	
	srand(GetTickCount());
	
	int nGridNum=WidthNum*HeightNum;
	int nMine=MineNum;
	int nPos=0;
	
	pMine=new int[nGridNum];
	for (int i=0;i<nGridNum;i++)
	{
		pMine[i]=GRID_GRID;
	}

	for (i=0;i<nMine;i++)
	{
		pMine[i]|=MASK_MINE;
	}
	
	for (i=0;i<nGridNum;i++)
	{
		int pos1=rand()%nGridNum;
		int pos2=rand()%nGridNum;

		int content=pMine[pos1];
		pMine[pos1]=pMine[pos2];
		pMine[pos2]=content;

	}

	gameBox.g_ScreenWidth=WidthNum*16+6+nXOffset+nXExtra;
	gameBox.g_ScreenHeigth=HeightNum*16+52+nYOffset+nYExtra;
	
	RECT rectWindow;
	GetWindowRect(gameBox.g_Mainhwnd,&rectWindow);

	SetWindowPos(gameBox.g_Mainhwnd,NULL,
					rectWindow.left,
					rectWindow.top,
					gameBox.g_ScreenWidth, gameBox.g_ScreenHeigth,
					SWP_SHOWWINDOW|SWP_NOZORDER);
	
	BuildGrid(WidthNum,HeightNum);

	int nBitFrameTime=0;
	
	bitmapTopL.BitBlt(gameBox.g_hdcCanvas,0,0);

	bitmapTop.BitBlt(gameBox.g_hdcCanvas,bitmapTopL._Width,0);
	nBitFrameTime=WidthNum/nXGridNumStep;
	while(nBitFrameTime>0)
	{
		bitmapTop.BitBlt(gameBox.g_hdcCanvas,bitmapTopL._Width+nBitFrameTime*nXGridNumStep*bitmapGrid._Width,0);
		nBitFrameTime--;
	}	
	bitmapTopR.BitBlt(gameBox.g_hdcCanvas,nXOffset+WidthNum*bitmapGrid._Width,0);

	bitmapFrameL.BitBlt(gameBox.g_hdcCanvas,0,bitmapTopL._Height);
	bitmapFrameR.BitBlt(gameBox.g_hdcCanvas,nXOffset+WidthNum*bitmapGrid._Width,bitmapTopL._Height);
	nBitFrameTime=HeightNum/nYGridNumStep;
	while(nBitFrameTime>0)
	{
		bitmapFrameL.BitBlt(gameBox.g_hdcCanvas,0,bitmapTopL._Height+nBitFrameTime*nYGridNumStep*bitmapGrid._Height);
		bitmapFrameR.BitBlt(gameBox.g_hdcCanvas,nXOffset+WidthNum*bitmapGrid._Width,bitmapTopL._Height+nBitFrameTime*nYGridNumStep*bitmapGrid._Height);
		nBitFrameTime--;
	}
	
	bitmapBottomL.BitBlt(gameBox.g_hdcCanvas,0,nYOffset+HeightNum*bitmapGrid._Height);
	bitmapBottom.BitBlt(gameBox.g_hdcCanvas,bitmapBottomL._Width,nYOffset+HeightNum*bitmapGrid._Height);
	nBitFrameTime=WidthNum/nXGridNumStep;
	while(nBitFrameTime>0)
	{
		bitmapBottom.BitBlt(gameBox.g_hdcCanvas,bitmapBottomL._Width+nBitFrameTime*nXGridNumStep*bitmapGrid._Width,nYOffset+HeightNum*bitmapGrid._Height);	
		nBitFrameTime--;
	}
	bitmapBottomR.BitBlt(gameBox.g_hdcCanvas,bitmapBottomL._Width+WidthNum*bitmapGrid._Width,nYOffset+HeightNum*bitmapGrid._Height);

	NeedRefresh=0;
	nTimeElapse=-1;

	nButtonXOffset=WidthNum*bitmapGrid._Width/2+bitmapTopL._Width - bitButton._Width/2;
	bitButton.BitBlt(gameBox.g_hdcCanvas,nButtonXOffset,nButtonYOffset);

	bitmapNumFrame1.BitBlt(gameBox.g_hdcCanvas,nNumFrameXOffset,nNumFrameYOffset);
	nNumFrameXOffset2=(WidthNum - 3)*bitmapGrid._Width - 3;
	bitmapNumFrame2.BitBlt(gameBox.g_hdcCanvas,nNumFrameXOffset+nNumFrameXOffset2,nNumFrameYOffset);
	bitmapChr0.BitBlt(gameBox.g_hdcCanvas,nNumFrameXOffset+nNumFrameXOffset2+1,nNumFrameYOffset+1);
	bitmapChr0.BitBlt(gameBox.g_hdcCanvas,nNumFrameXOffset+nNumFrameXOffset2+1+bitmapChr0._Width,nNumFrameYOffset+1);
	bitmapChr0.BitBlt(gameBox.g_hdcCanvas,nNumFrameXOffset+nNumFrameXOffset2+1+bitmapChr0._Width*2,nNumFrameYOffset+1);

	int nunit=(MineNum-MarkedNum)%10;
	int nDecade=(MineNum-MarkedNum)%100/10;
	int nHundred=(MineNum-MarkedNum)%1000/100;
	
	BltChr(bitchrs[nunit],nNumFrameXOffset+1+bitmapChr0._Width*2,nNumFrameYOffset+1);	
	BltChr(bitchrs[nDecade],nNumFrameXOffset+1+bitmapChr0._Width,nNumFrameYOffset+1);	
	BltChr(bitchrs[nHundred],nNumFrameXOffset+1,nNumFrameYOffset+1);

	return 0;
}

bool ButtonClicked(int x, int y)
{
	return (x >= bitmapTopL._Width+nButtonXOffset) && (x <= bitmapTopL._Width+nButtonXOffset+26) 
			&& (y >= nButtonYOffset) && (y <= nButtonYOffset+26);
}

bool GridClicked(int x, int y)
{
	return x>=nXOffset && y>=nYOffset && x<=nXOffset+WidthNum*bitmapGrid._Width && y<=nYOffset+HeightNum*bitmapGrid._Height;
}

int BltTime()
{
	if (nTimeElapse>=0)
	{
		nTimeElapse=GetTickCount() - nTimeStamp;
		nTimeElapse/=1000;

		int nunit=nTimeElapse%10;
		int nDecade=nTimeElapse%100/10;
		int nHundred=nTimeElapse%1000/100;

		BltChr(bitchrs[nunit],nNumFrameXOffset+nNumFrameXOffset2+1+bitmapChr0._Width*2,nNumFrameYOffset+1);	
		BltChr(bitchrs[nDecade],nNumFrameXOffset+nNumFrameXOffset2+1+bitmapChr0._Width,nNumFrameYOffset+1);	
		BltChr(bitchrs[nHundred],nNumFrameXOffset+nNumFrameXOffset2+1,nNumFrameYOffset+1);	

		int nRemain=MineNum-MarkedNum;

		if (nRemain<0)
		{
			nHundred=10;
			nRemain=-nRemain;
		}
		else
		{
			nHundred=(nRemain)%1000/100;
		}

		nunit=(nRemain)%10;
		nDecade=(nRemain)%100/10;		

		BltChr(bitchrs[nunit],nNumFrameXOffset+1+bitmapChr0._Width*2,nNumFrameYOffset+1);	
		BltChr(bitchrs[nDecade],nNumFrameXOffset+1+bitmapChr0._Width,nNumFrameYOffset+1);	
		BltChr(bitchrs[nHundred],nNumFrameXOffset+1,nNumFrameYOffset+1);
	}	
	return 0;
}

int Game_Main(void * parms = NULL, int num_parms = 0)
{

	if (KEYDOWN(VK_ESCAPE))
	{
		PostMessage(gameBox.g_Mainhwnd,WM_CLOSE,0,0);
	}
	
	if (GameOver==0 && nWin==0)
	{
		BltTime();
	}
	else
	{
		char szTemp[50];
		memset(szTemp,0,50);	

		if (nWin==1)
		{
			WNDCLASSEX wcexsub;
			
			wcexsub.cbSize = sizeof(WNDCLASSEX); 
			
			wcexsub.style			= CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS | CS_OWNDC;	
			wcexsub.lpfnWndProc	    = WndProcSub;
			wcexsub.cbClsExtra		= 0;
			wcexsub.cbWndExtra		= 0;
			wcexsub.hInstance		= gameBox.g_hInstance;
			wcexsub.hIcon			= LoadIcon(gameBox.g_hInstance, IDC_ICON);
			wcexsub.hCursor			= LoadCursor(gameBox.g_hInstance, IDC_ARROW);
			wcexsub.hbrBackground	= (HBRUSH)GetStockObject(BLACK_BRUSH);
			wcexsub.lpszMenuName	= NULL;//MAKEINTRESOURCE(MainMenu);
			wcexsub.lpszClassName	= WINDOW_SUB_CLASS_NAME;
			wcexsub.hIconSm			= LoadIcon(wcexsub.hInstance, IDI_APPLICATION);
			
			if (!RegisterClassEx(&wcexsub))
			{
				return 0;
			}
			
			if (!(g_subhwnd = CreateWindow(WINDOW_SUB_CLASS_NAME, 
				WINDOW_TITLE, 
				WS_OVERLAPPEDWINDOW | WS_VISIBLE,
				100,100,  // initial x,y
				400,400,  // initial width, height
				NULL, 
				NULL, 
				gameBox.g_hInstance, 
				NULL)))
			{
				return 0;
			}
			
		}
	}
	

	gameBox.BitBlt();
	
	if(NeedRefresh==2)
	{
		ReInit();
	}
	
	Sleep(10);
	return 0;
}

int Game_Init(void * parms = NULL, int num_parms = 0)
{
	bitmapGrid.Init(gameBox.g_hInstance,gameBox.g_hdcScreen,(strPicFolder+"grid.bmp").c_str(),16,16);
	bitmapOpen.Init(gameBox.g_hInstance,gameBox.g_hdcScreen,(strPicFolder+"open.bmp").c_str(),16,16);
	bitmapFlag.Init(gameBox.g_hInstance,gameBox.g_hdcScreen,(strPicFolder+"flag.bmp").c_str(),16,16);
	bitmapQuest.Init(gameBox.g_hInstance,gameBox.g_hdcScreen,(strPicFolder+"quest.bmp").c_str(),16,16);
	bitmapMine.Init(gameBox.g_hInstance,gameBox.g_hdcScreen,(strPicFolder+"Mine.bmp").c_str(),16,16);
	bitmapBomb.Init(gameBox.g_hInstance,gameBox.g_hdcScreen,(strPicFolder+"Bomb.bmp").c_str(),16,16);
	bitButton.Init(gameBox.g_hInstance,gameBox.g_hdcScreen,(strPicFolder+"button.bmp").c_str(),26,26);
	bitButton_c.Init(gameBox.g_hInstance,gameBox.g_hdcScreen,(strPicFolder+"button_c.bmp").c_str(),26,26);


	bitmapBottom.Init(gameBox.g_hInstance,gameBox.g_hdcScreen,(strPicFolder+"Bottom.bmp").c_str(),192,8);
	bitmapBottomL.Init(gameBox.g_hInstance,gameBox.g_hdcScreen,(strPicFolder+"BottomL.bmp").c_str(),9,8);
	bitmapBottomR.Init(gameBox.g_hInstance,gameBox.g_hdcScreen,(strPicFolder+"BottomR.bmp").c_str(),8,8);
	bitmapTop.Init(gameBox.g_hInstance,gameBox.g_hdcScreen,(strPicFolder+"top.bmp").c_str(),192,52);
	bitmapTopL.Init(gameBox.g_hInstance,gameBox.g_hdcScreen,(strPicFolder+"topL.bmp").c_str(),9,52);
	bitmapTopR.Init(gameBox.g_hInstance,gameBox.g_hdcScreen,(strPicFolder+"topR.bmp").c_str(),8,52);
	bitmapFrameL.Init(gameBox.g_hInstance,gameBox.g_hdcScreen,(strPicFolder+"FrameL.bmp").c_str(),9,192);
	bitmapFrameR.Init(gameBox.g_hInstance,gameBox.g_hdcScreen,(strPicFolder+"FrameR.bmp").c_str(),8,192);

	bitmapChr0.Init(gameBox.g_hInstance,gameBox.g_hdcScreen,(strPicFolder+"chr0.bmp").c_str(),13,23);
	bitmapChr1.Init(gameBox.g_hInstance,gameBox.g_hdcScreen,(strPicFolder+"chr1.bmp").c_str(),13,23);
	bitmapChr2.Init(gameBox.g_hInstance,gameBox.g_hdcScreen,(strPicFolder+"chr2.bmp").c_str(),13,23);
	bitmapChr3.Init(gameBox.g_hInstance,gameBox.g_hdcScreen,(strPicFolder+"chr3.bmp").c_str(),13,23);
	bitmapChr4.Init(gameBox.g_hInstance,gameBox.g_hdcScreen,(strPicFolder+"chr4.bmp").c_str(),13,23);
	bitmapChr5.Init(gameBox.g_hInstance,gameBox.g_hdcScreen,(strPicFolder+"chr5.bmp").c_str(),13,23);
	bitmapChr6.Init(gameBox.g_hInstance,gameBox.g_hdcScreen,(strPicFolder+"chr6.bmp").c_str(),13,23);
	bitmapChr7.Init(gameBox.g_hInstance,gameBox.g_hdcScreen,(strPicFolder+"chr7.bmp").c_str(),13,23);
	bitmapChr8.Init(gameBox.g_hInstance,gameBox.g_hdcScreen,(strPicFolder+"chr8.bmp").c_str(),13,23);
	bitmapChr9.Init(gameBox.g_hInstance,gameBox.g_hdcScreen,(strPicFolder+"chr9.bmp").c_str(),13,23);
	bitmapChr10.Init(gameBox.g_hInstance,gameBox.g_hdcScreen,(strPicFolder+"chr10.bmp").c_str(),13,23);

	bitmapNumFrame1.Init(gameBox.g_hInstance,gameBox.g_hdcScreen,(strPicFolder+"NumFrame.bmp").c_str(),41,25);
	bitmapNumFrame2.Init(gameBox.g_hInstance,gameBox.g_hdcScreen,(strPicFolder+"NumFrame.bmp").c_str(),41,25);
	
	bitchrs[0]=&bitmapChr0;
	bitchrs[1]=&bitmapChr1;
	bitchrs[2]=&bitmapChr2;
	bitchrs[3]=&bitmapChr3;
	bitchrs[4]=&bitmapChr4;
	bitchrs[5]=&bitmapChr5;
	bitchrs[6]=&bitmapChr6;
	bitchrs[7]=&bitmapChr7;
	bitchrs[8]=&bitmapChr8;
	bitchrs[9]=&bitmapChr9;
	bitchrs[10]=&bitmapChr10;

	int nRecordLevel1=999;
	int nRecordLevel2=999;
	int nRecordLevel3=999;

	char szTemp[50];
	memset(szTemp,0,50);
	::GetPrivateProfileString("level1","record","",szTemp,50,"./record.ini");	
	nRecordLevel1=atoi(szTemp);
	memset(szTemp,0,50);
	::GetPrivateProfileString("level1","name","",szTemp,50,"./record.ini");	
	strNameLevel1=szTemp;

	memset(szTemp,0,50);
	::GetPrivateProfileString("level2","record","",szTemp,50,"./record.ini");	
	nRecordLevel2=atoi(szTemp);
	memset(szTemp,0,50);
	::GetPrivateProfileString("level2","name","",szTemp,50,"./record.ini");	
	strNameLevel2=szTemp;

	memset(szTemp,0,50);
	::GetPrivateProfileString("level3","record","",szTemp,50,"./record.ini");	
	nRecordLevel3=atoi(szTemp);
	memset(szTemp,0,50);
	::GetPrivateProfileString("level3","name","",szTemp,50,"./record.ini");	
	strNameLevel3=szTemp;


	
	srand(GetTickCount());

	int nGridNum=WidthNum*HeightNum;
	int nMine=MineNum;
	int nPos=0;

	ReInit();

	SetBkMode(gameBox.g_hdcCanvas,TRANSPARENT);	
	
	return 0;
}

int Game_Shutdown(void * parms = NULL, int num_parms = 0)
{	

	gameBox.Release();

	bitmapGrid.Destory();
	bitmapOpen.Destory();
	bitmapFlag.Destory();
	bitmapQuest.Destory();
	bitmapMine.Destory();
	bitmapBomb.Destory();
	bitButton.Destory();
	bitButton_c.Destory();
	
	
	bitmapBottom.Destory();
	bitmapBottomL.Destory();
	bitmapBottomR.Destory();
	bitmapTop.Destory();
	bitmapTopL.Destory();
	bitmapTopR.Destory();
	bitmapFrameL.Destory();
	bitmapFrameR.Destory();
	
	bitmapChr0.Destory();
	bitmapChr1.Destory();
	bitmapChr2.Destory();
	bitmapChr3.Destory();
	bitmapChr4.Destory();
	bitmapChr5.Destory();
	bitmapChr6.Destory();
	bitmapChr7.Destory();
	bitmapChr8.Destory();
	bitmapChr9.Destory();
	bitmapChr10.Destory();
	
	bitmapNumFrame1.Destory();
	bitmapNumFrame2.Destory();

	delete[] pMine;
	pMine=NULL;
	return 0;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;
	RECT rect;

	static int wm_paint_count=0;

	switch (message) 
	{
	case WM_CREATE:		
		break;
	case WM_CHAR:

		break;	

	case WM_LBUTTONDOWN:
		if (GridClicked(LOWORD(lParam),HIWORD(lParam)))
		{
			ClickGrid(LOWORD(lParam), HIWORD(lParam));
		}
		else
		{			
			if (ButtonClicked(LOWORD(lParam),HIWORD(lParam)))
			{
				bitButton_c.BitBlt(gameBox.g_hdcCanvas,nButtonXOffset,nButtonYOffset);
				NeedRefresh=1;
			}
		}		
		break;
	case WM_LBUTTONUP:
		if (NeedRefresh == 1)
		{
			NeedRefresh=2;
		}
		break;
	case WM_RBUTTONDOWN:

		if (GridClicked(LOWORD(lParam),HIWORD(lParam)))
		{
			if (wParam & MK_LBUTTON)
			{
				OpenSurroundGrid(LOWORD(lParam), HIWORD(lParam));
			}
			else
			{
				MarkGrid(LOWORD(lParam), HIWORD(lParam));		
			}
		}
		
		break;
	case WM_RBUTTONUP:
		break;

	case WM_MOUSEMOVE:
		//sprintf(buffer,"(%d,%d) ",LOWORD(lParam),HIWORD(lParam));
		//SetWindowText(hWnd,buffer);	
		if (ButtonClicked(LOWORD(lParam),HIWORD(lParam)) && NeedRefresh == 1)
		{
			NeedRefresh=2;
		}
		break;
		
	case WM_COMMAND:
		wmId    = LOWORD(wParam); 
		wmEvent = HIWORD(wParam); 
		// Parse the menu selections:
		switch (wmId)
		{
			case MENU_RESTART:
				NeedRefresh=2;
				break;
			case MENU_lEVEL1:
				nCurrentLevel=1;
				WidthNum=12;
				HeightNum=12;
				NeedRefresh=2;
				break;
			case MENU_lEVEL2:
				nCurrentLevel=1;
				WidthNum=16;
				HeightNum=16;
				MineNum=40;
				NeedRefresh=2;
				break;
			case MENU_lEVEL3:
				nCurrentLevel=1;
				WidthNum=24;
				HeightNum=24;
				MineNum=100;
				NeedRefresh=2;
				break;
			default:
				break;
		}
		break;

	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		// TODO: Add any drawing code here...		
		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

LRESULT CALLBACK WndProcSub(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;
		
	switch (message) 
	{
	case WM_CREATE:		
		break;
	case WM_CHAR:
		break;	

	case WM_LBUTTONDOWN:
		//LOWORD(lParam),HIWORD(lParam))			
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		// TODO: Add any drawing code here...		
		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		//PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

int WINAPI WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
 	// TODO: Place code here.
	MSG msg;
	HWND hWnd;
	WNDCLASSEX wcex;
	
	wcex.cbSize = sizeof(WNDCLASSEX); 
	
	wcex.style			= CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS | CS_OWNDC;	
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, IDC_ICON);
	wcex.hCursor		= LoadCursor(hInstance, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)GetStockObject(BLACK_BRUSH);
	wcex.lpszMenuName	= MAKEINTRESOURCE(MainMenu);
	wcex.lpszClassName	= WINDOW_CLASS_NAME;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, IDI_APPLICATION);
	
	if (!RegisterClassEx(&wcex))
	{
		return 0;
	}
	
	gameBox.g_hInstance=hInstance;
	gameBox.g_ScreenWidth=WidthNum*16+6+nXOffset+nXExtra;
	gameBox.g_ScreenHeigth=HeightNum*16+52+nYOffset+nYExtra;

	if (!(hWnd = CreateWindow(WINDOW_CLASS_NAME, 
								WINDOW_TITLE, 
								(WS_OVERLAPPEDWINDOW|WS_VISIBLE) & (~WS_MAXIMIZEBOX) & (~WS_SIZEBOX),
								//WS_POPUP | WS_VISIBLE,
								(GetSystemMetrics(SM_CXSCREEN) - gameBox.g_ScreenWidth)/2,
								(GetSystemMetrics(SM_CYSCREEN) - gameBox.g_ScreenHeigth)/2,
								gameBox.g_ScreenWidth, gameBox.g_ScreenHeigth, 
								NULL, 
								NULL, 
								hInstance, 
								NULL)))
	{
		return 0;
	}

	gameBox.Init(hWnd,640,480);

	if (Game_Init()!=0)
	{
		MessageBox(hWnd,"Game_Init error","",MB_OK);
		
	}
	
	while(TRUE)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
			{
				break;
			}

			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}		


		Sleep(1);			
		Game_Main();
	}

	Game_Shutdown();	

	return msg.wParam;
}


