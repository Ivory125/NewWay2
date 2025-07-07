/// 1.
// Search
	// BLOCK_CHAT
	void BlockChat(TPacketBlockChat* p);
	// END_OF_BLOCK_CHAT

// Add below
#ifdef __MULTI_LANGUAGE_SYSTEM__
	void ChangeLanguage(const TRequestChangeLanguage* p);
#endif