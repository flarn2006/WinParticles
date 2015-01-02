#pragma once
#include "CompoundDispItem.h"
#include "Gradient.h"
#include <vector>
#include <commdlg.h>

class CGradientEditor : public CCompoundDispItem
{
private:
	class CStepHandle : public CDisplayItem
	{
	private:
		CGradientEditor *parent;
		CGradient *gradient;
		int stepIndex;
		int lastDragX;
		bool dragging;
		RECT bounds;
		int posXMin, posXMax, posY;
		bool posInfoSet;

		const int WIDTH = 12;
		const int HEIGHT = 16;

		void UpdateBounds();

	protected:
		virtual void OnDraw(HDC hDC, const LPRECT clientRect);
		virtual void OnMouseDown(int x, int y);
		virtual void OnMouseMove(int x, int y);
		virtual void OnMouseUp(int x, int y);
		virtual void OnRightClick(int x, int y);
		virtual bool OccupiesPoint(int x, int y);

	public:
		CStepHandle(CGradientEditor *parent);
		~CStepHandle();
		void SetGradientInfo(CGradient *gradient, int stepIndex);
		void SetPositioningInfo(int xMin, int xMax, int y);
		void StartDragging(int fromX);
		bool IsBeingDragged();
	};

	HBITMAP shBitmap, shBmpMask, shBmpColorMask, cellMarkerBmp;
	HDC shBitmapDC, cellMarkerBmpDC;
	CGradient *gradient;
	COLORREF tint;
	std::vector<CStepHandle*> stepHandles;
	LPRECT clientRect;

	CStepHandle *AddGradientStep(int x);

protected:
	virtual void OnDraw(HDC hDC, const LPRECT clientRect);
	virtual void OnMouseMove(int x, int y);
	virtual void OnMouseDown(int x, int y);
	virtual void OnRightClick(int x, int y);
	virtual bool OccupiesPoint(int x, int y);

public:
	CGradientEditor(CGradient *gradient);
	~CGradientEditor();
	void SetGradient(CGradient *gradient);
	void SetTint(COLORREF tint);
	bool IsOKToSwitchGradients();
};

