/// 1.
// Search @ void LocaleService_LoadLocaleStringFile
	if (g_bAuthServer)
		return;

// Add above
#ifndef __MULTI_LANGUAGE_SYSTEM__
	if (g_stLocaleFilename.empty())
		return;
#endif

/// 2.
// Search @ void LocaleService_LoadLocaleStringFile
	fprintf(stderr, "LocaleService %s\n", g_stLocaleFilename.c_str());

	locale_init(g_stLocaleFilename.c_str());

// Replace with
#ifdef __MULTI_LANGUAGE_SYSTEM__
	for (BYTE locale = 0; locale < LOCALE_MAX_NUM; locale++)
	{
		if (locale == LOCALE_YMIR)
			g_stLocaleFilename = g_stServiceBasePath + "/country/" + get_locale(LOCALE_DEFAULT) + "/" + "locale_string.txt";
		else
			g_stLocaleFilename = g_stServiceBasePath + "/country/" + get_locale(locale) + "/" + "locale_string.txt";

		if (g_stLocaleFilename.empty())
			continue;

		fprintf(stderr, "LocaleService %s\n", get_locale(locale));

		locale_init(g_stLocaleFilename.c_str(), locale);
	}
#else
	fprintf(stderr, "LocaleService %s\n", g_stLocaleFilename.c_str());

	locale_init(g_stLocaleFilename.c_str());
#endif

/// 3.
// Search
void LocaleService_LoadEmpireTextConvertTables()

// Add above
#ifdef __MULTI_LANGUAGE_SYSTEM__
void LocaleService_LoadQuestTranslateFile()
{
	if (g_stLocaleFilename.empty())
		return;

	if (g_bAuthServer)
		return;

	for (BYTE locale = 0; locale < LOCALE_MAX_NUM; locale++)
	{
		if (locale == LOCALE_YMIR)
			g_stLocaleFilename = g_stServiceBasePath + "/country/" + get_locale(LOCALE_DEFAULT) + "/" + "locale_quest.txt";
		else
			g_stLocaleFilename = g_stServiceBasePath + "/country/" + get_locale(locale) + "/" + "locale_quest.txt";

		if (g_stLocaleFilename.empty())
			continue;

		locale_quest_translate_init(g_stLocaleFilename.c_str(), locale);
	}
}

void LocaleService_LoadItemNameFile()
{
	if (g_stLocaleFilename.empty())
		return;

	if (g_bAuthServer)
		return;

	for (BYTE locale = 0; locale < LOCALE_MAX_NUM; locale++)
	{
		if (locale == LOCALE_YMIR)
			g_stLocaleFilename = g_stServiceBasePath + "/country/" + get_locale(LOCALE_DEFAULT) + "/" + "item_names.txt";
		else
			g_stLocaleFilename = g_stServiceBasePath + "/country/" + get_locale(locale) + "/" + "item_names.txt";

		if (g_stLocaleFilename.empty())
			continue;

		locale_item_init(g_stLocaleFilename.c_str(), locale);
	}
}

void LocaleService_LoadMobNameFile()
{
	if (g_stLocaleFilename.empty())
		return;

	if (g_bAuthServer)
		return;

	for (BYTE locale = 0; locale < LOCALE_MAX_NUM; locale++)
	{
		if (locale == LOCALE_YMIR)
			g_stLocaleFilename = g_stServiceBasePath + "/country/" + get_locale(LOCALE_DEFAULT) + "/" + "mob_names.txt";
		else
			g_stLocaleFilename = g_stServiceBasePath + "/country/" + get_locale(locale) + "/" + "mob_names.txt";

		if (g_stLocaleFilename.empty())
			continue;

		locale_mob_init(g_stLocaleFilename.c_str(), locale);
	}
}

void LocaleService_LoadSkillNameFile()
{
	if (g_stLocaleFilename.empty())
		return;

	if (g_bAuthServer)
		return;

	for (BYTE locale = 0; locale < LOCALE_MAX_NUM; locale++)
	{
		if (locale == LOCALE_YMIR)
			g_stLocaleFilename = g_stServiceBasePath + "/country/" + get_locale(LOCALE_DEFAULT) + "/" + "skill_names.txt";
		else
			g_stLocaleFilename = g_stServiceBasePath + "/country/" + get_locale(locale) + "/" + "skill_names.txt";

		if (g_stLocaleFilename.empty())
			continue;

		locale_skill_init(g_stLocaleFilename.c_str(), locale);
	}
}
#endif

/// 4.
// Search
static void __CheckPlayerSlot(const std::string& service_name)

// Add above
#ifdef __MULTI_LANGUAGE_SYSTEM__
static void __LocaleService_Init_Europe()
{
	g_stLocale = "latin1";
	g_stServiceBasePath = "locale/europe";
	g_stQuestDir = "locale/europe/quest";
	g_stServiceMapPath = "locale/europe/map";

	g_setQuestObjectDir.clear();
	g_setQuestObjectDir.insert("locale/europe/quest/object");
	g_stLocaleFilename = "locale/europe/locale_string.txt";

	g_iUseLocale = true;

	check_name = check_name_alphabet;

	PK_PROTECT_LEVEL = 15;
}
#endif

/// 5.
// Search
	else if ("taiwan" == g_stServiceName)
	{
		__LocaleService_Init_Taiwan();
	}

// Add below
#ifdef __MULTI_LANGUAGE_SYSTEM__
	else if ("europe" == g_stServiceName)
	{
		__LocaleService_Init_Europe();
	}
#endif

/// 6.
// Search
	else if (!g_stLocal.compare("taiwan"))
		g_eLocalType = LC_TAIWAN;

// Add below
#ifdef __MULTI_LANGUAGE_SYSTEM__
	else if (!g_stLocal.compare("europe"))
		g_eLocalType = LC_EUROPE;
#endif

/// 7.
// Search
	case LC_CANADA: // 캐나다 GF에서 서비스 시작

// Add below
#ifdef __MULTI_LANGUAGE_SYSTEM__
	case LC_EUROPE:
#endif