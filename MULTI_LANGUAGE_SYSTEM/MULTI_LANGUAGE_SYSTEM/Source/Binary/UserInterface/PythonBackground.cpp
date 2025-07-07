/// 1.
// Search @ void CPythonBackground::Initialize
	std::string stAtlasInfoFileName (LocaleService_GetLocalePath());
	stAtlasInfoFileName += "/AtlasInfo.txt";

// Replace with
#ifdef ENABLE_MULTI_LANGUAGE_SYSTEM
	const std::string stAtlasInfoFileName = "locale/common/AtlasInfo.txt";
#else
	std::string stAtlasInfoFileName (LocaleService_GetLocalePath());
	stAtlasInfoFileName += "/AtlasInfo.txt";
#endif