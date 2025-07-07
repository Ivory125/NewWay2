/// 1.
// Search @ void DBManager::SendAuthLogin
	ptod.dwBillID = pkLD->GetBillID();

// Add below
#ifdef __MULTI_LANGUAGE_SYSTEM__
	ptod.bLanguage = r.bLanguage;
#endif

/// 2.
// Search @ void DBManager::SendAuthLogin
	sys_log(0, "SendAuthLogin %s key %u", ptod.szLogin, ptod.dwID);

// Replace with
#ifdef __MULTI_LANGUAGE_SYSTEM__
	sys_log(0, "SendAuthLogin %s language %d key %u", ptod.szLogin, ptod.bLanguage, ptod.dwID);
#else
	sys_log(0, "SendAuthLogin %s key %u", ptod.szLogin, ptod.dwID);
#endif

/// 3.
/* Search @ void DBManager::AnalyzeReturnQuery
	@ case QID_AUTH_LOGIN:
*/
			char szStatus[ACCOUNT_STATUS_MAX_LEN + 1];
			DWORD dwID = 0;

// Add below
#ifdef __MULTI_LANGUAGE_SYSTEM__
			BYTE bLanguage = LOCALE_DEFAULT;
#endif

/// 4.
/* Search @ void DBManager::AnalyzeReturnQuery
	@ case QID_AUTH_LOGIN:
*/
			BYTE bNotAvail = 0;
			str_to_number(bNotAvail, row[col++]);

// Add above
#ifdef __MULTI_LANGUAGE_SYSTEM__
			if (!row[col])
			{
				sys_err("error column %d", col);
				M2_DELETE(pinfo);
				break;
			}
			str_to_number(bLanguage, row[col++]);
#endif

/// 5.
/* Search @ void DBManager::AnalyzeReturnQuery
	@ case QID_AUTH_LOGIN:
*/
			else if (strcmp(szStatus, "OK"))
			{
				LoginFailure(d, szStatus);
				sys_log(0, "   STATUS: %s", szStatus);
				M2_DELETE(pinfo);
			}

// Add below
#ifdef __MULTI_LANGUAGE_SYSTEM__
			else if (pinfo->bLanguage >= LOCALE_MAX_NUM)
			{
				LoginFailure(d, "INVLANG");
				sys_log(0, "Invalid language selected.");
				M2_DELETE(pinfo);
			}
			else if (!pinfo->bLanguage)
			{
				LoginFailure(d, "NOLANG");
				sys_log(0, "No language selected.");
				M2_DELETE(pinfo);
			}
#endif

/// 6.
/* Search @ void DBManager::AnalyzeReturnQuery
	@ case QID_AUTH_LOGIN:
*/
					snprintf(szQuery, sizeof(szQuery), "UPDATE account SET last_play=NOW() WHERE id=%u", dwID);

// Replace with
#ifdef __MULTI_LANGUAGE_SYSTEM__
					snprintf(szQuery, sizeof(szQuery), "UPDATE account SET last_play=NOW(), language=%u WHERE id=%u", pinfo->bLanguage, dwID);
#else
					snprintf(szQuery, sizeof(szQuery), "UPDATE account SET last_play=NOW() WHERE id=%u", dwID);
#endif

/// 7.
/* Search @ void DBManager::AnalyzeReturnQuery
	@ case QID_AUTH_LOGIN:
*/
				strlcpy(r.social_id, szSocialID, sizeof(r.social_id));

// Add below
#ifdef __MULTI_LANGUAGE_SYSTEM__
				r.bLanguage = pinfo->bLanguage; // bLanguage;
#endif