// WinParticles.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "WinParticles.h"
#include "ParticleSys.h"
#include "BackBuffer.h"
#include "Gradient.h"
#include "Common.h"
#include "DisplayItem.h"
#ifdef _DEBUG
#include "DisplayItemTest.h"
#endif
#include <string>
#include <vector>
#include <sstream>
#include <commdlg.h>

#define MAX_LOADSTRING 100
#define NUM_GRADIENTS 4
#define MAX_PARAM 6

#define SELPARAM_CHAR(x) (selParam == (x) ? '>' : ' ')
#define MF_CHECK_BOOL(b) ((b) ? MF_CHECKED : MF_UNCHECKED)

// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name
HDC particleBmpDC;
int selParam;
CParticleSys *psys;
HCURSOR curEmitter;

// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPTSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

 	// TODO: Place code here.
	MSG msg;
	HACCEL hAccelTable;

	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_WINPARTICLES, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_WINPARTICLES));

	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int)msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	curEmitter = LoadCursor(hInstance, MAKEINTRESOURCE(IDC_EMITTER));

	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_WINPARTICLES));
	wcex.hCursor		= curEmitter;
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_WINPARTICLES);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_WINPARTICLES));

	return RegisterClassEx(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;

   hInst = hInstance; // Store instance handle in our global variable

   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

void SelectParam(int paramNum, double *deltaMult);
void SetVelocityMode(CParticleSys::VelocityMode mode, HWND mainWnd);

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hDC;
	static HFONT font;
	static CBackBuffer *bbuf;
	static CGradient *gradients[NUM_GRADIENTS];
	static HBITMAP particleBitmap;
	std::wostringstream out;
	int fps = 60;
	static double emitterX, emitterY;
	double temp1, temp2;
	double delta;
	static double deltaMult;
	static CHOOSECOLOR colorDlg;
	static COLORREF customColors[16];
	static bool rectVelYSelected = false;
	static std::vector<CDisplayItem*> displayItems;
	static bool mouseControlsParams = true;

	switch (message)
	{
	case WM_CREATE:
		psys = new CParticleSys();
		psys->SetAcceleration(0.0, -600.0);

		SelectParam(0, &deltaMult);

		gradients[0] = new CGradient(5);
		gradients[0]->SetStep(0, 0.0, RGB(255, 255, 0));
		gradients[0]->SetStep(1, 0.25, RGB(255, 0, 0));
		gradients[0]->SetStep(2, 0.5, RGB(0, 0, 0));
		gradients[0]->SetStep(3, 0.51, RGB(64, 64, 64));
		gradients[0]->SetStep(4, 1.0, RGB(0, 0, 0));

		gradients[1] = new CGradient(7);
		gradients[1]->SetStep(0, 0.0, RGB(255, 0, 0));
		gradients[1]->SetStep(1, 1.0 / 6, RGB(255, 255, 0));
		gradients[1]->SetStep(2, 2.0 / 6, RGB(0, 255, 0));
		gradients[1]->SetStep(3, 3.0 / 6, RGB(0, 255, 255));
		gradients[1]->SetStep(4, 4.0 / 6, RGB(0, 0, 255));
		gradients[1]->SetStep(5, 5.0 / 6, RGB(255, 0, 255));
		gradients[1]->SetStep(6, 1.0, RGB(255, 0, 0));

		gradients[2] = new CGradient(2);
		gradients[2]->SetStep(0, 0.0, RGB(255, 255, 255));
		gradients[2]->SetStep(1, 1.0, RGB(0, 0, 0));

		gradients[3] = new CGradient(2);
		gradients[3]->SetStep(0, 0.0, RGB(255, 255, 255));
		gradients[3]->SetStep(1, 1.0, RGB(255, 255, 255));

		particleBitmap = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_PARTICLES));
		particleBmpDC = CreateCompatibleDC(NULL);
		SelectObject(particleBmpDC, particleBitmap);

		psys->SetDefGradient(gradients[0]);

		font = CreateFont(0, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, NONANTIALIASED_QUALITY, DEFAULT_PITCH, L"Fixedsys");
		SetTimer(hWnd, 0, 1000 / fps, NULL);
		bbuf = new CBackBuffer(hWnd);

		colorDlg.lStructSize = sizeof(colorDlg);
		colorDlg.hwndOwner = hWnd;
		colorDlg.lpCustColors = customColors;
		colorDlg.Flags = CC_RGBINIT;

		for (int i = 0; i < 16; i++) customColors[i] = RGB(255, 255, 255);

#ifdef _DEBUG
		displayItems.push_back(new CDisplayItemTest());
