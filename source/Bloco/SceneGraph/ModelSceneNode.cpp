 #include "Bloco.h"


using namespace ClearModelSDK;

bool ProcessModel( ModelElement* pModelElement, SceneNode* pSceneNode, SceneNode* root, Scene* pScene )
{
	///Check here
	for (unsigned int i = 0; i < pModelElement->GetChildCount() ; i++)
	{
		shared_ptr<ModelElement> pChild = pModelElement->GetChild( i );

		string type = pChild->GetType();

		if ( type == "MeshNode" )
		{
			//Get new ActorID
			ActorId id = g_pApp->m_pGame->GetNewActorID();

			ModelChildObjectParams* params = DEBUG_CLIENTBLOCK ModelChildObjectParams(AT_ModelChild_Mesh);

			ActorParams* rootParams = root->VGetActorParams();

			params->m_Id			= id;
			params->m_bPhysicActor  = rootParams->m_bPhysicActor;
			params->m_pRootNode		= root;
			strcpy(params->m_sName,((ClearModelSDK::CMeshNode*)pChild.get())->GetName().c_str());
			params->m_pModelElement = pChild.get();
			params->m_bDynamic		= rootParams->m_bDynamic;
			
			//---------------------------------------------
			//Fire New Actor Event:
			//Logic: calls params->VCreate()
			//View:  calls params->VCreateScenenNode()
			//---------------------------------------------
			const EvtData_New_Actor actorEvent( id , params );
			const bool bSuccess = safeTriggerEvent( actorEvent );

			SAFE_DELETE(params);
		}
	}

	return true;
}

 ModelSceneNode::ModelSceneNode( const optional<ActorId> actorId, string name, std::string fileName, Mat *transform ) : 
	SceneNode(actorId, name, RenderPass_Actor,transform)
{
	m_sFilename = fileName;
	m_IsModified = false;
}

 ModelSceneNode::~ModelSceneNode()
{
// 	for (unsigned int i = 0; i < m_SceneNodes.size() ; i++)
// 	{
// 		SAFE_DELETE(m_SceneNodes[i]);
// 	}

/*	m_SceneNodes.clear();*/
}

HRESULT ModelSceneNode::VRender( Scene *pScene )
{
	return S_OK;
}

HRESULT ModelSceneNode::VOnUpdate( Scene *pScene, DWORD const elapsedMs )
{
	return SceneNode::VOnUpdate(pScene,elapsedMs);
}

HRESULT ModelSceneNode::VPick( Scene *pScene, RayCast *pRayCast )
{
	return SceneNode::VPick(pScene, pRayCast);
}

HRESULT ModelSceneNode::VOnRestore( Scene *pScene )
{
	m_pModel = g_pResourceManager->LoadModel( m_sFilename );
	if (!m_pModel)
	{
		return E_FAIL;
	}

	if (!ProcessModel( m_pModel->GetRootNode().get(), this, this, pScene ))
		return E_FAIL;

	VSetTransform(VGet()->ToWorld());




	


	if (VGetActorParams()->m_bPhysicActor)
	{
		btCompoundShape* compound = new btCompoundShape();

		for (int i = 0; i < m_vecMeshSceneNodes.size() ; i++)
		{
			btTransform localTrans;
			localTrans = Mat_to_btTransform(m_vecMeshSceneNodes[i]->m_matLocal);

			compound->addChildShape(localTrans,m_vecMeshSceneNodes[i]->GetCollisionshape());
		}

		shared_ptr<IActor> actor = g_pApp->m_pGame->VGetActor( *VGet()->ActorId() );

		g_pApp->m_pGame->VGetGamePhysics()->VAddCompoundShape(compound,actor.get(),SpecificGravity(PhysDens_Iron_Cast), PhysMat_Normal);
	}
	

	for (int i = 0; i < m_vecMeshSceneNodes.size() ; i++)
	{
		shared_ptr<IActor> actor = g_pApp->m_pGame->VGetActor( *m_vecMeshSceneNodes[i]->VGet()->ActorId() );
		g_pApp->m_pGame->VGetGamePhysics()->VSetCompoundShapeChild(*VGet()->ActorId(),actor.get(),i);
	}

	return S_OK;
}

