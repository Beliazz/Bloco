#include "Bloco.h"

class BLOCO_API CD3D11Font
{
public:
	CD3D11Font(wstring family) : m_family(family) {}
	~CD3D11Font(){VDelete();}

	bool VCreate(IFW1Factory* pFactory);
	void VDelete()
	{
		SAFE_RELEASE(m_pWrapper);
	}
	bool VOnRestore(IFW1Factory* pFactory);
	void VOnLostDevice();
	void VRender(ID3D11DeviceContext* pContext, wstring text, float size, Vec pos, Col color);
	void VRender(ID3D11DeviceContext* pContext, string text, float size, Vec pos, Col color);

private:
	wstring	m_family;
	IFW1FontWrapper* m_pWrapper;

};