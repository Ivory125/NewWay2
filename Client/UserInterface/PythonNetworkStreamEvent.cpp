#include "StdAfx.h"
#include "PythonNetworkStream.h"

void CPythonNetworkStream::OnRemoteDisconnect()
{
	PyCallClassMemberFunc(m_poHandler, "SetLoginPhase", Py_BuildValue("()"));
}

void CPythonNetworkStream::OnDisconnect()
{
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
// Main Game
void CPythonNetworkStream::OnScriptEventStart(int iSkin, int iIndex)
{
	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "OpenQuestWindow", Py_BuildValue("(ii)", iSkin, iIndex));
}
//martysama0134's 4ee2cf2c13ce32ff35a20010fbba866e
