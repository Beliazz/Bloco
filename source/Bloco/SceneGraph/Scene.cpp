#include "Bloco.h"

////////////////////////////////////////////////////
// SceneNode Implementation
////////////////////////////////////////////////////

//
// SceneNode::SceneNode
//
 BLOCO_API SceneNode::~SceneNode()
{
	m_Children.clear();
}

//
// SceneNode::VOnRestore					- Chapter 14, page 475
//
 BLOCO_API HRESULT SceneNode::VOnRestore(Scene *pScene)
{
	// This is meant to be called from any class
	// that inherits from SceneNode and overloads
	// VOnRestore()

	SceneNodeList::iterator i = m_Children.begin();
	SceneNodeList::iterator end = m_Children.end();
	while (i != end)
	{
		(*i)->VOnRestore(pScene);
		i++;
	}
	return S_OK;
}

//
// SceneNode::VSetTransform
//
void BLOCO_API SceneNode::VSetTransform( Mat toWorld, Mat fromWorld )
{
	*m_Props.m_World        = toWorld;
	*m_Props.m_InverseWorld = fromWorld;
}

//
// SceneNode::VAddTransform					- Chapter 14, page 476
//
void BLOCO_API SceneNode::VAddTransform( Mat value )
{
	*m_Props.m_World *= value;
}


//
// SceneNode::VPreRender					- Chapter 14, page 476
//
HRESULT BLOCO_API SceneNode::VPreRender(Scene *pScene) 
{
	/*pScene->PushAndSetMatrix(m_Props.m_ToWorld);*/
	return S_OK;
}

//
// SceneNode::VPostRender					- Chapter 14, page 476
//
HRESULT BLOCO_API SceneNode::VPostRender(Scene *pScene) 
{
	/*pScene->PopMatrix();*/
	return S_OK;
}

//
// SceneNode::VIsVisible				- Chapter 14, page 477
//	
bool BLOCO_API SceneNode::VIsVisible(Scene *pScene) const
{
	// transform the location of this node into the camera space 
	// of the camera attached to the scene

	// 	Mat4x4 toWorld, fromWorld;
	// 	pScene->GetCamera()->VGet()->Transform(&toWorld, &fromWorld);
	// 	Vec3 pos = VGet()->ToWorld().GetPosition();
	// 
	// 	pos = fromWorld.Xform(pos);
	// 
	// 	Frustum const &frustum = pScene->GetCamera()->GetFrustum();
	// 	return frustum.Inside(pos, VGet()->Radius());

	return true;
}

//
// SceneNode::VOnUpdate					- Chapter 14, page 476
//
HRESULT BLOCO_API SceneNode::VOnUpdate(Scene *pScene, DWORD const elapsedMs)
{
	// This is meant to be called from any class
	// that inherits from SceneNode and overloads
	// VOnUpdate()

	SceneNodeList::iterator i = m_Children.begin();
	SceneNodeList::iterator end = m_Children.end();

	while (i != end)
	{
		(*i)->VOnUpdate(pScene, elapsedMs);
		i++;
	}
	return S_OK;
}

//
// SceneNode::VRender					- Chapter 14, page 477
//
HRESULT BLOCO_API SceneNode::VRender(Scene *pScene)
{
	// 	m_Props.m_Material.Use();
	// 
	// 	switch (m_Props.m_AlphaType)
	// 	{
	// 		case AlphaTexture:
	// 			// Nothing to do here....
	// 			break;
	// 
	// 		case AlphaMaterial:
	// 			DXUTGetD3D9Device()->SetRenderState( D3DRS_COLORVERTEX, true);
	// 			DXUTGetD3D9Device()->SetRenderState( D3DRS_DIFFUSEMATERIALSOURCE, D3DMCS_MATERIAL );
	// 			break;
	// 
	// 		case AlphaVertex:
	// 			assert(0 && _T("Not implemented!"));
	// 			break;
	// 	}

	return S_OK;
}

//
// SceneNode::VRenderChildren					- Chapter 14, page 478
//
HRESULT BLOCO_API SceneNode::VRenderChildren(Scene *pScene)
{
	// Iterate through the children....
	SceneNodeList::iterator i = m_Children.begin();
	SceneNodeList::iterator end = m_Children.end();

	while (i != end)
	{
		if ((*i)->VPreRender(pScene)==S_OK)
		{
			// You could short-circuit rendering
			// if an object returns E_FAIL from
			// VPreRender()

			// Don't render this node if you can't see it
			if ((*i)->VIsVisible(pScene) && (*i)->VIsEnable() )
			{

				(*i)->VRender(pScene);

				// 				float alpha = (*i)->VGet()->m_Material.GetAlpha();
				// 				if (alpha==fOPAQUE)
				// 				{
				// 					(*i)->VRender(pScene);
				// 				}
				// 				else if (alpha!=989/*fTRANSPARENT*/)
				// 				{
				// 					// The object isn't totally transparent...
				// 					AlphaSceneNode *asn = GCC_NEW AlphaSceneNode;
				// 					assert(asn);
				// 					asn->m_pNode = *i;
				// 					asn->m_Concat = *pScene->GetTopMatrix();
				// 					
				// 					Vec worldPos(asn->m_Concat.GetPosition());
				// 					
				// 					Mat4x4 fromWorld = pScene->GetCamera()->VGet()->FromWorld();
				// 
				// 					Vec screenPos = fromWorld.Xform(worldPos);
				// 
				// 					asn->m_ScreenZ = screenPos.z;
				// 
				// 					pScene->AddAlphaSceneNode(asn);
				//				}

				(*i)->VRenderChildren(pScene);
			}
		}
		(*i)->VPostRender(pScene);
		i++;
	}

	return S_OK;
}

