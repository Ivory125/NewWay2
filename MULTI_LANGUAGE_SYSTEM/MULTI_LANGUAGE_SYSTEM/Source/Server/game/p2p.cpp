/// 1.
// Search @ void P2P_MANAGER::Boot
		p.bChannel = g_bChannel;

// Add below
#ifdef __MULTI_LANGUAGE_SYSTEM__
		p.bLanguage = d ? d->GetLanguage() : LOCALE_YMIR;
#endif

/// 2.
// Search @ void P2P_MANAGER::Login
		pkCCI->bEmpire = p->bEmpire;

// Add below
#ifdef __MULTI_LANGUAGE_SYSTEM__
		pkCCI->bLanguage = p->bLanguage;
#endif