/// 1.
// Search @ void CGuildManager::DeclareWar
			char buf[256];
			snprintf(buf, sizeof(buf), LC_TEXT("%s 길드가 %s 길드에 선전포고를 하였습니다!"), TouchGuild(guild_id1)->GetName(), TouchGuild(guild_id2)->GetName());
			SendNotice(buf);

// Replace with
#ifdef __MULTI_LANGUAGE_SYSTEM__
			SendNotice(LC_TEXT("%s 길드가 %s 길드에 선전포고를 하였습니다!"), TouchGuild(guild_id1)->GetName(), TouchGuild(guild_id2)->GetName());
#else
			char buf[256];
			snprintf(buf, sizeof(buf), LC_TEXT("%s 길드가 %s 길드에 선전포고를 하였습니다!"), TouchGuild(guild_id1)->GetName(), TouchGuild(guild_id2)->GetName());
			SendNotice(buf);
#endif

/// 2.
// Search @ void CGuildManager::WaitStartWar
		char buf[256];
		snprintf(buf, sizeof(buf), LC_TEXT("%s 길드와 %s 길드가 잠시 후 전쟁을 시작합니다!"), g1->GetName(), g2->GetName());
		SendNotice(buf);

// Replace with
#ifdef __MULTI_LANGUAGE_SYSTEM__
		SendNotice(LC_TEXT("%s 길드와 %s 길드가 잠시 후 전쟁을 시작합니다!"), g1->GetName(), g2->GetName());
#else
		char buf[256];
		snprintf(buf, sizeof(buf), LC_TEXT("%s 길드와 %s 길드가 잠시 후 전쟁을 시작합니다!"), g1->GetName(), g2->GetName());
		SendNotice(buf);
#endif

/// 3.
// Search @ void CGuildManager::StartWar
	char buf[256];
	snprintf(buf, sizeof(buf), LC_TEXT("%s 길드와 %s 길드가 전쟁을 시작하였습니다!"), g1->GetName(), g2->GetName());
	SendNotice(buf);

// Replace with
#ifdef __MULTI_LANGUAGE_SYSTEM__
	SendNotice(LC_TEXT("%s 길드와 %s 길드가 전쟁을 시작하였습니다!"), g1->GetName(), g2->GetName());
#else
	char buf[256];
	snprintf(buf, sizeof(buf), LC_TEXT("%s 길드와 %s 길드가 전쟁을 시작하였습니다!"), g1->GetName(), g2->GetName());
	SendNotice(buf);
#endif

/// 4.
// Search @ void SendGuildWarOverNotice
		char buf[256];

		if (bDraw)
		{
			snprintf(buf, sizeof(buf), LC_TEXT("%s 길드와 %s 길드 사이의 전쟁이 무승부로 끝났습니다."), g1->GetName(), g2->GetName());
		}
		else
		{
			if (g1->GetWarScoreAgainstTo(g2->GetID()) > g2->GetWarScoreAgainstTo(g1->GetID()))
			{
				snprintf(buf, sizeof(buf), LC_TEXT("%s 길드가 %s 길드와의 전쟁에서 승리 했습니다."), g1->GetName(), g2->GetName());
			}
			else
			{
				snprintf(buf, sizeof(buf), LC_TEXT("%s 길드가 %s 길드와의 전쟁에서 승리 했습니다."), g2->GetName(), g1->GetName());
			}
		}

		SendNotice(buf);

// Replace with
#ifdef __MULTI_LANGUAGE_SYSTEM__
		if (bDraw)
		{
			SendNotice(LC_TEXT("%s 길드와 %s 길드 사이의 전쟁이 무승부로 끝났습니다."), g1->GetName(), g2->GetName());
		}
		else
		{
			if (g1->GetWarScoreAgainstTo(g2->GetID()) > g2->GetWarScoreAgainstTo(g1->GetID()))
				SendNotice(LC_TEXT("%s 길드가 %s 길드와의 전쟁에서 승리 했습니다."), g1->GetName(), g2->GetName());
			else
				SendNotice(LC_TEXT("%s 길드가 %s 길드와의 전쟁에서 승리 했습니다."), g2->GetName(), g1->GetName());
		}
#else
		char buf[256];

		if (bDraw)
		{
			snprintf(buf, sizeof(buf), LC_TEXT("%s 길드와 %s 길드 사이의 전쟁이 무승부로 끝났습니다."), g1->GetName(), g2->GetName());
		}
		else
		{
			if (g1->GetWarScoreAgainstTo(g2->GetID()) > g2->GetWarScoreAgainstTo(g1->GetID()))
			{
				snprintf(buf, sizeof(buf), LC_TEXT("%s 길드가 %s 길드와의 전쟁에서 승리 했습니다."), g1->GetName(), g2->GetName());
			}
			else
			{
				snprintf(buf, sizeof(buf), LC_TEXT("%s 길드가 %s 길드와의 전쟁에서 승리 했습니다."), g2->GetName(), g1->GetName());
			}
		}

		SendNotice(buf);
#endif

/// 5.
// Search @ void CGuildManager::CancelWar
		char buf[256 + 1];
		snprintf(buf, sizeof(buf), LC_TEXT("%s 길드와 %s 길드 사이의 전쟁이 취소되었습니다."), g1->GetName(), g2->GetName());
		SendNotice(buf);

// Replace with
#ifdef __MULTI_LANGUAGE_SYSTEM__
		SendNotice(LC_TEXT("%s 길드와 %s 길드 사이의 전쟁이 취소되었습니다."), g1->GetName(), g2->GetName());
#else
		char buf[256 + 1];
		snprintf(buf, sizeof(buf), LC_TEXT("%s 길드와 %s 길드 사이의 전쟁이 취소되었습니다."), g1->GetName(), g2->GetName());
		SendNotice(buf);
#endif