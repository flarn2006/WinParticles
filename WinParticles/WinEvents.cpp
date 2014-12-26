#include "stdafx.h"
#include "WinEvents.h"
#include <commdlg.h>
#include "ParticleBitmap.h"
#include "Common.h"
#include "RootDisplay.h"
#include "PresetManager.h"
#include "resource.h"
#include "ChaoticGradient.h"
#include "AnimatedParam.h"

#define MAX_LOADSTRING 100
#define NUM_GRADIENTS 6
#define FPS 60

#define SELPARAM_CHAR(x) (selParam == (x) ? '>' : ' ')
#define MF_CHECK_BOOL(b) ((b) ? MF_CHECKED : MF_UNCHECKED)

extern HINSTANCE hInst;
extern CParamAgent::ParamID selParam;
extern CParticleSys *psys;
extern HCURSOR curEmitter;
extern bool additiveDrawing;
extern CHOOSECOLOR colorDlg;
extern CParticleBitmap bitmap;
extern HFONT font;
extern tstring cmdLine;
extern CRootDisplay *display;
extern CGradient *gradients[NUM_GRADIENTS];
extern CPresetManager *presetMgr;
extern int verbosity;

CWinEvents::CWinEvents(HWND hWnd)
{
	this->hWnd = hWnd;

	DragAcceptFiles(hWnd, TRUE);

	srand(GetTickCount());
	psys = new CParticleSys();
	psys->SetAcceleration(0.0, -600.0);
	
	agent = new CParamAgent(psys);
	presetMgr = new CPresetManager(psys);

	SelectParam(agent, CParamAgent::ParamID::MIN_VELOCITY, deltaMult);

	InitializeGradients(gradients, false);

	psys->SetDefGradient(gradients[0]);
	selGradientNum = 0;

	font = CreateFont(0, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, NONANTIALIASED_QUALITY, DEFAULT_PITCH, TEXT("Fixedsys"));
	SetTimer(hWnd, 0, 1000 / FPS, NULL);
	bbuf = new CBackBuffer(hWnd);

	colorDlg.lStructSize = sizeof(colorDlg);
	colorDlg.hwndOwner = hWnd;
	colorDlg.lpCustColors = customColors;
	colorDlg.Flags = CC_RGBINIT;

	for (int i = 0; i < 16; i++) customColors[i] = RGB(255, 255, 255);

	// This will be deleted when the background color is changed, so GetStockObject(BLACK_BRUSH) won't work here.
	backgroundBrush = CreateSolidBrush(0);

	for (int i = 0; i < CParamAgent::ParamID::PARAM_COUNT; i++) {
		animParams[i].SetParamAgent(agent);
		animParams[i].SetParamID((CParamAgent::ParamID)i);
		animations[i].SetTarget(&animParams[i]);
		psys->GetAnimationsVector().push_back(&animations[i]);
	}

	display = new CRootDisplay();
	display->InitBitmapEditor(&bitmap);
	display->InitGradientEditor(psys->GetDefGradient());
	display->InitAnimEditor(animations);

	bitmap.LoadDefaultBitmap();

	if (!cmdLine.empty()) {
		if (cmdLine.front() == '"' && cmdLine.back() == '"') cmdLine = cmdLine.substr(1, cmdLine.length() - 2);
		presetMgr->LoadPreset(cmdLine.c_str());
		if (presetMgr->DidLastPresetIncludeGradient()) SelectGradient(-1, selGradientNum);
	}
}

CWinEvents::~CWinEvents()
{
	delete psys;
	delete bbuf;
	delete agent;
	delete presetMgr;
	DeleteObject(font);
	for (int i = 0; i < NUM_GRADIENTS; i++) {
		delete gradients[i];
	}
	delete display;
}

void CWinEvents::InitializeGradients(CGradient *gradients[], bool deleteFirst)
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

