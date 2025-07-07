#pragma once

#include "AbstractSingleton.h"

class IAbstractChat : public TAbstractSingleton<IAbstractChat>
{
	public:
		IAbstractChat() {}
		virtual ~IAbstractChat() {}

		virtual void AppendChat(int iType, const char * c_szChat) = 0;
};
//martysama0134's 4ee2cf2c13ce32ff35a20010fbba866e
