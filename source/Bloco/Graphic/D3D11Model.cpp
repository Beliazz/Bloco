#include "Bloco.h"

/************************************************************************/
/*CD3D11Model  !!NEEDS WORK!!!                                          */
/************************************************************************/

CD3D11Model::CD3D11Model( cgl::PD3D11Effect pEffect, ClearModelSDK::CMeshNode* mesh ) : CD3D11Mesh( pEffect ),
		m_pMeshNode(mesh)
{
	m_pConvexShape = NULL;
}

CD3D11Model::~CD3D11Model()
{
}

bool CD3D11Model::getElement(char* pDest, UINT elementIndex, cgl::CGL_INPUT_ELEMENT_SIGNATURE* inputElementSignature, UINT inputElementCount  )
{
	memcpy(pDest,m_pMeshNode->GetVertexBuffer()->GetData( inputElementSignature->semanticName, elementIndex ),sizeof(float)*3);
	return true;
}

bool CD3D11Model::isCompatible( std::vector<cgl::CGL_INPUT_ELEMENT_SIGNATURE>& pRequiredElements )
{
	for (UINT i = 0; i < pRequiredElements.size(); i++)
	{
		if (!m_pMeshNode->HasVertexData( pRequiredElements[i].semanticName ))
			return false;
	}
	return true;
}

bool CD3D11Model::Create()
{
	//Parse Materialgroups
	for (unsigned int i = 0; i < m_pMeshNode->GetMaterialConnection().m_vecMaterialGroups.size() ; i++)
	{
		shared_ptr<CD3D11MaterialGroup> matGroup = shared_ptr<CD3D11MaterialGroup>( DEBUG_CLIENTBLOCK CD3D11MaterialGroup(m_pEffect));


		matGroup->AddIndexData( m_pMeshNode->GetMaterialConnection().m_vecMaterialGroups[i].m_vecVertexIndices );


		//Get Assigned Materil Index
		DWORD matIndex = m_pMeshNode->GetMaterialConnection().m_vecMaterialGroups[i].m_dMaterialIndex;

		if (!matGroup->m_Material.Init())
		{
			printf("[SYSTEM] CD3D11Model::Create() matGroup->m_Material.Init() faild (ModelName = %s)\n",m_pMeshNode->GetName().c_str());
			return false;
		}

		//Create Material from Model
		matGroup->m_Material.FromModel( m_pMeshNode->GetMaterial( matIndex )  );


		//If Material has an Texture, load it!
		if (m_pMeshNode->GetMaterial( matIndex ).m_HasTexture)
		{
			for (int j = 0; j < m_pMeshNode->GetTextureCount() ; j++)
			{
				if (m_pMeshNode->GetTexture(j).m_dAssignedMaterial == matIndex )
				{

					string filename   = m_sContentDirectory + m_pMeshNode->GetTexture(j).m_sFilename;
					string textureFor =  m_pMeshNode->GetTexture(j).m_sTextureFor;

					if ( textureFor == "DiffuseColor" )
					{
						matGroup->SetTexture(eTextureType_Diffuse,filename);
					}
					else if ( textureFor == "SpecularColor" )
					{
						matGroup->SetTexture(eTextureType_Specular,filename);
					}
					else if ( textureFor == "NormalMap" )
					{
						matGroup->SetTexture(eTextureType_Normal,filename);
					}
					else
					{
						printf("[SYSTEM] CD3D11Model::Create() invalid texturetype (%s)\n",textureFor.c_str());
						return false;
					}
				}
			}
		}

		AddMaterialGroup(matGroup);
	}

	m_bHasAnimation = (m_pMeshNode->GetBoneCount() != 0 );

	return true;
}

btCollisionShape* CD3D11Model::GetCollisionShape()
{
	return m_pConvexShape;
}

