#include "stdafx.h"
#include "ParticleSys.h"
#include "Common.h"
#include <cstdlib>
#include <cmath>

CParticleSys::CParticleSys()
{
	psys = new std::vector<CParticle>();
	emitterX = 0.0;
	emitterY = 0.0;
	timeSinceEmit = 0.0;
	defaultTint = RGB(255, 255, 255);
	DefaultParameters();
	
	originalDefGrad = defaultGradient = new CGradient(3);
	defaultGradient->SetStep(0, 0.0, RGB(0, 255, 0));
	defaultGradient->SetStep(1, 0.5, RGB(255, 255, 0));
	defaultGradient->SetStep(2, 1.0, RGB(255, 0, 0));

	livingCount = 0;
}

CParticleSys::~CParticleSys()
{
	delete psys;
	if (defaultGradient == originalDefGrad) delete defaultGradient;
}

std::vector<CParticle> *CParticleSys::GetParticles()
{
	return psys;
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
	minAngle = -180.0;
	maxAngle = 180.0;
	minVelX = -60.0;
	maxVelX = 60.0;
	minVelY = -60.0;
	maxVelY = 60.0;
	maxAge = 1.0;
	emissionRate = 1000.0;
	emissionRadius = 0.0;
	ax = 0.0;
	ay = 0.0;
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

void CParticleSys::GetAngle(double *min, double *max)
{
	if (min) *min = minAngle;
	if (max) *max = maxAngle;
}

void CParticleSys::SetAngle(double min, double max)
{
	minAngle = min;
	maxAngle = max;
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

CGradient *CParticleSys::GetDefGradient()
{
	return defaultGradient;
}

void CParticleSys::SetDefGradient(CGradient *gradient)
{
	if (defaultGradient == originalDefGrad) delete defaultGradient;
	defaultGradient = gradient;
}

COLORREF CParticleSys::GetDefaultTint()
{
	return defaultTint;
}

void CParticleSys::SetDefaultTint(COLORREF tint)
{
	defaultTint = tint;
}

void CParticleSys::SimMovingEmitter(double time, double destX, double destY)
{
	double srcX = emitterX;
	double srcY = emitterY;

	DWORD startTickCount = GetTickCount();

	// First, simulate all existing particles.
	int deadCount = 0;
	livingCount = 0;
	for (std::vector<CParticle>::iterator i = psys->begin(); i != psys->end(); i++) {
		i->Simulate(time);

		if (i->IsDead())
			deadCount++;
		else
			livingCount++;

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

	if (!psys->empty())
		if (100 * deadCount / (signed)psys->size() >= DISPOSE_THRESHOLD) DisposeOfDead();
}

void CParticleSys::Simulate(double time)
{
	SimMovingEmitter(time, emitterX, emitterY);
}

void CParticleSys::Draw(HDC hDC, LPRECT rect)
{
	for (std::vector<CParticle>::iterator i = psys->begin(); i != psys->end(); i++) {
		double x, y;
		i->GetPosition(&x, &y);
		if (rect->left <= x && x <= rect->right && rect->top <= y && y <= rect->bottom) {
			i->Draw(hDC);
		}
	}
}

int CParticleSys::GetLiveParticleCount()
{
	return livingCount;
}

CParticle &CParticleSys::CreateParticle(double x, double y)
{
	CParticle p;
	double angle;
	if (velocityMode == CParticleSys::VelocityMode::MODE_POLAR) {
		angle = RandInRange(minAngle * DEG_TO_RAD, maxAngle * DEG_TO_RAD);
	} else {
		angle = RandInRange(-M_PI, M_PI);
	}
	double vel = RandInRange(minVelocity, maxVelocity);
	double radiusMultiplier;
	if (minVelocity == maxVelocity) {
		radiusMultiplier = RandInRange(0.0, emissionRadius);
	} else {
		radiusMultiplier = Interpolate(vel, minVelocity, maxVelocity, 0.0, emissionRadius);
	}
	x += std::cos(angle) * radiusMultiplier;
	y += std::sin(angle) * radiusMultiplier;
	p.SetPosition(x, y);

	if (velocityMode == CParticleSys::VelocityMode::MODE_POLAR) {
		p.SetVelocity(vel * std::cos(angle), vel * std::sin(angle));
	} else if (velocityMode == CParticleSys::VelocityMode::MODE_RECT) {
		p.SetVelocity(RandInRange(minVelX, maxVelX), RandInRange(minVelY, maxVelY));
	}

	livingCount++;

	p.SetAcceleration(ax, ay);
	p.SetMaxAge(maxAge);
	p.SetGradient(defaultGradient);
	p.SetTint(defaultTint);
	psys->push_back(p);
	return psys->back();
}

void CParticleSys::DisposeOfDead()
{
	std::vector<CParticle> *newVec = new std::vector<CParticle>();

	for (std::vector<CParticle>::iterator i = psys->begin(); i != psys->end(); i++) {
		if (!i->IsDead()) {
			newVec->push_back(*i);
		}
	}

	delete psys;
	psys = newVec;
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