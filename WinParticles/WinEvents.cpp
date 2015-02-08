#include "stdafx.h"
#include "WinEvents.h"
#include <commdlg.h>
#include "ParticleBitmap.h"
#include "RootDisplay.h"
#include "PresetManager.h"
#include "resource.h"
#include "AnimatedParam.h"
#include "TextItem.h"
#include "DynamicTextItem.h"
#include "ParamTextItem.h"

#define MAX_LOADSTRING 100
#define NUM_GRADIENTS 5
#define MIN_FPS 32
#define TARGET_FPS 60

#define SELPARAM_CHAR(x) (selParam == (x) ? '>' : ' ')
#define MF_CHECK_BOOL(b) ((b) ? MF_CHECKED : MF_UNCHECKED)

extern HINSTANCE hInst;
extern CParticleSys *psys;
extern HCURSOR curEmitter;
extern bool additiveDrawing;
extern CHOOSECOLOR colorDlg;
extern CParticleBitmap bitmap;
extern HFONT font;
extern tstring cmdLine;
extern CRootDisplay *display;
extern CGradient gradients[NUM_GRADIENTS];
extern CPresetManager *presetMgr;
extern int verbosity;
extern double deltaMult;

CWinEvents::CWinEvents(HWND hWnd)
{
	this->hWnd = hWnd;
	emitterX = emitterY = 0;
	DragAcceptFiles(hWnd, TRUE);

	//SetWindowLongPtr(hWnd, GWL_EXSTYLE, GetWindowLongPtr(hWnd, GWL_EXSTYLE) | WS_EX_CLIENTEDGE);

	srand(GetTickCount());
	psys = new CParticleSys();
	psys->SetAcceleration(0.0, -600.0);
	
	agent = new CParamAgent(psys);
	presetMgr = new CPresetManager(psys, animations);

	InitializeGradients(gradients);

	psys->SetGradient(gradients[0]);
	selGradientNum = 0;

	font = CreateFont(0, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, NONANTIALIASED_QUALITY, DEFAULT_PITCH, TEXT("Fixedsys"));
	SetTimer(hWnd, 0, 1000 / (TARGET_FPS * 3 / 2), NULL); 
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
	display->InitGradientEditor(&psys->GetGradient());
	display->InitAnimEditor(animations);
	SetupTextDisplay(*display->GetTextDisplay());

	bitmap.LoadDefaultBitmap();

	if (!cmdLine.empty()) {
		if (cmdLine.front() == '"' && cmdLine.back() == '"') cmdLine = cmdLine.substr(1, cmdLine.length() - 2);
		presetMgr->LoadPreset(cmdLine.c_str());
		if (presetMgr->DidLastPresetIncludeGradient()) SelectGradient(-1, selGradientNum);
	}

	SetVelocityMode(CParticleSys::VelocityMode::MODE_POLAR, hWnd);
	UpdateViewMenuChecks();
}

CWinEvents::~CWinEvents()
{
	delete psys;
	delete bbuf;
	delete agent;
	delete presetMgr;
	DeleteObject(font);
	delete display;
}

void CWinEvents::InitializeGradients(CGradient *gradients)
{
	gradients[0].AddStep(0.0, RGB(255, 255, 0));
	gradients[0].AddStep(0.25, RGB(255, 0, 0));
	gradients[0].AddStep(0.5, RGB(0, 0, 0));
	gradients[0].AddStep(0.51, RGB(64, 64, 64));
	gradients[0].AddStep(1.0, RGB(0, 0, 0));

	gradients[1].AddStep(0.0, RGB(255, 0, 0));
	gradients[1].AddStep(1.0 / 6, RGB(255, 255, 0));
	gradients[1].AddStep(2.0 / 6, RGB(0, 255, 0));
	gradients[1].AddStep(3.0 / 6, RGB(0, 255, 255));
	gradients[1].AddStep(4.0 / 6, RGB(0, 0, 255));
	gradients[1].AddStep(5.0 / 6, RGB(255, 0, 255));
	gradients[1].AddStep(1.0, RGB(255, 0, 0));

	gradients[2].AddStep(0.0, RGB(255, 255, 255));
	gradients[2].AddStep(1.0, RGB(0, 0, 0));

	gradients[3].AddStep(0.0, RGB(255, 255, 255));
	gradients[3].AddStep(1.0, RGB(255, 255, 255));

	gradients[4] = CGradient(5);
	RandomizeGradient(gradients[4]);
}

