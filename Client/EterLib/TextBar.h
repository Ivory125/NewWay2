#pragma once

#include "DibBar.h"

class CTextBar : public CDibBar
{
	public:
		CTextBar(int fontSize, bool isBold);
		virtual ~CTextBar();

		void TextOut(int ix, int iy, const char * c_szText);
		void SetTextColor(int r, int g, int b);
		void GetTextExtent(const char * c_szText, SIZE* p_size);

	protected:
		void __SetFont(int fontSize, bool isBold);

		void OnCreate();

	protected:
		HFONT m_hFont;
		HFONT m_hOldFont;

		int		m_fontSize;
		bool	m_isBold;
};
//martysama0134's 4ee2cf2c13ce32ff35a20010fbba866e
