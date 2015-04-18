#include "stdafx.h"
#include "VelocityModeParamTextItem.h"

CVelocityModeParamTextItem::CVelocityModeParamTextItem(CParticleSys *psys)
{
	this->psys = psys;
}

void CVelocityModeParamTextItem::GetItemText(tstring &text) const
{
	// For some reason using prefixTextMap's operator[] doesn't work here.
	text = prefixTextMap.at(psys->GetVelocityMode()) + to_tstring(GetValue());
}

void CVelocityModeParamTextItem::SetPrefixText(CParticleSys::VelocityMode velMode, const tstring &text)
{
	prefixTextMap[velMode] = text;
}