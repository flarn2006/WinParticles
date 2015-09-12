#include "stdafx.h"
#include "ParticleSys.h"
#include "ParticleBitmap.h"
#include <cstdlib>
#include <cmath>

extern CParticleBitmap bitmap;

CParticleSys::CParticleSys()
{
	emitterX = 0.0;
	emitterY = 0.0;
	timeSinceEmit = 0.0;
	defaultTint = RGB(255, 255, 255);
	DefaultParameters();
	
	gradient.AddStep(0.0, 0xFFFFFF);
	gradient.AddStep(1.0, 0xFFFFFF);

	flags = 0;
}

std::list<CParticle> *CParticleSys::GetParticles()
{
	return &psys;
}

void CParticleSys::GetEmitterPos(double *x, double *y)
{
	if (x) *x = emitterX;
	if (y) *y = emitterY;
}

void CParticleSys::SetEmitterPos(double x, double y)
{
	emitterX = x;
	emitterY = y;
}

void CParticleSys::DefaultParameters()
{
	velocityMode = CParticleSys::VelocityMode::MODE_POLAR;
	minVelocity = 0.0;
	maxVelocity = 60.0;
	baseAngle = -180.0;
	angleSize = 360.0;
	minVelX = -60.0;
	maxVelX = 60.0;
	minVelY = -60.0;
	maxVelY = 60.0;
	maxAge = 1.0;
	emissionRate = 1000.0;
	emissionRadius = 0.0;
	innerRadius = 0.0;
	ax = 0.0;
	ay = 0.0;
	displayChance = 255;
}

CParticleSys::VelocityMode CParticleSys::GetVelocityMode()
{
	return velocityMode;
}

void CParticleSys::SetVelocityMode(CParticleSys::VelocityMode mode)
{
	velocityMode = mode;
}

void CParticleSys::GetVelocity(double *min, double *max)
{
	if (min) *min = minVelocity;
	if (max) *max = maxVelocity;
}

void CParticleSys::SetVelocity(double min, double max)
{
	minVelocity = min;
	maxVelocity = max;
}

void CParticleSys::SetVelocity(double velocity)
{
	minVelocity = maxVelocity = velocity;
}

void CParticleSys::GetAngle(double *base, double *size)
{
	if (base) *base = baseAngle;
	if (size) *size = angleSize;
}

void CParticleSys::SetAngle(double base, double size)
{
	baseAngle = base;
	angleSize = size;
}

void CParticleSys::GetRectVelocityX(double *min, double *max)
{
	if (min) *min = minVelX;
	if (max) *max = maxVelX;
}

void CParticleSys::SetRectVelocityX(double min, double max)
{
	minVelX = min;
	maxVelX = max;
}

void CParticleSys::SetRectVelocityX(double velocity)
{
	minVelX = maxVelX = velocity;
}

void CParticleSys::GetRectVelocityY(double *min, double *max)
{
	if (min) *min = minVelY;
	if (max) *max = maxVelY;
}

void CParticleSys::SetRectVelocityY(double min, double max)
{
	minVelY = min;
	maxVelY = max;
}

void CParticleSys::SetRectVelocityY(double velocity)
{
	minVelY = maxVelY = velocity;
}

void CParticleSys::GetAcceleration(double *x, double *y)
{
	if (x) *x = ax;
	if (y) *y = ay;
}

void CParticleSys::SetAcceleration(double x, double y)
{
	ax = x;
	ay = y;
}

double CParticleSys::GetMaxAge()
{
	return maxAge;
}

void CParticleSys::SetMaxAge(double maxAge)
{
	this->maxAge = maxAge;
}

double CParticleSys::GetEmissionRate()
{
	return emissionRate;
}

void CParticleSys::SetEmissionRate(double emissionRate)
{
	this->emissionRate = emissionRate;
}

double CParticleSys::GetEmissionRadius()
{
	return emissionRadius;
}

void CParticleSys::SetEmissionRadius(double emissionRadius)
{
	this->emissionRadius = emissionRadius;
}

