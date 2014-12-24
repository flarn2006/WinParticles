#pragma once
#include "AnimTarget.h"
#include "ParamAgent.h"

class CAnimatedParam : public CAnimTarget<double>
{
private:
	CParamAgent *agent;
	CParamAgent::ParamID paramID;

public:
	CAnimatedParam();
	CAnimatedParam(CParamAgent *agent, CParamAgent::ParamID paramID = CParamAgent::ParamID::MIN_PARAM);
	~CAnimatedParam();

	virtual double GetValue();
	virtual void SetValue(double newValue);

	void SetParamAgent(CParamAgent *agent);
	void SetParamAgent(CParamAgent *agent, CParamAgent::ParamID paramID);
	void SetParamID(CParamAgent::ParamID paramID);
};

