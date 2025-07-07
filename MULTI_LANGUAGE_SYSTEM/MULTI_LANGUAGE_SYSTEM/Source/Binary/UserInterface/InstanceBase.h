/// 1.
// Search @ struct SCreateData
			short m_sAlignment;
			BYTE m_byPKMode;

// Add above
#ifdef ENABLE_MULTI_LANGUAGE_SYSTEM
			BYTE m_bLanguage;
#endif

/// 2.
// Search
		void SetStateFlags(DWORD dwStateFlags);

// Add below
#ifdef ENABLE_MULTI_LANGUAGE_SYSTEM
		void SetLanguage(BYTE bLang) { m_bLanguage = bLang; }
		BYTE GetLanguage() const { return m_bLanguage; }
#endif

/// 3.
// Search
	protected:
		CAffectFlagContainer m_kAffectFlagContainer;
		DWORD m_adwCRCAffectEffect[AFFECT_NUM];

// Add above
#ifdef ENABLE_MULTI_LANGUAGE_SYSTEM
		BYTE m_bLanguage;
#endif
