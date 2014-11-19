#pragma once
#include "Particle.h"
#include <vector>

class CParticleSys
{
public:
	enum VelocityMode { MODE_POLAR, MODE_RECT };

private:
	const int DISPOSE_THRESHOLD = 10; //dispose of dead at this percentage dead
	std::vector<CParticle> *psys;
	double emitterX, emitterY;
	VelocityMode velocityMode;
	double minVelocity, maxVelocity;
	double minVelX, maxVelX, minVelY, maxVelY;
	double ax, ay;
	double maxAge;
	double emissionRate;
	double emissionRadius;
	double timeSinceEmit;
	CGradient *originalDefGrad;
	CGradient *defaultGradient;
	COLORREF defaultTint;
	int livingCount;

public:
	CParticleSys();
	~CParticleSys();

	std::vector<CParticle> *GetParticles();

	void GetEmitterPos(double *x, double *y);
	void SetEmitterPos(double x, double y);

	void DefaultParameters();

	VelocityMode GetVelocityMode();
	void SetVelocityMode(VelocityMode mode);

	void GetVelocity(double *min, double *max);
	void SetVelocity(double min, double max);
	void SetVelocity(double velocity);

	void GetRectVelocityX(double *min, double *max);
	void SetRectVelocityX(double min, double max);
	void SetRectVelocityX(double velocity);

	void GetRectVelocityY(double *min, double *max);
	void SetRectVelocityY(double min, double max);
	void SetRectVelocityY(double velocity);

	void GetAcceleration(double *x, double *y);
	void SetAcceleration(double x, double y);

	double GetMaxAge();
	void SetMaxAge(double maxAge);
	
	double GetEmissionRate();
	void SetEmissionRate(double emissionRate);

	double GetEmissionRadius();
	void SetEmissionRadius(double emissionRadius);

	CGradient *GetDefGradient();
	void SetDefGradient(CGradient *grad);

	COLORREF GetDefaultTint();
	void SetDefaultTint(COLORREF tint);

	void SimMovingEmitter(double time, double destX, double destY);
	void Simulate(double time);
	void Draw(HDC hDC, LPRECT rect);

	int GetLiveParticleCount();

	CParticle &CreateParticle(double x, double y);

	void DisposeOfDead();

	static const char *VelocityModeText(VelocityMode mode);
};

