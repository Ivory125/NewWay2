/// 1.
// Search
		char buf[256];
		snprintf(buf, sizeof(buf), LC_TEXT("<���> ���� ������ %d ���� �Ǿ����ϴ�"), point);

// Replace with
#ifndef __MULTI_LANGUAGE_SYSTEM__
		char buf[256];
		snprintf(buf, sizeof(buf), LC_TEXT("<���> ���� ������ %d ���� �Ǿ����ϴ�"), point);
#endif

/// 2.
// Search
			ch->ChatPacket(CHAT_TYPE_INFO, buf);

// Replace with
#ifdef __MULTI_LANGUAGE_SYSTEM__
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<���> ���� ������ %d ���� �Ǿ����ϴ�"), point);
#else
			ch->ChatPacket(CHAT_TYPE_INFO, buf);
#endif