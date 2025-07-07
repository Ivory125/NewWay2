/// 1.
// Search
	const char* GetName() { return m_pProto ? m_pProto->szLocaleName : NULL; }

// Replace with
#ifdef __MULTI_LANGUAGE_SYSTEM__
	const char* GetName();
#else
	const char* GetName() { return m_pProto ? m_pProto->szLocaleName : NULL; }
#endif

/// 2. If you have __ITEM_DROP_RENEWAL__
#if defined(__ITEM_DROP_RENEWAL__) || defined(__MULTI_LANGUAGE_SYSTEM__)
	const char* GetName();
#else
#ifdef __MULTI_LANGUAGE_SYSTEM__
	const char* GetName();
#else
	const char* GetName() { return m_pProto ? m_pProto->szLocaleName : NULL; }
#endif
#endif