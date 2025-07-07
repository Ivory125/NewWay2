/// 1.
// Add
#if defined(ENABLE_ATLAS_MARK_INFO) || defined(ENABLE_MULTI_LANGUAGE_SYSTEM)
#include "PythonNonPlayer.h"
#endif

/// 2.
// Search
	_snprintf(szAtlasMarkInfoFileName, sizeof(szAtlasMarkInfoFileName), "%s/map/%s_point.txt", LocaleService_GetLocalePath(), rkMap.GetName().c_str());

// Replace with
#if defined(ENABLE_MULTI_LANGUAGE_SYSTEM)
	_snprintf(szAtlasMarkInfoFileName, sizeof(szAtlasMarkInfoFileName), "locale/common/map/%s_point.txt", rkMap.GetName().c_str());
#else
	_snprintf(szAtlasMarkInfoFileName, sizeof(szAtlasMarkInfoFileName), "%s/map/%s_point.txt", LocaleService_GetLocalePath(), rkMap.GetName().c_str());
#endif

/// 3.
// Search
	const std::string strType[TYPE_COUNT] = { "OPC", "OPCPVP", "OPCPVPSELF", "NPC", "MONSTER", "WARP", "WAYPOINT" };

// Replace with
#if !defined(ENABLE_MULTI_LANGUAGE_SYSTEM) && !defined(ENABLE_ATLAS_MARK_INFO)
	const std::string strType[TYPE_COUNT] = { "OPC", "OPCPVP", "OPCPVPSELF", "NPC", "MONSTER", "WARP", "WAYPOINT" };
#endif

/// 4.
// Search
		const std::string& c_rstrType = rVector[0].c_str();
		const std::string& c_rstrPositionX = rVector[1].c_str();
		const std::string& c_rstrPositionY = rVector[2].c_str();
		const std::string& c_rstrText = rVector[3].c_str();

		TAtlasMarkInfo aAtlasMarkInfo;
		// memset(&aAtlasMarkInfo, 0, sizeof(aAtlasMarkInfo));

		for (int i = 0; i < TYPE_COUNT; ++i)
		{
			if (0 == c_rstrType.compare(strType[i]))
				aAtlasMarkInfo.m_byType = (BYTE)i;
		}
		aAtlasMarkInfo.m_fX = atof(c_rstrPositionX.c_str());
		aAtlasMarkInfo.m_fY = atof(c_rstrPositionY.c_str());
		aAtlasMarkInfo.m_strText = c_rstrText;

		aAtlasMarkInfo.m_fScreenX = aAtlasMarkInfo.m_fX / m_fAtlasMaxX * m_fAtlasImageSizeX - (float)m_WhiteMark.GetWidth() / 2.0f;
		aAtlasMarkInfo.m_fScreenY = aAtlasMarkInfo.m_fY / m_fAtlasMaxY * m_fAtlasImageSizeY - (float)m_WhiteMark.GetHeight() / 2.0f;

		switch (aAtlasMarkInfo.m_byType)
		{
		case TYPE_NPC:
			m_AtlasNPCInfoVector.push_back(aAtlasMarkInfo);
			break;
		case TYPE_WARP:
			m_AtlasWarpInfoVector.push_back(aAtlasMarkInfo);
			break;
	}

// Replace with
#if defined(ENABLE_ATLAS_MARK_INFO) || defined(ENABLE_MULTI_LANGUAGE_SYSTEM)
		const std::string& c_rstrPositionX = rVector[0].c_str();
		const std::string& c_rstrPositionY = rVector[1].c_str();
		const std::string& c_rstrVnum = rVector[2].c_str();
		const DWORD c_dwVnum = atoi(c_rstrVnum.c_str());

		const CPythonNonPlayer::TMobTable* c_pMobTable = CPythonNonPlayer::Instance().GetTable(c_dwVnum);
		if (c_pMobTable)
		{
			TAtlasMarkInfo aAtlasMarkInfo;
			aAtlasMarkInfo.m_fX = atof(c_rstrPositionX.c_str());
			aAtlasMarkInfo.m_fY = atof(c_rstrPositionY.c_str());
			aAtlasMarkInfo.m_strText = c_pMobTable->szLocaleName;
			if (c_pMobTable->bType == CActorInstance::TYPE_NPC)
				aAtlasMarkInfo.m_byType = TYPE_NPC;
			else if (c_pMobTable->bType == CActorInstance::TYPE_WARP)
			{
				aAtlasMarkInfo.m_byType = TYPE_WARP;
				int iPos = aAtlasMarkInfo.m_strText.find(" ");
				if (iPos >= 0)
					aAtlasMarkInfo.m_strText[iPos] = 0;

			}
			else if (c_pMobTable->bType == CActorInstance::TYPE_STONE && c_dwVnum >= 20702 && c_dwVnum <= 20706)
				aAtlasMarkInfo.m_byType = TYPE_NPC;

			aAtlasMarkInfo.m_fScreenX = aAtlasMarkInfo.m_fX / m_fAtlasMaxX * m_fAtlasImageSizeX - (float)m_WhiteMark.GetWidth() / 2.0f;
			aAtlasMarkInfo.m_fScreenY = aAtlasMarkInfo.m_fY / m_fAtlasMaxY * m_fAtlasImageSizeY - (float)m_WhiteMark.GetHeight() / 2.0f;

			switch (aAtlasMarkInfo.m_byType)
			{
			case TYPE_NPC:
				m_AtlasNPCInfoVector.push_back(aAtlasMarkInfo);
				break;
			case TYPE_WARP:
				m_AtlasWarpInfoVector.push_back(aAtlasMarkInfo);
				break;
			}
		}
#else
		const std::string& c_rstrType = rVector[0].c_str();
		const std::string& c_rstrPositionX = rVector[1].c_str();
		const std::string& c_rstrPositionY = rVector[2].c_str();
		const std::string& c_rstrText = rVector[3].c_str();

		TAtlasMarkInfo aAtlasMarkInfo;
		// memset(&aAtlasMarkInfo, 0, sizeof(aAtlasMarkInfo));

		for (int i = 0; i < TYPE_COUNT; ++i)
		{
			if (0 == c_rstrType.compare(strType[i]))
				aAtlasMarkInfo.m_byType = (BYTE)i;
		}
		aAtlasMarkInfo.m_fX = atof(c_rstrPositionX.c_str());
		aAtlasMarkInfo.m_fY = atof(c_rstrPositionY.c_str());
		aAtlasMarkInfo.m_strText = c_rstrText;

		aAtlasMarkInfo.m_fScreenX = aAtlasMarkInfo.m_fX / m_fAtlasMaxX * m_fAtlasImageSizeX - (float)m_WhiteMark.GetWidth() / 2.0f;
		aAtlasMarkInfo.m_fScreenY = aAtlasMarkInfo.m_fY / m_fAtlasMaxY * m_fAtlasImageSizeY - (float)m_WhiteMark.GetHeight() / 2.0f;

		switch (aAtlasMarkInfo.m_byType)
		{
		case TYPE_NPC:
			m_AtlasNPCInfoVector.push_back(aAtlasMarkInfo);
			break;
		case TYPE_WARP:
			m_AtlasWarpInfoVector.push_back(aAtlasMarkInfo);
			break;
		}
#endif