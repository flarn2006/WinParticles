#pragma once
class CDisplayItem
{
private:
	bool enabled;

protected:
	virtual void OnDraw(HDC hDC, LPRECT clientRect);
	virtual void OnMouseDown(int x, int y);
	virtual bool OccupiesPoint(int x, int y);

public:
	CDisplayItem();
	~CDisplayItem();

	bool GetEnabled();
	void SetEnabled(bool enabled);

	void Draw(HDC hDC, LPRECT clientRect);
	void MouseDown(int x, int y);
	bool OccupyingPoint(int x, int y);
};

