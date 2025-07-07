/// 1.
// Search @ void CClientManager::QUERY_SETUP
		strlcpy(r.passwd, "TEMP", sizeof(r.passwd));

// Add below
#ifdef __MULTI_LANGUAGE_SYSTEM__
		r.bLanguage = pck->bLanguage;
#endif

/// 2.
// Search @ void CClientManager::QUERY_AUTH_LOGIN
		strlcpy(r.passwd, "TEMP", sizeof(r.passwd));

// Add below
#ifdef __MULTI_LANGUAGE_SYSTEM__
		r.bLanguage = p->bLanguage;
#endif

/// 3.
// Search @ void CClientManager::QUERY_AUTH_LOGIN
		sys_log(0, "AUTH_LOGIN id(%u) login(%s) social_id(%s) login_key(%u), client_key(%u %u %u %u)",
			p->dwID, p->szLogin, p->szSocialID, p->dwLoginKey,
			p->adwClientKey[0], p->adwClientKey[1], p->adwClientKey[2], p->adwClientKey[3]);

// Replace with
		sys_log(0, "AUTH_LOGIN id(%u) login(%s)"
#ifdef __MULTI_LANGUAGE_SYSTEM__
			" language(%d)"
#endif
			" social_id(%s) login_key(%u), client_key(%u %u %u %u)",
			p->dwID,
			p->szLogin,
#ifdef __MULTI_LANGUAGE_SYSTEM__
			p->bLanguage,
#endif
			p->szSocialID,
			p->dwLoginKey,
			p->adwClientKey[0], p->adwClientKey[1], p->adwClientKey[2], p->adwClientKey[3]
		);

/// 4.
// Search @ void CClientManager::ProcessPackets
		case HEADER_GD_REQUEST_CHANNELSTATUS:
			RequestChannelStatus(peer, dwHandle);
			break;

// Add below
#ifdef __MULTI_LANGUAGE_SYSTEM__
		case HEADER_GD_REQUEST_CHANGE_LANGUAGE:
			ChangeLanguage((TRequestChangeLanguage*)data);
			break;
#endif

/// 5.
// Search
			else
			{
				sys_err("locale[LOCALE] = UNKNOWN(%s)", locale.szValue);
				exit(0);
			}

// Add above
#ifdef __MULTI_LANGUAGE_SYSTEM__
			else if (strcmp(locale.szValue, "europe") == 0)
			{
				sys_log(0, "locale[LOCALE] = %s", locale.szValue);

				if (g_stLocale != locale.szValue)
					sys_log(0, "Changed g_stLocale %s to %s", g_stLocale.c_str(), "latin1");

				g_stLocale = "latin1";
				g_stLocaleNameColumn = "locale_name";
			}
#endif

/// 6.
// Search @ void CClientManager::ResetLastPlayerID
		pkLD->SetLastPlayerID(0);

// Add below
#ifdef __MULTI_LANGUAGE_SYSTEM__
		pkLD->GetAccountRef().bLanguage = data->bLanguage;
#endif

/// 7.
// Add to the bottom of the document
#ifdef __MULTI_LANGUAGE_SYSTEM__
void CClientManager::ChangeLanguage(const TRequestChangeLanguage* packet)
{
	char szQuery[512];

	if (packet->bLanguage > LOCALE_YMIR&& packet->bLanguage < LOCALE_MAX_NUM)
	{
		sprintf(szQuery, "update account set `language` = %d where id = %d", packet->bLanguage, packet->dwAID);
	}
	else
	{
		sys_err("Invalid request change language (language : %d, aid : %d)", packet->bLanguage, packet->dwAID);
		return;
	}

	CDBManager::Instance().AsyncQuery(szQuery, SQL_ACCOUNT);
}
#endif