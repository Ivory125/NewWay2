/// 1.
// Search @ bool CAccountConnector::__AuthState_RecvPhase - TPacketCGLogin3
		m_strPassword = "";

		for (DWORD i = 0; i < 4; ++i)
			LoginPacket.adwClientKey[i] = g_adwEncryptKey[i];

// Add below
#ifdef ENABLE_MULTI_LANGUAGE_SYSTEM
		LoginPacket.bLanguage = rkNetStream.GetLanguage();
#endif