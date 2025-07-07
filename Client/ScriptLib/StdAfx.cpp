// stdafx.cpp : source file that includes just the standard includes
//	scriptLib.pch will be the pre-compiled header
//	stdafx.obj will contain the pre-compiled type information

#include "stdafx.h"

void SetExceptionSender(IPythonExceptionSender * pkExceptionSender)
{
	g_pkExceptionSender = pkExceptionSender;
}
//martysama0134's 4ee2cf2c13ce32ff35a20010fbba866e