void CWinEvents::SelectParam(CParamAgent *agent, CParamAgent::ParamID paramNum, double &deltaMult)
{
	selParam = paramNum;
	agent->SetSelParam(selParam);
	switch (paramNum) {
	case CParamAgent::ParamID::MIN_VELOCITY: case CParamAgent::ParamID::MAX_VELOCITY: deltaMult = 10.0; break;
	case CParamAgent::ParamID::BASE_ANGLE: case CParamAgent::ParamID::ANGLE_SIZE: deltaMult = 10.0; break; // (BASE_)ANGLE(_SIZE) = MIN(/MAX)_VELOCITY_Y in rect mode
	case CParamAgent::ParamID::ACCELERATION_X: deltaMult = 10.0; break;
	case CParamAgent::ParamID::ACCELERATION_Y: deltaMult = -10.0; break;
	case CParamAgent::ParamID::MAXIMUM_AGE: deltaMult = 0.1; break;
	case CParamAgent::ParamID::EMISSION_RATE: deltaMult = 10.0; break;
	case CParamAgent::ParamID::EMISSION_RADIUS: deltaMult = 1.0; break;
	case CParamAgent::ParamID::INNER_RADIUS: deltaMult = 1.0; break;
	}
	if (display) display->GetAnimEditor()->SetSelectedID(selParam);
}

void CWinEvents::SelectGradient(int gradientNum, int &selGradientNum)
{
	CGradient *toSelect;
	selGradientNum = gradientNum;

	if (gradientNum == -1) {
		toSelect = presetMgr->GetGradient();
	} else {
		toSelect = gradients[gradientNum];
	}

	psys->SetDefGradient(toSelect);
	display->GetGradientEditor()->SetGradient(toSelect);
}

void CWinEvents::SetVelocityMode(CParticleSys::VelocityMode mode, HWND mainWnd)
{
	HMENU hMenu = GetMenu(mainWnd);
	psys->SetVelocityMode(mode);

	CheckMenuItem(hMenu, ID_PARAMS_VM_POLAR, MF_CHECK_BOOL(mode == CParticleSys::VelocityMode::MODE_POLAR));
	CheckMenuItem(hMenu, ID_PARAMS_VM_RECT, MF_CHECK_BOOL(mode == CParticleSys::VelocityMode::MODE_RECT));

	EnableMenuItem(hMenu, ID_PARAMS_MINVEL, mode != CParticleSys::VelocityMode::MODE_POLAR);
	EnableMenuItem(hMenu, ID_PARAMS_MAXVEL, mode != CParticleSys::VelocityMode::MODE_POLAR);
	EnableMenuItem(hMenu, ID_PARAMS_MINANGLE, mode != CParticleSys::VelocityMode::MODE_POLAR);
	EnableMenuItem(hMenu, ID_PARAMS_MAXANGLE, mode != CParticleSys::VelocityMode::MODE_POLAR);
	EnableMenuItem(hMenu, ID_PARAMS_MINVELX, mode != CParticleSys::VelocityMode::MODE_RECT);
	EnableMenuItem(hMenu, ID_PARAMS_MAXVELX, mode != CParticleSys::VelocityMode::MODE_RECT);
	EnableMenuItem(hMenu, ID_PARAMS_MINVELY, mode != CParticleSys::VelocityMode::MODE_RECT);
	EnableMenuItem(hMenu, ID_PARAMS_MAXVELY, mode != CParticleSys::VelocityMode::MODE_RECT);
}

void CWinEvents::RandomizeGradient(CGradient *gradient)
{
	int count = gradient->GetStepCount();
	for (int i = 0; i < count; i++) {
		double position = RandInRange(0.0, 1.0);
		COLORREF color = RGB(rand() % 256, rand() % 256, rand() % 256);
		gradient->SetStep(i, position, color);
	}
}


