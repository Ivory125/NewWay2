/// 1.
// Add
#if defined(__MULTI_LANGUAGE_SYSTEM__)
#include "../../common/length.h"
#endif

/// 2.
// Search
	void locale_init(const char* filename);
	const char* locale_find(const char* string);

// Replace with
#ifdef __MULTI_LANGUAGE_SYSTEM__
	void locale_init(const char* filename, BYTE locale);
	const char* locale_find(const char* string, BYTE locale = LOCALE_YMIR);
	const char* get_locale(BYTE locale);

	void locale_quest_translate_init(const char* filename, BYTE locale);
	const char* locale_quest_translate_find(DWORD vnum, BYTE locale = LOCALE_YMIR);

	void locale_item_init(const char* filename, BYTE locale);
	const char* locale_item_find(DWORD vnum, BYTE locale = LOCALE_YMIR);

	void locale_mob_init(const char* filename, BYTE locale);
	const char* locale_mob_find(DWORD vnum, BYTE locale = LOCALE_YMIR);

	void locale_skill_init(const char* filename, BYTE locale);
	const char* locale_skill_find(DWORD vnum, BYTE locale = LOCALE_YMIR);
#else
	void locale_init(const char* filename);
	const char* locale_find(const char* string);
#endif

/// 3.
// Search
#define LC_TEXT(str) locale_find(str)

// Add below
#ifdef __MULTI_LANGUAGE_SYSTEM__
#define LC_LOCALE_TEXT(str, locale) locale_find(str, locale)
#define LC_LOCALE(locale) get_locale(locale)

#define LC_LOCALE_QUEST_TEXT(vnum, locale) locale_quest_translate_find(vnum, locale)
#define LC_LOCALE_ITEM_TEXT(vnum, locale) locale_item_find(vnum, locale)
#define LC_LOCALE_MOB_TEXT(vnum, locale) locale_mob_find(vnum, locale)
#define LC_LOCALE_SKILL_TEXT(vnum, locale) locale_skill_find(vnum, locale)
#endif