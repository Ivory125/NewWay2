/// 1.
// Search @ const char* CHARACTER::GetName()
	return m_stName.empty() ? (m_pkMobData ? m_pkMobData->m_table.szLocaleName : "") : m_stName.c_str();

// Replace with
#ifdef __MULTI_LANGUAGE_SYSTEM__
	BYTE bLocale = GetDesc() ? GetDesc()->GetLanguage() : LOCALE_YMIR;
	return m_stName.empty() ? (m_pkMobData ? LC_LOCALE_MOB_TEXT(GetRaceNum(), bLocale) : "") : m_stName.c_str();
#else
	return m_stName.empty() ? (m_pkMobData ? m_pkMobData->m_table.szLocaleName : "") : m_stName.c_str();
#endif

/// 2.
// Search @ void CHARACTER::EncodeInsertPacket
		strlcpy(addPacket.name, GetName(), sizeof(addPacket.name));

// Add above
#ifdef __MULTI_LANGUAGE_SYSTEM__
		addPacket.bLanguage = (IsPC() && GetDesc()) ? GetDesc()->GetLanguage() : 0;
#endif

/// 3.
// Search @ void CHARACTER::UpdatePacket
	pack.bPKMode = m_bPKMode;
	pack.dwMountVnum = GetMountVnum();

// Add below
#ifdef __MULTI_LANGUAGE_SYSTEM__
	pack.bLanguage = (IsPC() && GetDesc()) ? GetDesc()->GetLanguage() : 0;
#endif

/// 4.
// Search @ void CHARACTER::ChatPacket
	char chatbuf[CHAT_MAX_LEN + 1];
	va_list args;

// Add above
#ifdef __MULTI_LANGUAGE_SYSTEM__
	const char* localeFormat;
	if (type != CHAT_TYPE_COMMAND)
		localeFormat = LC_LOCALE_TEXT(format, d->GetLanguage());
	else
		localeFormat = format;

	if (!localeFormat)
		return;
#endif

/// 5.
// Search @ void CHARACTER::ChatPacket
	va_start(args, format);
	int len = vsnprintf(chatbuf, sizeof(chatbuf), format, args);
	va_end(args);

// Replace with
#ifdef __MULTI_LANGUAGE_SYSTEM__
	va_start(args, format);
	int len = vsnprintf(chatbuf, sizeof(chatbuf), localeFormat, args);
	va_end(args);
#else
	va_start(args, format);
	int len = vsnprintf(chatbuf, sizeof(chatbuf), format, args);
	va_end(args);
#endif

/// 6.
// Search @ void CHARACTER::WarpEnd
		p.bChannel = g_bChannel;

// Add below
#ifdef __MULTI_LANGUAGE_SYSTEM__
		p.bLanguage = GetDesc() ? GetDesc()->GetLanguage() : LOCALE_YMIR;
#endif

/// 7.
// Add to the bottom of the document
#ifdef __MULTI_LANGUAGE_SYSTEM__
bool CHARACTER::ChangeLanguage(BYTE bLanguage)
{
	if (!IsPC())
		return false;

	if (!CanWarp())
		return false;

	TPacketChangeLanguage packet;
	packet.bHeader = HEADER_GC_REQUEST_CHANGE_LANGUAGE;
	packet.bLanguage = bLanguage;
	GetDesc()->Packet(&packet, sizeof(packet));

	GetDesc()->SetLanguage(bLanguage);
	UpdatePacket();

	char buf[256];
	snprintf(buf, sizeof(buf), "%s Language %d", GetName(), bLanguage);
	LogManager::instance().CharLog(this, 0, "CHANGE_LANGUAGE", buf);

	return true;
}
#endif