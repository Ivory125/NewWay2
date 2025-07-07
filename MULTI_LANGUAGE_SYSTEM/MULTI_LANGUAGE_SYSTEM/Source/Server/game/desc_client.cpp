/// 1.
/* Search @ void CLIENT_DESC::SetPhase
	@ case PHASE_DBCLIENT:
*/
						strlcpy(pck.szHost, d->GetHostName(), sizeof(pck.szHost));
						pck.dwLoginKey = d->GetLoginKey();

// Add below
#ifdef __MULTI_LANGUAGE_SYSTEM__
						pck.bLanguage = r.bLanguage;
#endif