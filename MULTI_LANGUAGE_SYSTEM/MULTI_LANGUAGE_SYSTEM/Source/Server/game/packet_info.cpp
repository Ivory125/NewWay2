/// 1.
// Search
	Set(HEADER_CG_STATE_CHECKER, sizeof(BYTE), "ServerStateCheck");

// Add below
#ifdef __MULTI_LANGUAGE_SYSTEM__
	Set(HEADER_CG_CHANGE_LANGUAGE, sizeof(TPacketChangeLanguage), "ChangeLanguage", true);
#endif
#ifdef __EXTENDED_WHISPER_DETAILS__
	Set(HEADER_CG_WHISPER_DETAILS, sizeof(TPacketCGWhisperDetails), "WhisperDetails", true);
#endif