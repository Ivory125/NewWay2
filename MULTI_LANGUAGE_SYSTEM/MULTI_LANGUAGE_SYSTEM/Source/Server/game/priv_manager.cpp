/// 1.
// Search @ void CPrivManager::GiveGuildPriv
			char buf[100];
			snprintf(buf, sizeof(buf), LC_TEXT("%s ����� %s�� %d%% �����߽��ϴ�!"), g->GetName(), GetPrivName(type), value);
			SendNotice(buf);

// Replace with
#ifdef __MULTI_LANGUAGE_SYSTEM__
			SendGuildPrivNotice(LC_TEXT("%s ����� %s�� %d%% �����߽��ϴ�!"), g->GetName(), GetPrivName(type), value);
#else
			char buf[100];
			snprintf(buf, sizeof(buf), LC_TEXT("%s ����� %s�� %d%% �����߽��ϴ�!"), g->GetName(), GetPrivName(type), value);
			SendNotice(buf);
#endif

/// 2.
// Search @ void CPrivManager::GiveGuildPriv
			char buf[100];
			snprintf(buf, sizeof(buf), LC_TEXT("%s ����� %s�� �������� ���ƿԽ��ϴ�."), g->GetName(), GetPrivName(type));
			SendNotice(buf);

// Replace with
#ifdef __MULTI_LANGUAGE_SYSTEM__
			SendGuildPrivNotice(LC_TEXT("%s ����� %s�� �������� ���ƿԽ��ϴ�."), g->GetName(), GetPrivName(type));
#else
			char buf[100];
			snprintf(buf, sizeof(buf), LC_TEXT("%s ����� %s�� �������� ���ƿԽ��ϴ�."), g->GetName(), GetPrivName(type));
			SendNotice(buf);
#endif

/// 3.
// Search @ void CPrivManager::GiveEmpirePriv
		char buf[100];
		snprintf(buf, sizeof(buf), LC_TEXT("%s�� %s�� %d%% �����߽��ϴ�!"), GetEmpireName(empire), GetPrivName(type), value);

		if (empire)
			SendNotice(buf);
		else
			SendLog(buf);

// Replace with
#ifdef __MULTI_LANGUAGE_SYSTEM__
		SendPrivNotice(LC_TEXT("%s�� %s�� %d%% �����߽��ϴ�!"), GetEmpireName(empire), GetPrivName(type), value);
#else
		char buf[100];
		snprintf(buf, sizeof(buf), LC_TEXT("%s�� %s�� %d%% �����߽��ϴ�!"), GetEmpireName(empire), GetPrivName(type), value);

		if (empire)
			SendNotice(buf);
		else
			SendLog(buf);
#endif

/// 4.
// Search @ void CPrivManager::GiveEmpirePriv
		char buf[100];
		snprintf(buf, sizeof(buf), LC_TEXT("%s�� %s�� �������� ���ƿԽ��ϴ�."), GetEmpireName(empire), GetPrivName(type));

		if (empire)
			SendNotice(buf);
		else
			SendLog(buf);

// Replace with
#ifdef __MULTI_LANGUAGE_SYSTEM__
		SendPrivNotice(LC_TEXT("%s�� %s�� �������� ���ƿԽ��ϴ�."), GetEmpireName(empire), GetPrivName(type));
#else
		char buf[100];
		snprintf(buf, sizeof(buf), LC_TEXT("%s�� %s�� �������� ���ƿԽ��ϴ�."), GetEmpireName(empire), GetPrivName(type));

		if (empire)
			SendNotice(buf);
		else
			SendLog(buf);
#endif