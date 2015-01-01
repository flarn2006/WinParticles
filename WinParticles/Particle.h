#pragma once
#include <Windows.h>
#include "Gradient.h"

class CParticle
{
private:
	double px, py;
	double vx, vy;
	double ax, ay;
	double age;
	double maxAge;
	COLORREF tint;
	int fixedImage;
	int flags;
	CGradient *gradient;

	enum Flags {
		PF_DEAD = 1,
		PF_NOGRADIENT = 2
	};

public:
	CParticle();
	~CParticle();

	void GetPosition(double *x, double *y);
	void SetPosition(double x, double y);

	void GetVelocity(double *x, double *y);
	void SetVelocity(double x, double y);

	void GetAcceleration(double *x, double *y);
	void SetAcceleration(double x, double y);

	double GetMaxAge();
	void SetMaxAge(double maxAge);

	CGradient *GetGradient();
	void SetGradient(CGradient *gradient);

	double GetAge();

	COLORREF GetTint();
	void SetTint(COLORREF tint, bool ignoreGradient = false);

	int GetFixedImage();
	void SetFixedImage(int cellNum = -1);

	void SetDead();
	bool IsDead();

	void Simulate(double time);
	void Draw(HDC hDC);
};

