/// 1.
// Search @ EVENTFUNC(shutdown_event)
		char buf[64];
		snprintf(buf, sizeof(buf), LC_TEXT("�˴ٿ��� %d�� ���ҽ��ϴ�."), *pSec);
		SendNotice(buf);

// Replace with
#ifdef __MULTI_LANGUAGE_SYSTEM__
		SendNotice(LC_TEXT("�˴ٿ��� %d�� ���ҽ��ϴ�."), *pSec);
#else
		char buf[64];
		snprintf(buf, sizeof(buf), LC_TEXT("�˴ٿ��� %d�� ���ҽ��ϴ�."), *pSec);
		SendNotice(buf);
#endif

/// 2.
// Search @ void Shutdown
	char buf[64];
	snprintf(buf, sizeof(buf), LC_TEXT("%d�� �� ������ �˴ٿ� �˴ϴ�."), iSec);

	SendNotice(buf);

// Replace with
#ifdef __MULTI_LANGUAGE_SYSTEM__
	SendNotice(LC_TEXT("%d�� �� ������ �˴ٿ� �˴ϴ�."), iSec);
#else
	char buf[64];
	snprintf(buf, sizeof(buf), LC_TEXT("%d�� �� ������ �˴ٿ� �˴ϴ�."), iSec);

	SendNotice(buf);
#endif

/// 3.
// Search @ EVENTFUNC(timed_event)
				TPacketNeedLoginLogInfo acc_info;
				acc_info.dwPlayerID = ch->GetDesc()->GetAccountTable().id;

// Add below
#ifdef __MULTI_LANGUAGE_SYSTEM__
				acc_info.bLanguage = ch->GetDesc()->GetAccountTable().bLanguage;
#endif

/// 4.
// Search
		case LC_CANADA: country_code[0] = 'c'; country_code[1] = 'a'; country_code[2] = '\0'; break;

// Add below
#ifdef __MULTI_LANGUAGE_SYSTEM__
		case LC_EUROPE: country_code[0] = 'e'; country_code[1] = 'u'; country_code[2] = '\0'; break;
#endif

/// 5.
// Search @ ACMD(do_in_game_mall)
				country_code[0] = 'd'; country_code[1] = 'e'; country_code[2] = '\0';

// Replace with
#ifdef __MULTI_LANGUAGE_SYSTEM__
				country_code[0] = 'p'; country_code[1] = 't'; country_code[2] = '\0';
#else
				country_code[0] = 'd'; country_code[1] = 'e'; country_code[2] = '\0';
#endif