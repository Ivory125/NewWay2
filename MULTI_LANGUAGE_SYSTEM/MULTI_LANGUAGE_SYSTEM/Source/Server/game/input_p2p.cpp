/// 1.
// Search
void CInputP2P::Disconnect(const char* c_pData)

// Add above
#ifdef __MULTI_LANGUAGE_SYSTEM__
void SendLCNotice(const char* szNotice, bool bBigFont, ...)
{
	const DESC_MANAGER::DESC_SET& c_ref_set = DESC_MANAGER::instance().GetClientSet();
	DESC_MANAGER::DESC_SET::const_iterator it = c_ref_set.begin();

	while (it != c_ref_set.end())
	{
		LPDESC d = *(it++);
		if (d->GetCharacter())
		{
			std::string strMsg = szNotice;
			const char* c_pszBuf;

			if (!strMsg.empty() && std::all_of(strMsg.begin(), strMsg.end(), ::isdigit))
			{
				DWORD dwKey = atoi(szNotice);
				BYTE bLanguage = (d ? d->GetLanguage() : LOCALE_YMIR);

				c_pszBuf = LC_LOCALE_QUEST_TEXT(dwKey, bLanguage);
			}
			else
			{
				c_pszBuf = szNotice;
			}

			std::string strBuffFilter = c_pszBuf;
			std::string strReplace("%d");

			size_t pos = 0;
			while ((pos = strBuffFilter.find(strReplace)) != std::string::npos)
			{
				strBuffFilter.replace(pos, strReplace.length(), "%s");
			}

			const char* c_pszConvBuf = strBuffFilter.c_str();
			char szNoticeBuf[CHAT_MAX_LEN + 1];

			va_list args;
			va_start(args, bBigFont);
			int len = vsnprintf(szNoticeBuf, sizeof(szNoticeBuf), c_pszConvBuf, args);
			va_end(args);

			const char* c_pszToken;
			const char* c_pszLast = szNoticeBuf;

			std::string strBuff = szNoticeBuf;
			std::string strDelim = "[ENTER]";
			std::string strToken;

			while ((pos = strBuff.find(strDelim)) != std::string::npos)
			{
				strToken = strBuff.substr(0, pos);
				c_pszToken = strToken.c_str();
				d->GetCharacter()->ChatPacket(bBigFont ? CHAT_TYPE_BIG_NOTICE : CHAT_TYPE_NOTICE, "%s", c_pszToken);

				c_pszLast = strBuff.erase(0, pos + strDelim.length()).c_str();
			}
			d->GetCharacter()->ChatPacket(bBigFont ? CHAT_TYPE_BIG_NOTICE : CHAT_TYPE_NOTICE, "%s", c_pszLast);
		}
	}
}

void CInputP2P::LocaleNotice(const char* c_pData)
{
	TPacketGGLocaleNotice* p = (TPacketGGLocaleNotice*)c_pData;
	SendLCNotice(p->szNotice[0], p->bBigFont, p->szNotice[1], p->szNotice[2], p->szNotice[3], p->szNotice[4], p->szNotice[5]);
}
#endif

/// 2.
// Search
	case HEADER_GG_CHECK_AWAKENESS:
		IamAwake(d, c_pData);
		break;

// Add below
#ifdef __MULTI_LANGUAGE_SYSTEM__
	case HEADER_GG_LOCALE_NOTICE:
		LocaleNotice(c_pData);
		break;
#endif