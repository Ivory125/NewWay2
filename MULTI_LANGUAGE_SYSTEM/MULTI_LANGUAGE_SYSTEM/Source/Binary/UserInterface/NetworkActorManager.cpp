/// 1.
// Search @ void SNetworkActorData::__copy__
	m_byPKMode = src.m_byPKMode;

// Add below
#ifdef ENABLE_MULTI_LANGUAGE_SYSTEM
	m_bLanguage = src.m_bLanguage;
#endif

/// 2.
// Search @ SNetworkActorData::SNetworkActorData
	m_kAffectFlags.Clear();

// Add above
#ifdef ENABLE_MULTI_LANGUAGE_SYSTEM
	m_bLanguage = 0;
#endif

/// 3.
// Search @ CInstanceBase* CNetworkActorManager::__AppendCharacterManagerActor
	CInstanceBase* pOldInstance = rkChrMgr.GetInstancePtr(dwVID);

// Add above
#ifdef ENABLE_MULTI_LANGUAGE_SYSTEM
	kCreateData.m_bLanguage = rkNetActorData.m_bLanguage;
#endif

/// 4.
// Search @ void CNetworkActorManager::UpdateActor
		pkInstFind->SetStateFlags(c_rkNetUpdateActorData.m_dwStateFlags);

// Add below
#ifdef ENABLE_MULTI_LANGUAGE_SYSTEM
		pkInstFind->SetLanguage(c_rkNetUpdateActorData.m_bLanguage);
#endif

/// 5.
// Search @ void CNetworkActorManager::UpdateActor
	rkNetActorData.m_byPKMode = c_rkNetUpdateActorData.m_byPKMode;

// Add below
#ifdef ENABLE_MULTI_LANGUAGE_SYSTEM
	rkNetActorData.m_bLanguage = c_rkNetUpdateActorData.m_bLanguage;
#endif