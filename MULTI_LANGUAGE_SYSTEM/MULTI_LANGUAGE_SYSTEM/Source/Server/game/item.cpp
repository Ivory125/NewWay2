/// 1.
// Add to the bottom of the document
#if defined(__MULTI_LANGUAGE_SYSTEM__) && !defined(__ITEM_DROP_RENEWAL__)
const char* CItem::GetName()
{
	BYTE bLocale = (GetOwner() && GetOwner()->GetDesc()) ? GetOwner()->GetDesc()->GetLanguage() : LOCALE_YMIR;
	return m_pProto ? LC_LOCALE_ITEM_TEXT(GetVnum(), bLocale) : NULL;
}
#endif

/// 2. If you have __ITEM_DROP_RENEWAL__
#if defined(__ITEM_DROP_RENEWAL__)
const char* CItem::GetName()
{
#ifdef __MULTI_LANGUAGE_SYSTEM__
	BYTE bLocale = (GetOwner() && GetOwner()->GetDesc()) ? GetOwner()->GetDesc()->GetLanguage() : LOCALE_YMIR;
#endif

	static char szItemName[128];
	memset(szItemName, 0, sizeof(szItemName));
	if (GetProto())
	{
		int len = 0;
		switch (GetType())
		{
		case ITEM_POLYMORPH:
		{
			const DWORD dwMobVnum = GetSocket(0);
			const CMob* pMob = CMobManager::instance().Get(dwMobVnum);
			if (pMob)
#ifdef __MULTI_LANGUAGE_SYSTEM__
				len = snprintf(szItemName, sizeof(szItemName), "%s", LC_LOCALE_MOB_TEXT(dwMobVnum, bLocale));
#else
				len = snprintf(szItemName, sizeof(szItemName), "%s", pMob->m_table.szLocaleName);
#endif

			break;
		}
		case ITEM_SKILLBOOK:
		case ITEM_SKILLFORGET:
		{
			const DWORD dwSkillVnum = (GetVnum() == ITEM_SKILLBOOK_VNUM || GetVnum() == ITEM_SKILLFORGET_VNUM) ? GetSocket(0) : 0;
			const CSkillProto* pSkill = (dwSkillVnum != 0) ? CSkillManager::instance().Get(dwSkillVnum) : NULL;
			if (pSkill)
#ifdef __MULTI_LANGUAGE_SYSTEM__
				len = snprintf(szItemName, sizeof(szItemName), "%s", LC_LOCALE_SKILL_TEXT(dwSkillVnum, bLocale));
#else
				len = snprintf(szItemName, sizeof(szItemName), "%s", pSkill->szName);
#endif

			break;
		}
		}
#ifdef __MULTI_LANGUAGE_SYSTEM__
		len += snprintf(szItemName + len, sizeof(szItemName) - len, (len > 0) ? " %s" : "%s", LC_LOCALE_ITEM_TEXT(GetVnum(), bLocale));
#else
		len += snprintf(szItemName + len, sizeof(szItemName) - len, (len > 0) ? " %s" : "%s", GetProto()->szLocaleName);
#endif
	}

	return szItemName;
}
#endif