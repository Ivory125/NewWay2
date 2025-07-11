#ifndef __MILESLIB_CSOUNDMANAGERSTREAM_H__
#define __MILESLIB_CSOUNDMANAGERSTREAM_H__

#include "SoundBase.h"
#include "SoundInstance.h"

class CSoundManagerStream : public CSoundBase
{
	public:
		enum
		{
			MUSIC_INSTANCE_MAX_NUM = 3,
		};

	public:
		CSoundManagerStream();
		virtual ~CSoundManagerStream();

		bool					Initialize();
		void					Destroy();

		bool					SetInstance(DWORD dwIndex, const char* filename);
		CSoundInstanceStream *	GetInstance(DWORD dwIndex);

	protected:
		bool					CheckInstanceIndex(DWORD dwIndex);

	protected:
		CSoundInstanceStream	m_Instances[MUSIC_INSTANCE_MAX_NUM];
};

#endif
//martysama0134's 4ee2cf2c13ce32ff35a20010fbba866e
