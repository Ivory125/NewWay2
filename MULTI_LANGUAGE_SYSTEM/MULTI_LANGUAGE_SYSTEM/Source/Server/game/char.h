/// 1.
// Search
	void ResetStopTime();
	DWORD GetStopTime() const;

// Add below
#ifdef __MULTI_LANGUAGE_SYSTEM__
	bool ChangeLanguage(BYTE bLanguage);
#endif