#ifndef Scene_h__
#define Scene_h__

#include "Bloco.h"

#define fOPAQUE 1.0f

// Forward declarations
class SceneNode;
class Scene;
class RayCast;
class MovementController;
class BLOCO_API CameraNode;

typedef std::vector<shared_ptr<ISceneNode> > BLOCO_API SceneNodeList;
typedef std::map<ActorId, shared_ptr<ISceneNode> > BLOCO_API SceneActorMap;


enum Transparent_Type
{
	RGB_ZERO,
};

enum Light_Type
{
	Directional,
	Spot,
	Point,
};

struct sD3D11Material
{
	sD3D11Material()
	{
		Emmision	= Col(0.0f,0.0f,0.0f,0.0f);
		Ambient		= Col(0.0f,0.0f,0.0f,0.0f);
		Diffuse		= Col(0.0f,0.0f,0.0f,0.0f);
		Transparent = Col(0.0f,0.0f,0.0f,0.0f);

		Transparency = 1.0f;

		TransparentType = RGB_ZERO;
	}

	Col Emmision;
	Col Ambient;
	Col Diffuse;
	Col Transparent;
	Transparent_Type TransparentType;
	float Transparency;
};

class Material
{
	sD3D11Material m_Material;
public:
	Material() { m_Material = sD3D11Material(); }

	void SetAmbient( Col color ) { m_Material.Ambient = color; }
	Col  GetAmbient() { return m_Material.Ambient; }

	void SetDiffuse( Col color ) { m_Material.Diffuse = color; }
	Col  GetDiffuse() { return m_Material.Diffuse; }

	void SetEmmision( Col color ) { m_Material.Emmision = color; }
	Col  GetEmmision() { return m_Material.Emmision; } 

	void SetTransparent( Col color ) { m_Material.Transparent = color; }
	Col  GetTransparent() { return m_Material.Transparent; } 

	void SetTransparentType( Transparent_Type type ) { m_Material.TransparentType = type; }
	Transparent_Type  GetTransparentType() { return m_Material.TransparentType; } 

	void  SetTransparency( float transparency ) { m_Material.Transparency = transparency; }
	float GetTransparency() { return m_Material.Transparency; }
};

class D3D11Light
{
public:
	D3D11Light(Light_Type type, Vec direcetion, Vec position, Vec color = Vec(1.0f,1.0f,1.0f,1.0f)) :
		m_Type(type),
		m_Direction(direcetion),
		m_Position(position),
		m_Color(color)
		{
		}		



	void SetPosition( Vec position )	{ m_Position = position; }
	void SetPosition( Mat transform )	{ m_Position = MatGetPosition(transform); }
	void SetDirection( Vec direction )	{ m_Direction = direction; }
	void SetColor( Vec color )			{ m_Color= color; }
	void SetType( Light_Type type)		{ m_Type = type; }

	Vec GetPosition()			{ return m_Position; }
	Mat GetTranslation()		{ return MatTranslation( m_Position); }
	Vec GetDirection()			{ return m_Direction; }
	Vec GetColor()				{ return m_Color; }
	Light_Type GetLightType()	{ return m_Type;}

	void Serialize( std::ostrstream & ostm )
	{
		ostm << m_Type << "";

		m_Direction.Serialize( ostm );
		ostm << "";

		m_Position.Serialize( ostm );
		ostm << "";

		m_Color.Serialize( ostm );
		ostm << "";
	}

private:
	Light_Type m_Type;

	Vec m_Direction;
	Vec m_Position;
	Vec m_Color;
};

class BLOCO_API SceneNodeProperties
{
	friend class SceneNode;

protected:
	optional<ActorId>		m_ActorId;
	std::string				m_Name;
	Mat*					m_World;
	Mat*					m_InverseWorld;
	float					m_Radius;
	bool					m_enable;
	Material				m_Material;
	RenderPass				m_RenderPass;
	//For the Editor
	bool					m_IsSelected;

public:
	optional<ActorId> const &ActorId() const { return m_ActorId; }
	Mat  ToWorld()   { return *m_World; }
	Mat  FromWorld() { return *m_InverseWorld; }
	void Transform(Mat world, Mat invWorld) 
	{ 
		*m_World		= world; 
		*m_InverseWorld = invWorld; 
	}

	const char * Name() const			 { return m_Name.c_str(); }		
	void		 SetName(string name)	 { m_Name = name; }
	float	     Radius() const			 { return m_Radius; }
	bool		 IsSelected()			 { return m_IsSelected; }
	void		 SetSelected(bool value) { m_IsSelected = value;}
	bool		 IsEnable()				 { return m_enable; }
	void		 SetEnable(bool value)	 { m_enable = value;}
	Material     GetMaterial()			 { return m_Material; }
	RenderPass   RenderPass() const		 { return m_RenderPass; }
};

class BLOCO_API SceneNode : public ISceneNode
{
	friend class Scene;

protected:
	SceneNodeProperties		m_Props;

public:
	SceneNode(optional<ActorId> actorId, std::string name,RenderPass renderPass, Mat* to = NULL, Mat* from = NULL);