void CWinEvents::SelectGradient(int gradientNum, int &selGradientNum)
{
	CGradient *toSelect;
	selGradientNum = gradientNum;

	if (gradientNum == -1) {
		toSelect = presetMgr->GetGradient();
	} else {
		toSelect = &gradients[gradientNum];
	}

	psys->SetGradient(*toSelect);
	display->GetGradientEditor()->SetGradient(toSelect);
}

void CWinEvents::SetVelocityMode(CParticleSys::VelocityMode mode, HWND mainWnd)
{
	HMENU hMenu = GetMenu(mainWnd);
	psys->SetVelocityMode(mode);

	CheckMenuRadioItem(hMenu, ID_PARAMS_VM_POLAR, ID_PARAMS_VM_RECT, (mode == CParticleSys::VelocityMode::MODE_POLAR) ? ID_PARAMS_VM_POLAR : ID_PARAMS_VM_RECT, MF_BYCOMMAND);

	EnableMenuItem(hMenu, ID_PARAMS_MINVEL, mode != CParticleSys::VelocityMode::MODE_POLAR);
	EnableMenuItem(hMenu, ID_PARAMS_MAXVEL, mode != CParticleSys::VelocityMode::MODE_POLAR);
	EnableMenuItem(hMenu, ID_PARAMS_MINANGLE, mode != CParticleSys::VelocityMode::MODE_POLAR);
	EnableMenuItem(hMenu, ID_PARAMS_MAXANGLE, mode != CParticleSys::VelocityMode::MODE_POLAR);
	EnableMenuItem(hMenu, ID_PARAMS_MINVELX, mode != CParticleSys::VelocityMode::MODE_RECT);
	EnableMenuItem(hMenu, ID_PARAMS_MAXVELX, mode != CParticleSys::VelocityMode::MODE_RECT);
	EnableMenuItem(hMenu, ID_PARAMS_MINVELY, mode != CParticleSys::VelocityMode::MODE_RECT);
	EnableMenuItem(hMenu, ID_PARAMS_MAXVELY, mode != CParticleSys::VelocityMode::MODE_RECT);

	if (mode == CParticleSys::VelocityMode::MODE_RECT) {
		paramTextItems[0]->SetPrefixSuffixText(TEXT("Minimum X velocity: "), TEXT(""));
		paramTextItems[1]->SetPrefixSuffixText(TEXT("Maximum X velocity: "), TEXT(""));
		paramTextItems[2]->SetPrefixSuffixText(TEXT("Minimum Y velocity: "), TEXT(""));
		paramTextItems[3]->SetPrefixSuffixText(TEXT("Maximum Y velocity: "), TEXT(""));
	} else {
		paramTextItems[0]->SetPrefixSuffixText(TEXT("Minimum velocity:   "), TEXT(""));
		paramTextItems[1]->SetPrefixSuffixText(TEXT("Maximum velocity:   "), TEXT(""));
		paramTextItems[2]->SetPrefixSuffixText(TEXT("Center angle:       "), TEXT(""));
		paramTextItems[3]->SetPrefixSuffixText(TEXT("Angular size:       "), TEXT(""));
	}
}

