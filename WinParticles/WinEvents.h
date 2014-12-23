#pragma once
#include <Windows.h>
#include <shellapi.h>
#include "BackBuffer.h"
#include "ParamAgent.h"

class CWinEvents
{
private:
	HWND hWnd;
	CBackBuffer *bbuf;
	int selGradientNum;
	double emitterX, emitterY;
	double deltaMult;
	COLORREF customColors[16];
	bool mouseControlsParams = true;
	CParamAgent *agent;
	RECT clientRect;
	bool cursorHidden = false;
	bool randomizeGradientOnSelect = false;
	bool mouseMovesEmitter = true;
	COLORREF backgroundColor = 0;
	HBRUSH backgroundBrush;

	void SelectParam(CParamAgent *agent, int paramNum, double &deltaMult);
	void SetVelocityMode(CParticleSys::VelocityMode mode, HWND mainWnd);
	void RandomizeGradient(CGradient *gradient);
	void InitializeGradients(CGradient *gradients[], bool deleteFirst);
	void SelectGradient(int gradientNum, int &selGradientNum);

	static INT_PTR CALLBACK AboutDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);

public:
	CWinEvents(HWND hWnd);
	~CWinEvents();

	bool OnCommand(WORD command, WORD eventID);
	void OnSize();
	void OnPaint();
	void OnKeyDown(WORD key);
	void OnLButtonDown(SHORT x, SHORT y);
	void OnRButtonDown(SHORT x, SHORT y);
	void OnMButtonDown(SHORT x, SHORT y);
	void OnMouseMove(SHORT x, SHORT y);
	void OnLButtonUp(SHORT x, SHORT y);
	void OnMouseWheel(double delta);
	void OnTimer();
	void OnDropFiles(HDROP hDrop);
};

