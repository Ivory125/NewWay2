/// 1.
// Search
	int dungeon_notice(lua_State* L)
	{
		if (!lua_isstring(L, 1))
			return 0;

		CQuestManager& q = CQuestManager::instance();
		LPDUNGEON pDungeon = q.GetCurrentDungeon();

		if (pDungeon)
			pDungeon->Notice(lua_tostring(L, 1));

		return 0;
	}

// Replace with
	int dungeon_notice(lua_State* L)
	{
#ifndef __MULTI_LANGUAGE_SYSTEM__
		if (!lua_isstring(L, 1))
			return 0;
#endif

		CQuestManager& q = CQuestManager::instance();
		LPDUNGEON pDungeon = q.GetCurrentDungeon();

		if (pDungeon)
#ifdef __MULTI_LANGUAGE_SYSTEM__
		{
			pDungeon->Notice(
				lua_tostring(L, 1),
				lua_tostring(L, 2),
				lua_tostring(L, 3),
				lua_tostring(L, 4),
				lua_tostring(L, 5),
				lua_tostring(L, 6),
				lua_tostring(L, 7),
				lua_tostring(L, 8),
				lua_tostring(L, 9),
				lua_tostring(L, 10)
			);
		}
#else
			pDungeon->Notice(lua_tostring(L, 1));
#endif

		return 0;
	}