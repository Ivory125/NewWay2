/// 1.
// Search
void LocaleService_LoadLocaleStringFile();

// Add below
#ifdef __MULTI_LANGUAGE_SYSTEM__
void LocaleService_LoadQuestTranslateFile();
void LocaleService_LoadItemNameFile();
void LocaleService_LoadMobNameFile();
void LocaleService_LoadSkillNameFile();
#endif

/// 2.
// Search
	LC_TAIWAN,

// Add below
#ifdef __MULTI_LANGUAGE_SYSTEM__
	LC_EUROPE,
#endif