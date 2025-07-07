/// 1.
// Search @ void CPythonTextTail::ArrangeTextTail
		int iNameWidth, iNameHeight;
		pTextTail->pTextInstance->GetTextSize(&iNameWidth, &iNameHeight);

// Add below
#ifdef ENABLE_MULTI_LANGUAGE_SYSTEM
		CGraphicImageInstance* pLanguageInstance = pTextTail->pLanguageInstance;
#endif

/// 2.
// Search @ void CPythonTextTail::ArrangeTextTail
				else
				{
					pLevel->SetPosition(pTextTail->x - (iNameWidth / 2) - fxAdd - iTitleWidth, pTextTail->y, pTextTail->z);
				}

				pLevel->Update();

// Add below
#ifdef ENABLE_MULTI_LANGUAGE_SYSTEM
				if (pLanguageInstance)
				{
					int iLevelWidth, iLevelHeight;
					pLevel->GetTextSize(&iLevelWidth, &iLevelHeight);
					pLanguageInstance->SetPosition(pTextTail->x - (iNameWidth / 2) - iTitleWidth - iLevelWidth - pLanguageInstance->GetWidth() - 12.0f, pTextTail->y - 10.0f);
				}
#endif

/// 3.
// Search @ void CPythonTextTail::ArrangeTextTail
				else
				{
					pLevel->SetPosition(pTextTail->x - (iNameWidth / 2) - fxAdd, pTextTail->y, pTextTail->z);
				}

				pLevel->Update();

// Add below
#ifdef ENABLE_MULTI_LANGUAGE_SYSTEM
				if (pLanguageInstance)
				{
					pLanguageInstance->SetPosition(pTextTail->x - (iNameWidth / 2) - iLevelWidth - pLanguageInstance->GetWidth() - 8.0f, pTextTail->y - 10.0f);
				}
#endif

/// 4.
// Search @ void CPythonTextTail::Render
		if (pTextTail->pTitleTextInstance)
			pTextTail->pTitleTextInstance->Render();

// Add below
#ifdef ENABLE_MULTI_LANGUAGE_SYSTEM
		if (pTextTail->pLanguageInstance)
			pTextTail->pLanguageInstance->Render();
#endif

/// 5.
// Search @ void CPythonTextTail::RegisterCharacterTextTail
	pTextTail->pTitleTextInstance = NULL;
	pTextTail->pLevelTextInstance = NULL;

// Add below
#ifdef ENABLE_MULTI_LANGUAGE_SYSTEM
	pTextTail->pLanguageInstance = NULL;
#endif

/// 6.
// Search
	m_CharacterTextTailMap.insert(TTextTailMap::value_type(dwVirtualID, pTextTail));

// Add above
#ifdef ENABLE_MULTI_LANGUAGE_SYSTEM
	CGraphicImageInstance *& prLanguage = pTextTail->pLanguageInstance;

	if (!prLanguage)
	{
		BYTE bLanguage = pCharacterInstance->GetLanguage();
		if(pCharacterInstance->IsPC() && bLanguage)
		{
			std::string langName = "en";
			if (bLanguage == 1)
				langName = "en";
			else if (bLanguage == 2)
				langName = "pt";
			else if (bLanguage == 3)
				langName = "es";
			else if (bLanguage == 4)
				langName = "fr";
			else if (bLanguage == 5)
				langName = "de";
			else if (bLanguage == 6)
				langName = "ro";
			else if (bLanguage == 7)
				langName = "pl";
			else if (bLanguage == 8)
				langName = "it";
			else if (bLanguage == 9)
				langName = "cz";
			else if (bLanguage == 10)
				langName = "hu";
			else if (bLanguage == 11)
				langName = "tr";
			else
				langName = "eu"; // en

			char szFileName[256];
			sprintf(szFileName, "d:/ymir work/ui/game/flag/%s.tga", langName.c_str());

			if (CResourceManager::Instance().IsFileExist(szFileName))
			{
				CGraphicImage * pLanguageImage = (CGraphicImage *)CResourceManager::Instance().GetResourcePointer(szFileName);
				if (pLanguageImage)
				{
					prLanguage = CGraphicImageInstance::New();
					prLanguage->SetImagePointer(pLanguageImage);
				}
			}
		}
	}
#endif