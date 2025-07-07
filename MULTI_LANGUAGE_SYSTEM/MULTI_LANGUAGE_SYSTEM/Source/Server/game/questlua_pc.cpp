/// 1.
// Search
	void RegisterPCFunctionTable()

// Add above
#ifdef __MULTI_LANGUAGE_SYSTEM__
	int pc_get_language(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();

		if (ch && ch->GetDesc())
		{
			lua_pushstring(L, LC_LOCALE(ch->GetDesc()->GetLanguage()));
			return 1;
		}

		lua_pushstring(L, LC_LOCALE(LOCALE_DEFAULT));
		return 1;
	}
#endif

/// 2.
// Search
			{ NULL, NULL }

// Add above
#ifdef __MULTI_LANGUAGE_SYSTEM__
			{ "get_language", pc_get_language },
#endif