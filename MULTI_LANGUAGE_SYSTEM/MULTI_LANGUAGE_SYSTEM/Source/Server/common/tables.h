/// 1.
// Search
	HEADER_GD_REQUEST_CHANNELSTATUS = 140,

// Add below
#ifdef __MULTI_LANGUAGE_SYSTEM__
	HEADER_GD_REQUEST_CHANGE_LANGUAGE = 146,
#endif

/// 2.
// Search
	TSimplePlayer players[PLAYER_PER_ACCOUNT];
} TAccountTable;

// Add above
#ifdef __MULTI_LANGUAGE_SYSTEM__
	BYTE bLanguage;
#endif

/// 3.
// Search
} TPacketGDAuthLogin;

// Add above
#ifdef __MULTI_LANGUAGE_SYSTEM__
	BYTE bLanguage;
#endif

/// 4.
// Search
} TPacketLoginOnSetup;

// Add above
#ifdef __MULTI_LANGUAGE_SYSTEM__
	BYTE bLanguage;
#endif

/// 5.
// Search
} TPacketNeedLoginLogInfo;

// Add above
#ifdef __MULTI_LANGUAGE_SYSTEM__
	BYTE bLanguage;
#endif

/// 6.
// Add to the bottom of the document above #pragma pack()
#ifdef __MULTI_LANGUAGE_SYSTEM__
typedef struct SRequestChangeLanguage
{
	DWORD dwAID;
	BYTE bLanguage;
} TRequestChangeLanguage;
#endif