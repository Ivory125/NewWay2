/// 1.
// Search
	LocaleService_LoadLocaleStringFile();

// Add below
#ifdef __MULTI_LANGUAGE_SYSTEM__
	LocaleService_LoadQuestTranslateFile();
	LocaleService_LoadItemNameFile();
	LocaleService_LoadMobNameFile();
	LocaleService_LoadSkillNameFile();
#endif