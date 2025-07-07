/// 1.
// Search
#include "../Gamelib/ActorInstance.h"

// Add below
#ifdef ENABLE_MULTI_LANGUAGE_SYSTEM
#include "InstanceBase.h"
#endif

/// 2.
// Search @ bool CPythonNetworkStream::RecvCharacterAppendPacket
	kNetActorData.m_byPKMode = 0; /*chrAddPacket.bPKMode*/;

// Add below
#ifdef ENABLE_MULTI_LANGUAGE_SYSTEM
	kNetActorData.m_bLanguage = 0;
#endif

/// 2.
// Search @ bool CPythonNetworkStream::RecvCharacterAdditionalInfo
		kNetActorData.m_stName = chrInfoPacket.name;

// Replace with
#ifdef ENABLE_MULTI_LANGUAGE_SYSTEM
		const char* c_szNpcClientName;
		if (kNetActorData.m_bType == CActorInstance::TYPE_NPC
			&& kNetActorData.m_dwRace != 30000
			)
		{
			if (kNetActorData.m_dwRace > 34000 && kNetActorData.m_dwRace < 35000) ||
				(kNetActorData.m_dwRace > 20100 && kNetActorData.m_dwRace < 20110)
				)
			{
				char szPetName[CHARACTER_NAME_MAX_LEN];
				if (CPythonNonPlayer::Instance().GetName(kNetActorData.m_dwRace, &c_szNpcClientName))
					sprintf(szPetName, "%s - %s", chrInfoPacket.name, c_szNpcClientName);
				else
					sprintf(szPetName, "%s", chrInfoPacket.name);

				kNetActorData.m_stName = szPetName;
			}
			else
			{
				if (CPythonNonPlayer::Instance().GetName(kNetActorData.m_dwRace, &c_szNpcClientName))
					kNetActorData.m_stName = c_szNpcClientName;
				else
					kNetActorData.m_stName = chrInfoPacket.name;
			}
		}
		else
		{
			kNetActorData.m_stName = chrInfoPacket.name;
		}
#else
		kNetActorData.m_stName = chrInfoPacket.name;
#endif

/// 3.
// Search @ bool CPythonNetworkStream::RecvCharacterAdditionalInfo
		__RecvCharacterAppendPacket(&kNetActorData);

// Add above
#ifdef ENABLE_MULTI_LANGUAGE_SYSTEM
		kNetActorData.m_bLanguage = chrInfoPacket.bLanguage;
#endif

/// 4.
// Search @ bool CPythonNetworkStream::RecvCharacterUpdatePacket
	__RecvCharacterUpdatePacket(&kNetUpdateActorData);

// Add above
#ifdef ENABLE_MULTI_LANGUAGE_SYSTEM
	kNetUpdateActorData.m_bLanguage = chrUpdatePacket.bLanguage;
#endif