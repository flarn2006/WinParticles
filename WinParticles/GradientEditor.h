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
		HBITMAP bitmap, bmpMask, bmpColorMask;
		HDC bitmapDC;
		int posXMin, posXMax, posY;

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
	};

	CGradient *gradient;
	COLORREF tint;
	std::vector<CStepHandle*> stepHandles;

protected:
	virtual void OnDraw(HDC hDC, const LPRECT clientRect);

public:
	CGradientEditor(CGradient *gradient);
	~CGradientEditor();
	void SetGradient(CGradient *gradient);
	void SetTint(COLORREF tint);
};

