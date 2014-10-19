#pragma once
class CDisplayItem
{
private:
	bool enabled;
	bool mouseLastSeenInside;
	bool mouseIsDown;

protected:
	void SetMouseDownFlag(bool flag); //to force it to respond to mouse move/up events
	virtual void OnDraw(HDC hDC, const LPRECT clientRect);
	virtual void OnMouseDown(int x, int y);
	virtual void OnMouseMove(int x, int y);
	virtual void OnMouseUp(int x, int y);
	virtual void OnRightClick(int x, int y);
	virtual void OnMouseLeave();
	virtual bool OnKeyDown(UINT uCode);
	virtual bool OccupiesPoint(int x, int y);

public:
	CDisplayItem();
	~CDisplayItem();

	bool GetEnabled();
	void SetEnabled(bool enabled);

	void Draw(HDC hDC, const LPRECT clientRect);
	void MouseDown(int x, int y);
	void MouseMove(int x, int y);
	void MouseUp(int x, int y);
	void RightClick(int x, int y);
	bool KeyDown(UINT uCode);
	bool OccupyingPoint(int x, int y);

	bool IsMouseDown();
};

