#include "Bloco.h"

#define __STR2__(x) #x
#define __STR1__(x) __STR2__(x)
#define __LOC__ __FILE__ "("__STR1__(__LINE__)") : Warning Msg: "

#pragma message(__LOC__"Should be put to CGL")

/************************************************************************/
/* CD3D11Mesh                                                           */
/************************************************************************/

CD3D11Mesh::CD3D11Mesh( cgl::PD3D11Effect pEffect ) : m_pEffect(pEffect)
{
	m_pevMatWorld = cgl::CD3D11EffectVariableFromName::Create(m_pEffect, "matWorld");
	m_pevMatView  = cgl::CD3D11EffectVariableFromName::Create(m_pEffect, "matView");
	m_pevMatProj  = cgl::CD3D11EffectVariableFromName::Create(m_pEffect, "matProj");

	m_pevLightDirection  = cgl::CD3D11EffectVariableFromName::Create(m_pEffect, "f3LightDirection");
	m_pevCameraPosition  = cgl::CD3D11EffectVariableFromName::Create(m_pEffect, "f3CameraPosition");

	m_pSkeleton = shared_ptr<CD3D11Skeleton>( DEBUG_CLIENTBLOCK CD3D11Skeleton( m_pEffect ) );
}

CD3D11Mesh::~CD3D11Mesh()
{
	m_MaterialGroups.clear();
}

bool CD3D11Mesh::Init()
{
	DWORD techniqueIndex = 0;

	//Find a appropriate technique
	while (techniqueIndex < 20)
	{
		cgl::PD3D11EffectTechnique pTechnique = cgl::CD3D11EffectTechniqueFromIndex::Create(m_pEffect, techniqueIndex);

		//Check if this technique exsits. If not -> shader technique is incompatible
		if (!pTechnique->restore())
			return false;

		m_pPass			= cgl::CD3D11EffectPassFromIndex::Create(pTechnique, 0);
		m_pInputLayout	= cgl::CD3D11InputLayout::Create(m_pPass);
		m_pProcessor	= cgl::CGLInputDataProcessor::Create(m_pInputLayout, this, GetVertexCount());
		m_pInputBuffer	= cgl::CD3D11InputBuffer::Create(m_pInputLayout, m_pProcessor.get());

		techniqueIndex++;

		if (!m_pInputLayout->restore())
			continue;

		if(!m_pProcessor->isValid())
			continue;

		if (!m_pInputBuffer->restore())
			continue;

		break;
	}

	if (techniqueIndex == 20)
	{
		printf("[SYSTEM] Could not find a compatible Technique for this VertexShaderInput",m_pEffect->getName().c_str());
		return false;
	}



	for (unsigned int i = 0; i < m_MaterialGroups.size() ; i++)
	{
		if (!m_MaterialGroups[i]->Init())
		{
			printf("[SYSTEM] CD3D11Mesh::m_MaterialGroups[%i]->Init() failed\n",i);
			return false;
		}
	}

	if (m_bHasAnimation)
	{
		if (!m_pSkeleton->Init())
		{
			printf("[SYSTEM] CD3D11Mesh::m_pSkeleton->Init() failed\n");
			return false;
		}
	}

	if ( !m_pevMatWorld->restore() ||
		 !m_pevMatView->restore() ||
		 !m_pevMatProj->restore() ||
		 !m_pevLightDirection->restore() ||
		 !m_pevCameraPosition->restore() )
	{
		printf("[SYSTEM] CD3D11Mesh restore EffectVariable failed\n");
		return false;
	}

	return true;
}

bool CD3D11Mesh::Restore()
{
	if(!m_pProcessor->isValid())
	{
 		if (!m_pInputBuffer->restore())
 			return false;
	}

	if (!m_pInputBuffer->Update())
		return false;

	for (unsigned int i = 0; i < m_MaterialGroups.size() ; i++)
	{
		if (!m_MaterialGroups[i]->Restore())
			return false;
	}

	if ( !m_pevMatWorld->restore() ||
		!m_pevMatView->restore() ||
		!m_pevMatProj->restore() ||
		!m_pevLightDirection->restore() ||
		!m_pevCameraPosition->restore() )
	{
		return false;
	}

	if (m_bHasAnimation)
		m_pSkeleton->Restore();

	return true;
}