#endif
		
		break;
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// Parse the menu selections:
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		case ID_PARAMS_VM_POLAR: SetVelocityMode(CParticleSys::VelocityMode::MODE_POLAR, hWnd); break;
		case ID_PARAMS_VM_RECT: SetVelocityMode(CParticleSys::VelocityMode::MODE_RECT, hWnd); break;
		case ID_PARAMS_MINVEL: SelectParam(0, &deltaMult); break;
		case ID_PARAMS_MAXVEL: SelectParam(1, &deltaMult); break;
		case ID_PARAMS_MINVELX: SelectParam(0, &deltaMult); rectVelYSelected = false; break;
		case ID_PARAMS_MAXVELX: SelectParam(1, &deltaMult); rectVelYSelected = false; break;
		case ID_PARAMS_MINVELY: SelectParam(0, &deltaMult); rectVelYSelected = true; break;
		case ID_PARAMS_MAXVELY: SelectParam(1, &deltaMult); rectVelYSelected = true; break;
		case ID_PARAMS_XACCEL: SelectParam(2, &deltaMult); break;
		case ID_PARAMS_YACCEL: SelectParam(3, &deltaMult); break;
		case ID_PARAMS_MAXAGE: SelectParam(4, &deltaMult); break;
		case ID_PARAMS_EMISSIONRATE: SelectParam(5, &deltaMult); break;
		case ID_PARAMS_EMISSIONRADIUS: SelectParam(6, &deltaMult); break;
		case ID_PARAMS_TINT:
			colorDlg.rgbResult = psys->GetDefaultTint();
			if (ChooseColor(&colorDlg)) {
				psys->SetDefaultTint(colorDlg.rgbResult);
			}
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_SIZE:
		bbuf->UpdateSize();
		break;
	case WM_PAINT:
		hDC = bbuf->GetDC();

		// Draw black background and particles
		RECT clientRect;
		GetClientRect(hWnd, &clientRect);
		PatBlt(hDC, clientRect.left, clientRect.top, clientRect.right - clientRect.left, clientRect.bottom - clientRect.top, BLACKNESS);
		psys->Draw(hDC, &clientRect);

		// Draw all active display items
		for (std::vector<CDisplayItem*>::iterator i = displayItems.begin(); i != displayItems.end(); i++) {
			(*i)->Draw(hDC, &clientRect);
		}

		// Draw text display
		SelectObject(hDC, font);
		SetBkMode(hDC, TRANSPARENT);
		
#ifdef _DEBUG
		out << "DEBUG BUILD (performance is not optimal)" << std::endl;
