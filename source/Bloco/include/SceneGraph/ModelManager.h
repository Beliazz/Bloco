#ifndef ModelManager_h__
#define ModelManager_h__

#include "Bloco.h"

typedef map<string,shared_ptr<ClearModelSDK::CModel>> modelMap;
typedef map<string,cgl::PD3D11Resource>	textureMap;

class CResourceManager
{
public:
	CResourceManager();
	~CResourceManager();

	ClearModelSDK::CModel* LoadModel( string filename );
	cgl::PD3D11Resource LoadTexture( string filename, D3DX11_IMAGE_LOAD_INFO info = D3DX11_IMAGE_LOAD_INFO() );

private:
	modelMap   m_mapModels;
	textureMap m_mapTextures;
};


#endif