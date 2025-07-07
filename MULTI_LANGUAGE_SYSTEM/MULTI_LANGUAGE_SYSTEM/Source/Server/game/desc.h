/// 1.
// Search
	bool isChannelStatusRequested() const { return m_bChannelStatusRequested; }
	void SetChannelStatusRequested(bool bChannelStatusRequested) { m_bChannelStatusRequested = bChannelStatusRequested; }

// Add below
#ifdef __MULTI_LANGUAGE_SYSTEM__
	void SetLanguage(BYTE bLanguage) { m_accountTable.bLanguage = bLanguage; }
	BYTE GetLanguage() { return m_accountTable.bLanguage; }
#endif