bool CWinEvents::OnCommand(WORD command, WORD eventID)
{
	switch (command) {
	case IDM_ABOUT:
		DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, AboutDlgProc);
		break;
	case IDM_EXIT:
		DestroyWindow(hWnd);
		break;
	case ID_FILE_OPENPRESET:
		if (presetMgr->LoadPresetDlg(hWnd)) {
			if (presetMgr->DidLastPresetIncludeGradient()) {
				SelectGradient(-1, selGradientNum);
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
	case ID_PARAMS_MINVEL: SelectParam(agent, CParamAgent::ParamID::MIN_VELOCITY, deltaMult); break;
	case ID_PARAMS_MAXVEL: SelectParam(agent, CParamAgent::ParamID::MAX_VELOCITY, deltaMult); break;
	case ID_PARAMS_MINANGLE: SelectParam(agent, CParamAgent::ParamID::BASE_ANGLE, deltaMult); break;
	case ID_PARAMS_MAXANGLE: SelectParam(agent, CParamAgent::ParamID::ANGLE_SIZE, deltaMult); break;
	case ID_PARAMS_MINVELX: SelectParam(agent, CParamAgent::ParamID::MIN_VELOCITY_X, deltaMult); break;
	case ID_PARAMS_MAXVELX: SelectParam(agent, CParamAgent::ParamID::MAX_VELOCITY_X, deltaMult); break;
	case ID_PARAMS_MINVELY: SelectParam(agent, CParamAgent::ParamID::MIN_VELOCITY_Y, deltaMult); break;
	case ID_PARAMS_MAXVELY: SelectParam(agent, CParamAgent::ParamID::MAX_VELOCITY_Y, deltaMult); break;
	case ID_PARAMS_XACCEL: SelectParam(agent, CParamAgent::ParamID::ACCELERATION_X, deltaMult); break;
	case ID_PARAMS_YACCEL: SelectParam(agent, CParamAgent::ParamID::ACCELERATION_Y, deltaMult); break;
	case ID_PARAMS_MAXAGE: SelectParam(agent, CParamAgent::ParamID::MAXIMUM_AGE, deltaMult); break;
	case ID_PARAMS_EMISSIONRATE: SelectParam(agent, CParamAgent::ParamID::EMISSION_RATE, deltaMult); break;
	case ID_PARAMS_EMISSIONRADIUS: SelectParam(agent, CParamAgent::ParamID::EMISSION_RADIUS, deltaMult); break;
	case ID_PARAMS_INNERRADIUS: SelectParam(agent, CParamAgent::ParamID::INNER_RADIUS, deltaMult); break;
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
		return false;
	}
	return true;
}

void CWinEvents::OnSize()
{
	bbuf->UpdateSize();
	GetClientRect(hWnd, &clientRect);
	display->UpdateSize(&clientRect);
}

void CWinEvents::OnPaint()
{
	tostringstream out;
	double temp1, temp2;
	PAINTSTRUCT ps;

	HDC hDC = bbuf->GetDC();
	SelectObject(hDC, font);
	SetBkMode(hDC, TRANSPARENT);

	// Draw background and particles
	GetClientRect(hWnd, &clientRect);
	FillRect(hDC, &clientRect, backgroundBrush);
	psys->Draw(hDC, &clientRect);

	// Draw all active display items
	display->Draw(hDC, &clientRect);

	// Draw text display
	if (verbosity >= 2) {
#ifdef _DEBUG
		out << "DEBUG BUILD (performance is not optimal)" << std::endl;
#endif
		out << "Number of particles: " << psys->GetLiveParticleCount();
#ifdef _DEBUG
		out << " living" << std::endl;
		out << "                     " << psys->GetParticles()->size() << " total";
#endif
		out << std::endl;
		out << "Use mouse buttons/wheel or arrow keys to edit params" << std::endl;
		out << "Press ENTER to type a value directly" << std::endl;
	}
	if (verbosity >= 1) {
		out << "[-+] Adjustment multiplier: " << deltaMult << std::endl;
		out << "[Z]  Velocity mode: " << CParticleSys::VelocityModeText(psys->GetVelocityMode()) << std::endl;
		out << "Current parameters:" << std::endl;
		if (psys->GetVelocityMode() == CParticleSys::VelocityMode::MODE_POLAR) {
			psys->GetVelocity(&temp1, &temp2);
			out << SELPARAM_CHAR(CParamAgent::ParamID::MIN_VELOCITY) << " Minimum velocity:   " << temp1 << std::endl;
			out << SELPARAM_CHAR(CParamAgent::ParamID::MAX_VELOCITY) << " Maximum velocity:   " << temp2 << std::endl;
			psys->GetAngle(&temp1, &temp2);
			out << SELPARAM_CHAR(CParamAgent::ParamID::BASE_ANGLE) << " Starting angle:     " << temp1 << '°' << std::endl;
			out << SELPARAM_CHAR(CParamAgent::ParamID::ANGLE_SIZE) << " Angular size:       " << temp2 << '°' << std::endl;
		} else {
			psys->GetRectVelocityX(&temp1, &temp2);
			out << SELPARAM_CHAR(CParamAgent::ParamID::MIN_VELOCITY_X) << " Minimum X velocity: " << temp1 << std::endl;
			out << SELPARAM_CHAR(CParamAgent::ParamID::MAX_VELOCITY_X) << " Maximum X velocity: " << temp2 << std::endl;
			psys->GetRectVelocityY(&temp1, &temp2);
			out << SELPARAM_CHAR(CParamAgent::ParamID::MIN_VELOCITY_Y) << " Minimum Y velocity: " << temp1 << std::endl;
			out << SELPARAM_CHAR(CParamAgent::ParamID::MAX_VELOCITY_Y) << " Maximum Y velocity: " << temp2 << std::endl;
		}
		psys->GetAcceleration(&temp1, &temp2);
		out << SELPARAM_CHAR(CParamAgent::ParamID::ACCELERATION_X) << " X acceleration:     " << temp1 << std::endl;
		out << SELPARAM_CHAR(CParamAgent::ParamID::ACCELERATION_Y) << " Y acceleration:     " << temp2 << std::endl;
		out << SELPARAM_CHAR(CParamAgent::ParamID::MAXIMUM_AGE) << " Maximum age:        " << psys->GetMaxAge() << std::endl;
		out << SELPARAM_CHAR(CParamAgent::ParamID::EMISSION_RATE) << " Emission rate:      " << psys->GetEmissionRate() << std::endl;
		out << SELPARAM_CHAR(CParamAgent::ParamID::EMISSION_RADIUS) << " Emission radius:    " << psys->GetEmissionRadius() << std::endl;
		out << SELPARAM_CHAR(CParamAgent::ParamID::INNER_RADIUS) << " Inner radius:       " << psys->GetInnerRadius() << std::endl;
	}
	if (verbosity >= 2) {
		out << std::endl;
		out << "Press 0 - " << NUM_GRADIENTS << " to change colors" << std::endl;
		out << "[R] Reset parameters" << std::endl;
		out << "[C] Show/hide cursor" << std::endl;
		out << "[F] Freeze/unfreeze emitter" << std::endl;
		out << "[G] Reset gradient presets" << std::endl;
		out << "[A] Toggle additive drawing " << "(" << (additiveDrawing ? "ON" : "OFF") << ")" << std::endl;
		out << "[Q] Change text display" << std::endl;
		out << "[E] Toggle bitmap/gradient/animation editors" << std::endl;
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
}

void CWinEvents::OnKeyDown(WORD key)
{
	if (!display->KeyDown((UINT)key)) {
		if (0x31 <= key && key <= 0x30 + NUM_GRADIENTS) {
			if (display->GetGradientEditor()->IsOKToSwitchGradients()) {
				if (key == 0x35) {
					if (randomizeGradientOnSelect) RandomizeGradient(gradients[4]);
					randomizeGradientOnSelect = true;
				} else {
					randomizeGradientOnSelect = false;
				}
				SelectGradient(key - 0x31, selGradientNum);
			}
		} else if (key == (WPARAM)'0') {
			if (display->GetGradientEditor()->IsOKToSwitchGradients()) {
				SelectGradient(-1, selGradientNum);
			}
		} else if (key == (WPARAM)'R') {
			psys->DisableAllAnimations();
			display->GetAnimEditor()->Update();
			psys->DefaultParameters();
			SetVelocityMode(psys->GetVelocityMode(), hWnd);
		} else if (key == (WPARAM)'Z') {
			if (psys->GetVelocityMode() == CParticleSys::VelocityMode::MODE_POLAR)
				SetVelocityMode(CParticleSys::VelocityMode::MODE_RECT, hWnd);
			else
				SetVelocityMode(CParticleSys::VelocityMode::MODE_POLAR, hWnd);
		} else if (key == (WPARAM)'C') {
			cursorHidden = !cursorHidden;
			ShowCursor(!cursorHidden);
		} else if (key == (WPARAM)'F') {
			mouseMovesEmitter = !mouseMovesEmitter;
		} else if (key == (WPARAM)'G') {
			InitializeGradients(gradients, true);
			SelectGradient(selGradientNum, selGradientNum);
			psys->GetParticles()->clear();
		} else if (key == (WPARAM)'A') {
			additiveDrawing = !additiveDrawing;
		} else if (key == (WPARAM)'Q') {
			verbosity = (verbosity + 1) % 3;
		} else if (key == (WPARAM)'E') {
			if (!display->GetBitmapEditor()->IsMouseDown() && !display->GetGradientEditor()->IsMouseDown()) {
				bool enable = !display->GetBitmapEditor()->GetEnabled();
				display->GetBitmapEditor()->SetEnabled(enable);
				display->GetGradientEditor()->SetEnabled(enable);
				display->GetAnimEditor()->SetEnabled(enable);
			}
		} else if (key == VK_OEM_MINUS) {
			deltaMult /= 10;
		} else if (key == VK_OEM_PLUS) {
			deltaMult *= 10;
		} else if (key == VK_RETURN) {
			GetClientRect(hWnd, &clientRect);
			display->GetNumInputBox()->PromptForValue(agent);
		} else if (key == VK_UP) {
			selParam--;
			SelectParam(agent, selParam, deltaMult);
		} else if (key == VK_DOWN) {
			selParam++;
			SelectParam(agent, selParam, deltaMult);
		} else if (key == VK_LEFT) {
			agent->SetValue(agent->GetValue() - deltaMult);
		} else if (key == VK_RIGHT) {
			agent->SetValue(agent->GetValue() + deltaMult);
		}
	}
}

void CWinEvents::OnLButtonDown(int x, int y)
{
	if (mouseControlsParams) {
		selParam--;
		SelectParam(agent, selParam, deltaMult);
	} else {
		display->MouseDown(x, y);
	}
}

void CWinEvents::OnRButtonDown(int x, int y)
{
	if (mouseControlsParams) {
		selParam++;
		SelectParam(agent, selParam, deltaMult);
	} else {
		display->RightClick(x, y);
	}
}

void CWinEvents::OnMButtonDown(int x, int y)
{
	psys->GetParticles()->clear();
}

void CWinEvents::OnMouseMove(int x, int y)
{
	mouseControlsParams = !display->OccupyingPoint(x, y);

	if (mouseControlsParams) {
		SetCursor(curEmitter);
		SetClassLong(hWnd, GCL_HCURSOR, (DWORD)curEmitter);
		if (mouseMovesEmitter) {
			emitterX = (double)x;
			emitterY = (double)y;
		}
	} else {
		HCURSOR curArrow = LoadCursor(NULL, IDC_ARROW);
		SetCursor(curArrow);
		SetClassLong(hWnd, GCL_HCURSOR, (DWORD)curArrow);
	}

	display->MouseMove(x, y);
}

void CWinEvents::OnLButtonUp(int x, int y)
{
	display->MouseUp(x, y);
}

void CWinEvents::OnMouseWheel(short wheelDelta)
{
	if (!display->MouseWheel(wheelDelta)) {
		double delta = (double)wheelDelta / WHEEL_DELTA;
		delta *= deltaMult;
		agent->SetValue(agent->GetValue() + delta);
	}
}

void CWinEvents::OnTimer()
{
	psys->SimMovingEmitter(1.0 / FPS, emitterX, emitterY);
	InvalidateRect(hWnd, NULL, FALSE);
}

void CWinEvents::OnDropFiles(HDROP hDrop)
{
	if (DragQueryFile(hDrop, 0xFFFFFFFF, NULL, 0) == 1) {
		int numChars = DragQueryFile(hDrop, 0, NULL, 0) + 1;
		LPTSTR name = (LPTSTR)malloc(sizeof(TCHAR)* numChars);
		DragQueryFile(hDrop, 0, name, numChars);
		if (presetMgr->LoadPreset(name)) {
			if (presetMgr->DidLastPresetIncludeGradient()) {
				SelectGradient(-1, selGradientNum);
			}
		}
	} else {
		MessageBox(hWnd, TEXT("Sorry, you can only drop a single file into this application at a time."), TEXT("Error"), MB_ICONERROR);
	}
	DragFinish(hDrop);
}

CParamAgent *CWinEvents::GetParamAgent()
{
	return agent;
}

double CWinEvents::GetDeltaMult()
{
	return deltaMult;
}

INT_PTR CALLBACK CWinEvents::AboutDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message) {
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL) {
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		} else if (LOWORD(wParam) == IDC_GITHUB) {
			ShellExecute(NULL, TEXT("open"), TEXT("http://www.github.com/flarn2006/WinParticles"), NULL, NULL, SW_SHOWNORMAL);
			EndDialog(hDlg, IDOK);
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}
