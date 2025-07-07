/// 1.
// Search @ void CClientManager::QUERY_LOGIN_BY_KEY
	strlcpy(pkTab->status, "OK", sizeof(pkTab->status));

// Add below
#ifdef __MULTI_LANGUAGE_SYSTEM__
	pkTab->bLanguage = r.bLanguage;
#endif

/// 2.
// Search @ TAccountTable* CreateAccountTableFromRes
	str_to_number(pkTab->bEmpire, row[col++]);

// Add below
#ifdef __MULTI_LANGUAGE_SYSTEM__
	str_to_number(pkTab->bLanguage, row[col++]);
#endif