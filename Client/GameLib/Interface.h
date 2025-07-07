#pragma once

class IBackground : public CSingleton<IBackground>
{
	public:
		IBackground() {}
		virtual ~IBackground() {}

		virtual bool IsBlock(int x, int y) = 0;
};
//martysama0134's 4ee2cf2c13ce32ff35a20010fbba866e
