#pragma once
#include "ParticleSys.h"

class CParamAgent
{
public:
	enum ParamID {
		MIN_PARAM = 0,
		MIN_VELOCITY = 0,
		MAX_VELOCITY = 1,
		MIN_VELOCITY_X = 0,
		MAX_VELOCITY_X = 1,
		MIN_ANGLE = 2,
		MAX_ANGLE = 3,
		MIN_VELOCITY_Y = 2,
		MAX_VELOCITY_Y = 3,
		ACCELERATION_X = 4,
		ACCELERATION_Y = 5,
		MAXIMUM_AGE = 6,
		EMISSION_RATE = 7,
		EMISSION_RADIUS = 8,
		INNER_RADIUS = 9,
		MAX_PARAM = 9
	};

private:
	CParticleSys *psys;
	ParamID selParam;

public:
	CParamAgent(CParticleSys *pSys);
	~CParamAgent();
	ParamID GetSelParam();
	void SetSelParam(ParamID paramNum);
	double GetValue();
	double GetValue(ParamID paramNum);
	void SetValue(double newValue);
	void SetValue(ParamID paramNum, double newValue);
};

CParamAgent::ParamID operator--(CParamAgent::ParamID &var, int);
CParamAgent::ParamID operator++(CParamAgent::ParamID &var, int);