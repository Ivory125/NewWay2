/// 1.
// Search
	// Fishing
	bool SendFishingPacket(int iRotation);
	bool SendGiveItemPacket(DWORD dwTargetVID, TItemPos ItemPos, int iItemCount);

// Add below
#ifdef ENABLE_MULTI_LANGUAGE_SYSTEM
	// Multi Language
	bool SendChangeLanguagePacket(BYTE bLanguage);
	bool SendChangeLanguage(const char* lang);

	void SetLanguage(BYTE bLanguage) { m_bLanguage = bLanguage; }
	int GetLanguage() const { return m_bLanguage; }

protected:
	BYTE m_bLanguage;

public:
#endif

#ifdef ENABLE_EXTENDED_WHISPER_DETAILS
	bool SendWhisperDetails(const char* name);
#endif

/// 2.
// Search
	// Fishing
	bool RecvFishing();

// Add below
#ifdef ENABLE_MULTI_LANGUAGE_SYSTEM
	// Multi Language
	bool RecvRequestChangeLanguage();
#endif

#ifdef ENABLE_EXTENDED_WHISPER_DETAILS
	// Whisper Details
	bool RecvWhisperDetails();
#endif