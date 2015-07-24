#undef UNICODE //More like EWWWWnicode
#define NOCOMM // Removes serial communication API includes and such.
#define WIN32_LEAN_AND_MEAN // Removes various API such as cryptography, DDE, RPC, Shell, and Windows Sockets.
#define VC_EXTRALEAN // Defines both the above definitions as well as a ton more.

#include "resource.h" 
#include <Winsock2.h>
#pragma comment(lib, "Ws2_32.lib") //We link to dlls this way, keeps size down.
#pragma comment(lib, "Kernel32.lib")
#pragma comment(lib, "Gdi32.lib")
#pragma comment(lib, "user32.lib")

#include <stdint.h> //Need them fancy ints

//Tunes
#pragma comment( lib, "winmm.lib" )
#include "ufmod.h"
#pragma comment(lib, "ufmod.lib")

///////////////////////////////////////////////////////////////////////////
//////////////////////////////GLOBALS//////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
//Product Version
#define GLOBALVERSION 15
#define GLOBALEDITION 505

///////////////////////////////////////////////////////////////////////////
////////////////////////////SIZE CHEATS////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
void * _memcpy(void *d, const void * s, unsigned int n)
{
	__asm
	{
		mov     esi, dword ptr[s]
			mov     edi, dword ptr[d]
			mov     ecx, dword ptr[n]
			rep     movsb
	}

	return d;
}
uint32_t rand()
{
	static uint32_t seed = 123456789;
	seed = (1103515245 * seed + 12345);
	return seed;
}

HANDLE g_hHeap = 0;
extern "C" HANDLE crt_initialize() { return g_hHeap = HeapCreate(0, 0, 0); }
extern "C" BOOL crt_uninitialize() { return HeapDestroy(g_hHeap); }
extern "C" void * malloc(unsigned int size) { 
		return HeapAlloc(g_hHeap, HEAP_ZERO_MEMORY | HEAP_NO_SERIALIZE | HEAP_GENERATE_EXCEPTIONS, size);
}
extern "C" void free(void * p) { 
	HeapFree(g_hHeap, 1, p); 
}
void * __cdecl operator new(unsigned int size){
	LPVOID a = HeapAlloc(g_hHeap,  HEAP_ZERO_MEMORY | HEAP_NO_SERIALIZE | HEAP_GENERATE_EXCEPTIONS, size);
	return a;
}
void __cdecl operator delete(void *p) { 
	int a = HeapFree(g_hHeap, 1, p); 
}

///////////////////////////////////////////////////////////////////////////
//////////////////////////CRYPTO CODES/////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
//Helpers
int digits(int n) {
	return (n < 10) ? 1 : (1 + digits(n / 10));
}

//Compressor
void compress(uint32_t num, int rounds, char* target)
{
	static char *lu = "0123456789ABCDEFGHIJKLMNPQRSTUVWXYZ";
	uint32_t index = 0;
	int i;
	for (i = 0; i < rounds; i++)
	{
		index = num % 0x23;
		num /= 0x23;
		target[i] = lu[index];
	}
	target[rounds] = lu[-1];
}

uint32_t getHashCode(char *hostname)
{
	uint32_t num = 0x1505;
	uint32_t num2 = num;
	int length = lstrlen(hostname);
	int i;
	for (i = 0; i < length; i++)
	{
		num = (uint32_t)(((num << 5) + num) ^ hostname[i++]);
		if (i < length)
		{
			num2 = (uint32_t)(((num2 << 5) + num2) ^ hostname[i]);
		}
	}
	return (num + (num2 * 0x5d588b65));
}

