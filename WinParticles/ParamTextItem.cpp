#include "stdafx.h"
#include "ParamTextItem.h"
#include "AnimatedParam.h"
#include "RootDisplay.h"

extern CRootDisplay *display;

CParamTextItem::CParamTextItem()
{
	target = NULL;
	targetIsMine = false;
	animID = -1;
}

CParamTextItem::~CParamTextItem()
{
	if (targetIsMine) delete target;
}

void CParamTextItem::OnSelected()
{
	CAdjustableTextItem::OnSelected();
	if (animID >= 0) display->GetAnimEditor()->SetSelectedID(animID);
}

void CParamTextItem::SetTarget(CAnimTarget<double> &target)
{
	if (targetIsMine) delete this->target;
	this->target = &target;
	targetIsMine = false;
}

void CParamTextItem::SetTarget(CParamAgent &agent, CParamAgent::ParamID paramID)
{
	if (targetIsMine) delete this->target;
	this->target = new CAnimatedParam(&agent, paramID);
	targetIsMine = true;
}

double CParamTextItem::GetValue() const
{
	return target->GetValue();
}

void CParamTextItem::SetValue(double value)
{
	target->SetValue(value);
}

void CParamTextItem::SetAnimID(int animID)
{
	this->animID = animID;
}