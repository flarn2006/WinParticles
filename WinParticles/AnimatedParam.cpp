#include "stdafx.h"
#include "AnimatedParam.h"


CAnimatedParam::CAnimatedParam()
{
	SetParamAgent(NULL, CParamAgent::ParamID::MIN_PARAM);
}

CAnimatedParam::CAnimatedParam(CParamAgent *agent, CParamAgent::ParamID paramID)
{
	SetParamAgent(agent, paramID);
}

CAnimatedParam::~CAnimatedParam()
{
}

double CAnimatedParam::GetValue()
{
	if (agent) {
		return agent->GetValue(paramID);
	} else {
		return 0.0;
	}
}

void CAnimatedParam::SetValue(double newValue)
{
	if (agent) {
		agent->SetValue(paramID, newValue);
	}
}

void CAnimatedParam::SetParamAgent(CParamAgent *agent)
{
	this->agent = agent;
}

void CAnimatedParam::SetParamAgent(CParamAgent *agent, CParamAgent::ParamID paramID)
{
	SetParamAgent(agent);
	SetParamID(paramID);
}

void CAnimatedParam::SetParamID(CParamAgent::ParamID paramID)
{
	this->paramID = paramID;
}