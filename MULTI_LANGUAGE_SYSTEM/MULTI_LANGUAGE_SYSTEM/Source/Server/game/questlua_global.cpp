/// 1.
// Add
#ifdef __MULTI_LANGUAGE_SYSTEM__
#include "desc.h"
#endif

/// 2.
// Search
	int _get_locale(lua_State* L)
	{
		lua_pushstring(L, g_stLocale.c_str());
		return 1;
	}

// Replace with
	int _get_locale(lua_State* L)
	{
#ifdef __MULTI_LANGUAGE_SYSTEM__
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
		BYTE bLocale = (ch ? ch->GetDesc()->GetLanguage() : LOCALE_DEFAULT);
		lua_pushstring(L, LC_LOCALE(bLocale));
#else
		lua_pushstring(L, g_stLocale.c_str());
#endif
		return 1;
	}

/// 3.
// Search @ int _item_name
				lua_pushstring(L, pTable->szLocaleName);

// Replace with
#ifdef __MULTI_LANGUAGE_SYSTEM__
			{
				LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
				BYTE bLocale = (ch ? ch->GetDesc()->GetLanguage() : LOCALE_DEFAULT);
				lua_pushstring(L, LC_LOCALE_ITEM_TEXT(dwVnum, bLocale));
			}
#else
				lua_pushstring(L, pTable->szLocaleName);
#endif

/// 4.
// Search @ int _mob_name
				lua_pushstring(L, pkMob->m_table.szLocaleName);

#ifdef __MULTI_LANGUAGE_SYSTEM__
			{
				LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
				BYTE bLocale = (ch ? ch->GetDesc()->GetLanguage() : LOCALE_DEFAULT);
				lua_pushstring(L, LC_LOCALE_MOB_TEXT(dwVnum, bLocale));
			}
#else
				lua_pushstring(L, pkMob->m_table.szLocaleName);
#endif

/// 5.
// Search
	int _notice_all(lua_State* L)
	{
		ostringstream s;
		combine_lua_string(L, s);

		TPacketGGNotice p;
		p.bHeader = HEADER_GG_NOTICE;
		p.lSize = strlen(s.str().c_str()) + 1;

		TEMP_BUFFER buf;
		buf.write(&p, sizeof(p));
		buf.write(s.str().c_str(), p.lSize);

		P2P_MANAGER::instance().Send(buf.read_peek(), buf.size()); // HEADER_GG_NOTICE

		SendNotice(s.str().c_str());

		return 1;
	}

// Replace with
	int _notice_all(lua_State* L)
	{
#ifdef __MULTI_LANGUAGE_SYSTEM__
		if (lua_gettop(L) > MAX_QUEST_NOTICE_ARGS + 1)
		{
			sys_err("QUEST invalid argument.");
			return 0;
		}

		char szNoticeBuf[MAX_QUEST_NOTICE_ARGS + 1][CHAT_MAX_LEN + 1];
		int iLen;
		for (int i = 0; i < (MAX_QUEST_NOTICE_ARGS + 1); i++)
		{
			iLen = snprintf(szNoticeBuf[i], sizeof(szNoticeBuf[i]), "%s", lua_tostring(L, 1 + i));

			if (iLen < 0 || iLen >= (int)sizeof(szNoticeBuf[i]))
				iLen = sizeof(szNoticeBuf[i]) - 1;
		}

		TPacketGGLocaleNotice GGPacket;
		GGPacket.bHeader = HEADER_GG_LOCALE_NOTICE;
		GGPacket.bBigFont = false;
		for (int i = 0; i < (MAX_QUEST_NOTICE_ARGS + 1); i++)
			strlcpy(GGPacket.szNotice[i], szNoticeBuf[i], sizeof(GGPacket.szNotice[i]));
		P2P_MANAGER::instance().Send(&GGPacket, sizeof(TPacketGGLocaleNotice));

		SendLocaleNotice(szNoticeBuf[0], false, szNoticeBuf[1], szNoticeBuf[2], szNoticeBuf[3], szNoticeBuf[4], szNoticeBuf[5]);
#else
		ostringstream s;
		combine_lua_string(L, s);

		TPacketGGNotice p;
		p.bHeader = HEADER_GG_NOTICE;
		p.lSize = strlen(s.str().c_str()) + 1;

		TEMP_BUFFER buf;
		buf.write(&p, sizeof(p));
		buf.write(s.str().c_str(), p.lSize);

		P2P_MANAGER::instance().Send(buf.read_peek(), buf.size()); // HEADER_GG_NOTICE

		SendNotice(s.str().c_str());
#endif
		return 1;
	}

/// 6.
// Search
			{ NULL, NULL }

// Add above
#ifdef __MULTI_LANGUAGE_SYSTEM__
			{ "LC", _get_locale },
			{ "LANG", _get_locale },
#endif