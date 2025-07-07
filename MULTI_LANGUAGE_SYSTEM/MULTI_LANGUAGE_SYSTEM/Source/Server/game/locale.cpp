/// 1.
// Add
#if defined(__MULTI_LANGUAGE_SYSTEM__)
#include "../../common/length.h"
#include "CsvReader.h"
#endif

/// 2.
// Search
typedef std::map<std::string, std::string> LocaleStringMapType;

// Add below
#ifdef __MULTI_LANGUAGE_SYSTEM__
typedef std::map<DWORD, std::string> LocaleQuestTranslateMapType;
typedef std::map<DWORD, std::string> LocaleItemMapType;
typedef std::map<DWORD, std::string> LocaleMobMapType;
typedef std::map<DWORD, std::string> LocaleSkillMapType;
#endif

/// 3.
// Search
LocaleStringMapType localeString;

// Replace with
#ifdef __MULTI_LANGUAGE_SYSTEM__
LocaleQuestTranslateMapType localeQuestTranslate[LOCALE_MAX_NUM];
LocaleStringMapType localeString[LOCALE_MAX_NUM];
LocaleItemMapType localeItem[LOCALE_MAX_NUM];
LocaleMobMapType localeMob[LOCALE_MAX_NUM];
LocaleSkillMapType localeSkill[LOCALE_MAX_NUM];
#else
LocaleStringMapType localeString;
#endif

/// 4.
// Search
void locale_add(const char** strings)
{
	LocaleStringMapType::const_iterator iter = localeString.find(strings[0]);

	if (iter == localeString.end())
	{
		localeString.insert(std::make_pair(strings[0], strings[1]));
	}
}

const char* locale_find(const char* string)
{
	if (0 == g_iUseLocale || LC_IsKorea() || LC_IsWE_Korea())
		return (string);

	LocaleStringMapType::const_iterator iter = localeString.find(string);

	if (iter == localeString.end())
	{
		static char s_line[1024] = "@0949";
		strlcpy(s_line + 5, string, sizeof(s_line) - 5);

		sys_err("LOCALE_ERROR: \"%s\";", string);
		return s_line;
	}

	return iter->second.c_str();
}

// Replace with
#ifdef __MULTI_LANGUAGE_SYSTEM__
const char* get_locale(BYTE locale)
{
	switch (locale)
	{
	case LOCALE_EN:
		return "en";
	case LOCALE_PT:
		return "pt";
	case LOCALE_ES:
		return "es";
	case LOCALE_FR:
		return "fr";
	case LOCALE_DE:
		return "de";
	case LOCALE_RO:
		return "ro";
	case LOCALE_PL:
		return "pl";
	case LOCALE_IT:
		return "it";
	case LOCALE_CZ:
		return "cz";
	case LOCALE_HU:
		return "hu";
	case LOCALE_TR:
		return "tr";
	default:
		return "en";
	}
}

void locale_add(const char** strings, BYTE locale)
{
	LocaleStringMapType::const_iterator iter = localeString[locale].find(strings[0]);

	if (iter == localeString[locale].end())
	{
		localeString[locale].insert(std::make_pair(strings[0], strings[1]));
	}
}

const char* locale_find(const char* string, BYTE locale)
{
	if (!*string || locale == LOCALE_YMIR)
		return string;

	if (locale > LOCALE_MAX_NUM)
		locale = LOCALE_EN;

	LocaleStringMapType::const_iterator iter = localeString[locale].find(string);

	if (iter == localeString[locale].end())
	{
		// sys_err("LOCALE_ERROR: \"%s\";", string);
		return string;
	}

	return iter->second.c_str();
}

const char* locale_quest_translate_find(DWORD vnum, BYTE locale)
{
	if (locale > LOCALE_MAX_NUM)
		locale = LOCALE_EN;

	LocaleQuestTranslateMapType::const_iterator iter = localeQuestTranslate[locale].find(vnum);

	if (iter == localeQuestTranslate[locale].end())
	{
		return "NoName";
	}

	return iter->second.c_str();
}

const char* locale_item_find(DWORD vnum, BYTE locale)
{
	if (locale > LOCALE_MAX_NUM)
		locale = LOCALE_EN;

	LocaleItemMapType::const_iterator iter = localeItem[locale].find(vnum);

	if (iter == localeItem[locale].end())
	{
		return "NoName";
	}

	return iter->second.c_str();
}

