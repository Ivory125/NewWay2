/// 1.
// Search
		Notice(LC_TEXT("°áÈ¥½ÄÀÌ Á¾·áµË´Ï´Ù."));
		Notice(LC_TEXT("ÀÚµ¿À¸·Î ³ª°¡°ÔµË´Ï´Ù."));

// Replace with
#ifdef __MULTI_LANGUAGE_SYSTEM__
		Notice("°áÈ¥½ÄÀÌ Á¾·áµË´Ï´Ù.");
		Notice("ÀÚµ¿À¸·Î ³ª°¡°ÔµË´Ï´Ù.");
#else
		Notice(LC_TEXT("°áÈ¥½ÄÀÌ Á¾·áµË´Ï´Ù."));
		Notice(LC_TEXT("ÀÚµ¿À¸·Î ³ª°¡°ÔµË´Ï´Ù."));
#endif

/// 2.
// Search @ struct FNotice
		void operator() (LPCHARACTER ch)
		{
			ch->ChatPacket(CHAT_TYPE_NOTICE, "%s", m_psz);
		}

// Replace with
		void operator() (LPCHARACTER ch)
		{
#ifdef __MULTI_LANGUAGE_SYSTEM__
			if (!ch)
				return;

			BYTE bLocale = ch->GetDesc() ? ch->GetDesc()->GetLanguage() : LOCALE_YMIR;
			ch->ChatPacket(CHAT_TYPE_NOTICE, LC_LOCALE_TEXT(m_psz, bLocale));
#else
			ch->ChatPacket(CHAT_TYPE_NOTICE, "%s", m_psz);
#endif
		}