double CParticleSys::GetInnerRadius()
{
	return innerRadius;
}

void CParticleSys::SetInnerRadius(double innerRadius)
{
	this->innerRadius = innerRadius;
}

BYTE CParticleSys::GetDisplayChance()
{
	return displayChance;
}

void CParticleSys::SetDisplayChance(BYTE displayChance)
{
	this->displayChance = displayChance;
}

CGradient &CParticleSys::GetGradient()
{
	return gradient;
}

void CParticleSys::SetGradient(const CGradient &gradient)
{
	this->gradient = gradient;
}

COLORREF CParticleSys::GetDefaultTint()
{
	return defaultTint;
}

void CParticleSys::SetDefaultTint(COLORREF tint)
{
	defaultTint = tint;
}

void CParticleSys::Simulate(double time, double destX, double destY)
{
	double srcX = emitterX;
	double srcY = emitterY;

	DWORD startTickCount = GetTickCount();

	for (std::vector<CAnimationGeneric*>::iterator i = animations.begin(); i != animations.end(); i++) {
		(*i)->Run(time);
	}

	// First, simulate all existing particles.
	std::list<CParticle>::iterator i = psys.begin();
	while (i != psys.end()) {
		i->Simulate(time);

		if (i->IsDead()) {
			psys.erase(i++);
		} else {
			i++;
		}

		if (GetTickCount() > startTickCount + 250) break;
	}

	startTickCount = GetTickCount();

	// Now, create new particles and simulate those for the appropriate amounts of time.
	double dt = 1.0 / emissionRate;
	double t = time + timeSinceEmit;
	while (t >= dt) {
		double progress = 1.0 - t / (time + timeSinceEmit);
		emitterX = Interpolate(progress, 0.0, 1.0, srcX, destX);
		emitterY = Interpolate(progress, 0.0, 1.0, srcY, destY);
		t -= dt;
		CreateParticle(emitterX, emitterY).Simulate(t);
		if (GetTickCount() > startTickCount + 250) break;
	}
	timeSinceEmit = t;

	emitterX = destX;
	emitterY = destY;
}

void CParticleSys::Simulate(double time)
{
	Simulate(time, emitterX, emitterY);
}

void CParticleSys::SimulateInSteps(double time, double destX, double destY, int steps)
{
#ifdef _DEBUG
	// for some reason this really slows it down in debug builds
	Simulate(time, destX, destY);
#else
	double stepTime = time / steps;
	double dX = (destX - emitterX) / steps;
	double dY = (destY - emitterY) / steps;
	for (int i = 0; i < steps; i++) {
		Simulate(stepTime, emitterX + dX, emitterY + dY);
	}
#endif
}

void CParticleSys::SimulateInSteps(double time, int steps)
{
	double stepTime = time / steps;
	for (int i = 0; i < steps; i++) {
		Simulate(stepTime);
	}
}

void CParticleSys::Draw(HDC hDC, LPRECT rect)
{
	for (std::list<CParticle>::iterator i = psys.begin(); i != psys.end(); i++) {
		double x, y;
		i->GetPosition(&x, &y);
		if (rect->left <= x && x <= rect->right && rect->top <= y && y <= rect->bottom) {
			if (displayChance == 255 || rand() % 256 >= displayChance) {
				if (GetChaoticGradientFlag())
					i->Draw(hDC, gradient.ColorAtPoint(RandInRange(0.0, 1.0)));
				else
					i->Draw(hDC);
			}
		}
	}
}

bool CParticleSys::GetRandomColorMode()
{
	return (flags & SF_RANDOM_COLOR) > 0;
}

void CParticleSys::SetRandomColorMode(bool state)
{
	if (state)
		flags |= SF_RANDOM_COLOR;
	else
		flags &= ~SF_RANDOM_COLOR;
}

bool CParticleSys::GetRandomImageMode()
{
	return (flags & SF_RANDOM_IMAGE) > 0;
}

