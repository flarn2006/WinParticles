#include "stdafx.h"
#include "Particle.h"
#include "Common.h"

extern HDC particleBmpDC;

CParticle::CParticle()
{
	px = py = 0.0;
	vx = vy = 0.0;
	ax = ay = 0.0;
	age = 0.0;
	maxAge = 0.0;
	tint = RGB(255, 255, 255);
	dead = false;
}

CParticle::~CParticle()
{
}

void CParticle::GetPosition(double *x, double *y)
{
	if (x) *x = px;
	if (y) *y = py;
}

void CParticle::SetPosition(double x, double y)
{
	px = x;
	py = y;
}

void CParticle::GetVelocity(double *x, double *y)
{
	if (x) *x = vx;
	if (y) *y = vy;
}

void CParticle::SetVelocity(double x, double y)
{
	vx = x;
	vy = y;
}

void CParticle::GetAcceleration(double *x, double *y)
{
	if (x) *x = ax;
	if (y) *y = ay;
}

void CParticle::SetAcceleration(double x, double y)
{
	ax = x;
	ay = y;
}

double CParticle::GetMaxAge()
{
	return maxAge;
}

void CParticle::SetMaxAge(double maxAge)
{
	this->maxAge = maxAge;
}

CGradient *CParticle::GetGradient()
{
	return gradient;
}

void CParticle::SetGradient(CGradient *gradient)
{
	this->gradient = gradient;
	double a = gradient->ColorAtPoint(0.9);
}

double CParticle::GetAge()
{
	return age;
}

COLORREF CParticle::GetTint()
{
	return tint;
}

void CParticle::SetTint(COLORREF tint)
{
	this->tint = tint;
}

void CParticle::SetDead()
{
	dead = true;
}

bool CParticle::IsDead()
{
	return dead;
}

void CParticle::Simulate(double time)
{
	if (!dead) {
		vx += ax * time;
		vy += ay * time;
		px += vx * time;
		py += vy * time;

		age += time;

		if (age > maxAge && maxAge > 0) SetDead();
	}
}

void CParticle::Draw(HDC hDC)
{
	double relativeAge = age / maxAge;

	LONG x = (LONG)px - 2;
	LONG y = (LONG)py - 2;
	SetBkColor(hDC, 0);
	SetTextColor(hDC, MultiplyColors(tint, gradient->ColorAtPoint(relativeAge)));
	BitBlt(hDC, x, y, 5, 5, particleBmpDC, (int)(relativeAge * 6.0) * 5, 0, SRCPAINT);
}

/*void CParticle::Draw(HDC hDC, double maxAge)
{
	RECT rect;
	double relativeAge = age / maxAge;

	rect.left = (LONG)px;
	rect.right = (LONG)px + 1;
	rect.top = (LONG)py;
	rect.bottom = (LONG)py + 1;
	
	if (relativeAge < 0.75) {
		rect.right++;
		rect.bottom++;
	}

	if (relativeAge < 0.25) {
		rect.left--;
		rect.top--;
	}

	if (!dead) {
		HBRUSH brush = CreateSolidBrush(gradient->ColorAtPoint(relativeAge));
		FillRect(hDC, &rect, brush);
		DeleteObject(brush);
	} else {
		SelectObject(hDC, GetStockObject(WHITE_PEN));
		Rectangle(hDC, rect.left, rect.top, rect.right, rect.bottom);
	}
}*/