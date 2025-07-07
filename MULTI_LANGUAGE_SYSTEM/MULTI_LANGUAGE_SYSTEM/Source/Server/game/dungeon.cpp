/// 1.
// Add
#ifdef __MULTI_LANGUAGE_SYSTEM__
#include "desc_manager.h"
#endif

/// 2.
// Search
void CDungeon::Notice(const char* msg)

// Replace with
#ifdef __MULTI_LANGUAGE_SYSTEM__
void CDungeon::Notice(const char* msg, ...)
#else
void CDungeon::Notice(const char* msg)
#endif

/// 3.
// Search
	FNotice f(msg);
	pMap->for_each(f);

// Replace with
#ifdef __MULTI_LANGUAGE_SYSTEM__
	const DESC_MANAGER::DESC_SET& c_ref_set = DESC_MANAGER::instance().GetClientSet();

	if (!msg)
		return;

	DESC_MANAGER::DESC_SET::const_iterator it = c_ref_set.begin();

	while (it != c_ref_set.end())
	{
		LPDESC d = *(it++);

		if (d->GetCharacter())
		{
			LPSECTREE_MAP pSecMap = SECTREE_MANAGER::instance().GetMap(d->GetCharacter()->GetMapIndex());
			if (pSecMap != pMap)
				continue;

			std::string strMsg = msg;
			const char* c_pszBuf;

			if (!strMsg.empty() && std::all_of(strMsg.begin(), strMsg.end(), ::isdigit))
			{
				DWORD dwKey = atoi(msg);
				BYTE bLanguage = (d ? d->GetLanguage() : LOCALE_YMIR);

				c_pszBuf = LC_LOCALE_QUEST_TEXT(dwKey, bLanguage);
			}
			else
			{
				c_pszBuf = msg;
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
			va_start(args, msg);
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
				d->GetCharacter()->ChatPacket(CHAT_TYPE_NOTICE, "%s", c_pszToken);

				c_pszLast = strBuff.erase(0, pos + strDelim.length()).c_str();
			}
			d->GetCharacter()->ChatPacket(CHAT_TYPE_NOTICE, "%s", c_pszLast);
		}
	}
#else
	FNotice f(msg);
	pMap->for_each(f);
#endif