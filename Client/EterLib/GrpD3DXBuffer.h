#pragma once

class CDirect3DXBuffer
{
	public:
		CDirect3DXBuffer();
		CDirect3DXBuffer(LPD3DXBUFFER lpd3dxBuffer);
		virtual ~CDirect3DXBuffer();

		void Destroy();
		void Create(LPD3DXBUFFER lpd3dxBuffer);

		void*GetPointer();
		int  GetSize();

	protected:
		LPD3DXBUFFER m_lpd3dxBuffer;
};
//martysama0134's 4ee2cf2c13ce32ff35a20010fbba866e