bool ModelSceneNode::VRemoveChild( ActorId id )
{
	for(SceneNodeList::iterator i=m_Children.begin(); i!=m_Children.end(); ++i)
	{
		const SceneNodeProperties* pProps = (*i)->VGet();
		if(pProps->ActorId().valid() && id==*pProps->ActorId())
		{
			i = m_Children.erase(i);	//this can be expensive for vectors
			VGetActorParams()->m_IsModified = true;
			return true;
		}
		else
		{
			(*i)->VRemoveChild(id);
		}
	}
	return false;
}

void ModelSceneNode::AddMesh( MeshSceneNode* node )
{
	m_vecMeshSceneNodes.push_back( node );
}

void ModelSceneNode::VSetTransform( Mat toWorld /*= MatIdentity()*/,Mat fromWorld /*= MatIdentity()*/ )
{
	SceneNode::VSetTransform(toWorld,fromWorld);

// 	for (int i = 0; i < m_vecMeshSceneNodes.size() ; i++)
// 	{
// 		Mat childMat  = m_vecMeshSceneNodes[i]->m_matLocal;
// 		Mat parentMat = VGet()->ToWorld();
// 
// 		Mat resultMat = parentMat * childMat;
// 
// 		m_vecMeshSceneNodes[i]->VSetTransform(resultMat);
// 		g_pApp->m_pGame->VGetGamePhysics()->VKinematicMove(resultMat,*m_vecMeshSceneNodes[i]->VGet()->ActorId());
// 	}
}



 MeshSceneNode::MeshSceneNode( ClearModelSDK::ModelElement* pNode, const optional<ActorId> actorId, SceneNode* rootNode, Mat* actorTransform  ) :
		SceneNode(actorId, "", RenderPass_Actor, actorTransform ),
		m_pRootNode(rootNode)
{
	m_pMeshNode = dynamic_cast<ClearModelSDK::CMeshNode*>(pNode);
	m_pModel = NULL;

	VGet()->SetName( m_pMeshNode->GetName() );
}

 MeshSceneNode::~MeshSceneNode()
{
	SAFE_DELETE(m_pModel);
}

