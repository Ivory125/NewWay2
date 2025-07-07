#pragma once

class CNetworkDevice
{
	public:
		CNetworkDevice();
		virtual ~CNetworkDevice();

		void Destroy();
		bool Create();

	protected:
		void Initialize();

	protected:
		bool m_isWSA;
};
//martysama0134's 4ee2cf2c13ce32ff35a20010fbba866e
