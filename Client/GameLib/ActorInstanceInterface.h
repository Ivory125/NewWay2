#pragma once

#include "../eterGrnLib/ThingInstance.h"

class IActorInstance : public CGraphicThingInstance
{
public:
	enum
	{
		ID = ACTOR_OBJECT
	};
	int GetType() const { return ID; }

	IActorInstance() {}
	virtual ~IActorInstance() {}
	virtual bool TestCollisionWithDynamicSphere(const CDynamicSphereInstance & dsi) = 0;
	virtual DWORD GetVirtualID() = 0;
};
//martysama0134's 4ee2cf2c13ce32ff35a20010fbba866e
