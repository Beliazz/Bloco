 #include "Bloco.h"



CResourceManager::CResourceManager()
{

}

CResourceManager::~CResourceManager()
{
	m_mapModels.clear();
	m_mapTextures.clear();
}

ClearModelSDK::CModel* CResourceManager::LoadModel( string filename )
{
	modelMap::iterator it = m_mapModels.find( filename );

	if (it != m_mapModels.end())
		return it->second.get();


	shared_ptr<ClearModelSDK::CModel> pModel = shared_ptr<ClearModelSDK::CModel>( DEBUG_CLIENTBLOCK ClearModelSDK::CModel() );

	if (!pModel->Load( filename ))
		return nullptr;

	m_mapModels[filename] = pModel;

	return pModel.get();
}

cgl::PD3D11Resource CResourceManager::LoadTexture( string filename, D3DX11_IMAGE_LOAD_INFO info )
{
	textureMap::iterator it = m_mapTextures.find( filename );

	if (it != m_mapTextures.end())
		return it->second;
	
	cgl::PD3D11Resource pRes = cgl::CD3D11TextureFromFile::Create(filename, info);
	if (!pRes->restore())
		return nullptr;

// 	Resource res(TEXTURE_PATH(filename));
// 	ResHandlePtr handle = g_pApp->m_pResCache->GetHandle(&res);
// 	cgl::PD3D11Resource pRes = cgl::CD3D11TextureFromMemory::Create(handle->Buffer(), handle->Size(), info);
// 	if (!pRes->restore())
// 	{
// 		string suffix = GetSuffix(filename);
// 		std::transform(suffix.begin(), suffix.end(),suffix.begin(), ::toupper);
// 
// 		string newFilename = GetName( filename );
// 		newFilename += ".";
// 		newFilename += suffix;
// 
// 		Resource res(TEXTURE_PATH(newFilename));
// 		ResHandlePtr handle = g_pApp->m_pResCache->GetHandle(&res);
// 		cgl::PD3D11Resource pRes = cgl::CD3D11TextureFromMemory::Create(handle->Buffer(), handle->Size(), info);
// 		if (!pRes->restore())
// 		{
// 			return nullptr;
// 		}
// 	}

	m_mapTextures[filename] = pRes;

	return pRes;
}
