/// 1.
// Search
void SendNotice(const char* c_pszBuf)

// Add above
#ifdef __MULTI_LANGUAGE_SYSTEM__
void SendLocaleNotice(const char* c_pszNotice, bool bBigFont, ...)
{
	const DESC_MANAGER::DESC_SET& c_ref_set = DESC_MANAGER::instance().GetClientSet();
	DESC_MANAGER::DESC_SET::const_iterator it = c_ref_set.begin();

	while (it != c_ref_set.end())
	{
		LPDESC d = *(it++);
		if (d->GetCharacter())
		{
			std::string strMsg = c_pszNotice;
			const char* c_pszBuf;

			if (!strMsg.empty() && std::all_of(strMsg.begin(), strMsg.end(), ::isdigit))
			{
				DWORD dwKey = atoi(c_pszNotice);
				BYTE bLanguage = (d ? d->GetLanguage() : LOCALE_YMIR);

				c_pszBuf = LC_LOCALE_QUEST_TEXT(dwKey, bLanguage);
			}
			else
			{
				c_pszBuf = c_pszNotice;
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

void SendGuildPrivNotice(const char* c_pszBuf, const char* c_pszGuildName, const char* c_pszPrivName, int iValue)
{
	const DESC_MANAGER::DESC_SET& c_ref_set = DESC_MANAGER::instance().GetClientSet();

	if (!c_pszBuf)
		return;

	DESC_MANAGER::DESC_SET::const_iterator it = c_ref_set.begin();

	while (it != c_ref_set.end())
	{
		LPDESC d = *(it++);

		if (d->GetCharacter())
		{
			const char* c_pszPrivBuf = LC_LOCALE_TEXT(c_pszPrivName, d->GetLanguage());
			if (iValue != 0)
				TransNotice(d, c_pszBuf, c_pszGuildName, c_pszPrivBuf, iValue);
			else
				TransNotice(d, c_pszBuf, c_pszGuildName, c_pszPrivBuf);
		}
	}
}

void SendPrivNotice(const char* c_pszBuf, const char* c_pszEmpireName, const char* c_pszPrivName, int iValue)
{
	const DESC_MANAGER::DESC_SET& c_ref_set = DESC_MANAGER::instance().GetClientSet();

	if (!c_pszBuf)
		return;

	DESC_MANAGER::DESC_SET::const_iterator it = c_ref_set.begin();

	while (it != c_ref_set.end())
	{
		LPDESC d = *(it++);

		if (d->GetCharacter())
		{
			const char* c_pszEmpireBuf = LC_LOCALE_TEXT(c_pszEmpireName, d->GetLanguage());
			const char* c_pszPrivBuf = LC_LOCALE_TEXT(c_pszPrivName, d->GetLanguage());
			if (iValue != 0)
				TransNotice(d, c_pszBuf, c_pszEmpireBuf, c_pszPrivBuf, iValue);
			else
				TransNotice(d, c_pszBuf, c_pszEmpireBuf, c_pszPrivBuf);
		}
	}
}

void TransNotice(LPDESC pkDesc, const char* c_pszBuf, ...)
{
	if (!pkDesc)
		return;

	if (pkDesc->GetCharacter())
	{
		if (!c_pszBuf)
			return;

		const char* c_szLocaleFormat = LC_LOCALE_TEXT(c_pszBuf, pkDesc->GetLanguage());

		char szChatBuf[CHAT_MAX_LEN + 1];
		va_list args;

		va_start(args, c_pszBuf);
		int len = vsnprintf(szChatBuf, sizeof(szChatBuf), c_szLocaleFormat, args);
		va_end(args);

		pkDesc->GetCharacter()->ChatPacket(CHAT_TYPE_NOTICE, "%s", szChatBuf);
	}
}
#endif

/// 2.
// Search
void SendNotice(const char* c_pszBuf)

// Replace with
#ifdef __MULTI_LANGUAGE_SYSTEM__
void SendNotice(const char* c_pszBuf, ...)
#else
void SendNotice(const char* c_pszBuf)
#endif

/// 3.
// Search @ void SendNotice
	std::for_each(c_ref_set.begin(), c_ref_set.end(), notice_packet_func(c_pszBuf));

// Replace with
#ifdef __MULTI_LANGUAGE_SYSTEM__
	if (!c_pszBuf)
		return;

	DESC_MANAGER::DESC_SET::const_iterator it = c_ref_set.begin();

	while (it != c_ref_set.end())
	{
		LPDESC d = *(it++);

		if (d->GetCharacter())
		{
			const char* c_szLocaleFormat = LC_LOCALE_TEXT(c_pszBuf, d->GetLanguage());

			char szChatBuf[CHAT_MAX_LEN + 1];
			va_list args;

			va_start(args, c_pszBuf);
			int len = vsnprintf(szChatBuf, sizeof(szChatBuf), c_szLocaleFormat, args);
			va_end(args);

			d->GetCharacter()->ChatPacket(CHAT_TYPE_NOTICE, "%s", szChatBuf);
		}
	}
#else
	std::for_each(c_ref_set.begin(), c_ref_set.end(), notice_packet_func(c_pszBuf));
#endif

/// 4.
// Search
void SendNoticeMap(const char* c_pszBuf, int nMapIndex, bool bBigFont)

// Replace with
#ifdef __MULTI_LANGUAGE_SYSTEM__
void SendNoticeMap(const char* c_pszBuf, int nMapIndex, bool bBigFont, ...)
#else
void SendNoticeMap(const char* c_pszBuf, int nMapIndex, bool bBigFont)
#endif

/// 5.
// Search @ void SendNoticeMap
	std::for_each(c_ref_set.begin(), c_ref_set.end(), notice_map_packet_func(c_pszBuf, nMapIndex, bBigFont));

// Replace with
#ifdef __MULTI_LANGUAGE_SYSTEM__
	if (!c_pszBuf)
		return;

	DESC_MANAGER::DESC_SET::const_iterator it = c_ref_set.begin();

	while (it != c_ref_set.end())
	{
		LPDESC d = *(it++);

		if (d->GetCharacter())
		{
			if (d->GetCharacter()->GetMapIndex() != nMapIndex)
				continue;

			const char* c_szLocaleFormat = LC_LOCALE_TEXT(c_pszBuf, d->GetLanguage());

			char szChatBuf[CHAT_MAX_LEN + 1];
			va_list args;

			va_start(args, bBigFont);
			int len = vsnprintf(szChatBuf, sizeof(szChatBuf), c_szLocaleFormat, args);
			va_end(args);

			d->GetCharacter()->ChatPacket(bBigFont ? CHAT_TYPE_BIG_NOTICE : CHAT_TYPE_NOTICE, "%s", szChatBuf);
		}
	}
#else
	std::for_each(c_ref_set.begin(), c_ref_set.end(), notice_map_packet_func(c_pszBuf, nMapIndex, bBigFont));
#endif