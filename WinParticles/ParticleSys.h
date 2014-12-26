#pragma once
#include "Particle.h"
#include "Animation.h"
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
	double baseAngle, angleSize;
	double minVelX, maxVelX, minVelY, maxVelY;
	double ax, ay;
	double maxAge;
	double emissionRate;
	double emissionRadius;
	double innerRadius;
	double timeSinceEmit;
	CGradient *originalDefGrad;
	CGradient *defaultGradient;
	COLORREF defaultTint;
	int livingCount;
	std::vector<CAnimationGeneric*> animations;

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

	void GetAngle(double *base, double *size);
	void SetAngle(double base, double size);

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

	double GetInnerRadius();
	void SetInnerRadius(double innerRadius);

	CGradient *GetDefGradient();
	void SetDefGradient(CGradient *grad);

	COLORREF GetDefaultTint();
	void SetDefaultTint(COLORREF tint);

	void SimMovingEmitter(double time, double destX, double destY);
	void Simulate(double time);
	void Draw(HDC hDC, LPRECT rect);

	int GetLiveParticleCount();

	std::vector<CAnimationGeneric*> &GetAnimationsVector();
	void DisableAllAnimations();

	CParticle &CreateParticle(double x, double y);

	void DisposeOfDead();

	static const char *VelocityModeText(VelocityMode mode);
};

