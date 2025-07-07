/// 1.
// Search @ void CPythonNetworkStream::GamePhase
		case HEADER_GC_DRAGON_SOUL_REFINE:
			ret = RecvDragonSoulRefine();
			break;

// Add below
#ifdef ENABLE_MULTI_LANGUAGE_SYSTEM
		case HEADER_GC_REQUEST_CHANGE_LANGUAGE:
			ret = RecvRequestChangeLanguage();
			break;
#endif

#ifdef ENABLE_EXTENDED_WHISPER_DETAILS
		case HEADER_GC_WHISPER_DETAILS:
			ret = RecvWhisperDetails();
			break;
#endif

/// 2.
// Search
bool CPythonNetworkStream::RecvChatPacket()

// Add above
#ifdef ENABLE_MULTI_LANGUAGE_SYSTEM
static std::string SBS(const std::string& c_str, const std::string& c_strStartDelim, const std::string& c_strEndDelim)
{
	unsigned u_strFirstDelimPos = c_str.find(c_strStartDelim);
	unsigned u_strEndPosFirstDelim = u_strFirstDelimPos + c_strStartDelim.length();
	unsigned u_strLastDelimPos = c_str.find_first_of(c_strEndDelim, u_strEndPosFirstDelim);
	return c_str.substr(u_strEndPosFirstDelim, u_strLastDelimPos - u_strEndPosFirstDelim);
}

static const char* FilterChat(const char* c_pszBuffer)
{
	std::string strChatBuf = c_pszBuffer;
	std::string strHyperItemLink = "|cffffc700|H";

	int iFound = 0;
	std::string::size_type zPos = 0;
	while ((zPos = strChatBuf.find(strHyperItemLink, zPos)) != std::string::npos)
	{
		++iFound;
		zPos += strHyperItemLink.length();

		std::string strElement = strChatBuf.substr(zPos, strChatBuf.length());
		std::string strItemLink = SBS(strElement, "", "|h"); // ITEM
		std::string strItemVnum = SBS(strElement, "item:", ":");
		int32_t dwItemVnum = htoi(strItemVnum.c_str()); // VNUM
		std::string strItemName = SBS(strElement, "[", "]").c_str(); // ITEM NAME

		CItemManager::Instance().SelectItemData(dwItemVnum);
		CItemData* pItemData = CItemManager::Instance().GetSelectedItemDataPointer();
		if (!pItemData)
			return c_pszBuffer;

		const char* c_pszItemName = pItemData->GetName();

		char szSearchBuf[255];
		snprintf(szSearchBuf, sizeof(szSearchBuf), "[%s]", strItemName.c_str());
		std::string strReplace(szSearchBuf);

		char szReplaceBuf[255];
		snprintf(szReplaceBuf, sizeof(szReplaceBuf), "[%s]", c_pszItemName); // Replace VNUM w/ NAME

		size_t pos = strChatBuf.find(strReplace);
		strChatBuf.replace(pos, strReplace.length(), szReplaceBuf); // Replace
	}

	char szFilteredChat[1024];
	snprintf(szFilteredChat, sizeof(szFilteredChat), "%s", strChatBuf.c_str());
	return szFilteredChat;
}
#endif

/// 3.
// Search @ bool CPythonNetworkStream::RecvChatPacket
			CPythonTextTail::Instance().RegisterChatTail(kChat.dwVID, line);

// Replace with
#ifdef ENABLE_MULTI_LANGUAGE_SYSTEM
			CPythonTextTail::Instance().RegisterChatTail(kChat.dwVID, FilterChat(line));
#else
			CPythonTextTail::Instance().RegisterChatTail(kChat.dwVID, line);
#endif

/// 4.
// Search
			CPythonChat::Instance().AppendChat(kChat.type, buf);

// Replace with
#ifdef ENABLE_MULTI_LANGUAGE_SYSTEM
			CPythonChat::Instance().AppendChat(kChat.type, FilterChat(buf));
#else
			CPythonChat::Instance().AppendChat(kChat.type, buf);
#endif

/// 5.
// Search
		CPythonChat::Instance().AppendChat(kChat.type, buf);

// Replace with
#ifdef ENABLE_MULTI_LANGUAGE_SYSTEM
		CPythonChat::Instance().AppendChat(kChat.type, FilterChat(buf));
#else
		CPythonChat::Instance().AppendChat(kChat.type, buf);
#endif

/// 6.
// Search
		_snprintf(line, sizeof(line), "%s : %s", whisperPacket.szNameFrom, buf);

// Replace with
#ifdef ENABLE_MULTI_LANGUAGE_SYSTEM
		_snprintf(line, sizeof(line), "%s : %s", whisperPacket.szNameFrom, FilterChat(buf));
#else
		_snprintf(line, sizeof(line), "%s : %s", whisperPacket.szNameFrom, buf);
#endif

/// 7.
// Add to the bottom of the document
#ifdef ENABLE_MULTI_LANGUAGE_SYSTEM
bool CPythonNetworkStream::SendChangeLanguagePacket(BYTE bLanguage)
{
	TPacketChangeLanguage packet;
	packet.bHeader = HEADER_CG_CHANGE_LANGUAGE;
	packet.bLanguage = bLanguage;

	if (!Send(sizeof(packet), &packet))
		return false;

	return SendSequence();
}

bool CPythonNetworkStream::RecvRequestChangeLanguage()
{
	TPacketChangeLanguage packet;
	if (!Recv(sizeof(packet), &packet))
		return false;

	CPythonCharacterManager& rkChrMgr = CPythonCharacterManager::Instance();
	CInstanceBase* pkInstMain = rkChrMgr.GetMainInstancePtr();

	if (!pkInstMain)
	{
		TraceError("CPythonNetworkStream::RecvRequestChangeLanguage - MainCharacter is NULL");
		return false;
	}

	pkInstMain->SetLanguage(packet.bLanguage);
	pkInstMain->Update();

	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_RequestChangeLanguage", Py_BuildValue("(i)", packet.bLanguage));

	return true;
}
#endif

#ifdef ENABLE_EXTENDED_WHISPER_DETAILS
bool CPythonNetworkStream::SendWhisperDetails(const char* name)
{
	TPacketCGWhisperDetails packet;
	packet.header = HEADER_CG_WHISPER_DETAILS;
	strncpy(packet.name, name, sizeof(packet.name) - 1);
	if (!Send(sizeof(packet), &packet))
		return false;

	return SendSequence();
}

bool CPythonNetworkStream::RecvWhisperDetails()
{
	TPacketGCWhisperDetails packet;
	if (!Recv(sizeof(packet), &packet))
		return false;

#ifdef ENABLE_MULTI_LANGUAGE_SYSTEM
	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_RecieveWhisperDetails", Py_BuildValue("(si)", packet.name, packet.bLanguage));
#else
	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_RecieveWhisperDetails", Py_BuildValue("(s)", packet.name));
#endif

	return true;
}
#endif