#include "Bloco.h"

bool BLOCO_API CD3D11Font::VCreate(IFW1Factory* pFactory)
{
	HRESULT hResult = S_FALSE;
	pFactory->CreateFontWrapper(g_pApp->GetDevice(), m_family.c_str(), &m_pWrapper);
	if(FAILED(hResult))
	{
		return false;
	}

	return true;
}
void BLOCO_API CD3D11Font::VRender( ID3D11DeviceContext* pContext, wstring text, float size, Vec pos, Col color )
{
	m_pWrapper->DrawString(pContext, text.c_str(), m_family.c_str(), size, pos.GetX(), pos.GetY(), ColToHex(color), FW1_RESTORESTATE);
}
void BLOCO_API CD3D11Font::VRender( ID3D11DeviceContext* pContext, string text, float size, Vec pos, Col color )
{
	wstringstream ws;
	ws << text.c_str();
	VRender(pContext, ws.str(), size, pos, color);
}
bool BLOCO_API CD3D11Font::VOnRestore( IFW1Factory* pFactory )
{
	return VCreate(pFactory);
}
void BLOCO_API CD3D11Font::VOnLostDevice()
{
	VDelete();
}

