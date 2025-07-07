/// 1.
// Search
void LocaleService_ForceSetLocale(const char* name, const char* localePath);

// Add above
#ifdef ENABLE_MULTI_LANGUAGE_SYSTEM
const char* LocaleService_GetLoca();
bool LocaleService_SaveLoca(int iCodePage, const char* szLocale);
#endif