void getAuth(char* hostname, char* result) {
	//getSID
	uint32_t loSID = (uint32_t)getHashCode((char*)hostname);
	uint32_t hiSID = (uint16_t)getHashCode(lstrcatA(hostname, "|"));
	loSID = ((hiSID % 2) << 31) + (loSID >> 1);
	hiSID = ((loSID % 2) << 15) + (hiSID >> 1);
	hiSID = (uint32_t)((GLOBALVERSION << 26) + (hiSID << 10) + GLOBALEDITION);

	uint32_t a = (uint16_t)(hiSID % 1024);
	uint16_t b = (uint16_t)(hiSID >> 26);
	uint32_t d = ((hiSID << 6) >> 16);

	uint16_t  first = (uint16_t)(rand() % 65535);
	uint8_t sixth = (uint8_t)((first >> 4) ^ 65535);
	uint16_t  fourth = (uint16_t)loSID;
	uint16_t  fifth = (uint16_t)(loSID >> 16);
	uint16_t  second = (uint16_t)((b << 27) + (d << 10) + a);
	uint16_t  third = (uint16_t)(((b << 27) + (d << 10) + a) >> 16);

	uint8_t buffer[12];
	buffer[1] = first >> 8;
	buffer[0] = (buffer[1] << 8) ^ first;
	buffer[3] = second >> 8;
	buffer[2] = (buffer[3] << 8) ^ second;
	buffer[5] = third >> 8;
	buffer[4] = (buffer[5] << 8) ^ third;
	buffer[7] = fourth >> 8;
	buffer[6] = (buffer[7] << 8) ^ fourth;
	buffer[9] = fifth >> 8;
	buffer[8] = (buffer[9] << 8) ^ fifth;
	buffer[10] = sixth;
	buffer[11] = 0;

	int index;
	for (index = 0; index < 11; index++)
	{
		buffer[11] = (uint8_t)(buffer[11] + buffer[index]);
	}
	//Encrypt
	uint8_t buffera[12];
	for (index = 1; index < 6; index++)
	{
		int num2 = index * 2;
		buffer[num2] = (uint8_t)(buffer[num2] ^ buffer[0]);
		buffer[num2 + 1] = (uint8_t)(buffer[num2 + 1] ^ buffer[1]);
	}

	for (index = 0; index < 12; index++)
	{
		buffera[index] = (uint8_t)(buffer[index] >> 3);
		buffer[index] = (uint8_t)(buffer[index] << 5);
	}
	buffer[0] = (uint8_t)(buffer[0] + buffera[7]);
	buffer[1] = (uint8_t)(buffer[1] + buffera[0]);
	buffer[2] = (uint8_t)(buffer[2] + buffera[1]);
	buffer[3] = (uint8_t)(buffer[3] + buffera[2]);
	buffer[4] = (uint8_t)(buffer[4] + buffera[11]);
	buffer[5] = (uint8_t)(buffer[5] + buffera[4]);
	buffer[6] = (uint8_t)(buffer[6] + buffera[5]);
	buffer[7] = (uint8_t)(buffer[7] + buffera[6]);
	buffer[8] = (uint8_t)(buffer[8] + buffera[3]);
	buffer[9] = (uint8_t)(buffer[9] + buffera[8]);
	buffer[10] = (uint8_t)(buffer[10] + buffera[9]);
	buffer[11] = (uint8_t)(buffer[11] + buffera[10]);

	char uno[8]; char dos[8]; char trs[8];
	compress((uint32_t)((buffer[3] << 24) + (buffer[2] << 16) + (buffer[1] << 8) + buffer[0]), 7, uno);
	compress((uint32_t)((buffer[7] << 24) + (buffer[6] << 16) + (buffer[5] << 8) + buffer[4]), 7, dos);
	compress((uint32_t)((buffer[11] << 24) + (buffer[10] << 16) + (buffer[9] << 8) + buffer[8]), 7, trs);


	wsprintfA(result, "%s-%s-%s", uno, dos, trs);
}

static char serial[8];
void getPK(char* ret) {//Blech char*
	static uint32_t majors[16] = { 48335, 48334, 48333, 48332, 48331, 48330, 48329, 48328, 48327, 48326, 48325, 48324, 48323, 48322, 48321, 48320 };
	uint32_t major = majors[rand() % 16];
	char majorS[6]; char minorS[6]; char tinyS[3]; 
	compress(major, 5, majorS);
	compress(major << 1, 5, minorS);
	compress(GLOBALVERSION ^ GLOBALEDITION, 2, tinyS);

	wsprintfA(serial, "%u", (major << 1) ^ major);
	wsprintfA(ret, "%s-%s%s", majorS, tinyS, minorS);
}
///////////////////////////////////////////////////////////////////////////
/////////////////////////////GFX CODES/////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
HINSTANCE hInst;
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
static int wH = 260;
static int wW = 320;

