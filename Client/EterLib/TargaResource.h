#pragma once

#include "Ref.h"
#include "Resource.h"
#include "../eterImageLib/TGAImage.h"

class CTargaResource : public CResource
{
	public:
		typedef CRef<CTargaResource> TRef;

	public:
		static TType Type();

	public:
		CTargaResource(const char * c_pszFileName);
		virtual ~CTargaResource();

		DWORD *			GetMemPtr();
		void			GetRect(DWORD & w, DWORD & h);

		TGA_HEADER &	GetTgaHeader();

	protected:
		virtual bool OnLoad(int iSize, const void * c_pvBuf);
		virtual void OnClear();
		virtual bool OnIsEmpty() const;
		virtual bool OnIsType(TType type);

	protected:
		CTGAImage	image;
};
//martysama0134's 4ee2cf2c13ce32ff35a20010fbba866e
