/// 1.
// Search
extern void SendNotice(const char* c_pszBuf); // �� ���Ӽ������� ����

// Replace with
#ifdef __MULTI_LANGUAGE_SYSTEM__
extern void SendLocaleNotice(const char* c_pszNotice, bool bBigFont = false, ...); // Mainly for quests (notice_all, d.notice, etc...)
extern void SendGuildPrivNotice(const char* c_pszBuf, const char* c_pszGuildName, const char* c_pszPrivName, int iValue = 0);
extern void SendPrivNotice(const char* c_pszBuf, const char* c_pszEmpireName, const char* c_pszPrivName , int iValue = 0);
extern void TransNotice(LPDESC pkDesc, const char* c_pszBuf, ...); // Translate notice for priv setting (empire & guild)

extern void SendNotice(const char* c_pszBuf, ...); // Only for this game server
#else
extern void SendNotice(const char* c_pszBuf); // �� ���Ӽ������� ����
#endif

/// 2.
// Search
extern void SendNoticeMap(const char* c_pszBuf, int nMapIndex, bool bBigFont); // ���� �ʿ��� ����

// Replace with
#ifdef __MULTI_LANGUAGE_SYSTEM__
extern void SendNoticeMap(const char* c_pszBuf, int nMapIndex, bool bBigFont, ...);
#else
extern void SendNoticeMap(const char* c_pszBuf, int nMapIndex, bool bBigFont); // ���� �ʿ��� ����
#endif