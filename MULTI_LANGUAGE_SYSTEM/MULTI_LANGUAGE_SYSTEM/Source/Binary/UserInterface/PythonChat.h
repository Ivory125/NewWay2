/// 1.
// Search
#include "AbstractChat.h"

// Add below
#ifdef ENABLE_MULTI_LANGUAGE_SYSTEM
#include "PythonSystem.h"
#endif

/// 2.
// Search @ typedef struct SChatLine
			D3DXCOLOR aColor[CHAT_LINE_COLOR_ARRAY_MAX_NUM];
			CGraphicTextInstance Instance;

// Add below
#ifdef ENABLE_MULTI_LANGUAGE_SYSTEM
			CGraphicImageInstance pLanguage;
#endif