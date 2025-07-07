/// 1.
// Search
void initapp()

// Add above
#ifdef ENABLE_MULTI_LANGUAGE_SYSTEM
PyObject* appSetLoca(PyObject* poSelf, PyObject* poArgs)
{
	int iCodePage;
	if (!PyTuple_GetInteger(poArgs, 0, &iCodePage))
		return Py_BuildException();

	char* szLocale;
	if (!PyTuple_GetString(poArgs, 1, &szLocale))
		return Py_BuildException();

	return Py_BuildValue("i", LocaleService_SaveLoca(iCodePage, szLocale));
}

PyObject* appGetLoca(PyObject* poSelf, PyObject* poArgs)
{
	return Py_BuildValue("s", LocaleService_GetLoca());
}
#endif

/// 2.
// Search
		{ NULL, NULL },

// Add above
#ifdef ENABLE_MULTI_LANGUAGE_SYSTEM
		{ "SetLoca", appSetLoca, METH_VARARGS },
		{ "GetLoca", appGetLoca, METH_VARARGS },
#endif

/// 3.
// Add to the bottom of the document above }
#ifdef ENABLE_MULTI_LANGUAGE_SYSTEM
	PyModule_AddIntConstant(poModule, "ENABLE_MULTI_LANGUAGE_SYSTEM", 1);
	PyModule_AddIntConstant(poModule, "LOCALE_DEFAULT", CPythonApplication::LOCALE_DEFAULT);
	PyModule_AddIntConstant(poModule, "LOCALE_EN", CPythonApplication::LOCALE_EN);
	PyModule_AddIntConstant(poModule, "LOCALE_PT", CPythonApplication::LOCALE_PT);
	PyModule_AddIntConstant(poModule, "LOCALE_ES", CPythonApplication::LOCALE_ES);
	PyModule_AddIntConstant(poModule, "LOCALE_FR", CPythonApplication::LOCALE_FR);
	PyModule_AddIntConstant(poModule, "LOCALE_DE", CPythonApplication::LOCALE_DE);
	PyModule_AddIntConstant(poModule, "LOCALE_RO", CPythonApplication::LOCALE_RO);
	PyModule_AddIntConstant(poModule, "LOCALE_PL", CPythonApplication::LOCALE_PL);
	PyModule_AddIntConstant(poModule, "LOCALE_IT", CPythonApplication::LOCALE_IT);
	PyModule_AddIntConstant(poModule, "LOCALE_CZ", CPythonApplication::LOCALE_CZ);
	PyModule_AddIntConstant(poModule, "LOCALE_HU", CPythonApplication::LOCALE_HU);
	PyModule_AddIntConstant(poModule, "LOCALE_TR", CPythonApplication::LOCALE_TR);
	PyModule_AddIntConstant(poModule, "LOCALE_MAX_NUM", CPythonApplication::LOCALE_MAX_NUM);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_MULTI_LANGUAGE_SYSTEM", 0);
#endif

#ifdef ENABLE_EXTENDED_WHISPER_DETAILS
	PyModule_AddIntConstant(poModule, "ENABLE_EXTENDED_WHISPER_DETAILS", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_EXTENDED_WHISPER_DETAILS", 0);
#endif