	virtual ~SceneNode();

	virtual SceneNodeProperties *  VGet()  { return &m_Props; }

	virtual HRESULT VPreRender(Scene *pScene);
	virtual HRESULT VRender(Scene *pScene);
	virtual HRESULT VPostRender(Scene *pScene);

	virtual void	VSetTransform(Mat toWorld = MatIdentity() , Mat fromWorld = MatIdentity());
	virtual void    VAddTransform(Mat value);
	shared_ptr<ISceneNode> VFindChild( string name );
	virtual bool	VAddChild(shared_ptr<ISceneNode> kid);
	virtual bool	VRemoveChild(ActorId id);
	virtual HRESULT VRenderChildren(Scene *pScene);
	virtual HRESULT VOnRestore(Scene *pScene);
	virtual bool	VIsVisible(Scene *pScene) const;
	virtual bool	VIsEnable() {return VGet()->IsEnable(); }
	virtual void	VSetEnable( bool value ) { VGet()->SetEnable(value); }
	virtual HRESULT VPick(Scene *pScene, RayCast *pRayCast);

	virtual bool VSetTexture( string filename, unsigned int index ) { return false; }
	virtual void VSetPosition( Vec pos );
	virtual void VSetSelected( bool value ) { VGet()->SetSelected(value); }

	virtual HRESULT VOnUpdate(Scene *, DWORD const elapsedMs);
	// 
	// 	virtual void SetAlpha(float alpha) { return m_Props.SetAlpha(alpha); }
	// 
	Vec GetPosition() { return MatGetPosition(*m_Props.m_World); }
	void SetPosition( Vec &pos ) { *m_Props.m_World = MatTranslation(pos); }

	void SetRadius(const float radius) { m_Props.m_Radius = radius; }
	void SetMaterial(const Material &mat) { m_Props.m_Material = mat; }

	virtual ActorParams * VGetActorParams( void );
};

class BLOCO_API MovementController : public cil::ICILMouseHandler, public cil::CILKeyboardState
{
protected:
	Mat m_matFromWorld;
	Mat	m_matToWorld;
	Mat m_matPosition;

	// Orientation Controls
	float		m_fTargetYaw;
	float		m_fTargetPitch;
	float		m_fYaw;
	float		m_fPitch;
	float		m_fPitcnDown;
	float		m_maxSphOnDown;
	float		m_fYawOeed;
	float		m_currentSpeed;
	float		m_maxSpeed;

	shared_ptr<ISceneNode> m_object;

	bool m_mouseLButtonDown;
	bool m_bRotateWhenLButtonDown;

	POINT m_lastMousePos;

public:
	MovementController(shared_ptr<ISceneNode> object, float initialYaw, float initialPitch, bool rotateWhenLButtonDown);
	MovementController();

	string VGetName() {return "SceneNode keyboard handler";}

	void SetObject(shared_ptr<ISceneNode> newObject);
	void OnUpdate( const int deltaMilliseconds );

	void OnMouseMove(const POINT &mousePos, const POINT& delta);
	void OnLButtonDown(const POINT &mousePos);
	void OnLButtonUp(const POINT &mousePos);

	const Mat *GetToWorld() { return &m_matToWorld; }
	const Mat *GetFromWorld() { return &m_matFromWorld; }	
};

class BLOCO_API Scene : public IScene
{
protected:
	shared_ptr<SceneNode>  	m_Root;
	shared_ptr<CameraNode> 	m_Camera;
	/*	ID3DXMatrixStack 	   *m_MatrixStack;*/
	SceneActorMap 			m_ActorMap;
	MovementController*		m_MovementController;
	shared_ptr<ISceneNode>  m_selectedNode;
	shared_ptr<ISceneNode>  m_coordinateCrossNode;
	bool m_bMovmentController;

	LuaObject metaTable;

public:

	Scene();
	virtual ~Scene();


	virtual shared_ptr<ICamera> GetCamera();;

	HRESULT OnRender();
	HRESULT OnRestore();
	HRESULT OnUpdate(const int deltaMilliseconds);
	shared_ptr<ISceneNode> FindActor(ActorId id);
	bool AddChild(optional<ActorId> id, shared_ptr<ISceneNode> kid)
	{ 
		if (id.valid())
		{
			// This allows us to search for this later based on actor id
			m_ActorMap[*id] = kid;	
		}
		return m_Root->VAddChild(kid); 
	}
	bool RemoveChild(ActorId id)
	{ 
		m_ActorMap.erase(id);
		return m_Root->VRemoveChild(id); 
	}
	void SetCamera(shared_ptr<CameraNode> camera) { m_Camera = camera; }
	shared_ptr<SceneNode>  GetRoot() { return m_Root; }
	shared_ptr<ISceneNode> GetCoordinateCross() { return m_coordinateCrossNode; }
	void SetMovementController( ActorId id );
	void SetMovementController( shared_ptr<ISceneNode> object  );
	void EneableMovementController( bool value ) { m_bMovmentController = value; }
	void SetSelectedNode( ActorId id);
	virtual void VUnselected();
	/*Vec GetNodePosition( ActorId id);*/
	void GetNodePosition( LuaObject luaID , LuaObject luaposition );
	Mat GetGlobalTransform( ISceneNode* node );
	void EnableNode( ActorId id, bool value );

