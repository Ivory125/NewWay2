/// 1.
// Search @ void LocaleService_LoadConfig
	FILE* fp = fopen(fileName, "rt");

// Add above
#ifdef ENABLE_MULTI_LANGUAGE_SYSTEM
	bool bMultiLocale = false;
	const char* szMultiLocale = "loca.cfg";
	if (_access(szMultiLocale, 0) == 0)
	{
		bMultiLocale = true;
		fileName = szMultiLocale;
	}
#endif

/// 2.
// Search @ void LocaleService_LoadConfig
			sscanf(line, "%d %d %s", &id, &code, name);
			MULTI_LOCALE_REPORT_PORT = id;

// Replace with
#ifdef ENABLE_MULTI_LANGUAGE_SYSTEM
			if (bMultiLocale)
			{
				sscanf(line, "%d %s", &code, name);
			}
			else
			{
				sscanf(line, "%d %d %s", &id, &code, name);
			}
#else
			sscanf(line, "%d %d %s", &id, &code, name);
			MULTI_LOCALE_REPORT_PORT = id;
#endif

/// 3.
// Search
void LocaleService_ForceSetLocale(const char* name, const char* localePath)

// Add above
#ifdef ENABLE_MULTI_LANGUAGE_SYSTEM
const char* LocaleService_GetLoca() { return MULTI_LOCALE_NAME;}
bool LocaleService_SaveLoca(int iCodePage, const char* szLocale)
{
	MULTI_LOCALE_CODE = iCodePage;
	strcpy(MULTI_LOCALE_NAME, szLocale);
	sprintf(MULTI_LOCALE_PATH, "locale/%s", MULTI_LOCALE_NAME);

	SetDefaultCodePage(iCodePage);

	FILE* File;

	if (NULL == (File = fopen("loca.cfg", "wt")))
		return false;

	fprintf(File, "%d %s", iCodePage, szLocale);
	fclose(File);
	return true;
}
#endif