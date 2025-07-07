#pragma once

BOOL ShiftJIS_IsLeadByte( const char chByte );
BOOL ShiftJIS_IsTrailByte( const char chByte );
int ShiftJIS_StringCompareCI( LPCSTR szStringLeft, LPCSTR szStringRight, size_t sizeLength );
//martysama0134's 4ee2cf2c13ce32ff35a20010fbba866e