//
// SceneNode::VAddChild					- Chapter 14, page 480
//
bool BLOCO_API SceneNode::VAddChild(shared_ptr<ISceneNode> kid)
{
	m_Children.push_back(kid); 
	
	kid->m_pParent = this;


	// The radius of the sphere should be fixed right here
	Vec kidPos = MatGetPosition( kid->VGet()->ToWorld() );
	Vec dir = kidPos -  MatGetPosition( m_Props.ToWorld() );
	float newRadius = Vec4Length( dir ).GetX() + kid->VGet()->Radius();
	if (newRadius > m_Props.m_Radius)
		m_Props.m_Radius = newRadius;
	return true; 
}

//
// SceneNode::VRemoveChild					- not in the book
//
//   If an actor is destroyed it should be removed from the scene graph.
//   Generally the HumanView will recieve a message saying the actor has been
//   destroyed, and it will then call Scene::RemoveChild which will traverse 
//   the scene graph to find the child that needs removing.
//
bool BLOCO_API SceneNode::VRemoveChild(ActorId id)
{

	for(SceneNodeList::iterator i=m_Children.begin(); i!=m_Children.end(); ++i)
	{
		const SceneNodeProperties* pProps = (*i)->VGet();
		if(pProps->ActorId().valid() && id==*pProps->ActorId())
		{
			i = m_Children.erase(i);	//this can be expensive for vectors
			return true;
		}
		else
		{
			(*i)->VRemoveChild(id);
		}
	}
	return false;
}

//
// SceneNode::VPick	
//
HRESULT BLOCO_API SceneNode::VPick(Scene *pScene, RayCast *raycast) 
{
	for(SceneNodeList::const_iterator i=m_Children.begin(); i!=m_Children.end(); ++i)
	{
		HRESULT hr = (*i)->VPick(pScene, raycast);

		if (hr==E_FAIL)
			return E_FAIL;
	}

	return S_OK;
}

void BLOCO_API SceneNode::VSetPosition( Vec pos )
{
	VSetTransform(MatSetPosition(VGet()->ToWorld(), pos),MatSetPosition(VGet()->ToWorld(), pos));
}

shared_ptr<ISceneNode> BLOCO_API SceneNode::VFindChild( string name )
{
	if (VGet()->Name() == name)
		shared_ptr<ISceneNode>(this);

	for (unsigned int i = 0; i < m_Children.size() ; i++)
	{
		if (m_Children[i]->VGet()->Name() == name)
			return m_Children[i];
		
		shared_ptr<ISceneNode> node = m_Children[i]->VFindChild( name );
		if (node != NULL )
			return node;
	}
	return NULL;
}

 BLOCO_API SceneNode::SceneNode( optional<ActorId> actorId, std::string name,RenderPass renderPass, Mat* to /*= NULL*/, Mat* from /*= NULL*/ )
{


	if (!to)
	{
		m_Props.m_World  = (Mat*)malloc(sizeof(Mat));
		*m_Props.m_World = MatIdentity();
	}
	else
		m_Props.m_World = to;

	if (!from)
	{
		 m_Props.m_InverseWorld  = (Mat*)malloc(sizeof(Mat));
		*m_Props.m_InverseWorld = MatIdentity();
	}
	else
		m_Props.m_InverseWorld = from;

	m_pParent = NULL;
	m_Props.m_ActorId = actorId;
	m_Props.m_Name = name;
	m_Props.m_RenderPass = renderPass;
	m_Props.SetSelected(false);
	m_Props.m_enable = true;
}

ActorParams BLOCO_API * SceneNode::VGetActorParams( void )
{
	return g_pApp->m_pGame->VGetActor( *VGet()->ActorId() )->VGetParams().get();
}


////////////////////////////////////////////////////
// Scene Implementation
////////////////////////////////////////////////////

//
// Scene::Scene						- Chapter 14, page 483
//
 BLOCO_API Scene::Scene()
{
	m_Root.reset(DEBUG_CLIENTBLOCK RootNode());
	/*	D3DXCreateMatrixStack(0, &m_MatrixStack);*/

	m_bMovmentController = false;

/*	m_MovementController = DEBUG_CLIENTBLOCK MovementController();*/

	m_selectedNode = NULL;

	{
		metaTable = g_pApp->m_pLuaStateManager->GetGlobalState()->GetGlobals().CreateTable("Scene");
		metaTable.SetObject("__index", metaTable);

		metaTable.RegisterObjectDirect("GetNodePosition", (Scene*)0, &Scene::GetNodePosition);
		LuaObject luaStateManObj = g_pApp->m_pLuaStateManager->GetGlobalState()->BoxPointer(this);
		luaStateManObj.SetMetaTable(metaTable);
		g_pApp->m_pLuaStateManager->GetGlobalState()->GetGlobals().SetObject("Scene", luaStateManObj);
	}
}

//
// Scene::~Scene					- Chapter 14, page 483
//
 BLOCO_API Scene::~Scene()
{
	/*	SAFE_RELEASE(m_MatrixStack);*/
	//SAFE_DELETE(m_MovementController);
}

