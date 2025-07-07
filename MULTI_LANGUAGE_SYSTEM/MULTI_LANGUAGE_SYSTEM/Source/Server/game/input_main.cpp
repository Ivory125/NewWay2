/// 1.
// Search @ void SendBlockChatInfo
	if (hour > 0 && min > 0)
		snprintf(buf, sizeof(buf), LC_TEXT("%d 시간 %d 분 %d 초 동안 채팅금지 상태입니다"), hour, min, sec);
	else if (hour > 0 && min == 0)
		snprintf(buf, sizeof(buf), LC_TEXT("%d 시간 %d 초 동안 채팅금지 상태입니다"), hour, sec);
	else if (hour == 0 && min > 0)
		snprintf(buf, sizeof(buf), LC_TEXT("%d 분 %d 초 동안 채팅금지 상태입니다"), min, sec);
	else
		snprintf(buf, sizeof(buf), LC_TEXT("%d 초 동안 채팅금지 상태입니다"), sec);

	ch->ChatPacket(CHAT_TYPE_INFO, buf);

// Replace with
#ifdef __MULTI_LANGUAGE_SYSTEM__
	if (hour > 0 && min > 0)
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("%d 시간 %d 분 %d 초 동안 채팅금지 상태입니다"), hour, min, sec);
	else if (hour > 0 && min == 0)
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("%d 시간 %d 초 동안 채팅금지 상태입니다"), hour, sec);
	else if (hour == 0 && min > 0)
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("%d 분 %d 초 동안 채팅금지 상태입니다"), min, sec);
	else
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("%d 초 동안 채팅금지 상태입니다"), sec);
#else
	if (hour > 0 && min > 0)
		snprintf(buf, sizeof(buf), LC_TEXT("%d 시간 %d 분 %d 초 동안 채팅금지 상태입니다"), hour, min, sec);
	else if (hour > 0 && min == 0)
		snprintf(buf, sizeof(buf), LC_TEXT("%d 시간 %d 초 동안 채팅금지 상태입니다"), hour, sec);
	else if (hour == 0 && min > 0)
		snprintf(buf, sizeof(buf), LC_TEXT("%d 분 %d 초 동안 채팅금지 상태입니다"), min, sec);
	else
		snprintf(buf, sizeof(buf), LC_TEXT("%d 초 동안 채팅금지 상태입니다"), sec);

	ch->ChatPacket(CHAT_TYPE_INFO, buf);
#endif

/// 2.
// Search @ int CInputMain::Whisper
								len = snprintf(buf, sizeof(buf), LC_TEXT("%s 아이템이 필요합니다"), pTable->szLocaleName);

// Replace with
#ifdef __MULTI_LANGUAGE_SYSTEM__
							{
								BYTE bLocale = ch->GetDesc()->GetLanguage();
								len = snprintf(buf, sizeof(buf), LC_TEXT("%s 아이템이 필요합니다"), LC_LOCALE_ITEM_TEXT(ITEM_PRISM, bLocale));
							}
#else
								len = snprintf(buf, sizeof(buf), LC_TEXT("%s 아이템이 필요합니다"), pTable->szLocaleName);
#endif

/// 3.
// Search @ int CInputMain::Chat
					ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("%s 아이템이 필요합니다"), pTable->szLocaleName);

// Replace with
#ifdef __MULTI_LANGUAGE_SYSTEM__
				{
					BYTE bLocale = ch->GetDesc()->GetLanguage();
					ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("%s 아이템이 필요합니다"), LC_LOCALE_ITEM_TEXT(ITEM_PRISM, bLocale));
				}
#else
					ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("%s 아이템이 필요합니다"), pTable->szLocaleName);
#endif

/// 4.
// Search
	int len = snprintf(chatbuf, sizeof(chatbuf), "%s : %s", ch->GetName(), buf);

// Replace with
#ifdef __MULTI_LANGUAGE_SYSTEM__
	int len = snprintf(chatbuf, sizeof(chatbuf), "|L%s|l %s : %s", LC_LOCALE(ch->GetDesc()->GetLanguage()), ch->GetName(), buf);
