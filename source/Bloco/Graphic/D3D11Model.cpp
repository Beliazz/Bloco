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

	DWORD * indices = DEBUG_CLIENTBLOCK DWORD[m_pMeshNode->GetVertexBuffer()->m_vecPositions.size()];

	for (unsigned int i = 0; i < m_pMeshNode->GetVertexBuffer()->m_vecPositions.size() ; i++)
	{
		indices[i] = i;
	}


	btTriangleIndexVertexArray* mIndexVertexArray = new btTriangleIndexVertexArray(
		m_pMeshNode->GetVertexBuffer()->m_vecPositions.size()/3,
		(int*)indices,
		sizeof(DWORD)*3,
		m_pMeshNode->GetVertexBuffer()->m_vecPositions.size(),
		&((btScalar*)m_pMeshNode->GetVertexBuffer()->m_vecPositions.data())[0],
		sizeof(float)*3);




	m_pConvexShape = new btConvexTriangleMeshShape( mIndexVertexArray );
	btShapeHull *hull = new btShapeHull(m_pConvexShape);
	btScalar margin = m_pConvexShape->getMargin();
	hull->buildHull(margin);
	m_pConvexShape->setUserPointer(hull);

	m_bHasAnimation = false;

	if ( m_pMeshNode->GetBoneCount() > 0 && m_pMeshNode->GetAnimationTakeCount() > 0 )
	{
		m_bHasAnimation = true;
		
		//Animations tracks
		for (unsigned int i = 0; i < m_pMeshNode->GetAnimationTakeCount() ; i++)
		{
			shared_ptr<AnimationsTrack> track = shared_ptr<AnimationsTrack>( DEBUG_CLIENTBLOCK AnimationsTrack());

			ClearModelSDK::sAnimationsTake__ take = ClearModelSDK::sAnimationsTake__(m_pMeshNode->GetAnimationTake(i));

			track->SetName( take.m_sName );
			track->SetStartTime( take.m_fStart );
			track->SetEndTime( take.m_fEnd );

			for (unsigned int j = 0; j < take.m_vecAnimationKeys.size() ; j++)
			{
				AnimationsKey* key = DEBUG_CLIENTBLOCK AnimationsKey( take.m_vecAnimationKeys[j].m_fBoneIndex, 
																	  take.m_vecAnimationKeys[j].m_fTimestamp, 
																	  take.m_vecAnimationKeys[j].m_Matrix.m_data );

				track->AddAnimationsKey( shared_ptr<AnimationsKey>( key ));
			}

			AddAnimationsTrack( track );
		}
	}

	return true;
}

btCollisionShape* CD3D11Model::GetCollisionShape()
{
	return m_pConvexShape;
}