//
// Scene::OnRender					- Chapter 14, page 484
//

 BLOCO_API HRESULT Scene::OnRender()
{
	//     // Set up lighting states
	// 	// TODO: This should be a Light, that gets added to a list of lights....
	// 	// Some of which would cast shadows...
	// 
	// 	DXUTGetD3D9Device()->SetLight( 1, &m_Light );
	//     DXUTGetD3D9Device()->LightEnable( 1, TRUE );
	//     DXUTGetD3D9Device()->SetRenderState( D3DRS_LIGHTING, TRUE );
	// 
	// 	// The render passes usually go like this 
	// 	// 1. Static objects & terrain
	// 	// 2. Actors (dynamic objects that can move)
	// 	// 3. The Sky
	// 	// 4. Anything with Alpha

	if (m_Root /*&& m_Camera*/)
	{
		// The scene root could be anything, but it
		// is usually a SceneNode with the identity
		// matrix

		/*m_Camera->SetViewTransform(this);*/

		if (m_Root->VPreRender(this)==S_OK)
		{
			m_Root->VRender(this);
			m_Root->VRenderChildren(this);
			m_Root->VPostRender(this);
		}
	}
	
	// 	RenderAlphaPass();
	// 
	return S_OK;
}	

//
// Scene::OnRestore					- Chapter 14, page 484
//
 BLOCO_API HRESULT Scene::OnRestore()
{

	if (!m_Root)
		return S_OK;
	// 
	//     D3DXVECTOR3 vecLightDirUnnormalized(1.0f, -6.0f, 1.0f);
	// 	ZeroMemory( &m_Light, sizeof(D3DLIGHT9) );
	//     m_Light.Type        = D3DLIGHT_POINT;
	//     m_Light.Diffuse = g_White / 5.0f;
	//     m_Light.Specular = g_Black;
	//     m_Light.Ambient = g_White / 50.0f;
	// 
	// 	D3DXVec3Normalize( (D3DXVECTOR3*)&m_Light.Direction, &vecLightDirUnnormalized );
	//     m_Light.Position.x   = 5.0f;
	//     m_Light.Position.y   = 5.0f;
	//     m_Light.Position.z   = -2.0f;
	//     m_Light.Range        = 100.0f;
	//     m_Light.Falloff		= 1.0f;
	//     m_Light.Attenuation0	= 0.1f;
	//     m_Light.Attenuation1	= 0.0f;
	//     m_Light.Attenuation2	= 0.0f;
	//     m_Light.Theta			= 0.0f;
	//     m_Light.Phi			= 0.0f;
	//  
	return m_Root->VOnRestore(this);
}

//
// Scene::OnUpdate					- Chapter 14, page 485
//
HRESULT BLOCO_API Scene::OnUpdate(const int deltaMilliseconds)
{
	static DWORD lastTime = 0;
	DWORD elapsedTime = 0;
	DWORD now = timeGetTime();


	if (!m_Root)
		return S_OK;

	if (lastTime == 0)
	{
		lastTime = now;
	}

	elapsedTime = now - lastTime;
	lastTime = now;

	if(m_bMovmentController)
		m_MovementController->OnUpdate(deltaMilliseconds);

	return m_Root->VOnUpdate(this, elapsedTime);
}

//
// Scene::SetMovementController					- Chapter 14, page 485
//
void BLOCO_API Scene::SetMovementController( ActorId id )
{
	m_MovementController->SetObject( FindActor( id ) );
}

//
// Scene::SetMovementController					- Chapter 14, page 485
//
void BLOCO_API Scene::SetMovementController( shared_ptr<ISceneNode> object )
{
	m_MovementController->SetObject(object);
}

//
// Scene::FindActor					- Chapter 14, page 485
//
shared_ptr<ISceneNode> BLOCO_API Scene::FindActor( ActorId id )
{
	SceneActorMap::iterator it = m_ActorMap.find(id);

	if (it == m_ActorMap.end())
		return NULL;

	return it->second;
}

void BLOCO_API Scene::SetSelectedNode( ActorId id )
{
	shared_ptr<ISceneNode> node = FindActor(id);

	if (node)
	{
		if (m_selectedNode)
		{
			m_selectedNode->VSetSelected(false);
			m_selectedNode->VRemoveChild(*m_coordinateCrossNode->VGet()->ActorId());
		}

		m_coordinateCrossNode->VSetEnable(true);

		m_coordinateCrossNode->VSetTransform(node->VGet()->ToWorld());

		node->VAddChild(m_coordinateCrossNode);
		node->VSetSelected(true);
		m_selectedNode = node;
	}
}

void BLOCO_API Scene::EnableNode( ActorId id, bool value )
{
	shared_ptr<ISceneNode> node = FindActor(id);

	if (node)
		node->VSetEnable(value);
}

void BLOCO_API Scene::VUnselected()
{
	if (m_selectedNode)
	{
		m_selectedNode->VSetSelected(false);
		m_selectedNode->VRemoveChild(*m_coordinateCrossNode->VGet()->ActorId());
		m_coordinateCrossNode->VSetEnable(false);
	}
}

void BLOCO_API Scene::GetNodePosition( LuaObject luaID , LuaObject luaposition )
{
	if ( false == luaID.IsInteger() )
	{
		printf("[SYSTEM] Called GetNodePosition() and first param wasn't a number for actor ID!\n");
		return;
	}

	ISceneNode* node = FindActor(luaID.GetInteger()).get();

	if (node)
	{
		Vec position = MatGetPosition( GetGlobalTransform(node));


		const int tableCount = luaposition.GetTableCount();
		if ( 3 != tableCount )
		{
			printf("[SYSTEM] Called GetNodePosition() and second param didn't have exactly 3 entries!\n");
			return;
		}
		luaposition.SetNumber("x",position.GetX());
		luaposition.SetNumber("y",position.GetY());
		luaposition.SetNumber("z",position.GetZ());
	}
}

