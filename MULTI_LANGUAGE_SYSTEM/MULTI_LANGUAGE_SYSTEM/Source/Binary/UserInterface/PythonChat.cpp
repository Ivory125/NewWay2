/// 1.
// Search @ void CPythonChat::SChatLine::Delete
	pkChatLine->Instance.Destroy();

// Add below
#ifdef ENABLE_MULTI_LANGUAGE_SYSTEM
	pkChatLine->pLanguage.Destroy();
#endif

/// 2.
// Search @ CPythonChat::SChatLine::~SChatLine
	Instance.Destroy();

// Add below
#ifdef ENABLE_MULTI_LANGUAGE_SYSTEM
	pLanguage.Destroy();
#endif

/// 3.
// Search @ void CPythonChat::UpdateViewMode
		pChatLine->Instance.SetPosition(pChatSet->m_ix, pChatSet->m_iy + iHeight);

// Replace with
#ifdef ENABLE_MULTI_LANGUAGE_SYSTEM
		int iWidth = pChatSet->m_ix;
		if (pChatLine->pLanguage.GetWidth() > 0)
		{
			pChatLine->pLanguage.SetPosition(iWidth, pChatSet->m_iy + iHeight + 2);
			iWidth += pChatLine->pLanguage.GetWidth() + 3;
		}

		pChatLine->Instance.SetPosition(iWidth, pChatSet->m_iy + iHeight);
#else
		pChatLine->Instance.SetPosition(pChatSet->m_ix, pChatSet->m_iy + iHeight);
#endif

/// 4.
// Search @ void CPythonChat::UpdateEditMode
		pChatLine->Instance.SetPosition(pChatSet->m_ix, pChatSet->m_iy + iHeight);

// Replace with
#ifdef ENABLE_MULTI_LANGUAGE_SYSTEM
		int iWidth = pChatSet->m_ix;
		if (pChatLine->pLanguage.GetWidth() > 0)
		{
			pChatLine->pLanguage.SetPosition(iWidth, pChatSet->m_iy + iHeight + 2);
			iWidth += pChatLine->pLanguage.GetWidth() + 3;
		}
		pChatLine->Instance.SetPosition(iWidth, pChatSet->m_iy + iHeight);
#else
		pChatLine->Instance.SetPosition(pChatSet->m_ix, pChatSet->m_iy + iHeight);
#endif

/// 5.
// Search @ void CPythonChat::Update
		case BOARD_STATE_LOG:
			UpdateLogMode(dwID);
			break;

// Replace with
		case BOARD_STATE_LOG:
#ifdef ENABLE_MULTI_LANGUAGE_SYSTEM
		{
			UpdateLogMode(dwID);
			UpdateViewMode(dwID);
			UpdateEditMode(dwID);
		}
#else
			UpdateLogMode(dwID);
#endif
			break;

/// 6.
// Search @ void CPythonChat::Render
		CGraphicTextInstance & rInstance = (*itor)->Instance;
		rInstance.Render();

// Add below
#ifdef ENABLE_MULTI_LANGUAGE_SYSTEM
		CGraphicImageInstance& lInstance = (*itor)->pLanguage;
		lInstance.Render();
#endif

/// 7.
// Search @ void CPythonChat::AppendChat
	IAbstractApplication& rApp = IAbstractApplication::GetSingleton();
	SChatLine* pChatLine = SChatLine::New();

// Add below
#ifdef ENABLE_MULTI_LANGUAGE_SYSTEM
	std::string chatStr = c_szChat;
	std::size_t startLang = chatStr.find("|L");
	std::size_t endLang = chatStr.find("|l");

	if (startLang != std::string::npos && endLang != std::string::npos)
	{
		std::string strLanguage = chatStr.substr(startLang + 2, endLang - 2);
		char szLangName[256];
		sprintf(szLangName, "d:/ymir work/ui/game/flag/%s.tga", strLanguage.c_str());
		if (CResourceManager::Instance().IsFileExist(szLangName))
		{
			CGraphicImage* pLanguageImage = (CGraphicImage*)CResourceManager::Instance().GetResourcePointer(szLangName);
			if (pLanguageImage)
			{
				pChatLine->pLanguage.SetImagePointer(pLanguageImage);
			}
		}

		chatStr = chatStr.substr(endLang + 3, chatStr.length());
	}
#endif

/// 8.
// Search @ void CPythonChat::AppendChat
	pChatLine->Instance.SetValue(c_szChat);

// Replace with
#ifdef ENABLE_MULTI_LANGUAGE_SYSTEM
	pChatLine->Instance.SetValue(chatStr.c_str());
#else
	pChatLine->Instance.SetValue(c_szChat);
#endif