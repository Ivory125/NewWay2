/// 1.
// Search @ void CPVPManager::Insert
	snprintf(msg, sizeof(msg), LC_TEXT("%s���� ����û�� �߽��ϴ�. �³��Ϸ��� ��ᵿ�Ǹ� �ϼ���."), pkChr->GetName());

// Replace with
#ifdef __MULTI_LANGUAGE_SYSTEM__
	snprintf(msg, sizeof(msg), LC_LOCALE_TEXT("%s���� ����û�� �߽��ϴ�. �³��Ϸ��� ��ᵿ�Ǹ� �ϼ���.", pkVictim->GetDesc()->GetLanguage()), pkChr->GetName());
#else
	snprintf(msg, sizeof(msg), LC_TEXT("%s���� ����û�� �߽��ϴ�. �³��Ϸ��� ��ᵿ�Ǹ� �ϼ���."), pkChr->GetName());
#endif