Mat BLOCO_API Scene::GetGlobalTransform( ISceneNode* node )
{
	Mat matNode =  node->VGet()->ToWorld();


	if (node->m_pParent)
	{
		Mat matParent = GetGlobalTransform( node->m_pParent );

		return matNode * matParent;
	}

	return matNode;
}

shared_ptr<ISceneNode> BLOCO_API Scene::FindActorByName( string actorName )
{
	if (m_Root->VGet()->Name() == actorName)
		return m_Root;

	return m_Root->VFindChild(actorName);
}

shared_ptr<ICamera> Scene::GetCamera()
{
	return m_Camera;
}



////////////////////////////////////////////////////
// RootNode Implementation
////////////////////////////////////////////////////

//
// RootNode::RootNode					- Chapter 14, page 489
//
 BLOCO_API RootNode::RootNode()
	: SceneNode(optional_empty(), "Root", RenderPass_0)
{
	m_Children.reserve(RenderPass_Last);

	m_pParent = NULL;

	shared_ptr<SceneNode> staticGroup(DEBUG_CLIENTBLOCK SceneNode(optional_empty(), "StaticGroup", RenderPass_Static));
	m_Children.push_back(staticGroup);	// RenderPass_Static = 0

	shared_ptr<SceneNode> actorGroup(DEBUG_CLIENTBLOCK SceneNode(optional_empty(), "ActorGroup", RenderPass_Actor));
	m_Children.push_back(actorGroup);	// RenderPass_Actor = 1

	shared_ptr<SceneNode> skyGroup(DEBUG_CLIENTBLOCK SceneNode(optional_empty(), "SkyGroup", RenderPass_Sky));
	m_Children.push_back(skyGroup);	// RenderPass_Sky = 2
}

//
// RootNode::VAddChild					- Chapter 14, page 489
//
bool BLOCO_API RootNode::VAddChild(shared_ptr<ISceneNode> kid)
{
	// The Root node has children that divide the scene graph into render passes.
	// Scene nodes will get added to these children based on the value of the
	// render pass member variable.

	if (!m_Children[kid->VGet()->RenderPass()])
	{
		assert(0 && _T("There is no such render pass"));
		return false;
	}

	return m_Children[kid->VGet()->RenderPass()]->VAddChild(kid);
}

//
// RootNode::VRemoveChild					- added post-press
//
//   Returns false if nothing was removed
//
bool BLOCO_API RootNode::VRemoveChild(ActorId id)
{
	bool anythingRemoved = false;
	for(int i=RenderPass_0; i<RenderPass_Last; ++i)
	{
		if ( m_Children[i]->VGet()->ActorId() == id )
		{
			m_Children.erase(m_Children.begin() + i );
			return true;
		}

		if(m_Children[i]->VRemoveChild(id))
		{
			anythingRemoved = true;
		}
	}
	return anythingRemoved;
}

//
// RootNode::VRenderChildren					- Chapter 14, page 514
//
HRESULT BLOCO_API RootNode::VRenderChildren(Scene *pScene)
{
	// This code creates fine control of the render passes.
	for (int pass = RenderPass_0; pass < RenderPass_Last; ++pass)
	{
		switch(pass)
		{
		case RenderPass_Static:
		case RenderPass_Actor:
			m_Children[pass]->VRenderChildren(pScene);
			break;

		case RenderPass_Sky:
			{
				// 				DWORD oldZWriteEnable;
				// 				DXUTGetD3D9Device()->GetRenderState(D3DRS_ZWRITEENABLE, &oldZWriteEnable);
				// 				DXUTGetD3D9Device()->SetRenderState(D3DRS_ZWRITEENABLE, false);
				// 
				// 				DWORD oldLightMode;
				// 				DXUTGetD3D9Device()->GetRenderState( D3DRS_LIGHTING, &oldLightMode );
				// 				DXUTGetD3D9Device()->SetRenderState( D3DRS_LIGHTING, FALSE );
				// 
				// 				DWORD oldCullMode;
				// 				DXUTGetD3D9Device()->GetRenderState( D3DRS_CULLMODE, &oldCullMode );
				// 				DXUTGetD3D9Device()->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
				// 
				// 				m_Children[pass]->VRenderChildren(pScene);
				// 
				// 				// Notice that the render states are returned to 
				// 				// their original settings.....
				// 				// Could there be a better way???
				// 
				// 				DXUTGetD3D9Device()->SetRenderState( D3DRS_LIGHTING, oldLightMode );
				// 				DXUTGetD3D9Device()->SetRenderState( D3DRS_CULLMODE, oldCullMode );
				// 
				// 				DXUTGetD3D9Device()->SetRenderState(D3DRS_ZWRITEENABLE, oldZWriteEnable);
				break;
			}
		}
	}

	return S_OK;
}







// ////////////////////////////////////////////////////
// // CameraNode Implementation
// ////////////////////////////////////////////////////

// CameraNode::CameraNode					
//
 BLOCO_API CameraNode::CameraNode( const optional<ActorId> actorId, string name, Mat transform, Frustum &frustum ):
		SceneNode(actorId, name, RenderPass_0 ), 
		m_bActive(true),
		m_DebugCamera(false), 
		m_pTarget(shared_ptr<SceneNode>()), 
		m_ViewProjection(Vec( 0.0f, 0.0f, 0.0f, 1.0f ))
{
	m_Frustum.SetAspect(g_pApp->GetScreenWidth() / (FLOAT) g_pApp->GetScreenHeight());
	m_Projection = MatProjection(m_Frustum.m_Fov, m_Frustum.m_Aspect, m_Frustum.m_Near, m_Frustum.m_Far);
	
	m_Frustum = frustum; 

	Mat m_matFromWorld = MatInverse(transform, &Vec());
	VSetTransform(transform,m_matFromWorld);
}

