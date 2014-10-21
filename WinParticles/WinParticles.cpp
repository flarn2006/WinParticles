// WinParticles.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "WinParticles.h"
#include "ParticleSys.h"
#include "BackBuffer.h"
#include "Gradient.h"
#include "Common.h"
#include "DisplayItem.h"
#include "RootDisplay.h"
#include "NumericInputBox.h"
#include "ParamAgent.h"
#include "BitmapEditor.h"
#include "GradientEditor.h"
#include "ChaoticGradient.h"
#include "PresetManager.h"
#include <string>
#include <vector>
#include <sstream>
#include <commdlg.h>
#include <shellapi.h>

#define MAX_LOADSTRING 100
#define NUM_GRADIENTS 6
#define MAX_PARAM 6

#define SELPARAM_CHAR(x) (selParam == (x) ? '>' : ' ')
#define MF_CHECK_BOOL(b) ((b) ? MF_CHECKED : MF_UNCHECKED)

// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name
int selParam;
CParticleSys *psys;
HCURSOR curEmitter;
bool additiveDrawing = true;
CHOOSECOLOR colorDlg;
CParticleBitmap bitmap;

// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

void SelectParam(CParamAgent *agent, int paramNum, double *deltaMult);
void SetVelocityMode(CParticleSys::VelocityMode mode, HWND mainWnd);
void RandomizeGradient(CGradient *gradient);
void InitializeGradients(CGradient *gradients[], bool deleteFirst);

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
	static int selGradientNum;
	std::wostringstream out;
	int fps = 60;
	static double emitterX, emitterY;
	double temp1, temp2;
	double delta;
	static double deltaMult;
	static COLORREF customColors[16];
	static bool mouseControlsParams = true;
	static CRootDisplay *display;
	static CParamAgent *agent;
	static RECT clientRect;
	static bool cursorHidden = false;
	static bool randomizeGradientOnSelect = false;
	static bool mouseMovesEmitter = true;
	static COLORREF backgroundColor = 0;
	static HBRUSH backgroundBrush;
	static int verbosity = 2;
	static CPresetManager *presetMgr;

	switch (message)
	{
	case WM_CREATE:
		srand(GetTickCount());
		psys = new CParticleSys();
		psys->SetAcceleration(0.0, -600.0);

		agent = new CParamAgent(psys);
		presetMgr = new CPresetManager(psys);

		SelectParam(agent, 0, &deltaMult);

		InitializeGradients(gradients, false);

		psys->SetDefGradient(gradients[0]);
		selGradientNum = 0;

		font = CreateFont(0, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, NONANTIALIASED_QUALITY, DEFAULT_PITCH, L"Fixedsys");
		SetTimer(hWnd, 0, 1000 / fps, NULL);
		bbuf = new CBackBuffer(hWnd);

		colorDlg.lStructSize = sizeof(colorDlg);
		colorDlg.hwndOwner = hWnd;
		colorDlg.lpCustColors = customColors;
		colorDlg.Flags = CC_RGBINIT;

		for (int i = 0; i < 16; i++) customColors[i] = RGB(255, 255, 255);

		// This will be deleted when the background color is changed, so GetStockObject(BLACK_BRUSH) won't work here.
		backgroundBrush = CreateSolidBrush(0);

		display = new CRootDisplay();
		display->InitBitmapEditor(&bitmap);
		display->InitGradientEditor(psys->GetDefGradient());

		bitmap.LoadDefaultBitmap();
		
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
		case ID_FILE_OPENPRESET:
			if (presetMgr->LoadPresetDlg(hWnd)) {
				if (presetMgr->DidLastPresetIncludeGradient()) {
					psys->SetDefGradient(presetMgr->GetGradient());
					selGradientNum = -1;
					display->GetGradientEditor()->SetGradient(presetMgr->GetGradient());
				}
			}
			break;
		case ID_FILE_SAVEPRESET:
			presetMgr->SavePresetDlg(hWnd);
			break;
		case ID_FILE_SAVEPARAMS:
			presetMgr->SavePresetDlg(hWnd, CPresetManager::PMC_BASIC_PARAMS);
			break;
		case ID_FILE_SAVEGRADIENT:
			presetMgr->SavePresetDlg(hWnd, CPresetManager::PMC_GRADIENT);
			break;
		case ID_FILE_SAVEBITMAP:
			presetMgr->SavePresetDlg(hWnd, CPresetManager::PMC_BITMAP);
			break;
		case ID_PARAMS_VM_POLAR: SetVelocityMode(CParticleSys::VelocityMode::MODE_POLAR, hWnd); break;
		case ID_PARAMS_VM_RECT: SetVelocityMode(CParticleSys::VelocityMode::MODE_RECT, hWnd); break;
		case ID_PARAMS_MINVEL: SelectParam(agent, 0, &deltaMult); break;
		case ID_PARAMS_MAXVEL: SelectParam(agent, 1, &deltaMult); break;
		case ID_PARAMS_MINVELX: SelectParam(agent, 0, &deltaMult); agent->SetRectVelYBit(false); break;
		case ID_PARAMS_MAXVELX: SelectParam(agent, 1, &deltaMult); agent->SetRectVelYBit(false); break;
		case ID_PARAMS_MINVELY: SelectParam(agent, 0, &deltaMult); agent->SetRectVelYBit(true); break;
		case ID_PARAMS_MAXVELY: SelectParam(agent, 1, &deltaMult); agent->SetRectVelYBit(true); break;
		case ID_PARAMS_XACCEL: SelectParam(agent, 2, &deltaMult); break;
		case ID_PARAMS_YACCEL: SelectParam(agent, 3, &deltaMult); break;
		case ID_PARAMS_MAXAGE: SelectParam(agent, 4, &deltaMult); break;
		case ID_PARAMS_EMISSIONRATE: SelectParam(agent, 5, &deltaMult); break;
		case ID_PARAMS_EMISSIONRADIUS: SelectParam(agent, 6, &deltaMult); break;
		case ID_PARAMS_TINT:
			colorDlg.rgbResult = psys->GetDefaultTint();
			if (ChooseColor(&colorDlg)) {
				psys->SetDefaultTint(colorDlg.rgbResult);
				display->GetGradientEditor()->SetTint(colorDlg.rgbResult);
			}
			break;
		case ID_PARAMS_BACKGROUNDCOLOR:
			colorDlg.rgbResult = backgroundColor;
			if (ChooseColor(&colorDlg)) {
				backgroundColor = colorDlg.rgbResult;
				DeleteObject(backgroundBrush);
				backgroundBrush = CreateSolidBrush(backgroundColor);
			}
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_SIZE:
		bbuf->UpdateSize();
		GetClientRect(hWnd, &clientRect);
		display->UpdateSize(&clientRect);
		break;
	case WM_PAINT:
		hDC = bbuf->GetDC();

		// Draw background and particles
		GetClientRect(hWnd, &clientRect);
		FillRect(hDC, &clientRect, backgroundBrush);
		psys->Draw(hDC, &clientRect);

		// Draw all active display items
		display->Draw(hDC, &clientRect);

		// Draw text display
		SelectObject(hDC, font);
		SetBkMode(hDC, TRANSPARENT);
		
		if (verbosity >= 2) {
#ifdef _DEBUG
			out << "DEBUG BUILD (performance is not optimal)" << std::endl;
#endif
			out << "Number of particles: " << psys->GetParticles()->size() << std::endl;
			out << "Use mouse buttons/wheel or arrow keys to edit params" << std::endl;
			out << "Press ENTER to type a value directly" << std::endl;
		}
		if (verbosity >= 1) {
			out << "[-+] Adjustment multiplier: " << deltaMult << std::endl;
			out << "[Z]  Velocity mode: " << CParticleSys::VelocityModeText(psys->GetVelocityMode());
			if (psys->GetVelocityMode() == CParticleSys::VelocityMode::MODE_RECT)
				out << " ([X] toggles X/Y)" << std::endl;
			else
				out << std::endl;
			out << "Current parameters:" << std::endl;
			if (psys->GetVelocityMode() == CParticleSys::VelocityMode::MODE_POLAR) {
				psys->GetVelocity(&temp1, &temp2);
				out << SELPARAM_CHAR(0) << " Minimum velocity:   " << temp1 << std::endl;
				out << SELPARAM_CHAR(1) << " Maximum velocity:   " << temp2 << std::endl;
			} else {
				if (!agent->GetRectVelYBit()) {
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
		}
		if (verbosity >= 2) {
			out << std::endl;
			out << "Press 1 - " << NUM_GRADIENTS << " to change colors" << std::endl;
			out << "[R] Reset parameters" << std::endl;
			out << "[C] Show/hide cursor" << std::endl;
			out << "[F] Freeze/unfreeze emitter" << std::endl;
			out << "[G] Reset gradient presets" << std::endl;
			out << "[A] Toggle additive drawing " << "(" << (additiveDrawing ? "ON" : "OFF") << ")" << std::endl;
			out << "[Q] Change text display" << std::endl;
			out << "[E] Toggle bitmap/gradient editors" << std::endl;
		}

		clientRect.left += 5; clientRect.top += 5;
		SetTextColor(hDC, 0x000000);
		DrawText(hDC, out.str().c_str(), (int)out.str().length(), &clientRect, 0);
		clientRect.left -= 1; clientRect.top -= 1;
		SetTextColor(hDC, 0xFFFF00);
		DrawText(hDC, out.str().c_str(), (int)out.str().length(), &clientRect, 0);
		clientRect.left -= 4; clientRect.top -= 4;
		
		hDC = BeginPaint(hWnd, &ps);
		bbuf->CopyToFront(hDC);
		EndPaint(hWnd, &ps);
		break;
	case WM_KEYDOWN:
		if (!display->KeyDown((UINT)wParam)) {
			if (0x31 <= wParam && wParam <= 0x30 + NUM_GRADIENTS) {
				if (display->GetGradientEditor()->IsOKToSwitchGradients()) {
					if (wParam == 0x35) {
						if (randomizeGradientOnSelect) RandomizeGradient(gradients[4]);
						randomizeGradientOnSelect = true;
					} else {
						randomizeGradientOnSelect = false;
					}
					selGradientNum = wParam - 0x31;
					psys->SetDefGradient(gradients[selGradientNum]);
					display->GetGradientEditor()->SetGradient(gradients[selGradientNum]);
				}
			} else if (wParam == (WPARAM)'0') {
				if (display->GetGradientEditor()->IsOKToSwitchGradients()) {
					selGradientNum = -1;
					psys->SetDefGradient(presetMgr->GetGradient());
					display->GetGradientEditor()->SetGradient(presetMgr->GetGradient());
				}
			} else if (wParam == (WPARAM)'R') {
				psys->DefaultParameters();
				SetVelocityMode(psys->GetVelocityMode(), hWnd);
			} else if (wParam == (WPARAM)'Z') {
				if (psys->GetVelocityMode() == CParticleSys::VelocityMode::MODE_POLAR)
					SetVelocityMode(CParticleSys::VelocityMode::MODE_RECT, hWnd);
				else
					SetVelocityMode(CParticleSys::VelocityMode::MODE_POLAR, hWnd);
			} else if (wParam == (WPARAM)'X' && psys->GetVelocityMode() == CParticleSys::VelocityMode::MODE_RECT) {
				agent->SetRectVelYBit(!agent->GetRectVelYBit());
			} else if (wParam == (WPARAM)'C') {
				cursorHidden = !cursorHidden;
				ShowCursor(!cursorHidden);
			} else if (wParam == (WPARAM)'F') {
				mouseMovesEmitter = !mouseMovesEmitter;
			} else if (wParam == (WPARAM)'G') {
				InitializeGradients(gradients, true);
				CGradient *selected;
				if (selGradientNum == -1) {
					selected = presetMgr->GetGradient();
				} else {
					selected = gradients[selGradientNum];
				}
				psys->SetDefGradient(selected);
				display->GetGradientEditor()->SetGradient(selected);
				psys->GetParticles()->clear();
			} else if (wParam == (WPARAM)'A') {
				additiveDrawing = !additiveDrawing;
			} else if (wParam == (WPARAM)'Q') {
				verbosity = (verbosity + 1) % 3;
			} else if (wParam == (WPARAM)'E') {
				if (!display->GetBitmapEditor()->IsMouseDown() && !display->GetGradientEditor()->IsMouseDown()) {
					bool enable = !display->GetBitmapEditor()->GetEnabled();
					display->GetBitmapEditor()->SetEnabled(enable);
					display->GetGradientEditor()->SetEnabled(enable);
				}
			} else if (wParam == VK_OEM_MINUS) {
				deltaMult /= 10;
			} else if (wParam == VK_OEM_PLUS) {
				deltaMult *= 10;
			} else if (wParam == VK_RETURN) {
				GetClientRect(hWnd, &clientRect);
				display->GetNumInputBox()->PromptForValue(agent);
			} else if (wParam == VK_UP) {
				selParam--;
				if (selParam < 0) selParam = MAX_PARAM;
				SelectParam(agent, selParam, &deltaMult);
			} else if (wParam == VK_DOWN) {
				selParam++;
				if (selParam > MAX_PARAM) selParam = 0;
				SelectParam(agent, selParam, &deltaMult);
			} else if (wParam == VK_LEFT) {
				agent->SetValue(agent->GetValue() - deltaMult);
			} else if (wParam == VK_RIGHT) {
				agent->SetValue(agent->GetValue() + deltaMult);
			}
		}
		break;
	case WM_LBUTTONDOWN:
		if (mouseControlsParams) {
			selParam--;
			if (selParam < 0) selParam = MAX_PARAM;
			SelectParam(agent, selParam, &deltaMult);
		} else {
			display->MouseDown(LOWORD(lParam), HIWORD(lParam));
		}
		break;
	case WM_RBUTTONDOWN:
		if (mouseControlsParams) {
			selParam++;
			if (selParam > MAX_PARAM) selParam = 0;
			SelectParam(agent, selParam, &deltaMult);
		} else {
			display->RightClick(LOWORD(lParam), HIWORD(lParam));
		}
		break;
	case WM_MBUTTONDOWN:
		psys->GetParticles()->clear();
		break;
	case WM_MOUSEMOVE:
		mouseControlsParams = !display->OccupyingPoint(LOWORD(lParam), HIWORD(lParam));

		if (mouseControlsParams) {
			SetCursor(curEmitter);
			SetClassLong(hWnd, GCL_HCURSOR, (DWORD)curEmitter);
			if (mouseMovesEmitter) {
				emitterX = (double)LOWORD(lParam);
				emitterY = (double)HIWORD(lParam);
			}
		} else {
			HCURSOR curArrow = LoadCursor(NULL, IDC_ARROW);
			SetCursor(curArrow);
			SetClassLong(hWnd, GCL_HCURSOR, (DWORD)curArrow);
		}

		display->MouseMove(LOWORD(lParam), HIWORD(lParam));

		break;
	case WM_LBUTTONUP:
		display->MouseUp(LOWORD(lParam), HIWORD(lParam));
		break;
	case WM_MOUSEWHEEL:
		delta = (double)GET_WHEEL_DELTA_WPARAM(wParam) / WHEEL_DELTA;
		delta *= deltaMult;
		agent->SetValue(agent->GetValue() + delta);
		break;
	case WM_TIMER:
		psys->SimMovingEmitter(1.0 / fps, emitterX, emitterY);
		InvalidateRect(hWnd, NULL, FALSE);
		break;
	case WM_DESTROY:
		delete psys;
		delete bbuf;
		delete agent;
		delete presetMgr;
		DeleteObject(font);
		for (int i = 0; i < NUM_GRADIENTS; i++) {
			delete gradients[i];
		}
		delete display;
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

void InitializeGradients(CGradient *gradients[], bool deleteFirst)
{
	if (deleteFirst) {
		for (int i = 0; i < NUM_GRADIENTS; i++) {
			delete gradients[i];
		}
	}

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

	gradients[4] = new CGradient(5);
	RandomizeGradient(gradients[4]);

	gradients[5] = new CChaoticGradient(3);
	gradients[5]->SetStep(0, 0.0, 0x000000);
	gradients[5]->SetStep(1, 0.5, 0xFF8000);
	gradients[5]->SetStep(2, 1.0, 0xFFFFFF);
}

void SelectParam(CParamAgent *agent, int paramNum, double *deltaMult)
{
	selParam = paramNum;
	agent->SetSelParam(selParam);
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

void RandomizeGradient(CGradient *gradient)
{
	int count = gradient->GetStepCount();
	for (int i = 0; i < count; i++) {
		double position = RandInRange(0.0, 1.0);
		COLORREF color = RGB(rand() % 256, rand() % 256, rand() % 256);
		gradient->SetStep(i, position, color);
	}
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
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL) {
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		} else if (LOWORD(wParam) == IDC_GITHUB) {
			ShellExecute(NULL, L"open", L"http://www.github.com/flarn2006/WinParticles", NULL, NULL, SW_SHOWNORMAL);
			EndDialog(hDlg, IDOK);
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}