void CWinEvents::RandomizeGradient(CGradient &gradient)
{
	int count = gradient.GetStepCount();
	for (int i = 0; i < count; i++) {
		double position = RandInRange(0.0, 1.0);
		COLORREF color = RGB(rand() % 256, rand() % 256, rand() % 256);
		gradient.SetStep(i, position, color);
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
	case ID_PARAMS_MINVEL: display->GetTextDisplay()->SetSelectedItem(paramTextItems[CParamAgent::ParamID::MIN_VELOCITY]); break;
	case ID_PARAMS_MAXVEL: display->GetTextDisplay()->SetSelectedItem(paramTextItems[CParamAgent::ParamID::MAX_VELOCITY]); break;
	case ID_PARAMS_MINANGLE: display->GetTextDisplay()->SetSelectedItem(paramTextItems[CParamAgent::ParamID::BASE_ANGLE]); break;
	case ID_PARAMS_MAXANGLE: display->GetTextDisplay()->SetSelectedItem(paramTextItems[CParamAgent::ParamID::ANGLE_SIZE]); break;
	case ID_PARAMS_MINVELX: display->GetTextDisplay()->SetSelectedItem(paramTextItems[CParamAgent::ParamID::MIN_VELOCITY_X]); break;
	case ID_PARAMS_MAXVELX: display->GetTextDisplay()->SetSelectedItem(paramTextItems[CParamAgent::ParamID::MAX_VELOCITY_X]); break;
	case ID_PARAMS_MINVELY: display->GetTextDisplay()->SetSelectedItem(paramTextItems[CParamAgent::ParamID::MIN_VELOCITY_Y]); break;
	case ID_PARAMS_MAXVELY: display->GetTextDisplay()->SetSelectedItem(paramTextItems[CParamAgent::ParamID::MAX_VELOCITY_Y]); break;
	case ID_PARAMS_XACCEL: display->GetTextDisplay()->SetSelectedItem(paramTextItems[CParamAgent::ParamID::ACCELERATION_X]); break;
	case ID_PARAMS_YACCEL: display->GetTextDisplay()->SetSelectedItem(paramTextItems[CParamAgent::ParamID::ACCELERATION_Y]); break;
	case ID_PARAMS_MAXAGE: display->GetTextDisplay()->SetSelectedItem(paramTextItems[CParamAgent::ParamID::MAXIMUM_AGE]); break;
	case ID_PARAMS_EMISSIONRATE: display->GetTextDisplay()->SetSelectedItem(paramTextItems[CParamAgent::ParamID::EMISSION_RATE]); break;
	case ID_PARAMS_EMISSIONRADIUS: display->GetTextDisplay()->SetSelectedItem(paramTextItems[CParamAgent::ParamID::EMISSION_RADIUS]); break;
	case ID_PARAMS_INNERRADIUS: display->GetTextDisplay()->SetSelectedItem(paramTextItems[CParamAgent::ParamID::INNER_RADIUS]); break;
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
	case ID_VIEW_GRADIENTEDITOR:
		display->GetGradientEditor()->SetEnabled(!display->GetGradientEditor()->GetEnabled());
		UpdateViewMenuChecks();
		break;
	case ID_VIEW_BITMAPEDITOR:
		display->GetBitmapEditor()->SetEnabled(!display->GetBitmapEditor()->GetEnabled());
		UpdateViewMenuChecks();
		break;
	case ID_VIEW_ANIMATIONEDITOR:
		display->GetAnimEditor()->SetEnabled(!display->GetAnimEditor()->GetEnabled());
		UpdateViewMenuChecks();
		break;
	case ID_VIEW_SHOWALLTEXT:
		verbosity = 2;
		UpdateViewMenuChecks();
		break;
	case ID_VIEW_ONLYSHOWPARAMETERS:
		verbosity = 1;
		UpdateViewMenuChecks();
		break;
	case ID_VIEW_HIDETEXT:
		verbosity = 0;
		UpdateViewMenuChecks();
		break;
	case ID_HELP_CONTEXTHELP:
		display->GetHelpTextObj()->SetEnabled(!display->GetHelpTextObj()->GetEnabled());
		UpdateViewMenuChecks();
		break;
	default:
		return false;
	}
	return true;
}

void CWinEvents::UpdateViewMenuChecks()
{
	HMENU hMenu = GetMenu(hWnd);
	CheckMenuItem(hMenu, ID_VIEW_GRADIENTEDITOR, MF_CHECK_BOOL(display->GetGradientEditor()->GetEnabled()));
	CheckMenuItem(hMenu, ID_VIEW_BITMAPEDITOR, MF_CHECK_BOOL(display->GetBitmapEditor()->GetEnabled()));
	CheckMenuItem(hMenu, ID_VIEW_ANIMATIONEDITOR, MF_CHECK_BOOL(display->GetAnimEditor()->GetEnabled()));
	
	const UINT verbosityCommands[] = { ID_VIEW_HIDETEXT, ID_VIEW_ONLYSHOWPARAMETERS, ID_VIEW_SHOWALLTEXT };
	CheckMenuRadioItem(hMenu, ID_VIEW_SHOWALLTEXT, ID_VIEW_HIDETEXT, verbosityCommands[verbosity], MF_BYCOMMAND);

	CheckMenuItem(hMenu, ID_HELP_CONTEXTHELP, MF_CHECK_BOOL(display->GetHelpTextObj()->GetEnabled()));

	display->UpdateSize(&clientRect); //for animation editor positioning based on gradient editor status
}

