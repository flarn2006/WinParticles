#include "stdafx.h"
#include "Particle.h"
#include "ParticleBitmap.h"

extern CParticleBitmap bitmap;
extern bool additiveDrawing;

CParticle::CParticle()
{
	px = py = 0.0;
	vx = vy = 0.0;
	ax = ay = 0.0;
	age = 0.0;
	maxAge = 0.0;
	tint = RGB(255, 255, 255);
	fixedImage = -1;
	flags = 0;
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

void CParticle::SetTint(COLORREF tint, bool ignoreGradient)
{
	this->tint = tint;
	if (ignoreGradient)
		flags |= PF_NOGRADIENT;
	else
		flags &= ~PF_NOGRADIENT;
}

int CParticle::GetFixedImage()
{
	return fixedImage;
}

void CParticle::SetFixedImage(int cellNum)
{
	fixedImage = cellNum;
}

void CParticle::SetDead()
{
	flags |= PF_DEAD;
}

bool CParticle::IsDead()
{
	return (flags & PF_DEAD) > 0;
}

void CParticle::Simulate(double time)
{
	if (!IsDead()) {
		if (ax != 0) {
			double vx0 = vx;
			vx += ax * time;
			px += (vx + vx0) * (vx - vx0) / 2 / ax;
		} else {
			px += vx * time;
		}

		if (ay != 0) {
			double vy0 = vy;
			vy += ay * time;
			py += (vy + vy0) * (vy - vy0) / 2 / ay;
		} else {
			py += vy * time;
		}

		age += time;

		if (age > maxAge && maxAge >= 0) SetDead();
	}
}

void CParticle::Draw(HDC hDC, COLORREF color)
{
	if (!IsDead()) {
		if (fixedImage >= 0) {
			bitmap.Draw(hDC, (int)px, (int)py, color, fixedImage);
		} else {
			bitmap.Draw(hDC, (int)px, (int)py, color, age / maxAge);
		}
	}
}

void CParticle::Draw(HDC hDC)
{
	Draw(hDC, GetColor());
}

COLORREF CParticle::GetColor()
{
	if (flags & PF_NOGRADIENT)
		return tint;
	else
		return MultiplyColors(tint, gradient->ColorAtPoint(age / maxAge));
}