void CParticleSys::SetRandomImageMode(bool state)
{
	if (state)
		flags |= SF_RANDOM_IMAGE;
	else
		flags &= ~SF_RANDOM_IMAGE;
}

bool CParticleSys::GetChaoticGradientFlag()
{
	return (flags & SF_CHAOTIC_GRADIENT) > 0;
}

void CParticleSys::SetChaoticGradientFlag(bool state)
{
	if (state)
		flags |= SF_CHAOTIC_GRADIENT;
	else
		flags &= ~SF_CHAOTIC_GRADIENT;
}

std::vector<CAnimationGeneric*> &CParticleSys::GetAnimationsVector()
{
	return animations;
}

void CParticleSys::DisableAllAnimations()
{
	for (std::vector<CAnimationGeneric*>::iterator i = animations.begin(); i != animations.end(); i++) {
		(*i)->SetEnabled(false);
		(*i)->Reset();
	}
}

bool CParticleSys::GetAnimationStatus()
{
	for (std::vector<CAnimationGeneric*>::iterator i = animations.begin(); i != animations.end(); i++) {
		if ((*i)->GetEnabled()) return true;
	}
	return false;
}

bool CParticleSys::GetAnimationStatus(double &highestFreq)
{
	highestFreq = 0.0;
	bool anyEnabled = false;
	for (std::vector<CAnimationGeneric*>::iterator i = animations.begin(); i != animations.end(); i++) {
		if ((*i)->GetEnabled()) {
			double freq = (*i)->GetFrequency();
			if (freq > highestFreq) highestFreq = freq;
			anyEnabled = true;
		}
	}
	return anyEnabled;
}

CParticle &CParticleSys::CreateParticle(double x, double y)
{
	CParticle p;
	
	double angle, minAngle, maxAngle;
	minAngle = baseAngle - angleSize / 2;
	maxAngle = baseAngle + angleSize / 2;

	if (velocityMode == CParticleSys::VelocityMode::MODE_POLAR) {
		angle = RandInRange(minAngle * DEG_TO_RAD, maxAngle * DEG_TO_RAD);
	} else {
		angle = RandInRange(-M_PI, M_PI);
	}
	double vel = RandInRange(minVelocity, maxVelocity);
	double radiusMultiplier;
	if (minVelocity == maxVelocity) {
		radiusMultiplier = RandInRange(innerRadius, emissionRadius);
	} else {
		radiusMultiplier = Interpolate(vel, minVelocity, maxVelocity, innerRadius, emissionRadius);
	}
	x += std::cos(angle) * radiusMultiplier;
	y += std::sin(angle) * radiusMultiplier;
	p.SetPosition(x, y);

	if (velocityMode == CParticleSys::VelocityMode::MODE_POLAR) {
		p.SetVelocity(vel * std::cos(angle), vel * std::sin(angle));
	} else if (velocityMode == CParticleSys::VelocityMode::MODE_RECT) {
		p.SetVelocity(RandInRange(minVelX, maxVelX), RandInRange(minVelY, maxVelY));
	}

	p.SetAcceleration(ax, ay);
	p.SetMaxAge(maxAge);
	p.SetGradient(&gradient);

	if (flags & SF_RANDOM_COLOR) {
		p.SetTint(MultiplyColors(defaultTint, gradient.ColorAtPoint(RandInRange(0.0, 1.0))), true);
	} else {
		p.SetTint(defaultTint, false);
	}

	if (flags & SF_RANDOM_IMAGE) {
		p.SetFixedImage(rand() % bitmap.GetCellCount());
	} else {
		p.SetFixedImage(-1);
	}
	
	psys.push_back(p);
	return psys.back();
}

const char *CParticleSys::VelocityModeText(CParticleSys::VelocityMode mode)
{
	switch (mode) {
	case CParticleSys::VelocityMode::MODE_POLAR:
		return "POLAR";
	case CParticleSys::VelocityMode::MODE_RECT:
		return "RECTANGULAR";
	default:
		return "???"; // ...Profit!
	}
}