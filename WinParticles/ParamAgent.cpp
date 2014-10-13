#include "stdafx.h"
#include "ParamAgent.h"

CParamAgent::CParamAgent(CParticleSys *pSys)
{
	psys = pSys;
	selParam = 0;
}

CParamAgent::~CParamAgent()
{
}

int CParamAgent::GetSelParam()
{
	return selParam;
}

void CParamAgent::SetSelParam(int paramNum)
{
	selParam = paramNum;
}

// Parameter ID list:
// 0: Min velocity
// 1: Max velocity
// 2: X acceleration
// 3: Y acceleration
// 4: Maximum age
// 5: Emission rate
// 6: Emission radius

double CParamAgent::GetValue()
{
	return GetValue(selParam);
}

double CParamAgent::GetValue(int paramNum)
{
	double temp1, temp2;
	switch (paramNum) {
	case 0: //min velocity
		if (psys->GetVelocityMode() == CParticleSys::VelocityMode::MODE_POLAR) {
			psys->GetVelocity(&temp1, &temp2);
			return temp1;
		} else if (!rectVelYSelected) {
			psys->GetRectVelocityX(&temp1, &temp2);
			return temp1;
		} else {
			psys->GetRectVelocityY(&temp1, &temp2);
			return temp1;
		}
	case 1: //max velocity
		if (psys->GetVelocityMode() == CParticleSys::VelocityMode::MODE_POLAR) {
			psys->GetVelocity(&temp1, &temp2);
			return temp2;
		} else if (!rectVelYSelected) {
			psys->GetRectVelocityX(&temp1, &temp2);
			return temp2;
		} else {
			psys->GetRectVelocityY(&temp1, &temp2);
			return temp2;
		}
	case 2: //X acceleration
		psys->GetAcceleration(&temp1, &temp2);
		return temp1;
	case 3: //Y acceleration
		psys->GetAcceleration(&temp1, &temp2);
		return temp2;
	case 4: //maximum age
		return psys->GetMaxAge();
	case 5: //emission rate
		return psys->GetEmissionRate();
	case 6: //emission radius
		return psys->GetEmissionRadius();
	default:
		return 0.0;
	}
}

void CParamAgent::SetValue(double newValue)
{
	SetValue(selParam, newValue);
}

void CParamAgent::SetValue(int paramNum, double newValue)
{
	double temp1, temp2;
	switch (paramNum) {
	case 0: //min velocity
		if (psys->GetVelocityMode() == CParticleSys::VelocityMode::MODE_POLAR) {
			psys->GetVelocity(&temp1, &temp2);
			psys->SetVelocity(newValue, temp2);
		} else if (!rectVelYSelected) {
			psys->GetRectVelocityX(&temp1, &temp2);
			psys->SetRectVelocityX(newValue, temp2);
		} else {
			psys->GetRectVelocityY(&temp1, &temp2);
			psys->SetRectVelocityY(newValue, temp2);
		}
		break;
	case 1: //max velocity
		if (psys->GetVelocityMode() == CParticleSys::VelocityMode::MODE_POLAR) {
			psys->GetVelocity(&temp1, &temp2);
			psys->SetVelocity(temp1, newValue);
		} else if (!rectVelYSelected) {
			psys->GetRectVelocityX(&temp1, &temp2);
			psys->SetRectVelocityX(temp1, newValue);
		} else {
			psys->GetRectVelocityY(&temp1, &temp2);
			psys->SetRectVelocityY(temp1, newValue);
		}
		break;
	case 2: //X acceleration
		psys->GetAcceleration(&temp1, &temp2);
		psys->SetAcceleration(newValue, temp2);
		break;
	case 3: //Y acceleration
		psys->GetAcceleration(&temp1, &temp2);
		psys->SetAcceleration(temp1, newValue);
		break;
	case 4: //maximum age
		if (newValue < 0) newValue = 0.0;
		psys->SetMaxAge(newValue);
		break;
	case 5: //emission rate
		if (newValue < 0) newValue = 0.0;
		psys->SetEmissionRate(newValue);
		break;
	case 6: //emission radius
		psys->SetEmissionRadius(newValue);
		break;
	}
}

bool CParamAgent::GetRectVelYBit()
{
	return rectVelYSelected;
}

void CParamAgent::SetRectVelYBit(bool isYSelected)
{
	rectVelYSelected = isYSelected;
}