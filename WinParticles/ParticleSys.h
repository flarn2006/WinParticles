#pragma once
#include "Particle.h"
#include "Animation.h"
#include <vector>
#include <list>

class CParticleSys
{
public:
	enum VelocityMode { MODE_POLAR, MODE_RECT };

private:
	const int DISPOSE_THRESHOLD = 10; //dispose of dead at this percentage dead
	std::list<CParticle> psys;
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
	BYTE displayChance;
	CGradient gradient;
	COLORREF defaultTint;
	std::vector<CAnimationGeneric*> animations;
	int flags;

	enum Flags {
		SF_RANDOM_COLOR = 1,
		SF_RANDOM_IMAGE = 2,
		SF_CHAOTIC_GRADIENT = 4
	};

public:
	CParticleSys();

	std::list<CParticle> *GetParticles();

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

	BYTE GetDisplayChance();
	void SetDisplayChance(BYTE displayChance);

	CGradient &GetGradient();
	void SetGradient(const CGradient &gradient);

	COLORREF GetDefaultTint();
	void SetDefaultTint(COLORREF tint);

	void Simulate(double time, double destX, double destY);
	void Simulate(double time);
	void SimulateInSteps(double time, double destX, double destY, int steps);
	void SimulateInSteps(double time, int steps);
	void Draw(HDC hDC, LPRECT rect);

	bool GetRandomColorMode();
	void SetRandomColorMode(bool state);
	bool GetRandomImageMode();
	void SetRandomImageMode(bool state);
	bool GetChaoticGradientFlag();
	void SetChaoticGradientFlag(bool state);

	std::vector<CAnimationGeneric*> &GetAnimationsVector();
	void DisableAllAnimations();
	bool GetAnimationStatus();
	bool GetAnimationStatus(double &highestFreq);

	CParticle &CreateParticle(double x, double y);

	static const char *VelocityModeText(VelocityMode mode);
};

