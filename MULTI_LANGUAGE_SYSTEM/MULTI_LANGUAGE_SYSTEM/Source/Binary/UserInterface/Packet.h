/// 1.
// Search
	HEADER_CG_STATE_CHECKER = 206,

// Add below
#ifdef ENABLE_MULTI_LANGUAGE_SYSTEM
	HEADER_CG_CHANGE_LANGUAGE = 230,
#endif

/// 2.
// Search
	HEADER_GC_RESPOND_CHANNELSTATUS = 210,

// Add below
#ifdef ENABLE_MULTI_LANGUAGE_SYSTEM
	HEADER_GC_REQUEST_CHANGE_LANGUAGE = 216,
#endif

/// 3.
// Search
} TPacketCGLogin3;

// Add above
#ifdef ENABLE_MULTI_LANGUAGE_SYSTEM
	DWORD bLanguage;
#endif

/// 4.
// Search
} TPacketGCCharacterAdditionalInfo;

// Add above
#ifdef ENABLE_MULTI_LANGUAGE_SYSTEM
	BYTE bLanguage;
#endif

/// 5.
// Search
} TPacketGCCharacterUpdate;

// Add above
#ifdef ENABLE_MULTI_LANGUAGE_SYSTEM
	BYTE bLanguage;
#endif

/// 6.
// Add to the bottom of the document #pragma pack(pop)
#ifdef ENABLE_MULTI_LANGUAGE_SYSTEM
typedef struct SPacketChangeLanguage
{
	BYTE bHeader;
	BYTE bLanguage;
} TPacketChangeLanguage;
#endif

#ifdef ENABLE_EXTENDED_WHISPER_DETAILS
typedef struct SPacketCGWhisperDetails
{
	BYTE header;
	char name[CHARACTER_NAME_MAX_LEN + 1];
} TPacketCGWhisperDetails;

typedef struct SPacketGCWhisperDetails
{
	BYTE header;
	char name[CHARACTER_NAME_MAX_LEN + 1];
#ifdef ENABLE_MULTI_LANGUAGE_SYSTEM
	BYTE bLanguage;
#endif
} TPacketGCWhisperDetails;
#endif

/// 7.
// Search
	HEADER_CG_LOGIN5_OPENID = 116, // OpenID : 실행시 받은 인증키를 서버에 보냄.

// Add below
#ifdef ENABLE_EXTENDED_WHISPER_DETAILS
	HEADER_CG_WHISPER_DETAILS = 120,
#endif

/// 8.
// Search
	HEADER_GC_AUTH_SUCCESS_OPENID = 154,

// Add below
#ifdef ENABLE_EXTENDED_WHISPER_DETAILS
	HEADER_GC_WHISPER_DETAILS = 160,
#endif