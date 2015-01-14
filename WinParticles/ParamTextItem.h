#pragma once
#include "AdjustableTextItem.h"
#include "AnimTarget.h"
#include "ParamAgent.h"

class CParamTextItem : public CAdjustableTextItem
{
private:
	CAnimTarget<double> *target;
	bool targetIsMine;

public:
	CParamTextItem();
	~CParamTextItem();

	void SetTarget(CAnimTarget<double> &target);
	void SetTarget(CParamAgent &agent, CParamAgent::ParamID paramID);
	virtual double GetValue() const;
	virtual void SetValue(double value);
};

