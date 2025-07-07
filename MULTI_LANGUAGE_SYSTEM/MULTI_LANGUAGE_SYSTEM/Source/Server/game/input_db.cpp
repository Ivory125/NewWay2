/// 1.
// Search @ void CInputDB::PlayerLoad
		p.bChannel = g_bChannel;

// Add below
#ifdef __MULTI_LANGUAGE_SYSTEM__
		p.bLanguage = d ? d->GetLanguage() : LOCALE_YMIR;
#endif

/// 2.
// Search @ void CInputDB::Notice
	extern void SendNotice(const char* c_pszBuf);

// Replace with
#ifdef __MULTI_LANGUAGE_SYSTEM__
	extern void SendNotice(const char* c_pszBuf, ...);
#else
	extern void SendNotice(const char* c_pszBuf);
#endif