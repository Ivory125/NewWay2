#ifndef __INC_ETERPACKCURSOR_H__
#define __INC_ETERPACKCURSOR_H__

#include "EterPack.h"

class CEterPackCursor
{
	public:
		CEterPackCursor(CEterPack * pack);
		~CEterPackCursor();

		bool		Open(const char* filename);
		void		Close();
		void		Seek(long offset);
		bool		Read(LPVOID data, long size);
		long		Size();

	private:
		CEterPack * m_pPack;
		CMappedFile m_file;
		LPCVOID 	m_pData;
		long		m_ReadPoint;
};

#endif
//martysama0134's 4ee2cf2c13ce32ff35a20010fbba866e
