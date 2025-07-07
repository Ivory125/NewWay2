/// 1.
// Search @ struct SNetworkActorData
	SNetworkActorData();

// Add above
#ifdef ENABLE_MULTI_LANGUAGE_SYSTEM
	BYTE m_bLanguage;
#endif

/// 2.
// Search @ struct SNetworkUpdateActorData
	DWORD m_dwStateFlags; // 본래 Create 때만 쓰이는 변수임

// Add above
#ifdef ENABLE_MULTI_LANGUAGE_SYSTEM
	BYTE m_bLanguage;
#endif

/// 3.
// Search @ struct SNetworkUpdateActorData
		m_dwStateFlags = 0;
		m_kAffectFlags.Clear();

// Add above
#ifdef ENABLE_MULTI_LANGUAGE_SYSTEM
		m_bLanguage = 0;
#endif