// CameraNode::VRender					
//
HRESULT BLOCO_API CameraNode::VRender(Scene *pScene)
{
	// 	if (m_DebugCamera)
	// 	{
	// 		pScene->PopMatrix();
	// 
	// 		m_Frustum.Render();	
	// 
	// 		pScene->PushAndSetMatrix(m_Props.ToWorld());
	// 	}

	return S_OK;
} 

//
// CameraNode::VOnRestore					
//
HRESULT BLOCO_API CameraNode::VOnRestore(Scene *pScene)
{
	m_Frustum.SetAspect(g_pApp->GetScreenWidth() / (FLOAT) g_pApp->GetScreenHeight());
	m_Projection = MatProjection(m_Frustum.m_Fov, m_Frustum.m_Aspect, m_Frustum.m_Near, m_Frustum.m_Far);

	return S_OK;
}

void BLOCO_API CameraNode::SetCameraOffset( Vec camOffset )
{
	Vec position = MatGetPosition(GetView());

	Mat newView = MatRotation( camOffset.GetX(), camOffset.GetY(),camOffset.GetZ() );

	MatSetPosition(newView, position);

	VSetTransform(newView,newView);
}

Vec BLOCO_API CameraNode::ScreenTo3D( Vec coord )
{

	// Get the inverse view matrix
	D3DXMATRIX matView  = D3DXMATRIX( VGet()->FromWorld() );											//  pScene->GetCamera()->GetView();
	D3DXMATRIX matWorld = D3DXMATRIX((float*) MatTranslation(MatGetPosition(VGet()->FromWorld())) );	/**pScene->GetTopMatrix();*/
	D3DXMATRIX proj		= D3DXMATRIX( GetProjection().GetArray() );

	// Compute the vector of the Pick ray in screen space
	D3DXVECTOR3 v;
	v.x = ( ( ( 2.0f  * coord.GetX() ) / g_pApp->GetScreenWidth() ) - 1 ) / proj._11;
	v.y = -( ( ( 2.0f * coord.GetY() ) / g_pApp->GetScreenHeight() ) - 1 ) / proj._22;
	v.z = 1.0f;


	D3DXMATRIX mWorldView = /*matWorld * */matView;
	D3DXMATRIX m;
	D3DXMatrixInverse( &m, NULL, &mWorldView );


	// Transform the screen space Pick ray into 3D space
	float x = v.x * m._11 + v.y * m._21 + v.z * m._31;
	float y = v.x * m._12 + v.y * m._22 + v.z * m._32;
	float z = v.x * m._13 + v.y * m._23 + v.z * m._33;

	return Vec(x,y,z);
}


////////////////////////////////////////////////////
// MovementController Implementation - added post press
////////////////////////////////////////////////////

#define MAX(a, b) ((a) >= (b) ? (a) : (b))
#define MIN(a, b) ((a) <  (b) ? (a) : (b))

#define RADIANS_TO_DEGREES(x) ((x) * 180.0f / XM_PI)
#define DEGREES_TO_RADIANS(x) ((x) * XM_PI / 180.0f)

 BLOCO_API MovementController::MovementController(shared_ptr<ISceneNode> object, float initialYaw, float initialPitch, bool rotateWhenLButtonDown)
	: CILKeyboardState("movement controller keyboard handler"), m_object(object)
{
	m_bRotateWhenLButtonDown = rotateWhenLButtonDown;

	m_matToWorld   = m_object->VGet()->ToWorld();
	m_matFromWorld = m_object->VGet()->FromWorld();

	m_fTargetYaw = m_fYaw = RADIANS_TO_DEGREES(-initialYaw);
	m_fTargetPitch = m_fPitch = RADIANS_TO_DEGREES(initialPitch);

	m_maxSpeed = 30.0f;			// 30 meters per second
	m_currentSpeed = 0.0f;

	Vec pos = MatGetPosition( m_matToWorld );
	m_matPosition = MatTranslation(pos);
	
	//m_bLeftMouseDown = false;
	POINT ptCursor;
	GetCursorPos( &ptCursor );
	m_lastMousePos = ptCursor;

	m_mouseLButtonDown = false;

	memset(m_keys, false, 0xff);
 
 	g_pInput->AddListener((ICILMouseHandler*)this);
 	g_pInput->AddListener((ICILKeyboardHandler*)this);
}

 BLOCO_API MovementController::MovementController()
	: CILKeyboardState("movement controller keyboard handler")
{
	//m_bLeftMouseDown = false;
	POINT ptCursor;
	GetCursorPos( &ptCursor );
	m_lastMousePos = ptCursor;

	m_mouseLButtonDown = false;
	m_bRotateWhenLButtonDown = false;
	memset(m_keys, false, 0xff);

	g_pInput->AddListener((ICILMouseHandler*)this);
	g_pInput->AddListener((ICILKeyboardHandler*)this);
}


void BLOCO_API MovementController::OnLButtonDown( const POINT &mousePos )
{
	m_mouseLButtonDown = true;

	// We want mouise movement to be relative to the position
	// the cursor was at when the user first presses down on
	// the left button
	m_lastMousePos = mousePos;
}

