#pragma once
#include "ParticleSys.h"

class CParamAgent
{
private:
	CParticleSys *psys;
	int selParam;
	bool rectVelYSelected;

public:
	CParamAgent(CParticleSys *pSys);
	~CParamAgent();
	int GetSelParam();
	void SetSelParam(int paramNum);
	double GetValue();
	double GetValue(int paramNum);
	void SetValue(double newValue);
	void SetValue(int paramNum, double newValue);
	bool GetRectVelYBit();
	void SetRectVelYBit(bool isYSelected);
};