HRESULT MeshSceneNode::VOnRestore( Scene *pScene )
{
	//Effect
	ifstream myfile;
	int size = 0;
	shared_ptr<char> pData = NULL;

	myfile.open ("simple.fxc", ios::in|ios::binary|ios::ate);
	if (!myfile.is_open())
	{
		printf("[SYSTEM] MeshSceneNode::VOnRestore( Scene *pScene ) effectfile does not exist or is invalid (simple.fxc)\n");
		return E_FAIL;
	}

	size = (int)myfile.tellg();
	pData = shared_ptr<char>(new char[size]);
	myfile.seekg(0, std::ios_base::beg);
	myfile.read(pData.get(), size);
	myfile.close();

	cgl::PD3D11Effect pEffect = cgl::CD3D11EffectFromMemory::Create(pData.get(), size);
	if (!pEffect)
	{
		printf("[SYSTEM] MeshSceneNode::VOnRestore( Scene *pScene ) cgl::CD3D11EffectFromMemory::Create() failed\n");
		return E_FAIL;
	}

	m_pModel = DEBUG_CLIENTBLOCK CD3D11Model( pEffect, m_pMeshNode );

	m_pModel->SetContentDir( GetDirectory( ((ModelSceneNode*)m_pRootNode)->GetFilename() ) );

	if(!m_pModel->Create( ))
	{
		printf("[SYSTEM] MeshSceneNode::VOnRestore( Scene *pScene ) m_pModel->Create() failed\n");
		return E_FAIL;
	}
	
	//Add Bones
	for (unsigned int i = 0; i < m_pMeshNode->GetBoneCount() ; i++)
	{
		string name		  = m_pMeshNode->GetBone( i ).m_sName;
		string parentName = m_pMeshNode->GetBone( i ).m_sParentName;

		Mat globalMatrix   = Mat( m_pMeshNode->GetBone( i ).m_GlobalMatrix.m_data );
		Mat bindPoseMatrix = Mat( m_pMeshNode->GetBone( i ).m_BindPoseMatrix.m_data );


		//Get new ActorID
		ActorId id = g_pApp->m_pGame->GetNewActorID();

		BoneObjectParams* params =  DEBUG_CLIENTBLOCK BoneObjectParams(name,parentName,globalMatrix,bindPoseMatrix);

		ActorParams* rootParams = m_pRootNode->VGetActorParams();

		//---------------------------------------------
		//Fire New Actor Event:
		//Logic: calls params->VCreate()
		//View:  calls params->VCreateScenenNode()
		//---------------------------------------------
		const EvtData_New_Actor actorEvent( id , params );
		const bool bSuccess = safeTriggerEvent( actorEvent );

		SAFE_DELETE(params);

		CD3D11Bone* pBone =  DEBUG_CLIENTBLOCK CD3D11Bone( name, parentName, bindPoseMatrix, globalMatrix, NULL /*g_pApp->m_pGame->VGetActor(id).get()*/ );

		//g_pApp->m_pGame->VGetGamePhysics()->VAddBox(Vec(1.0f,1.0f,1.0f),g_pApp->m_pGame->VGetActor(id).get(),0.0,PhysMat_Playdough);

		m_pModel->AddBone( pBone );
	}





	if (!m_pModel->Init())
	{
		printf("[SYSTEM] MeshSceneNode::VOnRestore( Scene *pScene ) m_pModel->Init() faild \n");
		return false;
	}

	((ModelSceneNode*)m_pRootNode)->AddMesh( this );

	m_matLocal = Mat(m_pMeshNode->GetGlobalMatrix());

	m_pCollisionShape = m_pModel->GetCollisionShape();
	
	// Initialize the projection matrix
	if (!m_pModel->SetProjectionMatrix( pScene->GetCamera()->GetProjection() ))
	{
		printf("[SYSTEM] MeshSceneNode::VOnRestore( Scene *pScene ) m_pModel->SetProjectionMatrix( pScene->GetCamera()->GetProjection() ) failed\n");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT MeshSceneNode::VRender( Scene *pScene )
{
 	m_pModel->Render();

	return S_OK;
}

HRESULT MeshSceneNode::VOnUpdate( Scene *pScene, DWORD const elapsedMs )
{
	if (!m_pModel->SetViewMatrix( pScene->GetCamera()->GetView() ))
		return false;

	if (!m_pModel->SetWorldMatrix( VGet()->ToWorld() * m_pRootNode->VGet()->ToWorld() /** VGet()->ToWorld()*/ /** m_matLocal*/ ))
		return false;

	if (!m_pModel->SetLightDirection( Vec( -0.577f, 0.577f, -0.577f ) ))
		return false;

	if (!m_pModel->SetCameraPosition( pScene->GetCamera()->GetPosition() ))
		return false;

	m_pModel->Update( elapsedMs );

	return S_OK;
}


 EmptyMeshSceneNode::EmptyMeshSceneNode( shared_ptr<ClearModelSDK::ModelElement> pNode, const optional<ActorId> actorId, ModelSceneNode* rootNode ):
	SceneNode(actorId, "EmptyNode", RenderPass_Actor),
	m_pRootNode(rootNode)
{
	m_pModelElement = pNode.get();
}

 EmptyMeshSceneNode::~EmptyMeshSceneNode()
{
}

HRESULT EmptyMeshSceneNode::VOnRestore( Scene *pScene )
{
	if(!ProcessModel( m_pModelElement, this, m_pRootNode, pScene ) )
		return E_FAIL;

	//Create ModelObjectParams
	shared_ptr<ModelChildObjectParams> params = shared_ptr<ModelChildObjectParams>(DEBUG_CLIENTBLOCK ModelChildObjectParams(*VGet()->ActorId()));
	params->VCreate(g_pApp->m_pGame);

	EvtData_New_Actor* evdata = DEBUG_CLIENTBLOCK EvtData_New_Actor(*params->m_Id,params.get());
	IEventDataPtr pEvtData = IEventDataPtr(evdata);
	safeQueEvent(pEvtData);

	return S_OK;
}


BoneSceneNode::BoneSceneNode( ClearModelSDK::ModelElement*pNode, const optional<ActorId> actorId, SceneNode* rootNode, Mat* actorTransform ):
	SceneNode(actorId, "", RenderPass_Actor, actorTransform ),
	m_pRootNode(rootNode)
{
// 	m_pSkeletonNode = dynamic_cast<ClearModelSDK::CSkeletonNode*>(pNode);
// 
// 	VGet()->SetName( m_pSkeletonNode->GetName() );
}

BoneSceneNode::~BoneSceneNode()
{

}

HRESULT BoneSceneNode::VRender( Scene *pScene )
{

	return S_OK;
}

HRESULT BoneSceneNode::VOnUpdate( Scene *, DWORD const elapsedMs )
{

	return S_OK;
}

HRESULT BoneSceneNode::VOnRestore( Scene *pScene )
{
	return S_OK;
}