void CD3D11Mesh::Render()
{
	m_pPass->getDevice()->GetContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	m_pInputBuffer->Bind();
	m_pInputLayout->Bind();

	m_pPass->Apply();

 	for (unsigned int i = 0; i < m_MaterialGroups.size() ; i++)
 	{
 		m_MaterialGroups[i]->Render(m_pPass);
 	}
}

bool CD3D11Mesh::SetWorldMatrix( Mat worldMatrix )
{
	if (FAILED(m_pevMatWorld->get()->AsMatrix()->SetMatrix( worldMatrix.GetArray() )))
		return false;

	return true;
}

bool CD3D11Mesh::SetViewMatrix( Mat viewMatrix )
{
	if (FAILED(m_pevMatView->get()->AsMatrix()->SetMatrix( viewMatrix.GetArray() )))
		return false;

	return true;
}

bool CD3D11Mesh::SetProjectionMatrix( Mat projMatrix )
{
	if (FAILED(m_pevMatProj->get()->AsMatrix()->SetMatrix( projMatrix.GetArray() )))
		return false;

	return true;
}

void CD3D11Mesh::AddMaterialGroup( shared_ptr<CD3D11MaterialGroup> group )
{
	m_MaterialGroups.push_back( group );
}

bool CD3D11Mesh::SetLightDirection( Vec dir )
{
	if (FAILED(m_pevLightDirection->get()->AsVector()->SetFloatVector( dir.GetArray() )))
		return false;

	return true;
}

bool CD3D11Mesh::SetCameraPosition( Vec pos )
{
	if (FAILED(m_pevCameraPosition->get()->AsVector()->SetFloatVector( pos.GetArray() )))
		return false;

	return true;
}

void CD3D11Mesh::AddBone( CD3D11Bone* bone )
{
	m_pSkeleton->AddBone( bone );
}

void CD3D11Mesh::AddAnimationsTrack( shared_ptr<AnimationsTrack> track )
{
	m_pSkeleton->AddAnimationsTrack( track );
}

void CD3D11Mesh::Update( DWORD const elapsedMs )
{
	if (m_bHasAnimation)
		m_pSkeleton->Update( elapsedMs );
}



CD3D11MaterialGroup::CD3D11MaterialGroup( cgl::PD3D11Effect pEffect ): 
	m_pEffect(pEffect),
	m_Material( pEffect )
{
	m_pIndexBuffer = cgl::CD3D11IndexBuffer::Create(sizeof(DWORD));

	m_pSamplerLinear = NULL;
}

CD3D11MaterialGroup::~CD3D11MaterialGroup()
{
	SAFE_RELEASE(m_pSamplerLinear);
}

void CD3D11MaterialGroup::AddIndex( DWORD index )
{
	m_pIndexBuffer->AddData((char*)&index);
}

bool CD3D11MaterialGroup::Init()
{
	if (!m_pIndexBuffer->restore())
	{
		printf("[SYSTEM] CD3D11MaterialGroup::m_pIndexBuffer->restore() failed \n");
		return false;
	}

	if (!m_Material.Init())
	{
		printf("[SYSTEM] CD3D11MaterialGroup::m_Material.Init() failed \n");
		return false;
	}

	if (!m_Material.Set())
	{
		printf("[SYSTEM] CD3D11MaterialGroup::m_Material.Set() failed \n");
		return false;
	}

	for(MaterialGroupTextureMap::iterator it = m_textureMap.begin(); it != m_textureMap.end(); it++)
	{
		if(!it->second->Restore())
		{
			printf("[SYSTEM] CD3D11MaterialGroup::CD3D11MaterialGroupTexture::Restore() failed (%s)\n", it->second->GetFilename().c_str());
			return false;
		}
	}

	return true;
}

bool CD3D11MaterialGroup::Restore()
{
	if (!m_Material.Init())
	{
		printf("[SYSTEM] CD3D11MaterialGroup::m_Material.Init() failed \n");
		return false;
	}

	if (!m_Material.Set())
	{
		printf("[SYSTEM] CD3D11MaterialGroup::m_Material.Set() failed \n");
		return false;
	}


	for(MaterialGroupTextureMap::iterator it = m_textureMap.begin(); it != m_textureMap.end(); it++)
	{
		if(!it->second->Restore())
		{
			printf("[SYSTEM] CD3D11MaterialGroup::CD3D11MaterialGroupTexture::Restore() failed (%s)\n", it->second->GetFilename().c_str());
			return false;
		}
	}


	return true;
}

