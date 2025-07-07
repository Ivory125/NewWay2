#pragma once

int Ymir_WideCharToMultiByte(
  UINT CodePage,
  DWORD dwFlags,
  LPCWSTR lpWideCharStr,
  int cchWideChar,
  LPSTR lpMultiByteStr,
  int cbMultiByte,
  LPCSTR lpDefaultChar,
  LPBOOL lpUsedDefaultChar
);

int Ymir_MultiByteToWideChar(
  UINT CodePage,
  DWORD dwFlags,
  LPCSTR lpMultiByteStr,
  int cbMultiByte,
  LPWSTR lpWideCharStr,
  int cchWideChar
);
//martysama0134's 4ee2cf2c13ce32ff35a20010fbba866e