void CWinEvents::OnSize()
{
	bbuf->UpdateSize();
	GetClientRect(hWnd, &clientRect);
	display->UpdateSize(&clientRect);

	if (firstSizeEvent) {
		// clientRect.left and clientRect.top are always zero, according to MSDN. So it's like a SIZE structure, but it uses 8 extra bytes! Yay! =D
		emitterX = (double)(clientRect.right / 2);
		emitterY = (double)(clientRect.bottom / 2);
		psys->SetEmitterPos(emitterX, emitterY);
		firstSizeEvent = false;
	}
}

void CWinEvents::OnPaint()
{
	tostringstream out;
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
			InitializeGradients(gradients);
			SelectGradient(selGradientNum, selGradientNum);
			psys->GetParticles()->clear();
		
		} else if (key == (WPARAM)'A') {
			additiveDrawing = !additiveDrawing;
		
		} else if (key == (WPARAM)'S') {
			if (psys->GetChaoticGradientFlag()) {
				psys->SetChaoticGradientFlag(false);
				psys->SetRandomColorMode(false);
			} else {
				if (psys->GetRandomColorMode()) {
					psys->SetChaoticGradientFlag(true);
				} else {
					psys->SetRandomColorMode(true);
				}
			}

		} else if (key == (WPARAM)'D') {
			psys->SetRandomImageMode(!psys->GetRandomImageMode());
		
		} else if (key == (WPARAM)'H') {
			psys->SetChaoticGradientFlag(!psys->GetChaoticGradientFlag());
		
		} else if (key == (WPARAM)'Q') {
			verbosity = (verbosity + 1) % 3;
			UpdateViewMenuChecks();
		
		} else if (key == (WPARAM)'E') {
			if (!display->GetBitmapEditor()->IsMouseDown() && !display->GetGradientEditor()->IsMouseDown()) {
				bool enable = !display->GetBitmapEditor()->GetEnabled();
				display->GetBitmapEditor()->SetEnabled(enable);
				display->GetGradientEditor()->SetEnabled(enable);
				display->GetAnimEditor()->SetEnabled(enable);
				UpdateViewMenuChecks();
			}
		
		} else if (key == VK_OEM_MINUS) {
			deltaMult /= 10;
		
		} else if (key == VK_OEM_PLUS) {
			deltaMult *= 10;
		
		} else if (key == VK_RETURN) {
			GetClientRect(hWnd, &clientRect);
			display->GetTextDisplay()->EnterKey();
		
		} else if (key == VK_UP) {
			display->GetTextDisplay()->LeftClick();
		
		} else if (key == VK_DOWN) {
			display->GetTextDisplay()->RightClick();
		
		} else if (key == VK_LEFT) {
			display->GetTextDisplay()->MouseWheel(-WHEEL_DELTA);
		
		} else if (key == VK_RIGHT) {
			display->GetTextDisplay()->MouseWheel(WHEEL_DELTA);
		}
	}
}

void CWinEvents::OnLButtonDown(int x, int y)
{
	if (mouseControlsParams) {
		display->GetTextDisplay()->LeftClick();
	} else {
		display->MouseDown(x, y);
	}
}

