/// 1.
// Search
	HEADER_CG_STATE_CHECKER = 206,

// Add below
#ifdef __MULTI_LANGUAGE_SYSTEM__
	HEADER_CG_CHANGE_LANGUAGE = 230,
#endif

/// 2.
// Search
	HEADER_GC_RESPOND_CHANNELSTATUS = 210,

// Add below
#ifdef __MULTI_LANGUAGE_SYSTEM__
	HEADER_GC_REQUEST_CHANGE_LANGUAGE = 231,
#endif

/// 3.
// Search
	HEADER_GG_CHECK_AWAKENESS = 29,

// Add below
#ifdef __MULTI_LANGUAGE_SYSTEM__
	HEADER_GG_LOCALE_NOTICE = 31,
#endif

/// 4.
// Search
} TPacketGGLogin;

// Add above
#ifdef __MULTI_LANGUAGE_SYSTEM__
	BYTE bLanguage;
#endif

/// 5.
// Search
} TPacketCGLogin3;

// Add above
#ifdef __MULTI_LANGUAGE_SYSTEM__
	BYTE bLanguage;
#endif

/// 6.
// Search
} TPacketGCCharacterAdditionalInfo;

// Add above
#ifdef __MULTI_LANGUAGE_SYSTEM__
	BYTE bLanguage;
#endif

/// 7.
// Search
} TPacketGCCharacterUpdate;

// Add above
#ifdef __MULTI_LANGUAGE_SYSTEM__
	BYTE bLanguage;
#endif

/// 8.
// Add to the bottom of the document above #pragma pack()
#ifdef __MULTI_LANGUAGE_SYSTEM__
typedef struct SPacketGGLocaleNotice
{
	BYTE bHeader;
	bool bBigFont;
	char szNotice[MAX_QUEST_NOTICE_ARGS + 1][CHAT_MAX_LEN + 1];
} TPacketGGLocaleNotice;

typedef struct SPacketChangeLanguage
{
	BYTE bHeader;
	BYTE bLanguage;
} TPacketChangeLanguage;
#endif

#ifdef __EXTENDED_WHISPER_DETAILS__
typedef struct SPacketCGWhisperDetails
{
	BYTE header;
	char name[CHARACTER_NAME_MAX_LEN + 1];
} TPacketCGWhisperDetails;

typedef struct SPacketGCWhisperDetails
{
	BYTE header;
	char name[CHARACTER_NAME_MAX_LEN + 1];
#ifdef __MULTI_LANGUAGE_SYSTEM__
	BYTE bLanguage;
#endif
} TPacketGCWhisperDetails;
#endif

/// 9.
// Search
	HEADER_CG_LOGIN5_OPENID = 116, // OpenID : 클라이언트로부터 OpenID 인증키를 받는다.

// Add below
#ifdef __EXTENDED_WHISPER_DETAILS__
	HEADER_CG_WHISPER_DETAILS = 120,
#endif

/// 10.
// Search
	HEADER_GC_AUTH_SUCCESS_OPENID = 154,

// Add below
#ifdef __EXTENDED_WHISPER_DETAILS__
	HEADER_GC_WHISPER_DETAILS = 160,
#endif