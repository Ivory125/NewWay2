/// 1.
// Add
#ifdef __MULTI_LANGUAGE_SYSTEM__
#include "desc.h"
#endif

/// 2.
// Search @ bool NPC::OnChat
		if (AvailScript.empty())
			return false;

// Add below
#ifdef __MULTI_LANGUAGE_SYSTEM__
		BYTE bLocale = LOCALE_YMIR;
		CQuestManager& mgr = CQuestManager::instance();
		bLocale = mgr.GetCurrentCharacterPtr()->GetDesc()->GetLanguage();
#endif

/// 3.
// Search @ bool NPC::OnChat
			os << ", '" << LC_TEXT("´Ý±â") << "'";

// Replace with
#ifdef __MULTI_LANGUAGE_SYSTEM__
			os << ", '" << LC_LOCALE_TEXT("´Ý±â", bLocale) << "'";
#else
			os << ", '" << LC_TEXT("´Ý±â") << "'";
#endif