void BLOCO_API MovementController::OnLButtonUp( const POINT &mousePos )
{
	m_mouseLButtonDown = false;
}

void BLOCO_API MovementController::OnMouseMove( const POINT &mousePos, const POINT& delta )
{
	// There are two modes supported by this controller.

	if (m_bRotateWhenLButtonDown)
	{
		// Mode 1 - rotate the view only when the left mouse button is down
		// Only look around if the left button is down
		if(m_lastMousePos!=mousePos && m_mouseLButtonDown)
		{
			m_fTargetYaw = m_fTargetYaw - delta.x;
			m_fTargetPitch = m_fTargetPitch + delta.y;
			m_lastMousePos = mousePos;
		}
	}
	else if(m_lastMousePos!=mousePos)
	{
		// Mode 2 - rotate the controller when the mouse buttons are up
		m_fTargetYaw = m_fTargetYaw - delta.x;
		m_fTargetPitch = m_fTargetPitch + delta.y;
		m_lastMousePos = mousePos;
	}
}

void BLOCO_API MovementController::OnRButtonUp( const POINT &mousePos )
{

}

void BLOCO_API MovementController::OnRButtonDown( const POINT &mousePos )
{

}

void BLOCO_API MovementController::OnMButtonDown( const POINT &mousePos )
{

}

void BLOCO_API MovementController::OnMButtonUp( const POINT &mousePos )
{

}

void BLOCO_API MovementController::OnWheelMove( const POINT &mousePos, const short delta )
{

}





//  class MovementController::OnUpdate		- Chapter 9, page 246

void BLOCO_API MovementController::OnUpdate( const int deltaMilliseconds )
{
	if (!m_object)
		return;

	Vec pos = MatGetPosition( m_object->VGet()->ToWorld() );
	m_matPosition = MatTranslation(pos);

	bool bTranslating = false;
	Vec atWorld(0,0,0,0);
	Vec rightWorld(0,0,0,0);
	Vec upWorld(0,0,0,0);

	if (m_keys['W'] || m_keys['S'])
	{
		// In D3D, the "look at" default is always
		// the positive Z axis.
		Vec at = VEC_FORWARD; 
		if (m_keys['S'])
			at *= -1;

		// This will give us the "look at" vector 
		// in world space - we'll use that to move
		// the camera.
		atWorld = Vec4Transform(at, m_matToWorld);
		bTranslating = true;
	}

	if (m_keys['A'] || m_keys['D'])
	{
		// In D3D, the "right" default is always
		// the positive X axis.
		Vec right = VEC_RIGHT; 
		if (m_keys['A'])
			right *= -1;

		// This will give us the "right" vector 
		// in world space - we'll use that to move
		// the camera
		rightWorld = Vec4Transform(right, m_matToWorld);
		bTranslating = true;
	}

	if (m_keys['Q'] || m_keys['E'])
	{
		// In D3D, the "up" default is always
		// the positive Y axis.
		Vec up = VEC_UP; 
		if (!m_keys['Q'])
			up *= -1;

		//Unlike strafing, Up is always up no matter
		//which way you are looking
		upWorld = up;
		bTranslating = true;
	}

	//Handling rotation as a result of mouse position
	{
		// The secret formula!!! Don't give it away!
		//If you are seeing this now, then you must be some kind of elite hacker!
		m_fYaw += (m_fTargetYaw - m_fYaw) * ( .35f );
		m_fTargetPitch = MAX(-90, MIN(90, m_fTargetPitch));
		m_fPitch += (m_fTargetPitch - m_fPitch) * ( .35f );

		// Calculate the new rotation matrix from the camera
		// yaw and pitch.
		Mat matRot;
		matRot = MatRotationYawPitchRoll(DEGREES_TO_RADIANS(-m_fYaw), DEGREES_TO_RADIANS(m_fPitch), 0.0f);

		// Create the new object-to-world matrix, and the
		// new world-to-object matrix. 
		m_matToWorld = matRot * m_matPosition;
		Vec inverse;
		m_matFromWorld = MatInverse(m_matToWorld, &Vec()); 
		m_object->VSetTransform(m_matToWorld, m_matFromWorld);
	}

	if (bTranslating)
	{
		float elapsedTime = (float)deltaMilliseconds / 1000.0f;

		Vec direction = atWorld + rightWorld + upWorld;
		direction = VecNormalize(direction); 

		// Ramp the acceleration by the elapsed time.
		float numberOfSeconds = 1.f;
		m_currentSpeed += m_maxSpeed * ( (elapsedTime*elapsedTime) / numberOfSeconds);
		if (m_currentSpeed > m_maxSpeed)
			m_currentSpeed = m_maxSpeed;

		direction *= m_currentSpeed;

		Vec pos = MatGetPosition(m_matPosition) + direction;
		m_matPosition = MatSetPosition(m_matPosition, pos);
		m_matToWorld = MatSetPosition(m_matToWorld, pos);

		m_matFromWorld = MatInverse(m_matToWorld, &Vec());
		m_object->VSetTransform(m_matToWorld, m_matFromWorld);
	}
	else
	{
		m_currentSpeed = 0.0f;
	}
}

void BLOCO_API MovementController::SetObject( shared_ptr<ISceneNode> newObject )
{ 
	m_object = newObject; 

	m_matToWorld = m_object->VGet()->ToWorld();
	m_matFromWorld = m_object->VGet()->ToWorld();

	//m_object->VGet()->Transform(m_matToWorld, m_matFromWorld);

	m_fTargetYaw = m_fYaw = 0;
	m_fTargetPitch = m_fPitch = 0;

	m_maxSpeed = 30.0f;			// 30 meters per second
	m_currentSpeed = 0.0f;

	Vec pos = MatGetPosition( m_matToWorld );

	m_matPosition = MatTranslation(pos);
}

