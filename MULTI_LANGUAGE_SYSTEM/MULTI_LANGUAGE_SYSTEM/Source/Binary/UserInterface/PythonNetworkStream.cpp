/// 1.
// Search
		Set(HEADER_GC_DRAGON_SOUL_REFINE, CNetworkPacketHeaderMap::TPacketType(sizeof(TPacketGCDragonSoulRefine), STATIC_SIZE_PACKET));

// Add below
#ifdef ENABLE_MULTI_LANGUAGE_SYSTEM
		Set(HEADER_GC_REQUEST_CHANGE_LANGUAGE, CNetworkPacketHeaderMap::TPacketType(sizeof(TPacketChangeLanguage), STATIC_SIZE_PACKET));
#endif

#ifdef ENABLE_EXTENDED_WHISPER_DETAILS
		Set(HEADER_GC_WHISPER_DETAILS, CNetworkPacketHeaderMap::TPacketType(sizeof(TPacketGCWhisperDetails), STATIC_SIZE_PACKET));
#endif