void CD3D11MaterialGroup::Render(cgl::PD3D11EffectPass pPass)
{
	for(MaterialGroupTextureMap::iterator it = m_textureMap.begin(); it != m_textureMap.end(); it++)
	{
		if(!it->second->Bind())
		{
			printf("[SYSTEM] CD3D11MaterialGroup::Render(cgl::PD3D11EffectPass pPass) m_textureMap[]->second->Bind() failed\n");
		}
	}

	m_pIndexBuffer->Bind();
	//pPass->Apply();
	m_pIndexBuffer->Draw();
}

bool CD3D11MaterialGroup::SetTexture()
{
	if ( m_sTextureName == "NONE" || m_sTextureName =="" )
		m_sTextureName = FULLPATH("\\data\\texture\\NULL.dds");


	D3DX11_IMAGE_LOAD_INFO info;
	m_pShaderResource_Texture = g_pResourceManager->LoadTexture( m_sTextureName, info );
	if (!m_pShaderResource_Texture)
	{
		m_pShaderResource_Texture = g_pResourceManager->LoadTexture( FULLPATH("\\data\\texture\\NULL.dds"), info );
		if (!m_pShaderResource_Texture)
		{
			return false;
		}
	}

	m_pShaderResourceView = cgl::CD3D11ShaderResourceView::Create(m_pShaderResource_Texture);
	if (!m_pShaderResourceView->restore())
	{
		return false;
	}

	return true;
}


void CD3D11MaterialGroup::SetTexture( eTextureType type, string filename )
{
	MaterialGroupTextureMap::iterator texture = m_textureMap.find(type);

	if (texture == m_textureMap.end())
	{
		m_textureMap[type] = shared_ptr<CD3D11MaterialGroupTexture>( DEBUG_CLIENTBLOCK CD3D11MaterialGroupTexture( m_pEffect, type, filename ));
	}
	else
	{
		m_textureMap[type]->SetFilename(filename);
	}
}

void CD3D11MaterialGroup::AddIndexData( vector<DWORD> index )
{
	m_pIndexBuffer->SetData( (char*)index.data() , index.size() );
}

bool CD3D11MaterialGroupTexture::Restore()
{
	m_pevShaderResourceView = cgl::CD3D11EffectVariableFromName::Create( m_pEffect, sTextureTypes[m_eType] );

	if (!m_pevShaderResourceView->restore())
	{
		printf("[SYSTEM] CD3D11MaterialGroupTexture::m_pevShaderResourceView->restore() failed (%s)\n", sTextureTypes[m_eType]);
		return false;
	}

	// Create the sample state
	D3D11_SAMPLER_DESC sampDesc;
	ZeroMemory( &sampDesc, sizeof(sampDesc) );
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampDesc.MinLOD = 0;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
	HRESULT hr = g_pApp->GetDevice()->CreateSamplerState( &sampDesc, &m_pSamplerLinear );
	if( FAILED( hr ) )
	{
		printf("[SYSTEM] CD3D11MaterialGroupTexture::CreateSamplerState() failed\n");
		return false;
	}

	m_pShaderResource_Texture = g_pResourceManager->LoadTexture( m_sFilename );
	if (!m_pShaderResource_Texture)
	{
		printf("[SYSTEM] g_pResourceManager->LoadTexture() failed. File does not exists or is invalid (%s)\n", m_sFilename.c_str());

		m_pShaderResource_Texture = g_pResourceManager->LoadTexture( FULLPATH("\\data\\texture\\NULL.dds") );
		if (!m_pShaderResource_Texture)
		{
			printf("[SYSTEM] Try to load a NULL-Texture also failed (%s)\n", FULLPATH("\\data\\texture\\NULL.dds"));
			return false;
		}
	}

	m_pShaderResourceView = cgl::CD3D11ShaderResourceView::Create(m_pShaderResource_Texture);
	if (!m_pShaderResourceView->restore())
	{
		printf("[SYSTEM] CD3D11MaterialGroupTexture::m_pShaderResourceView->restore() failed \n");
		return false;
	}

	return true;
}

bool CD3D11MaterialGroupTexture::Bind()
{
	if(m_pevShaderResourceView->get()->AsShaderResource()->SetResource( m_pShaderResourceView->get() ) != S_OK)
	{
		printf("[SYSTEM] CD3D11MaterialGroupTexture::Bind() m_pevShaderResourceView->get()->AsShaderResource()->SetResource() failed\n");
		return false;
	}

	g_pApp->GetDeviceContext()->PSSetSamplers( 0, 1, &m_pSamplerLinear );

	return true;
}