void BLOCO_API MovementController::OnUpdateSimpleInput( const int deltaMilliseconds )
{
	if (!m_object)
		return;

	Vec pos = MatGetPosition( m_object->VGet()->ToWorld() );
	m_matPosition = MatTranslation(pos);

	bool bTranslating = false;
	Vec atWorld(0,0,0,0);
	Vec rightWorld(0,0,0,0);
	Vec upWorld(0,0,0,0);

	if (GetAsyncKeyState('W') || GetAsyncKeyState('S'))
	{
		// In D3D, the "look at" default is always
		// the positive Z axis.
		Vec at = VEC_FORWARD; 
		if (!GetAsyncKeyState('s'))
			at *= -1;

		// This will give us the "look at" vector 
		// in world space - we'll use that to move
		// the camera.
		atWorld = Vec4Transform(at, m_matToWorld);
		bTranslating = true;
	}

	if (GetAsyncKeyState('a') || GetAsyncKeyState('d'))
	{
		// In D3D, the "right" default is always
		// the positive X axis.
		Vec right = VEC_RIGHT; 
		if (!GetAsyncKeyState('a'))
			right *= -1;

		// This will give us the "right" vector 
		// in world space - we'll use that to move
		// the camera
		rightWorld = Vec4Transform(right, m_matToWorld);
		bTranslating = true;
	}

	if (GetAsyncKeyState('q') || GetAsyncKeyState('e'))
	{
		// In D3D, the "up" default is always
		// the positive Y axis.
		Vec up = VEC_UP; 
		if (!GetAsyncKeyState('q'))
			up *= -1;

		//Unlike strafing, Up is always up no matter
		//which way you are looking
		upWorld = up;
		bTranslating = true;
	}

	//Handling rotation as a result of mouse position
	{
		// The secret formula!!! Don't give it away!
		//If you are seeing this now, then you must be some kind of elite hacker!
		m_fYaw += (m_fTargetYaw - m_fYaw) * ( .35f );
		m_fTargetPitch = MAX(-90, MIN(90, m_fTargetPitch));
		m_fPitch += (m_fTargetPitch - m_fPitch) * ( .35f );

		// Calculate the DEBUG_CLIENTBLOCK rotation matrix from the camera
		// yaw and pitch.
		Mat matRot;
		matRot = MatRotationYawPitchRoll(DEGREES_TO_RADIANS(-m_fYaw), DEGREES_TO_RADIANS(m_fPitch), 0.0f);

		// Create the DEBUG_CLIENTBLOCK object-to-world matrix, and the
		// DEBUG_CLIENTBLOCK world-to-object matrix. 
		m_matToWorld = matRot * m_matPosition;
		Vec inverse;
		m_matFromWorld = MatInverse(m_matToWorld, &Vec()); 
		m_object->VSetTransform(m_matToWorld, m_matFromWorld);
	}

	if (bTranslating)
	{
		float elapsedTime = (float)deltaMilliseconds / 1000.0f;

		Vec direction = atWorld + rightWorld + upWorld;
		direction = VecNormalize(direction); 

		// Ramp the acceleration by the elapsed time.
		float numberOfSeconds = 1.f;
		m_currentSpeed += m_maxSpeed * ( (elapsedTime*elapsedTime) / numberOfSeconds);
		if (m_currentSpeed > m_maxSpeed)
			m_currentSpeed = m_maxSpeed;

		direction *= m_currentSpeed;

		Vec pos = MatGetPosition(m_matPosition) + direction;
		m_matPosition = MatSetPosition(m_matPosition, pos);
		m_matToWorld = MatSetPosition(m_matToWorld, pos);

		m_matFromWorld = MatInverse(m_matToWorld, &Vec());
		m_object->VSetTransform(m_matToWorld, m_matFromWorld);
	}
	else
	{
		m_currentSpeed = 0.0f;
	}
}













/************************************************************************/
/* Picking                                                              */
/************************************************************************/

 BLOCO_API RayCast::RayCast(CPoint point, DWORD maxIntersections)
{
	m_MaxIntersections = maxIntersections;
	m_IntersectionArray.reserve(m_MaxIntersections);
	m_bUseD3DXIntersect = true;
	m_bAllHits = true;
	m_NumIntersections = 0;
	m_Point = point;
}