#endif
		out << "Number of particles: " << psys->GetParticles()->size() << std::endl;
		out << "Press 1 - " << NUM_GRADIENTS << " to change colors" << std::endl;
		out << "Press R to reset parameters" << std::endl;
		out << "Use mouse buttons and wheel to edit params" << std::endl;
		out << "[-+] Adjustment multiplier: " << deltaMult << std::endl;
		out << "[Z]  Velocity mode: " << CParticleSys::VelocityModeText(psys->GetVelocityMode()) << std::endl;
		out << "Current parameters:" << std::endl;
		if (psys->GetVelocityMode() == CParticleSys::VelocityMode::MODE_POLAR) {
			psys->GetVelocity(&temp1, &temp2);
			out << SELPARAM_CHAR(0) << " Minimum velocity:   " << temp1 << std::endl;
			out << SELPARAM_CHAR(1) << " Maximum velocity:   " << temp2 << std::endl;
		} else {
			if (!rectVelYSelected) {
				psys->GetRectVelocityX(&temp1, &temp2);
				out << SELPARAM_CHAR(0) << " Minimum X velocity: " << temp1 << std::endl;
				out << SELPARAM_CHAR(1) << " Maximum X velocity: " << temp2 << std::endl;
			} else {
				psys->GetRectVelocityY(&temp1, &temp2);
				out << SELPARAM_CHAR(0) << " Minimum Y velocity: " << temp1 << std::endl;
				out << SELPARAM_CHAR(1) << " Maximum Y velocity: " << temp2 << std::endl;
			}
		}
		psys->GetAcceleration(&temp1, &temp2);
		out << SELPARAM_CHAR(2) << " X acceleration:     " << temp1 << std::endl;
		out << SELPARAM_CHAR(3) << " Y acceleration:     " << temp2 << std::endl;
		out << SELPARAM_CHAR(4) << " Maximum age:        " << psys->GetMaxAge() << std::endl;
		out << SELPARAM_CHAR(5) << " Emission rate:      " << psys->GetEmissionRate() << std::endl;
		out << SELPARAM_CHAR(6) << " Emission radius:    " << psys->GetEmissionRadius() << std::endl;

		clientRect.left += 5; clientRect.top += 5;
		SetTextColor(hDC, 0x000000);
		DrawText(hDC, out.str().c_str(), out.str().length(), &clientRect, 0);
		clientRect.left -= 1; clientRect.top -= 1;
		SetTextColor(hDC, 0xFFFF00);
		DrawText(hDC, out.str().c_str(), out.str().length(), &clientRect, 0);
		clientRect.left -= 4; clientRect.top -= 4;

		// Draw gradient at bottom
		for (int x = clientRect.left; x < clientRect.right; x++) {
			double point = Interpolate(x, clientRect.left, clientRect.right, 0.0, 1.0);
			HPEN gradientPen = CreatePen(PS_SOLID, 1, MultiplyColors(psys->GetDefaultTint(), psys->GetDefGradient()->ColorAtPoint(point)));
			SelectObject(hDC, gradientPen);
			MoveToEx(hDC, x, clientRect.bottom - 24, NULL);
			LineTo(hDC, x, clientRect.bottom);
			SelectObject(hDC, GetStockObject(WHITE_PEN)); //deselect pen so it can be deleted
			DeleteObject(gradientPen);
		}

		hDC = BeginPaint(hWnd, &ps);
		bbuf->CopyToFront(hDC);
		EndPaint(hWnd, &ps);
		break;
	case WM_KEYDOWN:
		if (0x31 <= wParam && wParam <= 0x30 + NUM_GRADIENTS) {
			psys->SetDefGradient(gradients[wParam - 0x31]);
		} else if (wParam == (WPARAM)'R') {
			psys->DefaultParameters();
			SetVelocityMode(psys->GetVelocityMode(), hWnd);
		} else if (wParam == (WPARAM)'Z') {
			if (psys->GetVelocityMode() == CParticleSys::VelocityMode::MODE_POLAR)
				SetVelocityMode(CParticleSys::VelocityMode::MODE_RECT, hWnd);
			else
				SetVelocityMode(CParticleSys::VelocityMode::MODE_POLAR, hWnd);
		} else if (wParam == (WPARAM)'X' && psys->GetVelocityMode() == CParticleSys::VelocityMode::MODE_RECT) {
			rectVelYSelected = !rectVelYSelected;
		} else if (wParam == VK_OEM_MINUS) {
			deltaMult /= 10;
		} else if (wParam == VK_OEM_PLUS) {
			deltaMult *= 10;
		}
		break;
	case WM_LBUTTONDOWN:
		if (mouseControlsParams) {
			selParam--;
			if (selParam < 0) selParam = MAX_PARAM;
			SelectParam(selParam, &deltaMult);
		} else {
			for (std::vector<CDisplayItem*>::iterator i = displayItems.begin(); i != displayItems.end(); i++) {
				(*i)->MouseDown(LOWORD(lParam), HIWORD(lParam));
			}
		}
		break;
	case WM_RBUTTONDOWN:
		if (mouseControlsParams) {
			selParam++;
			if (selParam > MAX_PARAM) selParam = 0;
			SelectParam(selParam, &deltaMult);
		}
		break;
	case WM_MBUTTONDOWN:
		psys->GetParticles()->clear();
		break;
	case WM_MOUSEMOVE:
		mouseControlsParams = true;
		for (std::vector<CDisplayItem*>::iterator i = displayItems.begin(); i != displayItems.end(); i++) {
			if ((*i)->OccupyingPoint(LOWORD(lParam), HIWORD(lParam))) {
				mouseControlsParams = false;
				break;
			}
		}
		if (mouseControlsParams) {
			SetCursor(curEmitter);
			emitterX = (double)LOWORD(lParam);
			emitterY = (double)HIWORD(lParam);
		} else {
			SetCursor(LoadCursor(NULL, IDC_ARROW));
		}
		break;
	case WM_MOUSEWHEEL:
		delta = (double)GET_WHEEL_DELTA_WPARAM(wParam) / WHEEL_DELTA;
		switch (selParam) {
		case 0: //min velocity
			if (psys->GetVelocityMode() == CParticleSys::VelocityMode::MODE_POLAR)
				psys->GetVelocity(&temp1, &temp2);
			else if (!rectVelYSelected)
				psys->GetRectVelocityX(&temp1, &temp2);
			else
				psys->GetRectVelocityY(&temp1, &temp2);
			temp1 += deltaMult * delta;
			if (temp1 < 0 && psys->GetVelocityMode() == CParticleSys::VelocityMode::MODE_POLAR) temp1 = 0;
			if (psys->GetVelocityMode() == CParticleSys::VelocityMode::MODE_POLAR)
				psys->SetVelocity(temp1, temp1 > temp2 ? temp1 : temp2);
			else if (!rectVelYSelected)
				psys->SetRectVelocityX(temp1, temp1 > temp2 ? temp1 : temp2);
			else
				psys->SetRectVelocityY(temp1, temp1 > temp2 ? temp1 : temp2);
			break;
		case 1: //max velocity
			if (psys->GetVelocityMode() == CParticleSys::VelocityMode::MODE_POLAR)
				psys->GetVelocity(&temp1, &temp2);
			else if (!rectVelYSelected)
				psys->GetRectVelocityX(&temp1, &temp2);
			else
				psys->GetRectVelocityY(&temp1, &temp2);
			temp2 += deltaMult * delta;
			if (temp2 < 0 && psys->GetVelocityMode() == CParticleSys::VelocityMode::MODE_POLAR) temp2 = 0;
			if (psys->GetVelocityMode() == CParticleSys::VelocityMode::MODE_POLAR)
				psys->SetVelocity(temp1, temp1 > temp2 ? temp1 : temp2);
			else if (!rectVelYSelected)
				psys->SetRectVelocityX(temp1, temp1 > temp2 ? temp1 : temp2);
			else
				psys->SetRectVelocityY(temp1, temp1 > temp2 ? temp1 : temp2);
			break;
		case 2: //X acceleration
			psys->GetAcceleration(&temp1, &temp2);
			temp1 += deltaMult * delta;
			psys->SetAcceleration(temp1, temp2);
			break;
		case 3: //Y acceleration
			psys->GetAcceleration(&temp1, &temp2);
			temp2 += deltaMult * delta;
			psys->SetAcceleration(temp1, temp2);
			break;
		case 4: //maximum age
			temp1 = psys->GetMaxAge();
			temp1 += deltaMult * delta;
			if (temp1 > 0) psys->SetMaxAge(temp1);
			break;
		case 5: //emission rate
			temp1 = psys->GetEmissionRate();
			temp1 += deltaMult * delta;
			if (temp1 < 0) temp1 = 0;
			if (temp1 > 100000) temp1 = 100000.0;
			psys->SetEmissionRate(temp1);
			break;
		case 6: //emission radius
			temp1 = psys->GetEmissionRadius();
			temp1 += deltaMult * delta;
			psys->SetEmissionRadius(temp1);
			break;
		}
		break;
	case WM_TIMER:
		psys->SimMovingEmitter(1.0 / fps, emitterX, emitterY);
		InvalidateRect(hWnd, NULL, FALSE);
		break;
	case WM_DESTROY:
		delete psys, bbuf;
		DeleteDC(particleBmpDC);
		DeleteObject(particleBitmap);
		DeleteObject(font);
		for (int i = 0; i < NUM_GRADIENTS; i++) {
			delete gradients[i];
		}
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

