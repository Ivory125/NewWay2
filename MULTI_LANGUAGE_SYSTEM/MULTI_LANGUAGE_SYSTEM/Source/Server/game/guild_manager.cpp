/// 1.
// Search @ void CGuildManager::DeclareWar
			char buf[256];
			snprintf(buf, sizeof(buf), LC_TEXT("%s ��尡 %s ��忡 �������� �Ͽ����ϴ�!"), TouchGuild(guild_id1)->GetName(), TouchGuild(guild_id2)->GetName());
			SendNotice(buf);

// Replace with
#ifdef __MULTI_LANGUAGE_SYSTEM__
			SendNotice(LC_TEXT("%s ��尡 %s ��忡 �������� �Ͽ����ϴ�!"), TouchGuild(guild_id1)->GetName(), TouchGuild(guild_id2)->GetName());
#else
			char buf[256];
			snprintf(buf, sizeof(buf), LC_TEXT("%s ��尡 %s ��忡 �������� �Ͽ����ϴ�!"), TouchGuild(guild_id1)->GetName(), TouchGuild(guild_id2)->GetName());
			SendNotice(buf);
#endif

/// 2.
// Search @ void CGuildManager::WaitStartWar
		char buf[256];
		snprintf(buf, sizeof(buf), LC_TEXT("%s ���� %s ��尡 ��� �� ������ �����մϴ�!"), g1->GetName(), g2->GetName());
		SendNotice(buf);

// Replace with
#ifdef __MULTI_LANGUAGE_SYSTEM__
		SendNotice(LC_TEXT("%s ���� %s ��尡 ��� �� ������ �����մϴ�!"), g1->GetName(), g2->GetName());
#else
		char buf[256];
		snprintf(buf, sizeof(buf), LC_TEXT("%s ���� %s ��尡 ��� �� ������ �����մϴ�!"), g1->GetName(), g2->GetName());
		SendNotice(buf);
#endif

/// 3.
// Search @ void CGuildManager::StartWar
	char buf[256];
	snprintf(buf, sizeof(buf), LC_TEXT("%s ���� %s ��尡 ������ �����Ͽ����ϴ�!"), g1->GetName(), g2->GetName());
	SendNotice(buf);

// Replace with
#ifdef __MULTI_LANGUAGE_SYSTEM__
	SendNotice(LC_TEXT("%s ���� %s ��尡 ������ �����Ͽ����ϴ�!"), g1->GetName(), g2->GetName());
#else
	char buf[256];
	snprintf(buf, sizeof(buf), LC_TEXT("%s ���� %s ��尡 ������ �����Ͽ����ϴ�!"), g1->GetName(), g2->GetName());
	SendNotice(buf);
#endif

/// 4.
// Search @ void SendGuildWarOverNotice
		char buf[256];

		if (bDraw)
		{
			snprintf(buf, sizeof(buf), LC_TEXT("%s ���� %s ��� ������ ������ ���ºη� �������ϴ�."), g1->GetName(), g2->GetName());
		}
		else
		{
			if (g1->GetWarScoreAgainstTo(g2->GetID()) > g2->GetWarScoreAgainstTo(g1->GetID()))
			{
				snprintf(buf, sizeof(buf), LC_TEXT("%s ��尡 %s ������ ���￡�� �¸� �߽��ϴ�."), g1->GetName(), g2->GetName());
			}
			else
			{
				snprintf(buf, sizeof(buf), LC_TEXT("%s ��尡 %s ������ ���￡�� �¸� �߽��ϴ�."), g2->GetName(), g1->GetName());
			}
		}

		SendNotice(buf);

// Replace with
#ifdef __MULTI_LANGUAGE_SYSTEM__
		if (bDraw)
		{
			SendNotice(LC_TEXT("%s ���� %s ��� ������ ������ ���ºη� �������ϴ�."), g1->GetName(), g2->GetName());
		}
		else
		{
			if (g1->GetWarScoreAgainstTo(g2->GetID()) > g2->GetWarScoreAgainstTo(g1->GetID()))
				SendNotice(LC_TEXT("%s ��尡 %s ������ ���￡�� �¸� �߽��ϴ�."), g1->GetName(), g2->GetName());
			else
				SendNotice(LC_TEXT("%s ��尡 %s ������ ���￡�� �¸� �߽��ϴ�."), g2->GetName(), g1->GetName());
		}
#else
		char buf[256];

		if (bDraw)
		{
			snprintf(buf, sizeof(buf), LC_TEXT("%s ���� %s ��� ������ ������ ���ºη� �������ϴ�."), g1->GetName(), g2->GetName());
		}
		else
		{
			if (g1->GetWarScoreAgainstTo(g2->GetID()) > g2->GetWarScoreAgainstTo(g1->GetID()))
			{
				snprintf(buf, sizeof(buf), LC_TEXT("%s ��尡 %s ������ ���￡�� �¸� �߽��ϴ�."), g1->GetName(), g2->GetName());
			}
			else
			{
				snprintf(buf, sizeof(buf), LC_TEXT("%s ��尡 %s ������ ���￡�� �¸� �߽��ϴ�."), g2->GetName(), g1->GetName());
			}
		}

		SendNotice(buf);
#endif

/// 5.
// Search @ void CGuildManager::CancelWar
		char buf[256 + 1];
		snprintf(buf, sizeof(buf), LC_TEXT("%s ���� %s ��� ������ ������ ��ҵǾ����ϴ�."), g1->GetName(), g2->GetName());
		SendNotice(buf);

// Replace with
#ifdef __MULTI_LANGUAGE_SYSTEM__
		SendNotice(LC_TEXT("%s ���� %s ��� ������ ������ ��ҵǾ����ϴ�."), g1->GetName(), g2->GetName());
#else
		char buf[256 + 1];
		snprintf(buf, sizeof(buf), LC_TEXT("%s ���� %s ��� ������ ������ ��ҵǾ����ϴ�."), g1->GetName(), g2->GetName());
		SendNotice(buf);
#endif