HRESULT BLOCO_API RayCast::Pick(Scene *pScene, optional<ActorId> actorId, vector<VertexNormalTexture>* vertices, vector<DWORD>* indices,  Mat world)
{
	if (!m_bAllHits && m_NumIntersections > 0)
		return S_OK;

	// Get the inverse view matrix
	D3DXMATRIX matView  = D3DXMATRIX( pScene->GetCamera()->GetView().GetArray() );			//  pScene->GetCamera()->GetView();
	D3DXMATRIX matWorld = D3DXMATRIX((float*) world.GetArray() );							/**pScene->GetTopMatrix();*/
	D3DXMATRIX proj		= D3DXMATRIX( pScene->GetCamera()->GetProjection().GetArray() );

	// Compute the vector of the Pick ray in screen space
	D3DXVECTOR3 v;
	v.x = ( ( ( 2.0f * m_Point.x ) / g_pApp->GetScreenWidth() ) - 1 ) / proj._11;
	v.y = -( ( ( 2.0f * m_Point.y ) / g_pApp->GetScreenHeight() ) - 1 ) / proj._22;
	v.z = 1.0f;


	D3DXMATRIX mWorldView = matWorld * matView;
	D3DXMATRIX m;
	D3DXMatrixInverse( &m, NULL, &mWorldView );

	// Transform the screen space Pick ray into 3D space
	m_vPickRayDir.x = v.x * m._11 + v.y * m._21 + v.z * m._31;
	m_vPickRayDir.y = v.x * m._12 + v.y * m._22 + v.z * m._32;
	m_vPickRayDir.z = v.x * m._13 + v.y * m._23 + v.z * m._33;
	m_vPickRayOrig.x = m._41;
	m_vPickRayOrig.y = m._42;
	m_vPickRayOrig.z = m._43;

	float fBary1, fBary2;
	float fDist;


	for( DWORD i = 0; i < indices->size()/3; i++ )
	{
		Vec v0 = vertices->at(indices->at(3 * i + 0)).pos;
		Vec v1 = vertices->at(indices->at(3 * i + 1)).pos;
		Vec v2 = vertices->at(indices->at(3 * i + 2)).pos;

		// Check if the Pick ray passes through this point
		if( IntersectTriangle( Vec((float*)m_vPickRayOrig), Vec((float*)m_vPickRayDir), v0, v1, v2, fDist, fBary1, fBary2 ) )
		{
			if( m_bAllHits || m_NumIntersections == 0 || fDist < m_IntersectionArray[0].m_fDist )
			{

				if( !m_bAllHits )
					m_NumIntersections = 0;

				++m_NumIntersections;

				m_IntersectionArray.resize(m_NumIntersections);

				Intersection* pIntersection;
				pIntersection = &m_IntersectionArray[m_NumIntersections-1];

				Vec normalDir = VecNormalize(Vec((float*)m_vPickRayDir));

				Vec hitPoint = normalDir * fDist;

				pIntersection->m_dwFace   = i;
				pIntersection->m_fDist    = fDist;
				pIntersection->m_hitPoint = hitPoint;
				pIntersection->m_actorId = actorId;

				if( m_NumIntersections == m_MaxIntersections )
					break;
			}
		}
	}

	return S_OK;
}

HRESULT BLOCO_API RayCast::Pick( Scene *pScene, optional<ActorId> actorId, vector<VertexNormalTexture>* vertices, Mat world )
{
	if (!m_bAllHits && m_NumIntersections > 0)
		return S_OK;

	// Get the inverse view matrix
	D3DXMATRIX matView  = D3DXMATRIX( pScene->GetCamera()->GetView().GetArray() );			//  pScene->GetCamera()->GetView();
	D3DXMATRIX matWorld = D3DXMATRIX((float*) world.GetArray() );							/**pScene->GetTopMatrix();*/
	D3DXMATRIX proj		= D3DXMATRIX( pScene->GetCamera()->GetProjection().GetArray() );

	// Compute the vector of the Pick ray in screen space
	D3DXVECTOR3 v;
	v.x = ( ( ( 2.0f * m_Point.x ) / g_pApp->GetScreenWidth() ) - 1 ) / proj._11;
	v.y = -( ( ( 2.0f * m_Point.y ) / g_pApp->GetScreenHeight() ) - 1 ) / proj._22;
	v.z = 1.0f;


	D3DXMATRIX mWorldView = matWorld * matView;
	D3DXMATRIX m;
	D3DXMatrixInverse( &m, NULL, &mWorldView );

	// Transform the screen space Pick ray into 3D space
	m_vPickRayDir.x = v.x * m._11 + v.y * m._21 + v.z * m._31;
	m_vPickRayDir.y = v.x * m._12 + v.y * m._22 + v.z * m._32;
	m_vPickRayDir.z = v.x * m._13 + v.y * m._23 + v.z * m._33;
	m_vPickRayOrig.x = m._41;
	m_vPickRayOrig.y = m._42;
	m_vPickRayOrig.z = m._43;

	float fBary1, fBary2;
	float fDist;


	for( DWORD i = 0; i < vertices->size()/3; i++ )
	{
		Vec v0 = vertices->at(3 * i + 0).pos;
		Vec v1 = vertices->at(3 * i + 1).pos;
		Vec v2 = vertices->at(3 * i + 2).pos;

		// Check if the Pick ray passes through this point
		if( IntersectTriangle( Vec((float*)m_vPickRayOrig), Vec((float*)m_vPickRayDir), v0, v1, v2, fDist, fBary1, fBary2 ) )
		{
			if( m_bAllHits || m_NumIntersections == 0 || fDist < m_IntersectionArray[0].m_fDist )
			{

				if( !m_bAllHits )
					m_NumIntersections = 0;

				++m_NumIntersections;

				m_IntersectionArray.resize(m_NumIntersections);

				Intersection* pIntersection;
				pIntersection = &m_IntersectionArray[m_NumIntersections-1];


				Vec normalDir = VecNormalize(Vec((float*)m_vPickRayDir));

				Vec hitPoint = normalDir * fDist;

				pIntersection->m_dwFace   = i;
				pIntersection->m_fDist    = fDist;
				pIntersection->m_hitPoint = hitPoint;
				pIntersection->m_actorId = actorId;

				if( m_NumIntersections == m_MaxIntersections )
					break;
			}
		}
}

return S_OK;
}

void BLOCO_API RayCast::Sort() 
{ 
	std::sort(m_IntersectionArray.begin(), m_IntersectionArray.end()) ; 
}