	virtual shared_ptr<ISceneNode> FindActorByName(string actorName);

	HRESULT Pick(RayCast *pRayCast) { return m_Root->VPick(this, pRayCast); }
};

class BLOCO_API RootNode : public SceneNode
{
public:
	RootNode();
	bool	VAddChild(shared_ptr<ISceneNode> kid);
	bool	VRemoveChild(ActorId id);
	HRESULT VRenderChildren(Scene *pScene);
	bool	VIsVisible(Scene *pScene) const { return true; }
};

class BLOCO_API ScreenElementScene : public IScreenElement, public Scene
{
public:
	ScreenElementScene() : Scene() { }

	// IScreenElement Implementation
	virtual void VOnUpdate(int deltaMS) { OnUpdate(deltaMS); };
	virtual HRESULT VOnRestore() 
	{ OnRestore(); return S_OK; }
	virtual HRESULT VOnRender(double fTime, float fElapsedTime)
	{ OnRender(); return S_OK; }
	virtual int VGetZOrder() const { return PASS_SCENE; }
	virtual void VSetZOrder(int const zOrder) { }

	// Don't handle any messages
	virtual LRESULT CALLBACK VOnMsgProc( AppMsg msg ) { return 0; }

	virtual bool VIsVisible() const { return true; }
	virtual void VSetVisible(bool visible) { }
	virtual bool VAddChild(optional<ActorId> id, shared_ptr<ISceneNode> kid) { return Scene::AddChild(id, kid); }
};

class BLOCO_API CameraNode : public SceneNode, public ICamera
{
public:
	CameraNode( const optional<ActorId> actorId, string name, Mat transform, Frustum &frustum);

	virtual HRESULT VRender(Scene *pScene);
	virtual HRESULT VOnRestore(Scene *pScene);
	virtual bool VIsVisible(Scene *pScene) const { return m_bActive; }
	
	Vec  ScreenTo3D(Vec coord);
	void SetCameraOffset( Vec camOffset );

	void SetTarget(shared_ptr<SceneNode> pTarget)
	{ m_pTarget = pTarget; }

	void ClearTarget() { m_pTarget = shared_ptr<SceneNode>(); }
	shared_ptr<SceneNode> GetTarget() { return m_pTarget; }

	Mat GetViewProjection()		{ return m_ViewProjection * VGet()->FromWorld(); }
	virtual Mat GetProjection() { return m_Projection; }
	virtual Mat GetView()		{ return VGet()->FromWorld(); }

protected:
	Mat				m_ViewProjection;
	bool			m_bActive;
	bool			m_DebugCamera;
	shared_ptr<SceneNode> m_pTarget;
	Vec m_vecEye;
	Vec m_vecLookAt;
	Vec m_vecUp;
};

/************************************************************************/
/* Picking                                                              */
/************************************************************************/
class BLOCO_API Intersection
{
public:
	FLOAT m_fDist;                  // distance from ray origin to intersection
	DWORD m_dwFace;					// the face index of the intersection
	Vec   m_hitPoint;
	FLOAT m_fBary1, m_fBary2;		// Barycentric coordinates of the intersection
	FLOAT m_tu, m_tv;               // texture coords of intersection
	optional<ActorId> m_actorId;	// Which actor was intersected if there was one
	Vec m_worldLoc;					// world location of intersection
	Vec m_actorLoc;					// actor local coordinates of intersection
	Vec m_normal;					// normal of intersection

	bool const operator <(Intersection const &other) { return m_fDist < other.m_fDist; }
};


typedef std::vector<Intersection> BLOCO_API IntersectionArray;


class BLOCO_API RayCast
{
public:
	RayCast(CPoint point, DWORD maxIntersections = 16);

	DWORD m_MaxIntersections;
	DWORD m_NumIntersections;
	bool m_bUseD3DXIntersect;      // Whether to use D3DXIntersect
	bool m_bAllHits;			// Whether to just get the first "hit" or all "hits"
	CPoint m_Point;

	D3DXVECTOR3 m_vPickRayDir;
	D3DXVECTOR3 m_vPickRayOrig;

	IntersectionArray m_IntersectionArray;


	HRESULT Pick(Scene *pScene, optional<ActorId> actorId, vector<VertexNormalTexture>* vertices, vector<DWORD>* indices, Mat world);
	HRESULT Pick(Scene *pScene, optional<ActorId> actorId, vector<VertexNormalTexture>* vertices, Mat world);
	/*HRESULT Pick(Scene *pScene, optional<ActorId> actorId, CD3D11Mesh* pMesh, Mat world);*/

	void Sort();
};

#endif