const char* locale_mob_find(DWORD vnum, BYTE locale)
{
	if (locale > LOCALE_MAX_NUM)
		locale = LOCALE_EN;

	LocaleMobMapType::const_iterator iter = localeMob[locale].find(vnum);

	if (iter == localeMob[locale].end())
		return "NoName";

	return iter->second.c_str();
}

const char* locale_skill_find(DWORD vnum, BYTE locale)
{
	if (locale > LOCALE_MAX_NUM)
		locale = LOCALE_EN;

	LocaleSkillMapType::const_iterator iter = localeSkill[locale].find(vnum);

	if (iter == localeSkill[locale].end())
		return "NoName";

	return iter->second.c_str();
}

void locale_quest_translate_init(const char* filename, BYTE locale)
{
	cCsvTable nameData;

	if (!nameData.Load(filename, '\t'))
	{
		fprintf(stderr, "%s couldn't be loaded or its format is incorrect.\n", filename);
		return;
	}

	nameData.Next();

	while (nameData.Next())
	{
		if (nameData.ColCount() > 1)
			localeQuestTranslate[locale].insert(std::make_pair(atoi(nameData.AsStringByIndex(0)), nameData.AsStringByIndex(1)));
	}

	nameData.Destroy();
}

void locale_item_init(const char* filename, BYTE locale)
{
	cCsvTable nameData;

	if (!nameData.Load(filename, '\t'))
	{
		fprintf(stderr, "%s couldn't be loaded or its format is incorrect.\n", filename);
		return;
	}

	nameData.Next();

	while (nameData.Next())
	{
		if (nameData.ColCount() > 1)
			localeItem[locale].insert(std::make_pair(atoi(nameData.AsStringByIndex(0)), nameData.AsStringByIndex(1)));
	}

	nameData.Destroy();
}

void locale_mob_init(const char* filename, BYTE locale)
{
	cCsvTable nameData;

	if (!nameData.Load(filename, '\t'))
	{
		fprintf(stderr, "%s couldn't be loaded or its format is incorrect.\n", filename);
		return;
	}

	nameData.Next();

	while (nameData.Next())
	{
		if (nameData.ColCount() > 1)
			localeMob[locale].insert(std::make_pair(atoi(nameData.AsStringByIndex(0)), nameData.AsStringByIndex(1)));
	}

	nameData.Destroy();
}

void locale_skill_init(const char* filename, BYTE locale)
{
	cCsvTable nameData;

	if (!nameData.Load(filename, '\t'))
	{
		fprintf(stderr, "%s couldn't be loaded or its format is incorrect.\n", filename);
		return;
	}

	nameData.Next();

	while (nameData.Next())
	{
		if (nameData.ColCount() > 1)
			localeSkill[locale].insert(std::make_pair(atoi(nameData.AsStringByIndex(0)), nameData.AsStringByIndex(1)));
	}

	nameData.Destroy();
}
#else
void locale_add(const char** strings)
{
	LocaleStringMapType::const_iterator iter = localeString.find(strings[0]);

	if (iter == localeString.end())
	{
		localeString.insert(std::make_pair(strings[0], strings[1]));
	}
}

const char* locale_find(const char* string)
{
	if (0 == g_iUseLocale || LC_IsKorea() || LC_IsWE_Korea())
		return (string);

	LocaleStringMapType::const_iterator iter = localeString.find(string);

	if (iter == localeString.end())
	{
		static char s_line[1024] = "@0949";
		strlcpy(s_line + 5, string, sizeof(s_line) - 5);

		sys_err("LOCALE_ERROR: \"%s\";", string);
		return s_line;
	}

	return iter->second.c_str();
}
#endif

/// 5.
// Search
void locale_init(const char* filename)

// Replace with
#ifdef __MULTI_LANGUAGE_SYSTEM__
void locale_init(const char* filename, BYTE locale)
#else
void locale_init(const char* filename)
#endif

/// 6.
// Search
			locale_add((const char**)strings);

// Replace with
#ifdef __MULTI_LANGUAGE_SYSTEM__
			locale_add((const char**)strings, locale);
#else
			locale_add((const char**)strings);
#endif