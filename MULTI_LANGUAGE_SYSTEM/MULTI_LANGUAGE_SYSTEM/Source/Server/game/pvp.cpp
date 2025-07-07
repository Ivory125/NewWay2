/// 1.
// Search @ void CPVPManager::Insert
	snprintf(msg, sizeof(msg), LC_TEXT("%s님이 대결신청을 했습니다. 승낙하려면 대결동의를 하세요."), pkChr->GetName());

// Replace with
#ifdef __MULTI_LANGUAGE_SYSTEM__
	snprintf(msg, sizeof(msg), LC_LOCALE_TEXT("%s님이 대결신청을 했습니다. 승낙하려면 대결동의를 하세요.", pkVictim->GetDesc()->GetLanguage()), pkChr->GetName());
#else
	snprintf(msg, sizeof(msg), LC_TEXT("%s님이 대결신청을 했습니다. 승낙하려면 대결동의를 하세요."), pkChr->GetName());
#endif