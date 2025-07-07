/// 1.
// Add to the bottom of the document above #pragma pack(pop)
#ifdef __MULTI_LANGUAGE_SYSTEM__
enum EMultiLocale
{
	MAX_QUEST_NOTICE_ARGS = 5
};

enum ELocale
{
	LOCALE_YMIR, // Korea
	LOCALE_EN, // United Kingdom
	LOCALE_PT, // Portugal
	LOCALE_ES, // Spain
	LOCALE_FR, // France
	LOCALE_DE, // Germany
	LOCALE_RO, // Romania
	LOCALE_PL, // Poland
	LOCALE_IT, // Italy
	LOCALE_CZ, // Czech
	LOCALE_HU, // Hungary
	LOCALE_TR, // Turkey
	LOCALE_MAX_NUM,
	LOCALE_DEFAULT = LOCALE_EN,
};
#endif