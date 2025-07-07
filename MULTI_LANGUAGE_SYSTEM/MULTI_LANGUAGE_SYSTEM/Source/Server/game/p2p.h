/// 1.
// Search @ typedef struct _CCI
	BYTE bChannel;

// Add below
#ifdef __MULTI_LANGUAGE_SYSTEM__
	BYTE bLanguage;
#endif

/// 2.
// Search
	typedef boost::unordered_map<DWORD, CCI*> TPIDCCIMap;

// Add below
#ifdef __MULTI_LANGUAGE_SYSTEM__
	typedef boost::unordered_map<BYTE, CCI*> TLocaleCCIMap;
#endif

/// 3.
// Search
	TPIDCCIMap m_map_dwPID_pkCCI;

// Add below
#ifdef __MULTI_LANGUAGE_SYSTEM__
	TLocaleCCIMap m_map_bLocale_pkCCI;
#endif