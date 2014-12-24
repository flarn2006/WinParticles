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

	void SelectParam(CParamAgent *agent, CParamAgent::ParamID paramNum, double &deltaMult);
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
	void OnLButtonDown(int x, int y);
	void OnRButtonDown(int x, int y);
	void OnMButtonDown(int x, int y);
	void OnMouseMove(int x, int y);
	void OnLButtonUp(int x, int y);
	void OnMouseWheel(double delta);
	void OnTimer();
	void OnDropFiles(HDROP hDrop);
};

