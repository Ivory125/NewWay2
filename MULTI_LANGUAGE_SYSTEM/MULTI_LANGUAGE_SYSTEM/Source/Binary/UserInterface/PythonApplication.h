/// 1.
// Search
	enum
	{
		CAMERA_MODE_NORMAL = 0,
		CAMERA_MODE_STAND = 1,
		CAMERA_MODE_BLEND = 2,

		EVENT_CAMERA_NUMBER = 101,
	};

// Add below
#ifdef ENABLE_MULTI_LANGUAGE_SYSTEM
	enum ELocales
	{
		LOCALE_DEFAULT,
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
	};
#endif