static HBITMAP* getPinkie(int i) {
	return (HBITMAP*)LoadBitmap(hInst, MAKEINTRESOURCE(i + 108));
}
HANDLE AddResourceFont(char* ResID, DWORD *Installed)
{
	DWORD Count = 0;
	char* type = "RT_FONT";
	HRSRC Resource = FindResourceA(hInst, ResID, type);
	if (Resource) {
		LoadResource(hInst, Resource);
		LockResource(Resource);
		return AddFontMemResourceEx(Resource, SizeofResource(hInst, Resource), 0, &Count);
	}
	return false;
}
HBITMAP CreateBitmapMask(HBITMAP hbmColour, COLORREF crTransparent)
{
	HDC hdcMem, hdcMem2;
	HBITMAP hbmMask;
	BITMAP bm;
	GetObject(hbmColour, sizeof(BITMAP), &bm);
	hbmMask = CreateBitmap(bm.bmWidth, bm.bmHeight, 1, 1, NULL);
	hdcMem = CreateCompatibleDC(0);
	hdcMem2 = CreateCompatibleDC(0);
	SelectObject(hdcMem, hbmColour);
	SelectObject(hdcMem2, hbmMask);
	SetBkColor(hdcMem, crTransparent);
	BitBlt(hdcMem2, 0, 0, bm.bmWidth, bm.bmHeight, hdcMem, 0, 0, SRCCOPY);
	BitBlt(hdcMem, 0, 0, bm.bmWidth, bm.bmHeight, hdcMem2, 0, 0, SRCINVERT);
	DeleteDC(hdcMem);
	DeleteDC(hdcMem2);

	return hbmMask;
}
int WINAPI winMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
	hInst = hInstance;

	MSG  msg;
	HWND hwnd;
	WNDCLASSA wc((WNDCLASSA()));

	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.lpszClassName = "Professional Party Time";
	wc.hInstance = hInst;
	wc.hbrBackground = GetSysColorBrush(RGB(0,0,0));
	wc.lpszMenuName = NULL;
	wc.lpfnWndProc = WndProc;
	wc.hCursor = LoadCursorA(NULL, IDC_ARROW);
	wc.hIcon = LoadIconA(NULL, IDI_APPLICATION);

	RegisterClassA(&wc);
	hwnd = CreateWindowExA(WS_EX_LAYERED | WS_EX_COMPOSITED, wc.lpszClassName, "",
		WS_VISIBLE | WS_SYSMENU | WS_POPUP,
		100, 100, wW, wH, NULL, NULL, hInst, NULL);
	SetLayeredWindowAttributes(hwnd, 0x00000000, 255, LWA_COLORKEY|LWA_ALPHA);

	ShowWindow(hwnd, nCmdShow);
	UpdateWindow(hwnd);	

	while (GetMessage(&msg, NULL, 0, 0)) {
		//Workaround for focusables stealing my Esc key
		if (msg.message == WM_KEYDOWN){
			if (msg.wParam == VK_ESCAPE) {
				SendMessage(hwnd, WM_CLOSE, 0, 0);
			}
		}
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return (int)msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg,
	WPARAM wParam, LPARAM lParam)
{
	static int xClick;
	static int yClick;
	static HBRUSH hbBg = CreateSolidBrush(0x002f2f2f);
	static HBRUSH hbBreak = CreateSolidBrush(0x00AA9966);

	static HFONT icdimitri22;
	static HFONT icdimitri20;
	static LOGFONTA logfont;
	static HBITMAP dbhbm;
	static HBITMAP dbphbm;
	static RECT nullRect = { 0, 0, wW, 126 };
	static char hostname[128];

	switch (msg)
	{
	case WM_COMMAND:{
		if (HIWORD(wParam) == EN_SETFOCUS && (LOWORD(wParam) == 12 || LOWORD(wParam) == 13 || LOWORD(wParam) == 14)) {
			HWND ctrl = GetDlgItem(hwnd, LOWORD(wParam));
			PostMessage(ctrl, EM_SETSEL, 0, -1);
			SetFocus(ctrl);
		}
		return 0;
	}
	case WM_CREATE:
	{
		WSADATA wsaData;
		WSAStartup(MAKEWORD(2, 2), &wsaData);
		gethostname(hostname, 128);
		char* sv = hostname;
		while (*sv != '\0')
		{
			if (*sv >= 'a' && *sv <= 'z')
				*sv = *sv - ('a' - 'A');
			sv++;
		}
		//Background
		HWND icBg = CreateWindowA("STATIC", NULL,
			WS_CHILD | WS_VISIBLE,
			0, 90, wW, 170,
			hwnd, (HMENU)10, hInst, NULL);

		HWND icPINKIETIEM = CreateWindowA("STATIC", "",
			WS_VISIBLE|WS_CHILD,
			110, 0, 122, 126,
			hwnd, (HMENU)11, hInst, NULL);
		SetTimer(hwnd, 1, 80, NULL);

		HWND icSer = CreateWindowA("EDIT", NULL,
			WS_TABSTOP | WS_VISIBLE | WS_CHILD | ES_RIGHT,
			wW-80-3, 189, 80, 16,
			hwnd, (HMENU)12, hInst, NULL);
		HWND icPK = CreateWindowA("EDIT", NULL,
			WS_TABSTOP | WS_VISIBLE | WS_CHILD | ES_RIGHT,
			wW-125-3, 214, 127, 16,
			hwnd, (HMENU)13, hInst, NULL);
		HWND icAC = CreateWindowA("EDIT", NULL,
			WS_TABSTOP | WS_VISIBLE | WS_CHILD | ES_RIGHT,
			wW-210-3, 239, 210, 16,
			hwnd, (HMENU)14, hInst, NULL);

		HFONT hfDefault = CreateFontA(16, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_OUTLINE_PRECIS,
			CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, VARIABLE_PITCH, "Lucida Sans");

		char pk[14]; char auth[24];
		getPK(pk);
		getAuth(hostname, auth);

		SendMessageA(icPK, WM_SETFONT, (WPARAM)hfDefault, MAKELPARAM(FALSE, 0));
		SendMessageA(icPK, WM_SETTEXT, 0, (LPARAM)pk);
		SendMessageA(icSer, WM_SETFONT, (WPARAM)hfDefault, MAKELPARAM(FALSE, 0));
		SendMessageA(icSer, WM_SETTEXT, 0, (LPARAM)serial);
		SendMessageA(icAC, WM_SETFONT, (WPARAM)hfDefault, MAKELPARAM(FALSE, 0));
		SendMessageA(icAC, WM_SETTEXT, 0, (LPARAM)auth);
		DeleteObject(hfDefault);

		//Load Dimitri from Res and set it up
		AddResourceFont(MAKEINTRESOURCEA(IDR_RT_FONT1), NULL);
		//-13,0,0,0,400,0,0,0,0,3,2,1,2,Dimitri Swank
		logfont.lfCharSet = DEFAULT_CHARSET;
		logfont.lfClipPrecision = CLIP_DEFAULT_PRECIS;
		logfont.lfEscapement = 0;
		_memcpy(logfont.lfFaceName, "Dimitri Swank", LF_FACESIZE);
		logfont.lfHeight = -22;
		logfont.lfItalic = FALSE;
		logfont.lfOrientation = 0;
		logfont.lfOutPrecision = OUT_TT_PRECIS;
		logfont.lfQuality = PROOF_QUALITY;
		logfont.lfStrikeOut = FALSE;
		logfont.lfUnderline = FALSE;
		logfont.lfWeight = FW_DONTCARE;
		icdimitri22 = CreateFontIndirectA(&logfont);
		logfont.lfHeight = -20;
		icdimitri20 = CreateFontIndirectA(&logfont);

		//Center Window on screen
		/*RECT rc;
		GetWindowRect(hwnd, &rc) ;
		SetWindowPos(hwnd, 0,
		(GetSystemMetrics(SM_CXSCREEN) - rc.right)/2,
		(GetSystemMetrics(SM_CYSCREEN) - rc.bottom)/2,
		0, 0, SWP_NOZORDER|SWP_NOSIZE);*/

		uFMOD_PlaySong(MAKEINTRESOURCE(IDR_MOD), 0, XM_RESOURCE);

		return 0;
	}
	case WM_PAINT: {
		//Vars
		HDC hdc;
		RECT wRect;
		RECT cRect;
		if (GetUpdateRect(hwnd, &wRect, FALSE) == 0) {
			return 0; //Nothing to paint
		}
		GetClientRect(hwnd, &cRect);
		PAINTSTRUCT gps;
		PAINTSTRUCT ps;
		hdc = BeginPaint(hwnd, &gps);
		LPCSTR out;
		static int fourthWall = 0;
		fourthWall = (fourthWall < 39) ? fourthWall + 1 : 0;

		switch (wRect.bottom)
		{
		case 205://SR
		case 230://PK
		case 255://AC
		case 260: {
			HWND icBG = GetDlgItem(hwnd, 10);
			HDC ichdc = BeginPaint(icBG, &ps);
			HFONT hfOld = (HFONT)SelectObject(ichdc, icdimitri22);

			SetTextColor(ichdc, 0x00FFFFFF);
			SetBkColor(ichdc, 0x002F2F2F);
			//BG
			FillRect(ichdc, &cRect, hbBg);
			//P3
			cRect.top = 5;
			cRect.bottom = cRect.top+35;
			out = "Professional Party People";
			DrawTextA(ichdc, out, -1, &cRect, DT_SINGLELINE | DT_CENTER);

			SelectObject(ichdc, icdimitri20);
			//AppName
			cRect.top = 60;
			cRect.bottom = cRect.top+30;
			out = "Infinite Cupcake Generator";
			DrawTextA(ichdc, out, -1, &cRect, DT_SINGLELINE|DT_CENTER);
			//Presents
			cRect.top = 30;
			cRect.bottom = cRect.top + 30;
			out = "Presents";
			DrawTextA(ichdc, out, -1, &cRect, DT_SINGLELINE | DT_CENTER);
			//Breaks
			cRect.top = 55;
			cRect.right = 320;
			cRect.bottom = cRect.top+4;
			FillRect(ichdc, &cRect, hbBreak);
			cRect.top = 85;
			cRect.bottom = cRect.top + 4;
			FillRect(ichdc, &cRect, hbBreak);
			//Serial
			GetClientRect(icBG, &cRect);
			cRect.top += 93;
			cRect.left += 8;
			out = "Serial:";
			DrawTextA(ichdc, out, -1, &cRect, DT_SINGLELINE);
			//Product Key
			cRect.top += 25;
			out = "Product Key:";
			DrawTextA(ichdc, out, -1, &cRect, DT_SINGLELINE);
			//Auth Code
			cRect.top += 25;
			out = "Auth Code:";
			DrawTextA(ichdc, out, -1, &cRect, DT_SINGLELINE);

			SelectObject(ichdc, hfOld);
			//Back up
			if (dbhbm == NULL) {
				HDC dbdc = CreateCompatibleDC(ichdc);
				dbhbm = CreateCompatibleBitmap(ichdc, wW, 170);
				HBITMAP hHBOld = (HBITMAP)SelectObject(dbdc, dbhbm);
				BitBlt(dbdc, 0, 0, wW, 170, ichdc, 0, 0, SRCCOPY);
				SelectObject(dbdc, hHBOld);
				DeleteObject(hHBOld);
				DeleteDC(dbdc);
			}
			EndPaint(icBG, &ps);
			if (dbphbm == NULL) {
				HWND icGn = GetDlgItem(hwnd, 11);
				HDC icghdc = BeginPaint(icGn, &ps);
				HDC pmdc = CreateCompatibleDC(icghdc);
				dbphbm = CreateCompatibleBitmap(icghdc, 122, 126);
				HBITMAP hHBOld = (HBITMAP)SelectObject(pmdc, dbphbm);
				SetBkMode(icghdc, TRANSPARENT);
				SetBkMode(pmdc, TRANSPARENT);
				BitBlt(pmdc, 0, 0, 122, 126, hdc, 110, 0, SRCCOPY);
				SelectObject(pmdc, hHBOld);
				DeleteObject(hHBOld);
				DeleteDC(pmdc);
				EndPaint(icGn, &ps);
			}
			break; 
		}
		case 126: {
			HWND icBG = GetDlgItem(hwnd, 10);
			//Reset form with our secret backup
			HDC ichdc = BeginPaint(icBG, &ps);
			HDC dbdc = CreateCompatibleDC(ichdc);
			HBITMAP temp = (HBITMAP)SelectObject(dbdc, dbhbm);
			BitBlt(ichdc, 0, 0, wW, 170, dbdc, 0, 0, SRCCOPY);
			SelectObject(dbdc, temp);
			DeleteDC(dbdc);
			EndPaint(icBG, &ps);

			//And Reset pinkie hdc
			HWND icGn = GetDlgItem(hwnd, 11);
			HDC icghdc = BeginPaint(icGn, &ps);
			HDC pmdc = CreateCompatibleDC(icghdc);
			HBITMAP oldBM = (HBITMAP)SelectObject(pmdc, dbphbm);
			SetBkMode(icghdc, TRANSPARENT);
			SetBkMode(pmdc, TRANSPARENT);
			BitBlt(icghdc, 0, 0, 122, 166, pmdc, 0, 0, SRCCOPY);
			
			//Setup
			BITMAP pinkBM;
			HBITMAP* pinkHBM = getPinkie(fourthWall);
			HBITMAP pinkMBM = CreateBitmapMask((HBITMAP)pinkHBM, 0x00000000);
			GetObject(pinkHBM, sizeof(pinkBM), &pinkBM);
			
			//Paint
			SelectObject(pmdc, pinkMBM);
			BitBlt(icghdc, 0, 0, pinkBM.bmWidth, pinkBM.bmHeight, pmdc, 0, 0, SRCAND);
			SelectObject(pmdc, pinkHBM);
			BitBlt(icghdc, 0, 0, pinkBM.bmWidth, pinkBM.bmHeight, pmdc, 0, 0, SRCPAINT);

			//Cleanup
			SelectObject(pmdc, oldBM);
			DeleteObject(pinkHBM);
			DeleteObject(pinkMBM);
			DeleteDC(pmdc);
			DeleteDC(icghdc);
			EndPaint(icGn, &ps);
			break;
		}
		default: {
			SetTextColor(hdc, 0x00FFFFFF);
			SetBkColor(hdc, 0x002F2F2F);
		}
		}
		EndPaint(hwnd, &gps);
		return 1;
	}
	case WM_ERASEBKGND: {
		return (LRESULT)1;
	}
	case WM_TIMER: {
		HWND icGn = GetDlgItem(hwnd, 11);
		InvalidateRect(icGn, &nullRect, TRUE);
		//UpdateWindow(icGn);
		RedrawWindow(hwnd, &nullRect, NULL, RDW_INTERNALPAINT | RDW_ERASE | RDW_INVALIDATE);
		return 0;
	}
	case WM_CTLCOLOREDIT:
	{
		DWORD CtrlID = GetDlgCtrlID((HWND)lParam);
		if (CtrlID == 12 || CtrlID == 13 || CtrlID == 14) {
			HDC hdc = (HDC)wParam;
			SetTextColor(hdc, 0x00FFFFFF);
			SetBkColor(hdc, RGB(0x2F, 0x2F, 0x2F));
			return (INT_PTR)hbBg;
		}
		return 0;
	}
	case WM_DESTROY:
	{
		DeleteObject(hbBg);
		DeleteObject(hbBreak);
		DeleteObject(icdimitri20);
		DeleteObject(icdimitri22);
		DeleteObject(dbhbm);
		DeleteObject(dbphbm);
		uFMOD_StopSong();
		PostQuitMessage(0);
		ExitProcess(0);
	}
	case WM_LBUTTONDOWN:
		//Get the click position
		xClick = LOWORD(lParam);
		yClick = HIWORD(lParam);
		if (xClick > 100 && xClick < 200 && yClick < 120) {
			HWND icPK = GetDlgItem(hwnd, 13);
			HWND icSer = GetDlgItem(hwnd, 12);
			HWND icAC = GetDlgItem(hwnd, 14);
			char pk[14]; char auth[24];
			getPK(pk);
			getAuth(hostname, auth);
			SendMessageA(icPK, WM_SETTEXT, 0, (LPARAM)pk);
			SendMessageA(icSer, WM_SETTEXT, 0, (LPARAM)serial);
			SendMessageA(icAC, WM_SETTEXT, 0, (LPARAM)auth);
			return 1;
		}
		//Restrict mouse input to current window
		SetCapture(hwnd);
		return 1;
	case WM_LBUTTONUP:
	{
		//Window no longer requires all mouse input
		ReleaseCapture();
		return 1;
	}
	case WM_MOUSEMOVE:
	{
		if (GetCapture() == hwnd)  //Check if this window has mouse input
		{
			//Get the window's screen coordinates
			RECT rcWindow;
			GetWindowRect(hwnd, &rcWindow);

			//Get the current mouse coordinates
			int xMouse = LOWORD(lParam);
			int yMouse = HIWORD(lParam);

			//Calculate the new window coordinates
			int xWindow = rcWindow.left + xMouse - xClick;
			int yWindow = rcWindow.top + yMouse - yClick;

			//Set the window's new screen position (don't resize or change z-order)
			SetWindowPos(hwnd, NULL, xWindow, yWindow, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
		}
		break;
	}
	}
	return DefWindowProcA(hwnd, msg, wParam, lParam);
}
extern "C" int winMainCRTStartup()
{
	crt_initialize();
	winMain(GetModuleHandle(NULL), NULL, NULL, 5);
	return 0;
}