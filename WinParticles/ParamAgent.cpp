#include "stdafx.h"
#include "ParamAgent.h"
#include "Common.h"

CParamAgent::CParamAgent(CParticleSys *pSys)
{
	psys = pSys;
	selParam = MIN_VELOCITY;
}

CParamAgent::~CParamAgent()
{
}

CParamAgent::ParamID CParamAgent::GetSelParam()
{
	return selParam;
}

void CParamAgent::SetSelParam(ParamID paramNum)
{
	selParam = paramNum;
}

double CParamAgent::GetValue()
{
	return GetValue(selParam);
}

double CParamAgent::GetValue(ParamID paramNum)
{
	double temp1, temp2;
	switch (paramNum) {
	case MIN_VELOCITY: // MIN_VELOCITY_X in rect mode
		if (psys->GetVelocityMode() == CParticleSys::VelocityMode::MODE_POLAR) {
			psys->GetVelocity(&temp1, &temp2);
			return temp1;
		} else {
			psys->GetRectVelocityX(&temp1, &temp2);
			return temp1;
		}
	case MAX_VELOCITY: // MAX_VELOCITY_X in rect mode
		if (psys->GetVelocityMode() == CParticleSys::VelocityMode::MODE_POLAR) {
			psys->GetVelocity(&temp1, &temp2);
			return temp2;
		} else {
			psys->GetRectVelocityX(&temp1, &temp2);
			return temp2;
		}
	case BASE_ANGLE: // MIN_VELOCITY_Y in rect mode
		if (psys->GetVelocityMode() == CParticleSys::VelocityMode::MODE_POLAR) {
			psys->GetAngle(&temp1, &temp2);
			return temp1;
		} else {
			psys->GetRectVelocityY(&temp1, &temp2);
			return temp1;
		}
	case ANGLE_SIZE: // MAX_VELOCITY_Y in rect mode
		if (psys->GetVelocityMode() == CParticleSys::VelocityMode::MODE_POLAR) {
			psys->GetAngle(&temp1, &temp2);
			return temp2;
		} else {
			psys->GetRectVelocityY(&temp1, &temp2);
			return temp2;
		}
	case ACCELERATION_X:
		psys->GetAcceleration(&temp1, &temp2);
		return temp1;
	case ACCELERATION_Y:
		psys->GetAcceleration(&temp1, &temp2);
		return temp2;
	case MAXIMUM_AGE:
		return psys->GetMaxAge();
	case EMISSION_RATE:
		return psys->GetEmissionRate();
	case EMISSION_RADIUS:
		return psys->GetEmissionRadius();
	case INNER_RADIUS:
		return psys->GetInnerRadius();
	default:
		return 0.0;
	}
}

void CParamAgent::SetValue(double newValue)
{
	SetValue(selParam, newValue);
}

void CParamAgent::SetValue(ParamID paramNum, double newValue)
{
	double temp1, temp2;
	switch (paramNum) {
	case MIN_VELOCITY: // MIN_VELOCITY_X in rect mode
		if (psys->GetVelocityMode() == CParticleSys::VelocityMode::MODE_POLAR) {
			psys->GetVelocity(&temp1, &temp2);
			psys->SetVelocity(newValue, temp2);
		} else {
			psys->GetRectVelocityX(&temp1, &temp2);
			psys->SetRectVelocityX(newValue, temp2);
		}
		break;
	case MAX_VELOCITY: // MAX_VELOCITY_X in rect mode
		if (psys->GetVelocityMode() == CParticleSys::VelocityMode::MODE_POLAR) {
			psys->GetVelocity(&temp1, &temp2);
			psys->SetVelocity(temp1, newValue);
		} else {
			psys->GetRectVelocityX(&temp1, &temp2);
			psys->SetRectVelocityX(temp1, newValue);
		}
		break;
	case BASE_ANGLE: // MIN_VELOCITY_Y in rect mode
		if (psys->GetVelocityMode() == CParticleSys::VelocityMode::MODE_POLAR) {
			psys->GetAngle(&temp1, &temp2);
			psys->SetAngle(WrapValue(newValue, -180.0, 180.0), temp2);
		} else {
			psys->GetRectVelocityY(&temp1, &temp2);
			psys->SetRectVelocityY(newValue, temp2);
		}
		break;
	case ANGLE_SIZE: // MAX_VELOCITY_Y in rect mode
		if (psys->GetVelocityMode() == CParticleSys::VelocityMode::MODE_POLAR) {
			psys->GetAngle(&temp1, &temp2);
			psys->SetAngle(temp1, ClampValue(newValue, 0.0, 360.0));
		} else {
			psys->GetRectVelocityY(&temp1, &temp2);
			psys->SetRectVelocityY(temp1, newValue);
		}
		break;
	case ACCELERATION_X:
		psys->GetAcceleration(&temp1, &temp2);
		psys->SetAcceleration(newValue, temp2);
		break;
	case ACCELERATION_Y:
		psys->GetAcceleration(&temp1, &temp2);
		psys->SetAcceleration(temp1, newValue);
		break;
	case MAXIMUM_AGE:
		if (newValue < 0) newValue = 0.0;
		psys->SetMaxAge(newValue);
		break;
	case EMISSION_RATE:
		if (newValue < 0) newValue = 0.0;
		psys->SetEmissionRate(newValue);
		break;
	case EMISSION_RADIUS:
		psys->SetEmissionRadius(newValue);
		break;
	case INNER_RADIUS:
		psys->SetInnerRadius(newValue);
		break;
	}
}

CParamAgent::ParamID operator--(CParamAgent::ParamID &var, int)
{
	CParamAgent::ParamID original = var;
	var = (CParamAgent::ParamID)(var - 1);
	if (var < CParamAgent::ParamID::MIN_PARAM) var = CParamAgent::ParamID::MAX_PARAM;
	return original;
}

CParamAgent::ParamID operator++(CParamAgent::ParamID &var, int)
{
	CParamAgent::ParamID original = var;
	var = (CParamAgent::ParamID)(var + 1);
	if (var > CParamAgent::ParamID::MAX_PARAM) var = CParamAgent::ParamID::MIN_PARAM;
	return original;
}