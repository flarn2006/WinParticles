#pragma once
#include "ParticleSys.h"

class CParamAgent
{
public:
	enum ParamID {
		MIN_PARAM = 0,
		MIN_VELOCITY = 0,
		MAX_VELOCITY = 1,
		ACCELERATION_X = 2,
		ACCELERATION_Y = 3,
		MAXIMUM_AGE = 4,
		EMISSION_RATE = 5,
		EMISSION_RADIUS = 6,
		MAX_PARAM = 6
	};

private:
	CParticleSys *psys;
	ParamID selParam;
	bool rectVelYSelected;

public:
	CParamAgent(CParticleSys *pSys);
	~CParamAgent();
	ParamID GetSelParam();
	void SetSelParam(ParamID paramNum);
	double GetValue();
	double GetValue(ParamID paramNum);
	void SetValue(double newValue);
	void SetValue(ParamID paramNum, double newValue);
	bool GetRectVelYBit();
	void SetRectVelYBit(bool isYSelected);
};

CParamAgent::ParamID operator--(CParamAgent::ParamID &var, int);
CParamAgent::ParamID operator++(CParamAgent::ParamID &var, int);