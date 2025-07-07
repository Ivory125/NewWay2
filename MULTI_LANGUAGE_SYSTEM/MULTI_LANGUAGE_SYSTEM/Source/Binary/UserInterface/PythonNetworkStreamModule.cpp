/// 1.
// Search
void initnet()

// Add above
#ifdef ENABLE_MULTI_LANGUAGE_SYSTEM
PyObject* netSendChangeLanguagePacket(PyObject* poSelf, PyObject* poArgs)
{
	int iLanguage;
	if (!PyTuple_GetInteger(poArgs, 0, &iLanguage))
		return Py_BuildException();

	CPythonNetworkStream::Instance().SendChangeLanguagePacket(iLanguage);
	return Py_BuildNone();
}

PyObject* netGetLanguage(PyObject* poSelf, PyObject* poArgs)
{
	CPythonNetworkStream& rkNetStream = CPythonNetworkStream::Instance();
	return Py_BuildValue("i", rkNetStream.GetLanguage());
}

PyObject* netSetLanguage(PyObject* poSelf, PyObject* poArgs)
{
	BYTE bLanguage;
	if (!PyTuple_GetInteger(poArgs, 0, &bLanguage))
		return Py_BuildException();

	CPythonNetworkStream::Instance().SetLanguage(bLanguage);
	return Py_BuildNone();
}
#endif

#ifdef ENABLE_EXTENDED_WHISPER_DETAILS
PyObject* netSendWhisperDetails(PyObject* poSelf, PyObject* poArgs)
{
	char* name;
	if (!PyTuple_GetString(poArgs, 0, &name))
		return Py_BuildException();

	CPythonNetworkStream& net = CPythonNetworkStream::Instance();
	net.SendWhisperDetails(name);

	return Py_BuildNone();
}
#endif

/// 2.
// Search
		{ NULL, NULL, NULL },

// Add above
#ifdef ENABLE_MULTI_LANGUAGE_SYSTEM
		{ "SendChangeLanguagePacket", netSendChangeLanguagePacket, METH_VARARGS },
		{ "SetLanguage", netSetLanguage, METH_VARARGS },
		{ "GetLanguage", netGetLanguage, METH_VARARGS },
#endif

#ifdef ENABLE_EXTENDED_WHISPER_DETAILS
		{ "SendWhisperDetails", netSendWhisperDetails, METH_VARARGS },
#endif