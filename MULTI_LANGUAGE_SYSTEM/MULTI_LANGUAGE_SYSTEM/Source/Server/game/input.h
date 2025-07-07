/// 1.
// Search
	void		Roulette(LPCHARACTER ch, const char* c_pData);

// Add below
#ifdef __MULTI_LANGUAGE_SYSTEM__
	void		ChangeLanguage(LPCHARACTER ch, BYTE bLanguage);
#endif
#ifdef __EXTENDED_WHISPER_DETAILS__
	void		WhisperDetails(LPCHARACTER ch, const char* c_pData);
#endif

/// 2.
// Search
	void		IamAwake(LPDESC d, const char* c_pData);

// Add below
#ifdef __MULTI_LANGUAGE_SYSTEM__
	void		LocaleNotice(const char* c_pData);
#endif