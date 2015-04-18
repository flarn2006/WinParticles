#pragma once
#include "ParamTextItem.h"
#include "ParticleSys.h"
#include <unordered_map>

class CVelocityModeParamTextItem : public CParamTextItem
{
private:
	CParticleSys *psys;
	std::unordered_map<CParticleSys::VelocityMode, tstring> prefixTextMap;

protected:
	virtual void GetItemText(tstring &text) const;

public:
	CVelocityModeParamTextItem(CParticleSys *psys);
	void SetPrefixText(CParticleSys::VelocityMode velMode, const tstring &text);
};