void SelectParam(int paramNum, double *deltaMult)
{
	selParam = paramNum;
	switch (paramNum) {
		case 0: case 1: *deltaMult = 10.0; break;
		case 2: *deltaMult = 10.0; break;
		case 3: *deltaMult = -10.0; break;
		case 4: *deltaMult = 0.1; break;
		case 5: *deltaMult = 10.0; break;
		case 6: *deltaMult = 1.0; break;
	}
}

void SetVelocityMode(CParticleSys::VelocityMode mode, HWND mainWnd)
{
	HMENU hMenu = GetMenu(mainWnd);
	psys->SetVelocityMode(mode);

	CheckMenuItem(hMenu, ID_PARAMS_VM_POLAR, MF_CHECK_BOOL(mode == CParticleSys::VelocityMode::MODE_POLAR));
	CheckMenuItem(hMenu, ID_PARAMS_VM_RECT, MF_CHECK_BOOL(mode == CParticleSys::VelocityMode::MODE_RECT));

	EnableMenuItem(hMenu, ID_PARAMS_MINVEL, mode != CParticleSys::VelocityMode::MODE_POLAR);
	EnableMenuItem(hMenu, ID_PARAMS_MAXVEL, mode != CParticleSys::VelocityMode::MODE_POLAR);
	EnableMenuItem(hMenu, ID_PARAMS_MINVELX, mode != CParticleSys::VelocityMode::MODE_RECT);
	EnableMenuItem(hMenu, ID_PARAMS_MAXVELX, mode != CParticleSys::VelocityMode::MODE_RECT);
	EnableMenuItem(hMenu, ID_PARAMS_MINVELY, mode != CParticleSys::VelocityMode::MODE_RECT);
	EnableMenuItem(hMenu, ID_PARAMS_MAXVELY, mode != CParticleSys::VelocityMode::MODE_RECT);
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}