void CWinEvents::OnRButtonDown(int x, int y)
{
	if (mouseControlsParams) {
		display->GetTextDisplay()->RightClick();
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
	display->SetHelpText();

	if (mouseControlsParams) {
		SetCursor(curEmitter);
		SetClassLongPtr(hWnd, GCLP_HCURSOR, (DWORD)curEmitter);
		if (mouseMovesEmitter) {
			emitterX = (double)x;
			emitterY = (double)y;
		}
	} else {
		HCURSOR curArrow = LoadCursor(NULL, IDC_ARROW);
		SetCursor(curArrow);
		SetClassLongPtr(hWnd, GCLP_HCURSOR, (DWORD)curArrow);
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
		display->GetTextDisplay()->MouseWheel(wheelDelta);
	}
}

void CWinEvents::OnTimer()
{
	fpsMonitor.NewFrame();

	double highestFreq;
	if (psys->GetAnimationStatus(highestFreq)) {
		minSteps = (int)(32.0 * highestFreq / fpsMonitor.GetFPS());
		maxSteps = max(8, minSteps * 8);
		minSteps = ClampValue(minSteps, 8, 1024);
		simulationSteps += (int)(5.0 * (fpsMonitor.GetFPS() - MIN_FPS));
		simulationSteps = ClampValue(simulationSteps, minSteps, maxSteps);
	} else {
		// Multiple steps won't do any good without animation.
		simulationSteps = 1;
	}

	psys->SimulateInSteps((double)fpsMonitor.GetLastFrameTime() / 1000, emitterX, emitterY, simulationSteps);
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

void CWinEvents::SetupTextDisplay(CTextDisplay &td)
{
#ifdef _DEBUG
	td.AddText(TEXT("DEBUG BUILD (performance is not optimal)"));
#endif
	td.AddItem(new CDynamicTextItem([this](tostringstream &ss) {
		ss << "FPS: " << fpsMonitor.GetFPS() << SkipToChar(14);
		ss << "Steps: " << simulationSteps << SkipToChar(27) << "(Min: " << minSteps << SkipToChar(39) << "Max: " << maxSteps << ")";
	}));

	td.AddItem(new CDynamicTextItem([](tostringstream &ss) {
		ss << "Number of particles: " << psys->GetParticles()->size();
	}));

	td.AddText();
	td.AddText(TEXT("Use mouse buttons/wheel or arrow keys to edit parameters."));
	td.AddText(TEXT("Press ENTER to type a value directly."));

	td.AddItem(new CDynamicTextItem([](tostringstream &ss) {
		ss << "[-+] Adjustment multiplier: " << deltaMult;
	}, 1));

	td.AddItem(new CDynamicTextItem([](tostringstream &ss) {
		ss << "[Z]  Velocity mode: " << CParticleSys::VelocityModeText(psys->GetVelocityMode());
	}));

	td.AddText();
	td.AddText(TEXT("Current parameters:"));

	const tchar_t *paramPrefixes[] = {
		TEXT("Minimum velocity:   "),
		TEXT("Maximum velocity:   "),
		TEXT("Center angle:       "),
		TEXT("Angular size:       "),
		TEXT("X acceleration:     "),
		TEXT("Y acceleration:     "),
		TEXT("Maximum age:        "),
		TEXT("Emission rate:      "),
		TEXT("Emission radius:    "),
		TEXT("Inner radius:       ")
	};

	const double defaultDeltaMults[] = { 10.0, 10.0, 10.0, 10.0, 10.0, -10.0, 0.1, 10.0, 1.0, 1.0 };

	for (int i = 0; i < CParamAgent::PARAM_COUNT; i++) {
		CParamTextItem *item = new CParamTextItem();
		item->SetPrefixSuffixText(paramPrefixes[i], TEXT(""));
		item->SetMinVerbosity(1);
		item->SetTarget(*agent, (CParamAgent::ParamID)i);
		item->SetDefaultDeltaMult(defaultDeltaMults[i]);
		item->SetAnimID(i);
		td.AddItem(item);
		paramTextItems[i] = item;
	}

	td.AddText();
	td.AddText(TEXT("Use number keys to select built-in gradients"));
	td.AddText(TEXT("[R] Reset parameters"));
	td.AddText(TEXT("[C] Show/hide cursor"));
	td.AddText(TEXT("[F] Freeze/unfreeze emitter"));
	td.AddText(TEXT("[G] Reset gradient presets"));
	
	td.AddItem(new CDynamicTextItem([](tostringstream &ss) {
		ss << "[A] Toggle additive drawing (" << (additiveDrawing ? "ON" : "OFF") << ")";
	}));

	td.AddItem(new CDynamicTextItem([](tostringstream &ss) {
		ss << "[S] Static (random) color mode (" << (psys->GetChaoticGradientFlag() ? "CHAOTIC" : (psys->GetRandomColorMode() ? "ON" : "OFF")) << ")";
	}));

	td.AddItem(new CDynamicTextItem([](tostringstream &ss) {
		ss << "[D] Static (random) image mode (" << (psys->GetRandomImageMode() ? "ON" : "OFF") << ")";
	}));

	td.AddText(TEXT("[Q] Change text display"));
	td.AddText(TEXT("[E] Toggle bitmap/gradient/animation editors"));
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