#else
	int len = snprintf(chatbuf, sizeof(chatbuf), "%s : %s", ch->GetName(), buf);
#endif

/// 5.
// Search
int CInputMain::Analyze(LPDESC d, BYTE bHeader, const char* c_pData)

// Add above
#ifdef __MULTI_LANGUAGE_SYSTEM__
void CInputMain::ChangeLanguage(LPCHARACTER ch, BYTE bLanguage)
{
	if (!ch)
		return;

	if (!ch->GetDesc())
		return;

	BYTE bCurrentLanguage = ch->GetDesc()->GetLanguage();

	if (bCurrentLanguage == bLanguage)
		return;

	if (bLanguage > LOCALE_YMIR && bLanguage < LOCALE_MAX_NUM)
	{
		TRequestChangeLanguage packet;
		packet.dwAID = ch->GetDesc()->GetAccountTable().id;
		packet.bLanguage = bLanguage;

		db_clientdesc->DBPacketHeader(HEADER_GD_REQUEST_CHANGE_LANGUAGE, 0, sizeof(TRequestChangeLanguage));
		db_clientdesc->Packet(&packet, sizeof(packet));

		ch->ChangeLanguage(bLanguage);
	}
}
#endif

#ifdef __EXTENDED_WHISPER_DETAILS__
void CInputMain::WhisperDetails(LPCHARACTER ch, const char* c_pData)
{
	TPacketCGWhisperDetails* CGWhisperDetails = (TPacketCGWhisperDetails*)c_pData;

	if (!*CGWhisperDetails->name)
		return;

	TPacketGCWhisperDetails GCWhisperDetails;
	GCWhisperDetails.header = HEADER_GC_WHISPER_DETAILS;
	strncpy(GCWhisperDetails.name, CGWhisperDetails->name, sizeof(GCWhisperDetails.name) - 1);

	BYTE bLanguage = LOCALE_DEFAULT;

	LPCHARACTER pkChr = CHARACTER_MANAGER::instance().FindPC(CGWhisperDetails->name);

	if (!pkChr)
	{
		LPDESC pkDesc = NULL;
		CCI* pkCCI = P2P_MANAGER::instance().Find(CGWhisperDetails->name);

		if (pkCCI)
		{
			pkDesc = pkCCI->pkDesc;
			if (pkDesc)
				bLanguage = pkCCI->bLanguage;
		}
	}
	else
	{
		if (pkChr->GetDesc())
			bLanguage = pkChr->GetDesc()->GetLanguage();
	}

	GCWhisperDetails.bLanguage = bLanguage;
	ch->GetDesc()->Packet(&GCWhisperDetails, sizeof(GCWhisperDetails));
}
#endif

/// 6.
// Search
	case HEADER_CG_DRAGON_SOUL_REFINE:
	{
		TPacketCGDragonSoulRefine* p = reinterpret_cast <TPacketCGDragonSoulRefine*>((void*)c_pData);
		switch (p->bSubType)
		{
		case DS_SUB_HEADER_CLOSE:
			ch->DragonSoul_RefineWindow_Close();
			break;
		case DS_SUB_HEADER_DO_REFINE_GRADE:
		{
			DSManager::instance().DoRefineGrade(ch, p->ItemGrid);
		}
		break;
		case DS_SUB_HEADER_DO_REFINE_STEP:
		{
			DSManager::instance().DoRefineStep(ch, p->ItemGrid);
		}
		break;
		case DS_SUB_HEADER_DO_REFINE_STRENGTH:
		{
			DSManager::instance().DoRefineStrength(ch, p->ItemGrid);
		}
		break;
		}
	}
	break;

// Add below
#ifdef __MULTI_LANGUAGE_SYSTEM__
	case HEADER_CG_CHANGE_LANGUAGE:
	{
		TPacketChangeLanguage* p = reinterpret_cast <TPacketChangeLanguage*>((void*)c_pData);
		ChangeLanguage(ch, p->bLanguage);
	}
	break;
#endif

#ifdef __EXTENDED_WHISPER_DETAILS__
	case HEADER_CG_WHISPER_DETAILS:
		WhisperDetails(ch, c_pData);
		break;
#endif